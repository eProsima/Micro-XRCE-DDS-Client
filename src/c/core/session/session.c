#include <micrortps/client/core/session/session.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/util/time.h>
#include <micrortps/client/core/communication/communication.h>
#include "log/message.h"

// Autogenerate this defines by the protocol?
#define HEARTBEAT_MAX_MSG_SIZE 16
#define ACKNACK_MAX_MSG_SIZE 16
#define CREATE_SESSION_MAX_MSG_SIZE 38
#define DELETE_SESSION_MAX_MSG_SIZE 16

#define MAX_CONNECTION_ATTEMPS 10

#define INITIAL_REQUEST_ID 0x000F

bool listen_message(Session* session, uint32_t poll_ms);

bool wait_status_agent(Session* session, uint8_t* buffer, size_t length, size_t attempts);
bool read_status_agent_message(Session* session, MicroBuffer* message, int* status);

void send_message(Session* session, uint8_t* buffer, size_t length);
int recv_message(Session* session, uint8_t**buffer, size_t* length, uint32_t poll_ms);

void write_send_heartbeat(Session* session, StreamId stream);
void write_send_acknack(Session* session, StreamId stream);

void read_message(Session* session, MicroBuffer* message);
void read_stream(Session* session, MicroBuffer* message, StreamId id, uint16_t seq_num);
void read_submessage_list(Session* session, MicroBuffer* submessages, StreamId stream_id);
void read_submessage(Session* session, MicroBuffer* submessage, uint8_t submessage_id, StreamId stream_id, uint8_t flags);

void read_fragment(Session* session, MicroBuffer* payload, StreamId stream_id, bool last_fragment);

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

uint16_t generate_request_id(Session* session)
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
//    size_t history = size / session->comm->mtu(session->comm);
    size_t history = 1;
    return add_input_reliable_buffer(&session->streams, buffer, size, history);
}

void run_session(Session* session, size_t read_attemps, uint32_t poll_ms)
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

    for(size_t i = 0; i < read_attemps; i++)
    {
        (void) listen_message(session, poll_ms);
    }

    for(unsigned i = 0; i < session->streams.output_reliable_size; ++i)
    {
        OutputReliableStream* stream = &session->streams.output_reliable[i];
        uint8_t* buffer; size_t length;
        if(output_reliable_stream_must_send(stream, &buffer, &length))
        {
            send_message(session, buffer, length); //fix: it must send several messages.
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
bool listen_message(Session* session, uint32_t poll_ms)
{
    //NOTE: Assuming that recv_message return always a message if it can mount it in 'poll_ms' milliseconds.
    uint8_t* data; size_t length;
    bool must_be_read = (RECV_DATA_OK == recv_message(session, &data, &length, poll_ms));
    if(must_be_read)
    {
        MicroBuffer mb;
        init_micro_buffer(&mb, data, length);
        read_message(session, &mb);
    }

    return must_be_read;
}

bool wait_status_agent(Session* session, uint8_t* buffer, size_t length, size_t attempts)
{
    uint32_t poll_ms = 1;
    for(size_t i = 0; i < attempts && check_session_info_pending_request(&session->info); ++i)
    {
        send_message(session, buffer, length);
        poll_ms = listen_message(session, poll_ms) ? 1 : poll_ms * 2;
    }

    bool status_agent_received = check_session_info_pending_request(&session->info);
    if(status_agent_received)
    {
        restore_session_info_request(&session->info);
    }
    return status_agent_received;
}

void send_message(Session* session, uint8_t* buffer, size_t length)
{
    (void) send_msg(session->comm, buffer, length);
    DEBUG_PRINT_MESSAGE(SEND, buffer, length);
}

int recv_message(Session* session, uint8_t**buffer, size_t* length, uint32_t poll_ms)
{
    int read_status = recv_msg(session->comm, buffer, length, poll_ms);
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
                read_status_agent(&session->info, payload);
            }
            break;

        case SUBMESSAGE_ID_STATUS:
            #ifdef PROFILE_STATUS_ANSWER
            read_status_submessage(session, payload, stream_id);
            #endif
            break;

        case SUBMESSAGE_ID_DATA:
            #ifdef PROFILE_DATA_ACCESS
            read_data_submessage(session, payload, stream_id, flags & FORMAT_MASK);
            #endif
            break;

        case SUBMESSAGE_ID_FRAGMENT:
            read_fragment(session, payload, stream_id, 0 != (flags & FLAG_LAST_FRAGMENT));
            break;

        case SUBMESSAGE_ID_HEARTBEAT:
            {
                InputReliableStream* stream = get_input_reliable_stream(&session->streams, stream_id.index);
                if(stream)
                {
                    read_heartbeat(stream, payload);
                }
            }
            break;

        case SUBMESSAGE_ID_ACKNACK:
            {
                OutputReliableStream* stream = get_output_reliable_stream(&session->streams, stream_id.index);
                if(stream)
                {
                    read_acknack(stream, payload);
                }
            }
            break;

        default:
            break;
    }
}

void read_fragment(Session* session, MicroBuffer* payload, StreamId stream_id, bool last_fragment)
{
    (void) session; (void) payload; (void) stream_id; (void) last_fragment;
    //TODO
}
