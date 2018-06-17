#include <micrortps/client/session/input_reliable_stream.h>
#include <micrortps/client/session/submessage.h>
#include <micrortps/client/serialization/xrce_protocol.h>
#include <microcdr/microcdr.h>
#include <string.h>

// Remove when Microcdr supports size_of functions
#define ACKNACK_PAYLOAD_SIZE    4

#define NACK_BITMAP_SIZE       16

void process_heartbeat(InputReliableStream* stream, uint16_t first_seq_num, uint16_t last_seq_num);
uint16_t compute_nack_bitmap(InputReliableStream* stream);

bool is_input_buffer_empty(InputReliableStream* stream, size_t position);
void set_input_buffer_empty(InputReliableStream* stream, size_t position);

//==================================================================
//                             PUBLIC
//==================================================================
void init_input_reliable_stream(InputReliableStream* stream, uint8_t* buffer, size_t size, size_t history)
{
    stream->buffer = buffer;
    stream->size = size;
    stream->history = history;

    stream->last_handled = UINT16_MAX;
    stream->last_announced = UINT16_MAX;
    stream->must_confirm = false;
}

bool receive_reliable_message(InputReliableStream* stream, uint16_t seq_num, uint8_t* buffer, size_t length)
{
    bool result = false;
    /* Check if the seq_num is valid for the stream state */
    SeqNum last_history = seq_num_add(stream->last_handled, stream->history);
    if(0 > seq_num_cmp(stream->last_handled, seq_num) || 0 <= seq_num_cmp(last_history, seq_num))
    {
        /* Check if the message received is not already received */
        size_t current_buffer_pos = (stream->size / stream->history) * (stream->last_handled % stream->history);
        if(is_input_buffer_empty(stream, current_buffer_pos))
        {
            /* Process the message */
            SeqNum next = seq_num_add(stream->last_handled, 1);
            if(seq_num == next) //TODO (fragment): ... && is not fragment (except last)
            {
                stream->last_handled = next;
                result = true;
            }
            else
            {
                memcpy(stream->buffer + current_buffer_pos, buffer, length);
            }
        }
    }

    return result;
}

bool next_input_reliable_buffer_available(InputReliableStream* stream, MicroBuffer* mb)
{
    SeqNum next = seq_num_add(stream->last_handled, 1);
    size_t max_message_size = stream->size / stream->history;
    size_t current_buffer_pos = (stream->last_handled % stream->history) * max_message_size;
    bool available_to_read = !is_input_buffer_empty(stream, current_buffer_pos);
    if(available_to_read)
    {
        stream->last_handled = next;
        init_micro_buffer(mb, stream->buffer, current_buffer_pos);
        set_input_buffer_empty(stream, current_buffer_pos);
    }

    return available_to_read;
}


bool input_reliable_stream_must_confirm(InputReliableStream* stream)
{
    bool result = stream->must_confirm;
    stream->must_confirm = false;
    return result;
}

void write_acknack(InputReliableStream* stream, MicroBuffer* mb)
{
    uint16_t nack_bitmap = compute_nack_bitmap(stream);

    ACKNACK_Payload payload;
    payload.first_unacked_seq_num = seq_num_add(stream->last_handled, 1);
    payload.nack_bitmap[0] = nack_bitmap >> 8;
    payload.nack_bitmap[1] = (nack_bitmap << 8) >> 8;

    (void) write_submessage_header(mb, SUBMESSAGE_ID_ACKNACK, ACKNACK_PAYLOAD_SIZE, 0);
    (void) serialize_ACKNACK_Payload(mb, &payload);
    (void) stream; (void) mb;
}

void read_heartbeat(InputReliableStream* stream, MicroBuffer* payload)
{
    HEARTBEAT_Payload heartbeat;
    deserialize_HEARTBEAT_Payload(payload, &heartbeat);

    process_heartbeat(stream, heartbeat.first_unacked_seq_nr, heartbeat.last_unacked_seq_nr);
}

//==================================================================
//                             PRIVATE
//==================================================================
void process_heartbeat(InputReliableStream* stream, uint16_t first_seq_num, uint16_t last_seq_num)
{
    if(0 > seq_num_cmp(seq_num_add(stream->last_handled, 1), first_seq_num))
    {
        stream->last_handled = seq_num_sub(first_seq_num, 1);
    }

    if(0 > seq_num_cmp(stream->last_announced, last_seq_num))
    {
        stream->last_announced = last_seq_num;
    }

    stream->must_confirm = true;
}

uint16_t compute_nack_bitmap(InputReliableStream* stream)
{
    uint16_t nack_bitmap = 0;
    uint16_t messages_to_process = seq_num_sub(stream->last_announced, stream->last_handled);
    size_t max_message_size = stream->size / stream->history;
    int history_buffers = (NACK_BITMAP_SIZE < messages_to_process) ? NACK_BITMAP_SIZE : messages_to_process;
    for(int i = 0; i < history_buffers; i++)
    {
        uint16_t current_pos = seq_num_add(stream->last_handled, i + 1);
        size_t current_buffer_pos = (current_pos % stream->history) * max_message_size;
        if(is_input_buffer_empty(stream, current_buffer_pos))
        {
            nack_bitmap |= 1 << i;
        }
    }

    return nack_bitmap;
}

bool is_buffer_empty(InputReliableStream* stream, size_t position)
{
    return 0 == stream->buffer[position + 1];
}

void set_buffer_empty(InputReliableStream* stream, size_t position)
{
    stream->buffer[position + 1] = 0;
}
