#include <micrortps/client/core/session/input_reliable_stream.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <string.h>

// Remove when Microcdr supports size_of functions
#define ACKNACK_PAYLOAD_SIZE     4

#define INTERNAL_BUFFER_OFFSET  sizeof(size_t)

void process_heartbeat(InputReliableStream* stream, uint16_t first_seq_num, uint16_t last_seq_num);
uint16_t compute_nack_bitmap(const InputReliableStream* stream);

size_t get_input_buffer_length(uint8_t* buffer);
void set_input_buffer_length(uint8_t* buffer, size_t length);
uint8_t* get_input_buffer(const InputReliableStream* stream, size_t history_pos);
size_t get_input_buffer_size(const InputReliableStream* stream);

//==================================================================
//                             PUBLIC
//==================================================================
void init_input_reliable_stream(InputReliableStream* stream, uint8_t* buffer, size_t size, size_t history)
{
    // assert for history (must be 2^)
    stream->buffer = buffer;
    stream->size = size;
    stream->history = history;

    for(size_t i = 0; i < stream->history; i++)
    {
        uint8_t* internal_buffer = get_input_buffer(stream, i);
        set_input_buffer_length(internal_buffer, 0);
    }

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
        uint8_t* internal_buffer = get_input_buffer(stream, stream->last_handled % stream->history);
        if(0 == get_input_buffer_length(internal_buffer))
        {
            /* Process the message */
            SeqNum next = seq_num_add(stream->last_handled, 1);
            if(seq_num == next) //TODO (fragment): ... && is not fragment (except last fragment)
            {
                stream->last_handled = next;
                result = true;
            }
            else
            {
                memcpy(internal_buffer, buffer, length);
            }
        }
    }

    return result;
}


bool next_input_reliable_buffer_available(InputReliableStream* stream, MicroBuffer* mb)
{
    SeqNum next = seq_num_add(stream->last_handled, 1);
    uint8_t* internal_buffer = get_input_buffer(stream, next % stream->history);
    size_t length = get_input_buffer_length(internal_buffer);
    bool available_to_read = 0 != length;
    if(available_to_read)
    {
        stream->last_handled = next;
        init_micro_buffer(mb, internal_buffer, length);
        set_input_buffer_length(internal_buffer, 0);
    }

    return available_to_read;
}


bool input_reliable_stream_must_confirm(InputReliableStream* stream)
{
    bool result = stream->must_confirm;
    stream->must_confirm = false;
    return result;
}

void write_acknack(const InputReliableStream* stream, MicroBuffer* mb)
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

void read_submessage_heartbeat(InputReliableStream* stream, MicroBuffer* payload)
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

uint16_t compute_nack_bitmap(const InputReliableStream* stream)
{
    uint16_t nack_bitmap = 0;
    size_t buffers_to_ack = seq_num_sub(stream->last_announced, stream->last_handled);

    for(size_t i = 0; i < buffers_to_ack; i++)
    {
        SeqNum seq_num = seq_num_add(stream->last_handled, i + 1);
        uint8_t* internal_buffer = get_input_buffer(stream, seq_num % stream->history);
        if(0 == get_input_buffer_length(internal_buffer))
        {
            nack_bitmap |= 1 << i;
        }
    }

    return nack_bitmap;
}

size_t get_input_buffer_length(uint8_t* buffer)
{
    return (size_t)*(buffer - INTERNAL_BUFFER_OFFSET);
}

void set_input_buffer_length(uint8_t* buffer, size_t length)
{
    memcpy(buffer - INTERNAL_BUFFER_OFFSET, &length, sizeof(size_t));
}

uint8_t* get_input_buffer(const InputReliableStream* stream, size_t history_pos)
{
    return stream->buffer + history_pos * (stream->size / stream->history) + INTERNAL_BUFFER_OFFSET;
}

size_t get_input_buffer_size(const InputReliableStream* stream)
{
    return (stream->size / stream->history) - INTERNAL_BUFFER_OFFSET;
}
