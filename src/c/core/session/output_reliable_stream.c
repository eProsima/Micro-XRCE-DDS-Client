#include <micrortps/client/core/session/output_reliable_stream.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/config.h>
#include <string.h>

// Remove when Microcdr supports size_of functions
#define HEARTBEAT_PAYLOAD_SIZE 4
//---

#define MIN_HEARTBEAT_TIME_INTERVAL ((int64_t) MR_CONFIG_MIN_HEARTBEAT_TIME_INTERVAL) // ms

#define MAX_HEARTBEAT_TRIES (sizeof(int64_t) * 8 - 1)
#define INTERNAL_BUFFER_OFFSET  sizeof(size_t)

static void process_acknack(mrOutputReliableStream* stream, uint16_t bitmap, uint16_t last_acked_seq_num);

static size_t get_output_buffer_length(uint8_t* buffer);
static void set_output_buffer_length(uint8_t* buffer, size_t length);
static uint8_t* get_output_buffer(const mrOutputReliableStream* stream, size_t history_pos);
static size_t get_output_buffer_size(const mrOutputReliableStream* stream);

// Implementation note: the SUBHEADER_SIZE must be used to represent the header of the fragment.

//==================================================================
//                             PUBLIC
//==================================================================
void init_output_reliable_stream(mrOutputReliableStream* stream, uint8_t* buffer, size_t size, uint16_t history, uint8_t header_offset)
{
    // assert for history (must be 2^)

    stream->buffer = buffer;
    stream->size = size;
    stream->offset = header_offset;
    stream->history = history;

    reset_output_reliable_stream(stream);
}

void reset_output_reliable_stream(mrOutputReliableStream* stream)
{
    for(size_t i = 0; i < stream->history; i++)
    {
        uint8_t* internal_buffer = get_output_buffer(stream, i);
        set_output_buffer_length(internal_buffer, stream->offset);
    }

    stream->last_written = 0;
    stream->last_sent = UINT16_MAX;
    stream->last_acknown = UINT16_MAX;

    stream->next_heartbeat_timestamp = INT64_MAX;
    stream->next_heartbeat_tries = 0;
    stream->send_lost = false;
}

bool prepare_reliable_buffer_to_write(mrOutputReliableStream* stream, size_t size, MicroBuffer* mb)
{
    bool available_to_write = false;

    uint8_t* internal_buffer = get_output_buffer(stream, stream->last_written % stream->history);
    size_t length = get_output_buffer_length(internal_buffer);

    /* Check if the message fit in the current buffer */
    if(length + submessage_padding(length) + size <= get_output_buffer_size(stream))
    {
        /* Check if there is space in the stream history to write */
        mrSeqNum last_available = seq_num_add(stream->last_acknown, stream->history);
        available_to_write = 0 >= seq_num_cmp(stream->last_written, last_available);
    }
    /* Check if the message fit in a new empty buffer */
    else if(stream->offset + size <= get_output_buffer_size(stream))
    {
        /* Check if there is space in the stream history to write */
        mrSeqNum next = seq_num_add(stream->last_written, 1);
        mrSeqNum last_available = seq_num_add(stream->last_acknown, stream->history);
        available_to_write = 0 >= seq_num_cmp(next, last_available);
        if(available_to_write)
        {
            internal_buffer = get_output_buffer(stream, next % stream->history);
            stream->last_written = next;
        }
    }

    if(available_to_write)
    {
        size_t current_length = get_output_buffer_length(internal_buffer);
        size_t current_padding = (current_length % 4 != 0) ? 4 - (current_length % 4) : 0;
        size_t future_length = current_length + current_padding + size;
        set_output_buffer_length(internal_buffer, future_length);
        init_micro_buffer_offset(mb, internal_buffer, (uint32_t)future_length, (uint32_t)current_length);
    }

    return available_to_write;
}

bool prepare_next_reliable_buffer_to_send(mrOutputReliableStream* stream, uint8_t** buffer, size_t* length, mrSeqNum* seq_num)
{
    *seq_num = seq_num_add(stream->last_sent, 1);
    *buffer = get_output_buffer(stream, *seq_num % stream->history);
    *length = get_output_buffer_length(*buffer);

    bool data_to_send = 0 >= seq_num_cmp(*seq_num, stream->last_written)
                        && *length > stream->offset
                        && seq_num_sub(stream->last_sent, stream->last_acknown) != stream->history;
    if(data_to_send)
    {
        stream->last_sent = *seq_num;
        if(stream->last_sent == stream->last_written)
        {
            stream->last_written = seq_num_add(stream->last_written, 1);
        }
    }

    return data_to_send;
}

bool update_output_stream_heartbeat_timestamp(mrOutputReliableStream* stream, int64_t current_timestamp)
{
    bool must_confirm = false;
    if(0 > seq_num_cmp(stream->last_acknown, stream->last_sent))
    {
        if(0 == stream->next_heartbeat_tries)
        {
            stream->next_heartbeat_timestamp = current_timestamp + MIN_HEARTBEAT_TIME_INTERVAL;
            stream->next_heartbeat_tries = 1;
        }
        else if(current_timestamp >= stream->next_heartbeat_timestamp)
        {
            int64_t increment = MIN_HEARTBEAT_TIME_INTERVAL << (++stream->next_heartbeat_tries % MAX_HEARTBEAT_TRIES);
            int64_t difference = current_timestamp - stream->next_heartbeat_timestamp;
            stream->next_heartbeat_timestamp += (difference > increment) ? difference : increment;
            must_confirm = true;
        }
    }
    else
    {
        stream->next_heartbeat_timestamp = INT64_MAX;
    }

    return must_confirm;
}

mrSeqNum begin_output_nack_buffer_it(const mrOutputReliableStream* stream)
{
    return stream->last_acknown;
}

bool next_reliable_nack_buffer_to_send(mrOutputReliableStream* stream, uint8_t** buffer, size_t *length, mrSeqNum* seq_num_it)
{
    bool it_updated = false;
    if(stream->send_lost)
    {
        bool check_next_buffer = true;
        while(check_next_buffer && !it_updated)
        {
            *seq_num_it = seq_num_add(*seq_num_it, 1);
            check_next_buffer = 0 >= seq_num_cmp(*seq_num_it, stream->last_sent);
            if(check_next_buffer)
            {
                *buffer = get_output_buffer(stream, *seq_num_it % stream->history);
                *length = get_output_buffer_length(*buffer);
                it_updated = *length != stream->offset;
            }
        }

        if(!it_updated)
        {
            stream->send_lost = false;
        }
    }

    return it_updated;
}

void write_heartbeat(const mrOutputReliableStream* stream, MicroBuffer* mb)
{
    HEARTBEAT_Payload payload;
    payload.first_unacked_seq_nr = seq_num_add(stream->last_acknown, 1);
    payload.last_unacked_seq_nr = stream->last_sent;

    (void) write_submessage_header(mb, SUBMESSAGE_ID_HEARTBEAT, HEARTBEAT_PAYLOAD_SIZE, 0);
    (void) serialize_HEARTBEAT_Payload(mb, &payload);
}

void read_acknack(mrOutputReliableStream* stream, MicroBuffer* payload)
{
    ACKNACK_Payload acknack;
    deserialize_ACKNACK_Payload(payload, &acknack);

    uint16_t nack_bitmap = (uint16_t)(((uint16_t)acknack.nack_bitmap[0] << 8) + (uint16_t)acknack.nack_bitmap[1]);

    process_acknack(stream, nack_bitmap, (uint16_t)seq_num_sub(acknack.first_unacked_seq_num, 1));
}

bool is_output_reliable_stream_busy(const mrOutputReliableStream* stream)
{
    return 0 > seq_num_cmp(stream->last_acknown, stream->last_sent);
}

//==================================================================
//                             PUBLIC
//==================================================================
void process_acknack(mrOutputReliableStream* stream, uint16_t bitmap, uint16_t last_acked_seq_num)
{
    size_t buffers_to_clean = seq_num_sub(last_acked_seq_num, stream->last_acknown);
    for(size_t i = 0; i < buffers_to_clean; i++)
    {
        stream->last_acknown = seq_num_add(stream->last_acknown, 1);
        uint8_t* internal_buffer = get_output_buffer(stream, stream->last_acknown % stream->history);
        set_output_buffer_length(internal_buffer, stream->offset); /* clear buffer */
    }

    uint16_t buffers_to_check_clean = seq_num_sub(stream->last_sent, last_acked_seq_num);
    for(uint16_t i = 0; i < buffers_to_check_clean; i++)
    {
        if(bitmap & (1 << i)) /* message lost */
        {
            stream->send_lost = true;
        }
        else
        {
            mrSeqNum seq_num = seq_num_add(last_acked_seq_num, i);
            uint8_t* internal_buffer = get_output_buffer(stream, seq_num % stream->history);
            set_output_buffer_length(internal_buffer, stream->offset); /* clear buffer */
        }
    }

    /* reset heartbeat interval */
    stream->next_heartbeat_tries = 0;
}

inline size_t get_output_buffer_length(uint8_t* buffer)
{
    size_t length;
    memcpy(&length, buffer - INTERNAL_BUFFER_OFFSET, sizeof(size_t));
    return length;
}

inline void set_output_buffer_length(uint8_t* buffer, size_t length)
{
    memcpy(buffer - INTERNAL_BUFFER_OFFSET, &length, sizeof(size_t));
}

inline uint8_t* get_output_buffer(const mrOutputReliableStream* stream, size_t history_pos)
{
    return stream->buffer + history_pos * (stream->size / stream->history) + INTERNAL_BUFFER_OFFSET;
}

inline size_t get_output_buffer_size(const mrOutputReliableStream* stream)
{
    return (stream->size / stream->history) - INTERNAL_BUFFER_OFFSET;
}
