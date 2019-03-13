#include <uxr/client/core/session/session.h>
#include <uxr/client/util/time.h>
#include <uxr/client/core/communication/communication.h>
#include <uxr/client/config.h>

#include "submessage_internal.h"
#include "session_internal.h"
#include "session_info_internal.h"
#include "stream/stream_storage_internal.h"
#include "stream/common_reliable_stream_internal.h"
#include "stream/input_best_effort_stream_internal.h"
#include "stream/input_reliable_stream_internal.h"
#include "stream/output_best_effort_stream_internal.h"
#include "stream/output_reliable_stream_internal.h"
#include "stream/seq_num_internal.h"
#include "../serialization/xrce_protocol_internal.h"
#include "../log/log_internal.h"

#define CREATE_SESSION_MAX_MSG_SIZE (MAX_HEADER_SIZE + SUBHEADER_SIZE + CREATE_CLIENT_PAYLOAD_SIZE)
#define DELETE_SESSION_MAX_MSG_SIZE (MAX_HEADER_SIZE + SUBHEADER_SIZE + DELETE_CLIENT_PAYLOAD_SIZE)
#define HEARTBEAT_MAX_MSG_SIZE      (MAX_HEADER_SIZE + SUBHEADER_SIZE + HEARTBEAT_PAYLOAD_SIZE)
#define ACKNACK_MAX_MSG_SIZE        (MAX_HEADER_SIZE + SUBHEADER_SIZE + ACKNACK_PAYLOAD_SIZE)

static bool listen_message(uxrSession* session, int poll_ms);
static bool listen_message_reliably(uxrSession* session, int poll_ms);

static bool wait_session_status(uxrSession* session, uint8_t* buffer, size_t length, size_t attempts);

static bool send_message(const uxrSession* session, uint8_t* buffer, size_t length);
static bool recv_message(const uxrSession* session, uint8_t** buffer, size_t* length, int poll_ms);

static void write_submessage_heartbeat(const uxrSession* session, uxrStreamId stream);
static void write_submessage_acknack(const uxrSession* session, uxrStreamId stream);

static void read_message(uxrSession* session, ucdrBuffer* message);
static void read_stream(uxrSession* session, ucdrBuffer* message, uxrStreamId id, uxrSeqNum seq_num);
static void read_submessage_list(uxrSession* session, ucdrBuffer* submessages, uxrStreamId stream_id);
static void read_submessage(uxrSession* session, ucdrBuffer* submessage,
                            uint8_t submessage_id, uxrStreamId stream_id, uint16_t length, uint8_t flags);

static void read_submessage_status(uxrSession* session, ucdrBuffer* submessage);
static void read_submessage_data(uxrSession* session, ucdrBuffer* submessage, uint16_t length, uxrStreamId stream_id, uint8_t format);
static void read_submessage_heartbeat(uxrSession* session, ucdrBuffer* submessage);
static void read_submessage_acknack(uxrSession* session, ucdrBuffer* submessage);
#ifdef PERFORMANCE_TESTING
static void read_submessage_performance(uxrSession* session, ucdrBuffer* submessage, uint16_t length);
#endif

static void process_status(uxrSession* session, uxrObjectId object_id, uint16_t request_id, uint8_t status);

static void on_new_output_reliable_stream_segment(ucdrBuffer* ub, uxrOutputReliableStream* args);
static FragmentationInfo on_get_fragmentation_info(uint8_t* submessage_header);

//==================================================================
//                             PUBLIC
//==================================================================

void uxr_init_session(uxrSession* session, uxrCommunication* comm, uint32_t key)
{
    session->comm = comm;

    session->request_list = NULL;
    session->status_list = NULL;
    session->request_status_list_size = 0;

    session->on_status = NULL;
    session->on_status_args = NULL;
    session->on_topic = NULL;
    session->on_topic_args = NULL;

    uxr_init_session_info(&session->info, 0x81, key);
    uxr_init_stream_storage(&session->streams);
}

void uxr_set_status_callback(uxrSession* session, uxrOnStatusFunc on_status_func, void* args)
{
    session->on_status = on_status_func;
    session->on_status_args = args;
}

void uxr_set_topic_callback(uxrSession* session, uxrOnTopicFunc on_topic_func, void* args)
{
    session->on_topic = on_topic_func;
    session->on_topic_args = args;
}

#ifdef PERFORMANCE_TESTING
void uxr_set_performance_callback(uxrSession* session, uxrOnPerformanceFunc on_echo_func, void* args)
{
    session->on_performance = on_echo_func;
    session->on_performance_args = args;
}
#endif

bool uxr_create_session(uxrSession* session)
{
    uxr_reset_stream_storage(&session->streams);

    uint8_t create_session_buffer[CREATE_SESSION_MAX_MSG_SIZE];
    ucdrBuffer ub;
    ucdr_init_buffer_offset(&ub, create_session_buffer, CREATE_SESSION_MAX_MSG_SIZE, uxr_session_header_offset(&session->info));

    uxr_buffer_create_session(&session->info, &ub, uxr_millis(), (uint16_t)(session->comm->mtu - INTERNAL_RELIABLE_BUFFER_OFFSET));
    uxr_stamp_create_session_header(&session->info, ub.init);

    bool received = wait_session_status(session, create_session_buffer, ucdr_buffer_length(&ub), UXR_CONFIG_MAX_SESSION_CONNECTION_ATTEMPTS);
    bool created = received && UXR_STATUS_OK == session->info.last_requested_status;
    return created;
}

bool uxr_delete_session(uxrSession* session)
{
    uint8_t delete_session_buffer[DELETE_SESSION_MAX_MSG_SIZE];
    ucdrBuffer ub;
    ucdr_init_buffer_offset(&ub, delete_session_buffer, DELETE_SESSION_MAX_MSG_SIZE, uxr_session_header_offset(&session->info));

    uxr_buffer_delete_session(&session->info, &ub);
    uxr_stamp_session_header(&session->info, 0, 0, ub.init);

    bool received = wait_session_status(session, delete_session_buffer, ucdr_buffer_length(&ub), UXR_CONFIG_MAX_SESSION_CONNECTION_ATTEMPTS);
    return received && UXR_STATUS_OK == session->info.last_requested_status;
}

uxrStreamId uxr_create_output_best_effort_stream(uxrSession* session, uint8_t* buffer, size_t size)
{
    uint8_t header_offset = uxr_session_header_offset(&session->info);
    return uxr_add_output_best_effort_buffer(&session->streams, buffer, size, header_offset);
}

uxrStreamId uxr_create_output_reliable_stream(uxrSession* session, uint8_t* buffer, size_t size, uint16_t history)
{
    uint8_t header_offset = uxr_session_header_offset(&session->info);
    return uxr_add_output_reliable_buffer(&session->streams, buffer, size, history, header_offset, on_new_output_reliable_stream_segment);
}

uxrStreamId uxr_create_input_best_effort_stream(uxrSession* session)
{
    return uxr_add_input_best_effort_buffer(&session->streams);
}

uxrStreamId uxr_create_input_reliable_stream(uxrSession* session, uint8_t* buffer, size_t size, uint16_t history)
{
    return uxr_add_input_reliable_buffer(&session->streams, buffer, size, history, on_get_fragmentation_info);
}

bool uxr_run_session_time(uxrSession* session, int timeout_ms)
{
    uxr_flash_output_streams(session);

    bool timeout = false;
    while(!timeout)
    {
         timeout = !listen_message_reliably(session, timeout_ms);
    }

    return uxr_output_streams_confirmed(&session->streams);
}

bool uxr_run_session_until_timeout(uxrSession* session, int timeout_ms)
{
    uxr_flash_output_streams(session);

    return listen_message_reliably(session, timeout_ms);
}

bool uxr_run_session_until_confirm_delivery(uxrSession* session, int timeout_ms)
{
    uxr_flash_output_streams(session);

    bool timeout = false;
    while(!uxr_output_streams_confirmed(&session->streams) && !timeout)
    {
        timeout = !listen_message_reliably(session, timeout_ms);
    }

    return uxr_output_streams_confirmed(&session->streams);
}

bool uxr_run_session_until_all_status(uxrSession* session, int timeout_ms, const uint16_t* request_list, uint8_t* status_list, size_t list_size)
{
    uxr_flash_output_streams(session);

    for(unsigned i = 0; i < list_size; ++i)
    {
        status_list[i] = UXR_STATUS_NONE;
    }

    session->request_list = request_list;
    session->status_list = status_list;
    session->request_status_list_size = list_size;

    bool timeout = false;
    bool status_confirmed = false;
    while(!timeout && !status_confirmed)
    {
        timeout = !listen_message_reliably(session, timeout_ms);
        status_confirmed = true;
        for(unsigned i = 0; i < list_size && status_confirmed; ++i)
        {
            status_confirmed = status_list[i] != UXR_STATUS_NONE
                            || request_list[i] == UXR_INVALID_REQUEST_ID; //CHECK: better give an error? an assert?
        }
    }

    session->request_status_list_size = 0;

    bool status_ok = true;
    for(unsigned i = 0; i < list_size && status_ok; ++i)
    {
        status_ok = status_list[i] == UXR_STATUS_OK || status_list[i] == UXR_STATUS_OK_MATCHED;
    }

    return status_ok;
}

bool uxr_run_session_until_one_status(uxrSession* session, int timeout_ms, const uint16_t* request_list, uint8_t* status_list, size_t list_size)
{
    uxr_flash_output_streams(session);

    for(unsigned i = 0; i < list_size; ++i)
    {
        status_list[i] = UXR_STATUS_NONE;
    }

    session->request_list = request_list;
    session->status_list = status_list;
    session->request_status_list_size = list_size;

    bool timeout = false;
    bool status_confirmed = false;
    while(!timeout && !status_confirmed)
    {
        timeout = !listen_message_reliably(session, timeout_ms);
        for(unsigned i = 0; i < list_size && !status_confirmed; ++i)
        {
            status_confirmed = status_list[i] != UXR_STATUS_NONE
                            || request_list[i] == UXR_INVALID_REQUEST_ID; //CHECK: better give an error? an assert?
        }
    }

    session->request_status_list_size = 0;

    return status_confirmed;
}

#ifdef PERFORMANCE_TESTING
bool uxr_buffer_performance(uxrSession *session,
                            uxrStreamId stream_id,
                            uint64_t epoch_time,
                            uint8_t* buf,
                            uint16_t len,
                            bool echo)
{
    bool rv = false;
    PERFORMANCE_Payload payload;
    payload.epoch_time_lsb = (uint32_t)(epoch_time & UINT32_MAX);
    payload.epoch_time_msb = (uint32_t)(epoch_time >> 32);
    payload.buf = buf;
    payload.len = len;
    ucdrBuffer mb;
    const uint16_t payload_length = (uint16_t)(sizeof(payload.epoch_time_lsb) +
                                               sizeof(payload.epoch_time_msb) +
                                               len);

    uint8_t flags = (echo) ? UXR_ECHO : 0;
    if(uxr_prepare_stream_to_write_submessage(session, stream_id, payload_length, &mb, SUBMESSAGE_ID_PERFORMANCE, flags))
    {
        (void) uxr_serialize_PERFORMANCE_Payload(&mb, &payload);
        rv = true;
    }
    return rv;
}
#endif

void uxr_flash_output_streams(uxrSession* session)
{
    for(uint8_t i = 0; i < session->streams.output_best_effort_size; ++i)
    {
        uxrOutputBestEffortStream* stream = &session->streams.output_best_effort[i];
        uxrStreamId id = uxr_stream_id(i, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);

        uint8_t* buffer; size_t length; uxrSeqNum seq_num;
        if(uxr_prepare_best_effort_buffer_to_send(stream, &buffer, &length, &seq_num))
        {
            uxr_stamp_session_header(&session->info, id.raw, seq_num, buffer);
            send_message(session, buffer, length);
        }
    }

    for(uint8_t i = 0; i < session->streams.output_reliable_size; ++i)
    {
        uxrOutputReliableStream* stream = &session->streams.output_reliable[i];
        uxrStreamId id = uxr_stream_id(i, UXR_RELIABLE_STREAM, UXR_OUTPUT_STREAM);

        uint8_t* buffer; size_t length; uxrSeqNum seq_num;
        while(uxr_prepare_next_reliable_buffer_to_send(stream, &buffer, &length, &seq_num))
        {
            uxr_stamp_session_header(&session->info, id.raw, seq_num, buffer);
            send_message(session, buffer, length);
        }
    }
}

//==================================================================
//                             PRIVATE
//==================================================================
bool listen_message(uxrSession* session, int poll_ms)
{
    uint8_t* data; size_t length;
    bool must_be_read = recv_message(session, &data, &length, poll_ms);
    if(must_be_read)
    {
        ucdrBuffer ub;
        ucdr_init_buffer(&ub, data, (uint32_t)length);
        read_message(session, &ub);
    }

    return must_be_read;
}

bool listen_message_reliably(uxrSession* session, int poll_ms)
{
    bool received = false;
    int32_t poll = (poll_ms >= 0) ? poll_ms : INT32_MAX;
    do
    {
        int64_t next_heartbeat_timestamp = INT64_MAX;
        int64_t timestamp = uxr_millis();
        for(uint8_t i = 0; i < session->streams.output_reliable_size; ++i)
        {
            uxrOutputReliableStream* stream = &session->streams.output_reliable[i];
            uxrStreamId id = uxr_stream_id(i, UXR_RELIABLE_STREAM, UXR_OUTPUT_STREAM);

            if(uxr_update_output_stream_heartbeat_timestamp(stream, timestamp))
            {
                write_submessage_heartbeat(session, id);
            }

            if(stream->next_heartbeat_timestamp < next_heartbeat_timestamp)
            {
                next_heartbeat_timestamp = stream->next_heartbeat_timestamp;
            }
        }

        int32_t poll_to_next_heartbeat = (next_heartbeat_timestamp != INT64_MAX) ? (int32_t)(next_heartbeat_timestamp - timestamp) : poll;
        if(0 == poll_to_next_heartbeat)
        {
            poll_to_next_heartbeat = 1;
        }

        int poll_chosen = (poll_to_next_heartbeat < poll) ? poll_to_next_heartbeat : poll;
        received = listen_message(session, poll_chosen);
        if(!received)
        {
            poll -= poll_chosen;
        }
    }
    while(!received && poll > 0);

    return received;
}

bool wait_session_status(uxrSession* session, uint8_t* buffer, size_t length, size_t attempts)
{
    session->info.last_requested_status = UXR_STATUS_NONE;

    int poll_ms = UXR_CONFIG_MIN_SESSION_CONNECTION_INTERVAL;
    for(size_t i = 0; i < attempts && session->info.last_requested_status == UXR_STATUS_NONE; ++i)
    {
        send_message(session, buffer, length);
        poll_ms = listen_message(session, poll_ms) ? UXR_CONFIG_MIN_SESSION_CONNECTION_INTERVAL : poll_ms * 2;
    }

    return session->info.last_requested_status != UXR_STATUS_NONE;
}

inline bool send_message(const uxrSession* session, uint8_t* buffer, size_t length)
{
    bool sent = session->comm->send_msg(session->comm->instance, buffer, length);
    UXR_DEBUG_PRINT_MESSAGE((sent) ? UXR_SEND : UXR_ERROR_SEND, buffer, length, session->info.key);
    return sent;
}

inline bool recv_message(const uxrSession* session, uint8_t**buffer, size_t* length, int poll_ms)
{
    bool received = session->comm->recv_msg(session->comm->instance, buffer, length, poll_ms);
    if(received)
    {
        UXR_DEBUG_PRINT_MESSAGE(UXR_RECV, *buffer, *length, session->info.key);
    }
    return received;
}

void write_submessage_heartbeat(const uxrSession* session, uxrStreamId id)
{
    uint8_t heartbeat_buffer[HEARTBEAT_MAX_MSG_SIZE];
    ucdrBuffer ub;
    ucdr_init_buffer_offset(&ub, heartbeat_buffer, HEARTBEAT_MAX_MSG_SIZE, uxr_session_header_offset(&session->info));

    const uxrOutputReliableStream* stream = &session->streams.output_reliable[id.index];

    /* Buffer submessage header. */
    uxr_buffer_submessage_header(&ub, SUBMESSAGE_ID_HEARTBEAT, HEARTBEAT_PAYLOAD_SIZE, 0);

    /* Buffer HEARTBEAT. */
    HEARTBEAT_Payload payload;
    payload.first_unacked_seq_nr = uxr_seq_num_add(stream->last_acknown, 1);
    payload.last_unacked_seq_nr = stream->last_sent;
    payload.stream_id = id.raw;
    (void) uxr_serialize_HEARTBEAT_Payload(&ub, &payload);

    /* Stamp message header. */
    uxr_stamp_session_header(&session->info, 0, 0, ub.init);
    send_message(session, heartbeat_buffer, ucdr_buffer_length(&ub));
}

void write_submessage_acknack(const uxrSession* session, uxrStreamId id)
{
    uint8_t acknack_buffer[ACKNACK_MAX_MSG_SIZE];
    ucdrBuffer ub;
    ucdr_init_buffer_offset(&ub, acknack_buffer, ACKNACK_MAX_MSG_SIZE, uxr_session_header_offset(&session->info));

    const uxrInputReliableStream* stream = &session->streams.input_reliable[id.index];

    /* Buffer submessage header. */
    uxr_buffer_submessage_header(&ub, SUBMESSAGE_ID_ACKNACK, ACKNACK_PAYLOAD_SIZE, 0);

    /* Buffer ACKNACK. */
    ACKNACK_Payload payload;
    uint16_t nack_bitmap = uxr_compute_acknack(stream, &payload.first_unacked_seq_num);
    payload.nack_bitmap[0] = (uint8_t)(nack_bitmap >> 8);
    payload.nack_bitmap[1] = (uint8_t)((nack_bitmap << 8) >> 8);
    payload.stream_id = id.raw;
    (void) uxr_serialize_ACKNACK_Payload(&ub, &payload);

    /* Stamp message header. */
    uxr_stamp_session_header(&session->info, 0, 0, ub.init);
    send_message(session, acknack_buffer, ucdr_buffer_length(&ub));
}

void read_message(uxrSession* session, ucdrBuffer* ub)
{
    uint8_t stream_id_raw; uxrSeqNum seq_num;
    if(uxr_read_session_header(&session->info, ub, &stream_id_raw, &seq_num))
    {
        uxrStreamId id = uxr_stream_id_from_raw(stream_id_raw, UXR_INPUT_STREAM);
        read_stream(session, ub, id, seq_num);
    }
}

void read_stream(uxrSession* session, ucdrBuffer* ub, uxrStreamId stream_id, uxrSeqNum seq_num)
{
    switch(stream_id.type)
    {
        case UXR_NONE_STREAM:
        {
            stream_id = uxr_stream_id_from_raw(0x00, UXR_INPUT_STREAM);
            read_submessage_list(session, ub, stream_id);
            break;
        }
        case UXR_BEST_EFFORT_STREAM:
        {
            uxrInputBestEffortStream* stream = uxr_get_input_best_effort_stream(&session->streams, stream_id.index);
            if(stream && uxr_receive_best_effort_message(stream, seq_num))
            {
                read_submessage_list(session, ub, stream_id);
            }
            break;
        }
        case UXR_RELIABLE_STREAM:
        {
            uxrInputReliableStream* stream = uxr_get_input_reliable_stream(&session->streams, stream_id.index);
            bool input_buffer_used;
            if(stream && uxr_receive_reliable_message(stream, seq_num, ub->iterator, ucdr_buffer_remaining(ub), &input_buffer_used))
            {
                if(!input_buffer_used)
                {
                    read_submessage_list(session, ub, stream_id);
                }

                ucdrBuffer next_mb;
                while(uxr_next_input_reliable_buffer_available(stream, &next_mb, SUBHEADER_SIZE))
                {
                    read_submessage_list(session, &next_mb, stream_id);
                }
            }
            write_submessage_acknack(session, stream_id);
            break;
        }
        default:
            break;
    }
}

void read_submessage_list(uxrSession* session, ucdrBuffer* submessages, uxrStreamId stream_id)
{
    uint8_t id; uint16_t length; uint8_t flags;
    while(uxr_read_submessage_header(submessages, &id, &length, &flags))
    {
        read_submessage(session, submessages, id, stream_id, length, flags);
    }
}

void read_submessage(uxrSession* session, ucdrBuffer* submessage, uint8_t submessage_id, uxrStreamId stream_id, uint16_t length, uint8_t flags)
{
    switch(submessage_id)
    {
        case SUBMESSAGE_ID_STATUS_AGENT:
            if(stream_id.type == UXR_NONE_STREAM)
            {
                uxr_read_create_session_status(&session->info, submessage);
            }
            break;

        case SUBMESSAGE_ID_STATUS:
            if(stream_id.type == UXR_NONE_STREAM)
            {
                uxr_read_delete_session_status(&session->info, submessage);
            }
            else
            {
                read_submessage_status(session, submessage);
            }
            break;

        case SUBMESSAGE_ID_DATA:
            read_submessage_data(session, submessage, length, stream_id, flags & FORMAT_MASK);
            break;

        case SUBMESSAGE_ID_HEARTBEAT:
            read_submessage_heartbeat(session, submessage);
            break;

        case SUBMESSAGE_ID_ACKNACK:
            read_submessage_acknack(session, submessage);
            break;

#ifdef PERFORMANCE_TESTING
        case SUBMESSAGE_ID_PERFORMANCE:
            read_submessage_performance(session, submessage, length);
            break;
#endif

        default:
            break;
    }
}

void read_submessage_status(uxrSession* session, ucdrBuffer* submessage)
{
    STATUS_Payload payload;
    uxr_deserialize_STATUS_Payload(submessage, &payload);

    uxrObjectId object_id; uint16_t request_id;
    uxr_parse_base_object_request(&payload.base.related_request, &object_id, &request_id);

    uint8_t status = payload.base.result.status;
    process_status(session, object_id, request_id, status);
}


extern void read_submessage_format(uxrSession* session, ucdrBuffer* data, uint16_t length, uint8_t format,
                                   uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id);

void read_submessage_data(uxrSession* session, ucdrBuffer* submessage, uint16_t length, uxrStreamId stream_id, uint8_t format)
{
    BaseObjectRequest base;
    uxr_deserialize_BaseObjectRequest(submessage, &base);
    length = (uint16_t)(length - 4); //CHANGE: by a future size_of_BaseObjectRequest

    uxrObjectId object_id; uint16_t request_id;
    uxr_parse_base_object_request(&base, &object_id, &request_id);

    process_status(session, object_id, request_id, UXR_STATUS_OK);

    if(session->on_topic != NULL)
    {
        read_submessage_format(session, submessage, length, format, stream_id, object_id, request_id);
    }
}

void read_submessage_heartbeat(uxrSession* session, ucdrBuffer* submessage)
{
    HEARTBEAT_Payload heartbeat;
    uxr_deserialize_HEARTBEAT_Payload(submessage, &heartbeat);
    uxrStreamId id = uxr_stream_id_from_raw(heartbeat.stream_id, UXR_INPUT_STREAM);

    uxrInputReliableStream* stream = uxr_get_input_reliable_stream(&session->streams, id.index);
    if(stream)
    {
        uxr_process_heartbeat(stream, heartbeat.first_unacked_seq_nr, heartbeat.last_unacked_seq_nr);
        write_submessage_acknack(session, id);
    }
}

void read_submessage_acknack(uxrSession* session, ucdrBuffer* submessage)
{
    ACKNACK_Payload acknack;
    uxr_deserialize_ACKNACK_Payload(submessage, &acknack);
    uxrStreamId id = uxr_stream_id_from_raw(acknack.stream_id, UXR_INPUT_STREAM);

    uxrOutputReliableStream* stream = uxr_get_output_reliable_stream(&session->streams, id.index);
    if(stream)
    {
        uint16_t nack_bitmap = (uint16_t)(((uint16_t)acknack.nack_bitmap[0] << 8) + acknack.nack_bitmap[1]);
        uxr_process_acknack(stream, nack_bitmap, acknack.first_unacked_seq_num);

        uint8_t* buffer; size_t length;
        uxrSeqNum seq_num_it = uxr_begin_output_nack_buffer_it(stream);
        while (uxr_next_reliable_nack_buffer_to_send(stream, &buffer, &length, &seq_num_it))
        {
            send_message(session, buffer, length);
        }
    }
}

#ifdef PERFORMANCE_TESTING
void read_submessage_performance(uxrSession* session, ucdrBuffer* submessage, uint16_t length)
{
    ucdrBuffer mb_performance;
    ucdr_init_buffer(&mb_performance, submessage->iterator, length);
    session->on_performance(session, &mb_performance, session->on_performance_args);
}
#endif

void process_status(uxrSession* session, uxrObjectId object_id, uint16_t request_id, uint8_t status)
{
    if(session->on_status != NULL)
    {
        session->on_status(session, object_id, request_id, status, session->on_status_args);
    }

    for(unsigned i = 0; i < session->request_status_list_size; ++i)
    {
        if(request_id == session->request_list[i])
        {
            session->status_list[i] = status;
            break;
        }
    }
}

bool uxr_prepare_stream_to_write_submessage(uxrSession* session, uxrStreamId stream_id, size_t payload_size, ucdrBuffer* ub, uint8_t submessage_id, uint8_t mode)
{
    bool available = false;
    size_t submessage_size = SUBHEADER_SIZE + payload_size + uxr_submessage_padding(payload_size);

    switch(stream_id.type)
    {
        case UXR_BEST_EFFORT_STREAM:
        {
            uxrOutputBestEffortStream* stream = uxr_get_output_best_effort_stream(&session->streams, stream_id.index);
            available = stream && uxr_prepare_best_effort_buffer_to_write(stream, submessage_size, ub);
            break;
        }
        case UXR_RELIABLE_STREAM:
        {
            uxrOutputReliableStream* stream = uxr_get_output_reliable_stream(&session->streams, stream_id.index);
            available = stream && uxr_prepare_reliable_buffer_to_write(stream, submessage_size, SUBHEADER_SIZE, ub);
            break;
        }
        default:
            break;
    }

    if(available)
    {
        (void) uxr_buffer_submessage_header(ub, submessage_id, (uint16_t)payload_size, mode);
    }

    return available;
}

void on_new_output_reliable_stream_segment(ucdrBuffer* ub, uxrOutputReliableStream* stream)
{
    uint8_t* last_buffer = uxr_get_output_buffer(stream, stream->last_written % stream->history);
    uint8_t last_fragment_flag = FLAG_LAST_FRAGMENT * (last_buffer == ub->init);

    (void) uxr_buffer_submessage_header(ub, SUBMESSAGE_ID_FRAGMENT, (uint16_t)(ucdr_buffer_remaining(ub) - SUBHEADER_SIZE), last_fragment_flag);
}

FragmentationInfo on_get_fragmentation_info(uint8_t* submessage_header)
{
    ucdrBuffer ub;
    ucdr_init_buffer(&ub, submessage_header, SUBHEADER_SIZE);

    uint8_t id; uint16_t length; uint8_t flags;
    uxr_read_submessage_header(&ub, &id, &length, &flags);

    FragmentationInfo fragmentation_info;
    if(SUBMESSAGE_ID_FRAGMENT == id)
    {
        fragmentation_info = FLAG_LAST_FRAGMENT & flags ? LAST_FRAGMENT : INTERMEDIATE_FRAGMENT;
    }
    else
    {
        fragmentation_info = NO_FRAGMENTED;
    }
    return fragmentation_info;
}

