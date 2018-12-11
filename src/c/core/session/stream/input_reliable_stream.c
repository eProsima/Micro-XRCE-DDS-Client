#include "seq_num_internal.h"
#include "input_reliable_stream_internal.h"
#include <ucdr/microcdr.h>

#include <string.h>

#define INTERNAL_BUFFER_OFFSET  sizeof(size_t)

//static bool on_full_buffer(ucdrBuffer* ub, void* args);

static size_t get_input_buffer_length(uint8_t* buffer);
static void set_input_buffer_length(uint8_t* buffer, size_t length);
static uint8_t* get_input_buffer(const uxrInputReliableStream* stream, size_t history_pos);
static size_t get_input_buffer_size(const uxrInputReliableStream* stream);

//==================================================================
//                             PUBLIC
//==================================================================
void uxr_init_input_reliable_stream(uxrInputReliableStream* stream, uint8_t* buffer, size_t size, uint16_t history)
{
    // assert for history (must be 2^)
    stream->buffer = buffer;
    stream->size = size;
    stream->history = history;

    uxr_reset_input_reliable_stream(stream);
}

void uxr_reset_input_reliable_stream(uxrInputReliableStream* stream)
{
    for(size_t i = 0; i < stream->history; i++)
    {
        uint8_t* internal_buffer = get_input_buffer(stream, i);
        set_input_buffer_length(internal_buffer, 0);
    }

    stream->last_handled = UINT16_MAX;
    stream->last_announced = UINT16_MAX;
}

bool uxr_receive_reliable_message(uxrInputReliableStream* stream, uint16_t seq_num, uint8_t* buffer, size_t length, bool* message_stored)
{
    //check if all fragment submessages are currently received.
    bool ready_to_read = false;

    /* Check if the seq_num is valid for the stream state */
    uxrSeqNum last_history = uxr_seq_num_add(stream->last_handled, stream->history);
    if(0 > uxr_seq_num_cmp(stream->last_handled, seq_num) && 0 <= uxr_seq_num_cmp(last_history, seq_num))
    {
        /* Process the message */
        uxrSeqNum next = uxr_seq_num_add(stream->last_handled, 1);
        FragmentationInfo fragmentation_info = stream->on_get_fragmentation_info(buffer, stream->get_fragmentation_info_args);

        if(NO_FRAGMENTED == fragmentation_info && seq_num == next)
        {
            stream->last_handled = next;
            if(0 > uxr_seq_num_cmp(stream->last_announced, stream->last_handled))
            {
                stream->last_announced = stream->last_handled;
            }
            ready_to_read = true;
            *message_stored = false;
        }
        else
        {
            /* Check if the message received is not already received */
            uint8_t* internal_buffer = get_input_buffer(stream, seq_num % stream->history);
            if(0 == get_input_buffer_length(internal_buffer))
            {
                memcpy(internal_buffer, buffer, length);
                set_input_buffer_length(internal_buffer, length);

                if(FINAL_FRAGMENT == fragmentation_info)
                {
                    if(/* this unlock a fragmentation message*/)
                    {
                        ready_to_read = true;
                        *message_stored = true;
                    }
                }
                if(INTERMEDIATE_FRAGMENT == fragmentation_info)
                {
                    if(/* this unlock a fragmentation message */)
                    {
                        ready_to_read = true;
                        *message_stored = true;
                    }
                }
            }
        }
    }

    if(0 > uxr_seq_num_cmp(stream->last_announced, seq_num))
    {
        stream->last_announced = seq_num;
    }

    return ready_to_read;
}

/*bool on_full_buffer(ucdrBuffer* ub, void* args)
{
    uxrInputReliableStream* stream = (uxrInputReliableStream*) args;
    //check the ub->final position, and give the next one if it is possible.
    //check if the current serialization is fragmentable?
    (void) stream; (void) ub;
    return true;
}*/

bool uxr_next_input_reliable_buffer_available(uxrInputReliableStream* stream, ucdrBuffer* ub)
{
    uxrSeqNum next = uxr_seq_num_add(stream->last_handled, 1);
    uint8_t* internal_buffer = get_input_buffer(stream, next % stream->history);
    size_t length = get_input_buffer_length(internal_buffer);
    bool available_to_read = 0 != length;
    if(available_to_read)
    {
        FragmentationInfo fragmentation_info = stream->on_get_fragmentation_info(stream->get_fragmentation_info_args, ub->init);
        if(NO_FRAGMENTED == fragmentation_info)
        {
            stream->last_handled = next;
            ucdr_init_buffer(ub, internal_buffer, (uint32_t)length);
            set_input_buffer_length(internal_buffer, 0);
        }
        else
        {
            //search deep
                set_input_buffer_length(internal_buffer, 0);
            stream->last_handled = deep;
            ucdr_init_buffer(ub, internal_buffer, (uint32_t)length);
            //set on_full_buffer
        }
    }


    return available_to_read;
}

void uxr_process_heartbeat(uxrInputReliableStream* stream, uint16_t first_seq_num, uint16_t last_seq_num)
{
    if(0 > uxr_seq_num_cmp(uxr_seq_num_add(stream->last_handled, 1), first_seq_num))
    {
        stream->last_handled = uxr_seq_num_sub(first_seq_num, 1);
    }

    if(0 > uxr_seq_num_cmp(stream->last_announced, last_seq_num))
    {
        stream->last_announced = last_seq_num;
    }
}

bool uxr_is_input_reliable_stream_busy(uxrInputReliableStream* stream)
{
    return stream->last_announced != stream->last_handled;
}

uint16_t uxr_compute_nack_bitmap(const uxrInputReliableStream* stream)
{
    uint16_t buffers_to_ack = uxr_seq_num_sub(stream->last_announced, stream->last_handled);
    uint16_t nack_bitmap = (buffers_to_ack > 0) ? 1 : 0;

    for(unsigned i = 0; i < (unsigned)buffers_to_ack; i++)
    {
        uxrSeqNum seq_num = uxr_seq_num_add(stream->last_handled, (uint16_t)(i + 1));
        uint8_t* internal_buffer = get_input_buffer(stream, seq_num % stream->history);
        if(0 == get_input_buffer_length(internal_buffer))
        {
            nack_bitmap = (uint16_t)(nack_bitmap | (1 << i));
        }
    }

    return nack_bitmap;
}
//==================================================================
//                             PRIVATE
//==================================================================
inline size_t get_input_buffer_length(uint8_t* buffer)
{
    return (size_t)*(buffer - INTERNAL_BUFFER_OFFSET);
}

inline void set_input_buffer_length(uint8_t* buffer, size_t length)
{
    memcpy(buffer - INTERNAL_BUFFER_OFFSET, &length, sizeof(size_t));
}

inline uint8_t* get_input_buffer(const uxrInputReliableStream* stream, size_t history_pos)
{
    return stream->buffer + history_pos * (stream->size / stream->history) + INTERNAL_BUFFER_OFFSET;
}

inline size_t get_input_buffer_size(const uxrInputReliableStream* stream)
{
    return (stream->size / stream->history) - INTERNAL_BUFFER_OFFSET;
}
