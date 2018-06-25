#include <micrortps/client/core/session/output_reliable_stream.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <string.h>

// Remove when Microcdr supports size_of functions
#define HEARTBEAT_PAYLOAD_SIZE 4

#define MIN_HEARTBEAT_TIME_INTERVAL_NS 1000000
#define INTERNAL_BUFFER_OFFSET  sizeof(size_t)

void process_acknack(OutputReliableStream* stream, uint16_t bitmap, uint16_t first_unacked_seq_num);

size_t get_output_buffer_length(uint8_t* buffer);
void set_output_buffer_length(uint8_t* buffer, size_t length);
uint8_t* get_output_buffer(const OutputReliableStream* stream, size_t history_pos);
size_t get_output_buffer_size(const OutputReliableStream* stream);

//==================================================================
//                             PUBLIC
//==================================================================
void init_output_reliable_stream(OutputReliableStream* stream, uint8_t* buffer, size_t size, size_t message_data_size, uint8_t header_offset)
{
    stream->buffer = buffer;
    stream->size = size;
    stream->offset = header_offset + SUBHEADER_SIZE;
    stream->history = size / (message_data_size + stream->offset + INTERNAL_BUFFER_OFFSET);

    for(size_t i = 0; i < stream->history; i++)
    {
        uint8_t* internal_buffer = get_output_buffer(stream, i);
        set_output_buffer_length(internal_buffer, stream->offset);
    }

    stream->last_written = 0;
    stream->last_sent = UINT16_MAX;
    stream->last_acknown = UINT16_MAX;

    stream->next_heartbeat_time_stamp = UINT32_MAX;
    stream->next_heartbeat_tries = 0;
}

bool prepare_reliable_buffer_to_write(OutputReliableStream* stream, size_t size, MicroBuffer* mb)
{
    /* Check if the message fit it the current buffer */
    uint8_t* internal_buffer = get_output_buffer(stream, stream->last_written % stream->history);
    bool available_to_write = false;
    if(get_output_buffer_length(internal_buffer) + size <= get_output_buffer_size(stream))
    {
        /* Check if there is space in the stream history to write */
        available_to_write = stream->last_written % stream->history != stream->last_acknown % stream->history;
        if(!available_to_write)
        {
            /* Check if the message fit in a new empty buffer */
            if(stream->offset + size <= get_output_buffer_size(stream))
            {
                /* Check if there is space in the stream history to write */
                SeqNum next = seq_num_add(stream->last_written, 1);
                available_to_write = next % stream->history != stream->last_acknown % stream->history;
                if(!available_to_write)
                {
                    internal_buffer = get_output_buffer(stream, next % stream->history);
                    stream->last_written = next;
                }
            }
        }
    }

    if(available_to_write)
    {
        size_t current_length = get_output_buffer_length(internal_buffer);
        size_t future_length = current_length + size;
        set_output_buffer_length(internal_buffer, future_length);
        init_micro_buffer_offset(mb, internal_buffer, future_length, current_length);
    }

    return available_to_write;
}

bool prepare_next_reliable_buffer_to_send(OutputReliableStream* stream, uint8_t** buffer, size_t* length, uint16_t* seq_num)
{
    bool data_to_send = (0 > seq_num_cmp(stream->last_sent, stream->last_written));
    if(data_to_send)
    {
        stream->last_sent = seq_num_add(stream->last_sent, 1);
        *seq_num = stream->last_sent;
        // When fragment, the SUBHEADER_SIZE must not be added
        *buffer = get_output_buffer(stream, stream->last_sent % stream->history) + SUBHEADER_SIZE;
        *length = get_output_buffer_length(*buffer);
        if(stream->last_sent == stream->last_written)
        {
            stream->last_written = seq_num_add(stream->last_written, 1);
        }
    }
    return data_to_send;
}


bool output_reliable_stream_must_notify(OutputReliableStream* stream, uint32_t current_timestamp)
{
    if(0 > seq_num_cmp(stream->last_acknown, stream->last_sent))
    {
        stream->next_heartbeat_time_stamp += MIN_HEARTBEAT_TIME_INTERVAL_NS * ++stream->next_heartbeat_tries;
    }
    else
    {
        stream->next_heartbeat_tries = 0;
        stream->next_heartbeat_time_stamp = UINT32_MAX;
    }

    return stream->next_heartbeat_time_stamp <= current_timestamp;
}

SeqNum begin_output_nack_buffer_it(const OutputReliableStream* stream)
{
    return stream->last_acknown;
}

bool next_reliable_nack_buffer_to_send(const OutputReliableStream* stream, uint8_t** buffer, size_t *length, SeqNum* seq_num_it)
{
    bool check_next_buffer = true;
    bool it_updated = false;
    while(check_next_buffer && !it_updated)
    {
        *seq_num_it = seq_num_add(*seq_num_it, 1);
        check_next_buffer = 0 >= seq_num_cmp(*seq_num_it, stream->last_sent);
        if(check_next_buffer)
        {
            // When fragment, the SUBHEADER_SIZE must not be added
            *buffer = get_output_buffer(stream, *seq_num_it % stream->history) + SUBHEADER_SIZE;
            *length = get_output_buffer_length(*buffer);
            it_updated = *length != stream->offset;
        }
    }

    return it_updated;
}

void write_heartbeat(const OutputReliableStream* stream, MicroBuffer* mb)
{
    HEARTBEAT_Payload payload;
    payload.first_unacked_seq_nr = seq_num_add(stream->last_acknown, 1);
    payload.last_unacked_seq_nr = stream->last_sent;

    (void) write_submessage_header(mb, SUBMESSAGE_ID_HEARTBEAT, HEARTBEAT_PAYLOAD_SIZE, 0);
    (void) serialize_HEARTBEAT_Payload(mb, &payload);
}

void read_submessage_acknack(OutputReliableStream* stream, MicroBuffer* payload)
{
    ACKNACK_Payload acknack;
    deserialize_ACKNACK_Payload(payload, &acknack);

    uint16_t nack_bitmap = ((uint16_t)acknack.nack_bitmap[0] << 8) + (uint16_t)acknack.nack_bitmap[1];

    process_acknack(stream, acknack.first_unacked_seq_num, nack_bitmap);
}

//==================================================================
//                             PUBLIC
//==================================================================
void process_acknack(OutputReliableStream* stream, uint16_t bitmap, uint16_t first_unacked_seq_num)
{
    size_t buffers_to_clean = seq_num_sub(first_unacked_seq_num, stream->last_acknown);
    for(size_t i = 0; i < buffers_to_clean; i++)
    {
        stream->last_acknown = seq_num_add(stream->last_acknown, 1);
        uint8_t* internal_buffer = get_output_buffer(stream, stream->last_acknown % stream->history);
        set_output_buffer_length(internal_buffer, stream->offset); /* clear buffer */
    }

    size_t buffers_to_check_clean = seq_num_sub(stream->last_sent, first_unacked_seq_num);
    for(size_t i = 0; i < buffers_to_check_clean; i++)
    {
        if(i & bitmap) /* message lost */
        {
            SeqNum seq_num = seq_num_add(first_unacked_seq_num, i);
            uint8_t* internal_buffer = get_output_buffer(stream, seq_num % stream->history);
            set_output_buffer_length(internal_buffer, stream->offset); /* clear buffer */
        }
    }
}

size_t get_output_buffer_length(uint8_t* buffer)
{
    return (size_t)*(buffer - INTERNAL_BUFFER_OFFSET);
}

void set_output_buffer_length(uint8_t* buffer, size_t length)
{
    memcpy(buffer - INTERNAL_BUFFER_OFFSET, &length, sizeof(size_t));
}

uint8_t* get_output_buffer(const OutputReliableStream* stream, size_t history_pos)
{
    return stream->buffer + history_pos * (stream->size / stream->history) + INTERNAL_BUFFER_OFFSET;
}

size_t get_output_buffer_size(const OutputReliableStream* stream)
{
    return (stream->size / stream->history) - INTERNAL_BUFFER_OFFSET;
}
