#include <uxr/client/config.h>
#include <string.h>

#include "seq_num_internal.h"
#include "output_reliable_stream_internal.h"
#include "../../serialization/xrce_protocol_internal.h"

#define MIN_HEARTBEAT_TIME_INTERVAL ((int64_t) UXR_CONFIG_MIN_HEARTBEAT_TIME_INTERVAL) // ms

#define MAX_HEARTBEAT_TRIES (sizeof(int64_t) * 8 - 1)
#define INTERNAL_BUFFER_OFFSET  sizeof(size_t)

static void process_acknack(uxrOutputReliableStream* stream, uint16_t bitmap, uint16_t last_acked_seq_num);
static bool on_full_output_buffer(ucdrBuffer* ub, void* args);

//==================================================================
//                             PUBLIC
//==================================================================
void uxr_init_output_reliable_stream(uxrOutputReliableStream* stream, uint8_t* buffer, size_t size, uint16_t history, uint8_t header_offset, OnNewFragment on_new_fragment)
{
    // assert for history (must be 2^)

    stream->buffer = buffer;
    stream->size = size;
    stream->offset = header_offset;
    stream->history = history;
    stream->on_new_fragment = on_new_fragment;

    uxr_reset_output_reliable_stream(stream);
}

void uxr_reset_output_reliable_stream(uxrOutputReliableStream* stream)
{
    for(size_t i = 0; i < stream->history; i++)
    {
        uint8_t* internal_buffer = uxr_get_output_buffer(stream, i);
        uxr_set_output_buffer_length(internal_buffer, stream->offset);
    }

    stream->last_written = 0;
    stream->last_sent = UINT16_MAX;
    stream->last_acknown = UINT16_MAX;

    stream->next_heartbeat_timestamp = INT64_MAX;
    stream->next_heartbeat_tries = 0;
    stream->send_lost = false;
}

bool uxr_prepare_reliable_buffer_to_write(uxrOutputReliableStream* stream, size_t size, size_t fragment_offset, ucdrBuffer* ub)
{
    bool available_to_write = false;

    uint8_t* internal_buffer = uxr_get_output_buffer(stream, stream->last_written % stream->history);
    size_t length = uxr_get_output_buffer_length(internal_buffer);

    /* Check if the message fit in the current buffer */
    if(length + size <= uxr_get_output_buffer_size(stream))
    {
        /* Check if there is space in the stream history to write */
        uxrSeqNum last_available = uxr_seq_num_add(stream->last_acknown, stream->history);
        available_to_write = 0 >= uxr_seq_num_cmp(stream->last_written, last_available);
        if(available_to_write)
        {
            size_t future_length = length + size;
            uxr_set_output_buffer_length(internal_buffer, future_length);
            ucdr_init_buffer_offset(ub, internal_buffer, (uint32_t)future_length, (uint32_t)length);
        }
    }
    /* Check if the message fit in a new empty buffer */
    else if(stream->offset + size <= uxr_get_output_buffer_size(stream))
    {
        /* Check if there is space in the stream history to write */
        uxrSeqNum next = uxr_seq_num_add(stream->last_written, 1);
        uxrSeqNum last_available = uxr_seq_num_add(stream->last_acknown, stream->history);
        available_to_write = 0 >= uxr_seq_num_cmp(next, last_available);
        if(available_to_write)
        {
            stream->last_written = next;
            uint8_t* buffer = uxr_get_output_buffer(stream, next % stream->history);
            uxr_set_output_buffer_length(buffer, size);
            ucdr_init_buffer(ub, buffer, (uint32_t)size);
        }
    }
    /* Check if the message fit in a fragmented message */
    else
    {
        size_t block_size = uxr_get_output_buffer_size(stream);
        size_t available_block_size = block_size - fragment_offset;
        size_t remaining_size = size - length;
        size_t necesary_blocks = remaining_size / available_block_size;
        size_t remaining_blocks = uxr_seq_num_sub(stream->last_acknown, uxr_seq_num_add(stream->last_written, 1));
        available_to_write = necesary_blocks <= remaining_blocks;
        if(available_to_write)
        {
            uxrSeqNum initial = (length == block_size) ? uxr_seq_num_add(stream->last_written, 1) : stream->last_written;
            uxrSeqNum next = initial;

            size_t blocks = necesary_blocks + remaining_size % available_block_size;
            for(size_t i = 0; i < blocks - 1; i++)
            {
                next = uxr_seq_num_add(next, 1);
                uint8_t* buffer = uxr_get_output_buffer(stream, next % stream->history);
                uxr_set_output_buffer_length(buffer, block_size);
            }

            uxrSeqNum final = uxr_seq_num_add(next, 1);
            uint8_t* final_buffer = uxr_get_output_buffer(stream, final % stream->history);
            size_t final_size = size - (available_block_size - length) % available_to_write;
            uxr_set_output_buffer_length(final_buffer, final_size);

            uint8_t* initial_buffer = uxr_get_output_buffer(stream, initial % stream->history);
            size_t initial_length = uxr_get_output_buffer_length(initial_buffer);
            ucdr_init_buffer_offset(ub, initial_buffer, (uint32_t)block_size, (uint32_t)initial_length);
            ucdr_set_on_full_buffer_callback(ub, on_full_output_buffer, stream);
            stream->on_new_fragment(ub, stream);
        }
    }

    return available_to_write;
}

bool on_full_output_buffer(ucdrBuffer* ub, void* args)
{
    uxrOutputReliableStream* stream = (uxrOutputReliableStream*) args;

    ub->init += (stream->size / stream->history);
    ub->iterator = ub->init;
    ub->final = ub->init + uxr_get_output_buffer_length(ub->init);

    stream->on_new_fragment(ub, stream);

    return false;
}

bool uxr_prepare_next_reliable_buffer_to_send(uxrOutputReliableStream* stream, uint8_t** buffer, size_t* length, uxrSeqNum* seq_num)
{
    *seq_num = uxr_seq_num_add(stream->last_sent, 1);
    *buffer = uxr_get_output_buffer(stream, *seq_num % stream->history);
    *length = uxr_get_output_buffer_length(*buffer);

    bool data_to_send = 0 >= uxr_seq_num_cmp(*seq_num, stream->last_written)
                        && *length > stream->offset
                        && uxr_seq_num_sub(stream->last_sent, stream->last_acknown) != stream->history;
    if(data_to_send)
    {
        stream->last_sent = *seq_num;
        if(stream->last_sent == stream->last_written)
        {
            stream->last_written = uxr_seq_num_add(stream->last_written, 1);
        }
    }

    return data_to_send;
}

bool uxr_update_output_stream_heartbeat_timestamp(uxrOutputReliableStream* stream, int64_t current_timestamp)
{
    bool must_confirm = false;
    if(0 > uxr_seq_num_cmp(stream->last_acknown, stream->last_sent))
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

uxrSeqNum uxr_begin_output_nack_buffer_it(const uxrOutputReliableStream* stream)
{
    return stream->last_acknown;
}

bool uxr_next_reliable_nack_buffer_to_send(uxrOutputReliableStream* stream, uint8_t** buffer, size_t *length, uxrSeqNum* seq_num_it)
{
    bool it_updated = false;
    if(stream->send_lost)
    {
        bool check_next_buffer = true;
        while(check_next_buffer && !it_updated)
        {
            *seq_num_it = uxr_seq_num_add(*seq_num_it, 1);
            check_next_buffer = 0 >= uxr_seq_num_cmp(*seq_num_it, stream->last_sent);
            if(check_next_buffer)
            {
                *buffer = uxr_get_output_buffer(stream, *seq_num_it % stream->history);
                *length = uxr_get_output_buffer_length(*buffer);
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

void uxr_buffer_heartbeat(const uxrOutputReliableStream* stream, ucdrBuffer* ub)
{
    HEARTBEAT_Payload payload;
    payload.first_unacked_seq_nr = uxr_seq_num_add(stream->last_acknown, 1);
    payload.last_unacked_seq_nr = stream->last_sent;

    (void) uxr_serialize_HEARTBEAT_Payload(ub, &payload);
}

void uxr_read_acknack(uxrOutputReliableStream* stream, ucdrBuffer* payload)
{
    ACKNACK_Payload acknack;
    uxr_deserialize_ACKNACK_Payload(payload, &acknack);

    uint16_t nack_bitmap = (uint16_t)(((uint16_t)acknack.nack_bitmap[0] << 8) + (uint16_t)acknack.nack_bitmap[1]);

    process_acknack(stream, nack_bitmap, (uint16_t)uxr_seq_num_sub(acknack.first_unacked_seq_num, 1));
}

bool uxr_is_output_reliable_stream_busy(const uxrOutputReliableStream* stream)
{
    return 0 > uxr_seq_num_cmp(stream->last_acknown, stream->last_sent);
}

//==================================================================
//                             PUBLIC
//==================================================================
void process_acknack(uxrOutputReliableStream* stream, uint16_t bitmap, uint16_t last_acked_seq_num)
{
    size_t buffers_to_clean = uxr_seq_num_sub(last_acked_seq_num, stream->last_acknown);
    for(size_t i = 0; i < buffers_to_clean; i++)
    {
        stream->last_acknown = uxr_seq_num_add(stream->last_acknown, 1);
        uint8_t* internal_buffer = uxr_get_output_buffer(stream, stream->last_acknown % stream->history);
        uxr_set_output_buffer_length(internal_buffer, stream->offset); /* clear buffer */
    }

    stream->send_lost = (0 < bitmap);

    /* reset heartbeat interval */
    stream->next_heartbeat_tries = 0;
}

inline size_t uxr_get_output_buffer_length(uint8_t* buffer)
{
    size_t length;
    memcpy(&length, buffer - INTERNAL_BUFFER_OFFSET, sizeof(size_t));
    return length;
}

inline void uxr_set_output_buffer_length(uint8_t* buffer, size_t length)
{
    memcpy(buffer - INTERNAL_BUFFER_OFFSET, &length, sizeof(size_t));
}

inline uint8_t* uxr_get_output_buffer(const uxrOutputReliableStream* stream, size_t history_pos)
{
    return stream->buffer + history_pos * (stream->size / stream->history) + INTERNAL_BUFFER_OFFSET;
}

inline size_t uxr_get_output_buffer_size(const uxrOutputReliableStream* stream)
{
    return (stream->size / stream->history) - INTERNAL_BUFFER_OFFSET;
}
