#include <micrortps/client/session/session.h>
#include <micrortps/client/session/submessage.h>
#include <micrortps/client/serialization/xrce_protocol.h>
#include <micrortps/client/util/time.h>
#include "../log/message.h"
#include <micrortps/client/communication/communication.h>

// Autogenerate this defines by the protocol?
#define HEARTBEAT_MAX_MSG_SIZE 16
#define ACKNACK_MAX_MSG_SIZE 16
#define CREATE_SESSION_MAX_MSG_SIZE 38
#define DELETE_SESSION_MAX_MSG_SIZE 16

#define MAX_CONNECTION_ATTEMPS 10

#define INITIAL_REQUEST_ID 2

int wait_status_agent(Session* session, uint8_t* buffer, size_t length, size_t attempts);

void send_message(Session* session, uint8_t* buffer, size_t length);
int recv_message(Session* session, uint8_t**buffer, size_t* length, uint32_t poll_ms);

void write_send_heartbeat(Session* session, StreamId stream);
void write_send_acknack(Session* session, StreamId stream);

void read_message(Session* session, MicroBuffer* message);
void read_stream(Session* session, MicroBuffer* message, StreamId id, uint16_t seq_num);
void read_submessage_list(Session* session, MicroBuffer* submessages, StreamId stream_id);
void read_submessage(Session* session, MicroBuffer* submessage, uint8_t submessage_id, StreamId stream_id, uint8_t flags);

void read_fragment(Session* session, MicroBuffer* payload, StreamId stream_id, bool last_fragment);
void read_heartbeat(Session* session, MicroBuffer* payload, StreamId stream_id);
void read_acknack(Session* session, MicroBuffer* payload, StreamId stream_id);

//==================================================================
//                             PUBLIC
//==================================================================

int create_session(Session* session, uint8_t session_id, uint32_t key, Communication* comm)
{
    session->comm = comm;
    session->last_request_id = INITIAL_REQUEST_ID;
    init_session_info(&session->info, session_id, key);
    init_stream_storage(&session->streams);

    uint8_t create_session_buffer[CREATE_SESSION_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, create_session_buffer, CREATE_SESSION_MAX_MSG_SIZE, session_header_offset(&session->info));

    write_create_session(&session->info, &mb, get_nano_time());
    stamp_first_session_header(&session->info, mb.init);
    return wait_status_agent(session, create_session_buffer, micro_buffer_length(&mb), MAX_CONNECTION_ATTEMPS);
}

int delete_session(Session* session)
{
    uint8_t delete_session_buffer[DELETE_SESSION_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, delete_session_buffer, DELETE_SESSION_MAX_MSG_SIZE, session_header_offset(&session->info));

    write_delete_session(&session->info, &mb);
    stamp_session_header(&session->info, 0, 0, mb.init);
    return wait_status_agent(session, delete_session_buffer, micro_buffer_length(&mb), MAX_CONNECTION_ATTEMPS);
}

int generate_request_id(Session* session)
{
    return ++session->last_request_id;
}

StreamId create_output_best_effort_stream(Session* session, uint8_t* buffer, size_t size)
{
    uint8_t offset = session_header_offset(&session->info);
    return add_output_best_effort_buffer(&session->streams, buffer, size, offset);
}

StreamId create_output_reliable_stream(Session* session, uint8_t* buffer, size_t size, size_t message_size)
{
    size_t history = size / message_size;
    uint8_t offset = session_header_offset(&session->info) + SUBHEADER_SIZE;
    return add_output_reliable_buffer(&session->streams, buffer, size, history, offset);
}

StreamId create_input_best_effort_stream(Session* session)
{
    return add_input_best_effort_buffer(&session->streams);
}

StreamId create_input_reliable_stream(Session* session, uint8_t* buffer, size_t size)
{
    size_t history = size / session->comm->mtu(session->comm);
    return add_input_reliable_buffer(&session->streams, buffer, size, history);
}

void run_session(Session* session, size_t max_attemps, uint32_t poll_ms)
{
    for(unsigned i = 0; i < session->streams.output_best_effort_size; ++i)
    {
        OutputBestEffortStream* stream = &session->streams.output_best_effort[i];
        StreamId id = create_stream_id(i, BEST_EFFORT_STREAM, OUTPUT_STREAM);

        uint8_t* buffer; size_t length; uint16_t seq_num;
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

        uint8_t* buffer; size_t length; uint16_t seq_num;
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

    int recv_status = RECV_DATA_OK;
    for(size_t i = 0; i < max_attemps || recv_status == RECV_DATA_TIMEOUT; ++i)
    {
        uint8_t* data; size_t length;
//        int read_status = session->communication->recv_data(session->communication, data, &length, poll_ms);
        int read_status = 0;
        if(read_status == RECV_DATA_OK)
        {
            MicroBuffer mb;
            init_micro_buffer(&mb, buffer, length);
            read_message(session, &mb);
        }
    }

    for(unsigned i = 0; i < session->streams.output_reliable_size; ++i)
    {
        OutputReliableStream* stream = &session->streams.output_reliable[i];
        uint8_t* buffer; size_t length;
        if(output_reliable_stream_must_send(stream, &buffer, &length))
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


//==================================================================
//                             PRIVATE
//==================================================================
int wait_status_agent(Session* session, uint8_t* buffer, size_t length, size_t attempts)
{
    int status_agent = 0;
    uint32_t poll_ms = 1;
    for(size_t i = 0; i < attempts || status_agent; ++i)
    {
        send_message(session, buffer, length);

        uint8_t* in_buffer; size_t in_length;
        int read_status = recv_message(session, &in_buffer, &in_length, poll_ms);
        if(read_status == RECV_DATA_OK)
        {
            MicroBuffer in_mb;
            init_micro_buffer(&in_mb, in_buffer, in_length);
            read_status_agent_message(&session->info, &in_mb, &status_agent);
        }
        else if (read_status == RECV_DATA_TIMEOUT)
        {
            poll_ms *= 2;
        }
    }

    return status_agent;
}

void send_message(Session* session, uint8_t* buffer, size_t length)
{
//    session->communication->send_data(session->communication, buffer, length);
    DEBUG_PRINT_MESSAGE(SEND, buffer, length);
}

int recv_message(Session* session, uint8_t**buffer, size_t* length, uint32_t poll_ms)
{
    int read_status = session->comm->recv_data(session->comm, buffer, length, poll_ms);
    DEBUG_PRINT_MESSAGE(SEND, *buffer, *length);
    return read_status;
}

void write_send_heartbeat(Session* session, StreamId id)
{
    uint8_t heartbeat_buffer[HEARTBEAT_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, heartbeat_buffer, HEARTBEAT_MAX_MSG_SIZE, session_header_offset(&session->info));

    OutputReliableStream* stream = &session->streams.output_reliable[id.index];

    write_heartbeat(stream, &mb);
    stamp_session_header(&session->info, 0, id.raw, mb.init);
    send_message(session, heartbeat_buffer, micro_buffer_length(&mb));
}

void write_send_acknack(Session* session, StreamId id)
{
    uint8_t acknack_buffer[ACKNACK_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, acknack_buffer, ACKNACK_MAX_MSG_SIZE, session_header_offset(&session->info));

    InputReliableStream* stream = &session->streams.input_reliable[id.index];

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
            if(stream && receive_reliable_message(stream, seq_num, mb->init, micro_buffer_length(mb)))
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
    SubmessageId id; uint16_t length; SubmessageFlags flags;
    while(read_submessage_header(submessages, &id, &length, &flags) && micro_buffer_remaining(submessages) >= length)
    {
        read_submessage(session, submessages, id, stream_id, flags);
    }
}

void read_submessage(Session* session, MicroBuffer* submessage, uint8_t submessage_id, StreamId stream_id, uint8_t flags)
{
    switch(submessage_id)
    {
        case SUBMESSAGE_ID_STATUS:
            #ifdef PROFILE_STATUS_ANSWER
            read_status_submessage(session, submessage, stream_id);
            #endif
            break;

        case SUBMESSAGE_ID_DATA:
            #ifdef PROFILE_DATA_ACCESS
            read_data_submessage(session, submessage, stream_id, flags & FORMAT_MASK);
            #endif
            break;

        case SUBMESSAGE_ID_FRAGMENT:
            read_fragment(session, submessage, stream_id, 0 != (flags & FLAG_LAST_FRAGMENT));
            break;

        case SUBMESSAGE_ID_HEARTBEAT:
            read_heartbeat(session, submessage, stream_id);
            break;

        case SUBMESSAGE_ID_ACKNACK:
            read_acknack(session, submessage, stream_id);
            break;

        default:
            break;
    }
}

void read_fragment(Session* session, MicroBuffer* payload, StreamId stream_id, bool last_fragment)
{
    (void)session; (void)payload; (void)stream_id; (void)last_fragment;
    //TODO
    //Save flag into InputReliable to control it later in run_sesion stage
}

void read_heartbeat(Session* session, MicroBuffer* payload, StreamId stream_id)
{
    HEARTBEAT_Payload heartbeat;
    deserialize_HEARTBEAT_Payload(payload, &heartbeat);

    InputReliableStream* stream = get_input_reliable_stream(&session->streams, stream_id.index);
    if(stream)
    {
        process_heartbeat(stream, heartbeat.first_unacked_seq_nr, heartbeat.last_unacked_seq_nr);
    }
}

void read_acknack(Session* session, MicroBuffer* payload, StreamId stream_id)
{
    ACKNACK_Payload acknack;
    deserialize_ACKNACK_Payload(payload, &acknack);

    OutputReliableStream* stream = get_output_reliable_stream(&session->streams, stream_id.index);
    if(stream)
    {
        uint16_t bitmap = (MACHINE_ENDIANNESS == LITTLE_ENDIANNESS)
            ? ((uint16_t)acknack.nack_bitmap[0] << 8) + (uint16_t)acknack.nack_bitmap[1]
            : (uint16_t)acknack.nack_bitmap[0] + ((uint16_t)acknack.nack_bitmap[1] << 8);

        process_acknack(stream, acknack.first_unacked_seq_num, bitmap);
    }
}
