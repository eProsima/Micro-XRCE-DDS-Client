#include <micrortps/client/core/session/session.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/util/time.h>
#include <micrortps/client/core/communication/communication.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include "log/message.h"

// Autogenerate these defines by the protocol generator tool?
#define HEARTBEAT_MAX_MSG_SIZE 16
#define ACKNACK_MAX_MSG_SIZE 16
#define CREATE_SESSION_MAX_MSG_SIZE 38
#define DELETE_SESSION_MAX_MSG_SIZE 16

#define MAX_CONNECTION_ATTEMPS 10

#define INITIAL_REQUEST_ID 0x0010

static uint16_t generate_request_id(Session* session);

static bool listen_message(Session* session, int poll_ms);

static bool wait_session_status(Session* session, uint8_t* buffer, size_t length, size_t attempts);

static void send_message(const Session* session, uint8_t* buffer, size_t length);
static bool recv_message(const Session* session, uint8_t**buffer, size_t* length, int poll_ms);

static void write_send_heartbeat(const Session* session, StreamId stream);
static void write_send_acknack(const Session* session, StreamId stream);

static void read_message(Session* session, MicroBuffer* message);
static void read_stream(Session* session, MicroBuffer* message, StreamId id, uint16_t seq_num);
static void read_submessage_list(Session* session, MicroBuffer* submessages, StreamId stream_id);
static void read_submessage(Session* session, MicroBuffer* submessage, uint8_t submessage_id, StreamId stream_id, uint8_t flags);

static void read_submessage_fragment(Session* session, MicroBuffer* payload, StreamId stream_id, bool last_fragment);
static void read_submessage_status(Session* session, MicroBuffer* submessage);

//==================================================================
//                             PUBLIC
//==================================================================

int create_session(Session* session, uint8_t session_id, uint32_t key, Communication* comm)
{
    session->comm = comm;
    session->last_request_id = INITIAL_REQUEST_ID;

    session->on_status = NULL;
    session->on_status_args = NULL;

    init_session_info(&session->info, session_id, key);
    init_stream_storage(&session->streams);

    uint8_t create_session_buffer[CREATE_SESSION_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, create_session_buffer, CREATE_SESSION_MAX_MSG_SIZE, session_header_offset(&session->info));

    write_create_session(&session->info, &mb, get_nano_time());
    stamp_create_session_header(&session->info, mb.init);
    set_session_info_request(&session->info, STATE_LOGIN);

    return wait_session_status(session, create_session_buffer, micro_buffer_length(&mb), MAX_CONNECTION_ATTEMPS);
}

int delete_session(Session* session)
{
    uint8_t delete_session_buffer[DELETE_SESSION_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, delete_session_buffer, DELETE_SESSION_MAX_MSG_SIZE, session_header_offset(&session->info));

    write_delete_session(&session->info, &mb);
    stamp_session_header(&session->info, 0, 0, mb.init);
    set_session_info_request(&session->info, STATE_LOGOUT);

    return wait_session_status(session, delete_session_buffer, micro_buffer_length(&mb), MAX_CONNECTION_ATTEMPS);
}

void set_status_callback(Session* session, OnStatusFunc on_status_func, void* args)
{
    session->on_status = on_status_func;
    session->on_status_args = args;
}

#ifdef PROFILE_READ_ACCESS
void set_topic_callback(Session* session, OnTopicFunc on_topic_func, void* args)
{
    session->on_topic = on_topic_func;
    session->on_topic_args = args;
}
#endif

StreamId create_output_best_effort_stream(Session* session, uint8_t* buffer, size_t size)
{
    uint8_t header_offset = session_header_offset(&session->info);
    return add_output_best_effort_buffer(&session->streams, buffer, size, header_offset);
}

StreamId create_output_reliable_stream(Session* session, uint8_t* buffer, size_t size, size_t history)
{
    uint8_t header_offset = session_header_offset(&session->info);
    return add_output_reliable_buffer(&session->streams, buffer, size, history, header_offset);
}

StreamId create_input_best_effort_stream(Session* session)
{
    return add_input_best_effort_buffer(&session->streams);
}

StreamId create_input_reliable_stream(Session* session, uint8_t* buffer, size_t size, size_t history)
{
    return add_input_reliable_buffer(&session->streams, buffer, size, history);
}

void run_session(Session* session, size_t read_attemps, int poll_ms)
{
    for(unsigned i = 0; i < session->streams.output_best_effort_size; ++i)
    {
        OutputBestEffortStream* stream = &session->streams.output_best_effort[i];
        StreamId id = create_stream_id(i, BEST_EFFORT_STREAM, OUTPUT_STREAM);

        uint8_t* buffer; size_t length; SeqNum seq_num;
        if(prepare_best_effort_buffer_to_send(stream, &buffer, &length, &seq_num))
        {
            stamp_session_header(&session->info, id.raw, seq_num, buffer);
            send_message(session, buffer, length);
        }
    }

    for(unsigned i = 0; i < session->streams.output_reliable_size; ++i)
    {
        OutputReliableStream* stream = &session->streams.output_reliable[i];
        StreamId id = create_stream_id(i, RELIABLE_STREAM, OUTPUT_STREAM);

        uint8_t* buffer; size_t length; SeqNum seq_num;
        while(prepare_next_reliable_buffer_to_send(stream, &buffer, &length, &seq_num))
        {
            stamp_session_header(&session->info, id.raw, seq_num, buffer);
            send_message(session, buffer, length);
        }

        if(output_reliable_stream_must_notify(stream, get_nano_time()))
        {
            write_send_heartbeat(session, id);
        }
    }

    for(size_t i = 0; i < read_attemps; i++)
    {
        (void) listen_message(session, poll_ms);
    }

    for(unsigned i = 0; i < session->streams.output_reliable_size; ++i)
    {
        OutputReliableStream* stream = &session->streams.output_reliable[i];
        uint8_t* buffer; size_t length;
        SeqNum seq_num_it = begin_output_nack_buffer_it(stream);
        if(next_reliable_nack_buffer_to_send(stream, &buffer, &length, &seq_num_it))
        {
            send_message(session, buffer, length);
        }
    }

    for(unsigned i = 0; i < session->streams.input_reliable_size; ++i)
    {
        //NOTE: check input_reliable_for_fragments here. Add a struct InputReliable to manage it.

        InputReliableStream* stream = &session->streams.input_reliable[i];
        StreamId id = create_stream_id(i, RELIABLE_STREAM, INPUT_STREAM);

        if(input_reliable_stream_must_confirm(stream))
        {
            write_send_acknack(session, id);
        }
    }
}

bool prepare_stream_to_write(Session* session, StreamId stream_id, size_t size, MicroBuffer* mb)
{
    bool available = false;
    switch(stream_id.type)
    {
        case BEST_EFFORT_STREAM:
        {
            OutputBestEffortStream* stream = get_output_best_effort_stream(&session->streams, stream_id.index);
            available = stream && prepare_best_effort_buffer_to_write(stream, size, mb);
            break;
        }
        case RELIABLE_STREAM:
        {
            OutputReliableStream* stream = get_output_reliable_stream(&session->streams, stream_id.index);
            available = stream && prepare_reliable_buffer_to_write(stream, size, mb);
            break;
        }
        default:
            break;
    }

    return available;
}

uint16_t init_base_object_request(Session* session, mrObjectId object_id, BaseObjectRequest* base)
{
    uint16_t request_id = generate_request_id(session);

    base->request_id.data[0] = (uint8_t) (request_id >> 8);
    base->request_id.data[1] = (uint8_t) request_id;
    object_id_to_raw(object_id, base->object_id.data);

    return request_id;
}

//==================================================================
//                             PRIVATE
//==================================================================
inline uint16_t generate_request_id(Session* session)
{
    uint16_t last_request_id = (UINT16_MAX == session->last_request_id)
        ? INITIAL_REQUEST_ID
        : session->last_request_id;

    session->last_request_id = last_request_id + 1;
    return last_request_id;
}

bool listen_message(Session* session, int poll_ms)
{
    //NOTE: Assuming that recv_message return always a message if it can mount it in 'poll_ms' milliseconds.
    uint8_t* data; size_t length;
    bool must_be_read = recv_message(session, &data, &length, poll_ms);
    if(must_be_read)
    {
        MicroBuffer mb;
        init_micro_buffer(&mb, data, length);
        read_message(session, &mb);
    }

    return must_be_read;
}

bool wait_session_status(Session* session, uint8_t* buffer, size_t length, size_t attempts)
{
    int poll_ms = 1;
    for(size_t i = 0; i < attempts && session_info_pending_request(&session->info); ++i)
    {
        send_message(session, buffer, length);
        poll_ms = listen_message(session, poll_ms) ? 1 : poll_ms * 2;
    }

    bool received = !session_info_pending_request(&session->info);
    if(!received)
    {
        reset_session_info_request(&session->info);
    }

    return received;
}

inline void send_message(const Session* session, uint8_t* buffer, size_t length)
{
    bool send = session->comm->send_msg(session->comm->instance, buffer, length);
    if(send)
    {
        DEBUG_PRINT_MESSAGE(SEND, buffer, length);
    }
}

inline bool recv_message(const Session* session, uint8_t**buffer, size_t* length, int poll_ms)
{
    bool received = session->comm->recv_msg(session->comm->instance, buffer, length, poll_ms);
    if(received)
    {
        DEBUG_PRINT_MESSAGE(RECV, *buffer, *length);
    }
    return received;
}

void write_send_heartbeat(const Session* session, StreamId id)
{
    uint8_t heartbeat_buffer[HEARTBEAT_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, heartbeat_buffer, HEARTBEAT_MAX_MSG_SIZE, session_header_offset(&session->info));

    const OutputReliableStream* stream = &session->streams.output_reliable[id.index];

    write_heartbeat(stream, &mb);
    stamp_session_header(&session->info, 0, id.raw, mb.init);
    send_message(session, heartbeat_buffer, micro_buffer_length(&mb));
}

void write_send_acknack(const Session* session, StreamId id)
{
    uint8_t acknack_buffer[ACKNACK_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, acknack_buffer, ACKNACK_MAX_MSG_SIZE, session_header_offset(&session->info));

    const InputReliableStream* stream = &session->streams.input_reliable[id.index];

    write_acknack(stream, &mb);
    stamp_session_header(&session->info, 0, id.raw, mb.init);
    send_message(session, acknack_buffer, micro_buffer_length(&mb));
}

void read_message(Session* session, MicroBuffer* mb)
{
    uint8_t stream_id_raw; uint16_t seq_num;
    if(read_session_header(&session->info, mb, &stream_id_raw, &seq_num))
    {
        StreamId id = create_stream_id_from_raw(stream_id_raw, INPUT_STREAM);
        read_stream(session, mb, id, seq_num);
    }
}

void read_stream(Session* session, MicroBuffer* mb, StreamId stream_id, uint16_t seq_num)
{
    switch(stream_id.type)
    {
        case NONE_STREAM:
        {
            read_submessage_list(session, mb, stream_id);
            break;
        }
        case BEST_EFFORT_STREAM:
        {
            InputBestEffortStream* stream = get_input_best_effort_stream(&session->streams, stream_id.index);
            if(stream && receive_best_effort_message(stream, seq_num))
            {
                read_submessage_list(session, mb, stream_id);
            }
            break;
        }
        case RELIABLE_STREAM:
        {
            InputReliableStream* stream = get_input_reliable_stream(&session->streams, stream_id.index);
            if(stream && receive_reliable_message(stream, seq_num, mb->iterator, micro_buffer_length(mb)))
            {
                read_submessage_list(session, mb, stream_id);
                MicroBuffer next_mb;
                while(next_input_reliable_buffer_available(stream, &next_mb))
                {
                    read_submessage_list(session, &next_mb, stream_id);
                }
            }
            break;
        }
        default:
            break;
    }
}

void read_submessage_list(Session* session, MicroBuffer* submessages, StreamId stream_id)
{
    uint8_t id; uint16_t length; uint8_t flags;
    while(read_submessage_header(submessages, &id, &length, &flags))
    {
        read_submessage(session, submessages, id, stream_id, flags);
    }
}

void read_submessage(Session* session, MicroBuffer* payload, uint8_t submessage_id, StreamId stream_id, uint8_t flags)
{
    switch(submessage_id)
    {
        case SUBMESSAGE_ID_STATUS_AGENT:
            if(stream_id.type == NONE_STREAM)
            {
                read_create_session_status(&session->info, payload);
            }
            break;

        case SUBMESSAGE_ID_STATUS:
            if(stream_id.type == NONE_STREAM)
            {
                read_delete_session_status(&session->info, payload);
            }
            else
            {
                read_submessage_status(session, payload);
            }
            break;

        case SUBMESSAGE_ID_DATA:
            #ifdef PROFILE_READ_ACCESS
            read_submessage_data(session, payload, stream_id, flags & FORMAT_MASK);
            #endif
            break;

        case SUBMESSAGE_ID_FRAGMENT:
            read_submessage_fragment(session, payload, stream_id, 0 != (flags & FLAG_LAST_FRAGMENT));
            break;

        case SUBMESSAGE_ID_HEARTBEAT:
            {
                InputReliableStream* stream = get_input_reliable_stream(&session->streams, stream_id.index);
                if(stream)
                {
                    read_submessage_heartbeat(stream, payload);
                }
            }
            break;

        case SUBMESSAGE_ID_ACKNACK:
            {
                OutputReliableStream* stream = get_output_reliable_stream(&session->streams, stream_id.index);
                if(stream)
                {
                    read_submessage_acknack(stream, payload);
                }
            }
            break;

        default:
            break;
    }
}

void read_submessage_fragment(Session* session, MicroBuffer* payload, StreamId stream_id, bool last_fragment)
{
    (void) session; (void) payload; (void) stream_id; (void) last_fragment;
    //TODO
}

void read_submessage_status(Session* session, MicroBuffer* submessage)
{
    STATUS_Payload payload;
    deserialize_STATUS_Payload(submessage, &payload);

    if(session->on_status != NULL)
    {
        mrObjectId object_id = create_object_id_from_raw(payload.base.related_request.object_id.data);
        uint16_t request_id = (((uint16_t) payload.base.related_request.request_id.data[0]) << 8)
                            + payload.base.related_request.request_id.data[1];
        uint8_t status = payload.base.result.status;

        session->on_status(session, object_id, request_id, status, session->on_status_args);
    }
}
