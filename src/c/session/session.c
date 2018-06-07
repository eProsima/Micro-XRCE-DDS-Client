#include <micrortps/client/session/session.h>
#include <micrortps/client/util/time.h>
#include "../xrce_protocol_serialization.h"

#define HEARTBEAT_MAX_MSG_SIZE 50
#define ACKNACK_MAX_MSG_SIZE 50
#define SUBHEADER_SIZE 4

void send_message(Session* session, uint8_t* buffer, size_t length);
void send_heartbeat(Session* session, OutputReliableStream* stream);
void send_acknack(Session* session, InputReliableStream* stream);

void read_message(Session* session, MicroBuffer* message);
void read_stream(Session* session, MicroBuffer* message);
void read_submessage_list(Session* session, MicroBuffer* submessages, StreamId stream_id);
void read_submessage(Session* session, MicroBuffer* submessage, uint8_t submessage_id, StreamId stream_id, uint8_t flags);

void read_fragment(Session* session, MicroBuffer* payload, StreamId stream_id, bool last_fragment); //TODO
void read_heartbeat(Session* session, MicroBuffer* payload, StreamId stream_id); //TODO
void read_acknack(Session* session, MicroBuffer* payload, StreamId stream_id); //TODO


void run_session(Session* session, size_t max_attemps, uint32_t poll_ms)
{
    (void) max_attemps; (void) poll_ms;

    for(int i = 0; i < session->streams.output_best_effort_size; i++)
    {
        OutputBestEffortStream* stream = &session->streams.output_best_effort[i];
        uint8_t* buffer; size_t length;
        if(prepare_best_effort_buffer_to_send(stream, &buffer, &length))
        {
            send_message(session, buffer, length);
        }
    }

    for(int i = 0; i < session->streams.output_reliable_size; i++)
    {
        OutputReliableStream* stream = &session->streams.output_reliable[i];
        uint8_t* buffer; size_t length;
        while(prepare_next_reliable_buffer_to_send(stream, &buffer, &length))
        {
            send_message(session, buffer, length);
        }

        if(output_reliable_stream_must_notify(stream, get_nano_time()))
        {
            send_heartbeat(session, stream);
        }
    }

    int recv_status = RECV_DATA_OK;
    for(size_t i = 0; i < max_attemps || recv_status == RECV_DATA_TIMEOUT; i++)
    {
        uint8_t* data; size_t length;
        int read_status = session->communication->recv_data(session->communication, &data, &length, poll_ms);
        if(read_status == RECV_DATA_OK)
        {
            MicroBuffer mb;
            init_micro_buffer(&mb, data, length);
            read_message(session, &mb);
        }
    }

    for(int i = 0; i < session->streams.input_reliable_size; i++)
    {
        InputReliableStream* stream = &session->streams.input_reliable[i];
        if(input_reliable_stream_must_confirm(stream))
        {
            send_acknack(session, stream);
        }
    }
}

void send_message(Session* session, uint8_t* buffer, size_t length)
{
    stamp_session_header(&session->info, buffer);
    session->communication->send_data(session->communication, buffer, length);
}

void send_heartbeat(Session* session, OutputReliableStream* stream)
{
    uint8_t heartbeat_buffer[HEARTBEAT_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, heartbeat_buffer, HEARTBEAT_MAX_MSG_SIZE, session_header_offset(&session->info));

    write_heartbeat(stream, &mb);
    send_message(session, heartbeat_buffer, micro_buffer_length(&mb));
}

void send_acknack(Session* session, InputReliableStream* stream)
{
    uint8_t acknack_buffer[ACKNACK_MAX_MSG_SIZE];
    MicroBuffer mb;
    init_micro_buffer_offset(&mb, acknack_buffer, ACKNACK_MAX_MSG_SIZE, session_header_offset(&session->info));

    write_acknack(stream, &mb);
    send_message(session, acknack_buffer, micro_buffer_length(&mb));
}

void read_message(Session* session, MicroBuffer* mb)
{
    if(read_session_header(&session->info, mb))
    {
        read_stream(session, mb);
    }
}

void read_stream(Session* session, MicroBuffer* mb)
{
    StreamId stream_id;
    uint16_t seq_num;
    if(read_stream_header(&session->streams, mb, &stream_id, &seq_num))
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
                if(stream && receive_reliable_message(stream, seq_num, mb))
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
}

void read_submessage_list(Session* session, MicroBuffer* submessages, StreamId stream_id)
{
    while(micro_buffer_remaining(submessages) > SUBHEADER_SIZE)
    {
        SubmessageHeader subheader;
        deserialize_SubmessageHeader(submessages, &subheader);
        submessages->endianness = (subheader.flags & FLAG_ENDIANNESS) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;

        if(micro_buffer_remaining(submessages) > subheader.length) //CHECK: or >= ??
        {
             break;
        }

        read_submessage(session, submessages, subheader.id, stream_id, subheader.flags);

        align_to(submessages, 4);
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

