#include "seq_num_internal.h"
#include "input_reliable_stream_internal.h"
#include "common_reliable_stream_internal.h"
#include <ucdr/microcdr.h>

#include <string.h>

static bool has_all_fragments(uxrInputReliableStream* stream, uxrSeqNum seq_num_of_fragment, size_t* fragments);
static bool on_full_input_buffer(ucdrBuffer* ub, void* args);

//==================================================================
//                             PUBLIC
//==================================================================
void uxr_init_input_reliable_stream(uxrInputReliableStream* stream, uint8_t* buffer, size_t size, uint16_t history, OnGetFragmentationInfo on_get_fragmentation_info)
{
    // assert for history (must be 2^)
    stream->buffer = buffer;
    stream->size = size;
    stream->history = history;
    stream->on_get_fragmentation_info = on_get_fragmentation_info;

    uxr_reset_input_reliable_stream(stream);
}

void uxr_reset_input_reliable_stream(uxrInputReliableStream* stream)
{
    for(size_t i = 0; i < stream->history; i++)
    {
        uint8_t* internal_buffer = uxr_get_input_buffer(stream, i);
        uxr_set_reliable_buffer_length(internal_buffer, 0);
    }

    stream->last_handled = UINT16_MAX;
    stream->last_announced = UINT16_MAX;
}

bool uxr_receive_reliable_message(uxrInputReliableStream* stream, uint16_t seq_num, uint8_t* buffer, size_t length, bool* message_stored)
{
    bool ready_to_read = false;

    /* Check if the seq_num is valid for the stream state */
    uxrSeqNum last_history = uxr_seq_num_add(stream->last_handled, stream->history);
    if(0 > uxr_seq_num_cmp(stream->last_handled, seq_num) && 0 <= uxr_seq_num_cmp(last_history, seq_num))
    {
        /* Process the message */
        FragmentationInfo fragmentation_info = stream->on_get_fragmentation_info(buffer);
        uxrSeqNum next = uxr_seq_num_add(stream->last_handled, 1);

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
            uint8_t* internal_buffer = uxr_get_input_buffer(stream, seq_num % stream->history);
            if(0 == uxr_get_reliable_buffer_length(internal_buffer))
            {
                memcpy(internal_buffer, buffer, length);
                uxr_set_reliable_buffer_length(internal_buffer, length);

                if(NO_FRAGMENTED != fragmentation_info)
                {
                    size_t fragments;
                    if(has_all_fragments(stream, seq_num, &fragments))
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

bool uxr_next_input_reliable_buffer_available(uxrInputReliableStream* stream, ucdrBuffer* ub, size_t fragment_offset)
{
    uxrSeqNum next = uxr_seq_num_add(stream->last_handled, 1);
    uint8_t* internal_buffer = uxr_get_input_buffer(stream, next % stream->history);
    size_t length = uxr_get_reliable_buffer_length(internal_buffer);
    bool available_to_read = 0 != length;
    if(available_to_read)
    {
        FragmentationInfo fragmentation_info = stream->on_get_fragmentation_info(ub->init);
        if(NO_FRAGMENTED == fragmentation_info)
        {
            ucdr_init_buffer(ub, internal_buffer, (uint32_t)length);
            uxr_set_reliable_buffer_length(internal_buffer, 0);
            stream->last_handled = next;
        }
        else
        {
            size_t fragments;
            (void) has_all_fragments(stream, next, &fragments);
            uxr_set_reliable_buffer_length(internal_buffer, 0);
            ucdr_init_buffer(ub, internal_buffer + fragment_offset, (uint32_t)length);
            ucdr_set_on_full_buffer_callback(ub, on_full_input_buffer, stream);
            stream->last_handled = uxr_seq_num_add(stream->last_handled, (uint16_t)fragments);
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
        uint8_t* internal_buffer = uxr_get_input_buffer(stream, seq_num % stream->history);
        if(0 == uxr_get_reliable_buffer_length(internal_buffer))
        {
            nack_bitmap = (uint16_t)(nack_bitmap | (1 << i));
        }
    }

    return nack_bitmap;
}

uint8_t* uxr_get_input_buffer(const uxrInputReliableStream* stream, size_t history_pos)
{
    return uxr_get_reliable_buffer(stream->buffer, stream->size, stream->history, history_pos);
}

size_t uxr_get_input_buffer_size(const uxrInputReliableStream* stream)
{
    return uxr_get_reliable_buffer_size(stream->size, stream->history);
}

//==================================================================
//                             PRIVATE
//==================================================================
bool has_all_fragments(uxrInputReliableStream* stream, uxrSeqNum seq_num_of_fragment, size_t* fragments)
{
    uxrSeqNum next = seq_num_of_fragment;
    uxrSeqNum previous = uxr_seq_num_sub(seq_num_of_fragment, 1);

    bool exists_message = true;
    bool intermediate_fragment_found = true;
    while(exists_message && intermediate_fragment_found && 0 < uxr_seq_num_cmp(previous, stream->last_handled))
    {
        uint8_t* previous_buffer = uxr_get_input_buffer(stream, previous % stream->history);
        exists_message = 0 != uxr_get_reliable_buffer_length(previous_buffer);
        if(exists_message)
        {
            FragmentationInfo previous_fragmentation_info = stream->on_get_fragmentation_info(previous_buffer);
            intermediate_fragment_found = INTERMEDIATE_FRAGMENT == previous_fragmentation_info;
            if(intermediate_fragment_found)
            {
                (*fragments)++;
                previous = uxr_seq_num_sub(previous, 1);
            }
        }
    }

    if(!intermediate_fragment_found) // exists_message is true if intermediate_fragment_found is false
    {
        intermediate_fragment_found = true;
        while(exists_message && intermediate_fragment_found && 0 >= uxr_seq_num_cmp(next, stream->last_announced))
        {
            uint8_t* next_buffer = uxr_get_input_buffer(stream, next % stream->history);
            exists_message = 0 != uxr_get_reliable_buffer_length(next_buffer);
            if(exists_message)
            {
                FragmentationInfo next_fragmentation_info = stream->on_get_fragmentation_info(next_buffer);
                intermediate_fragment_found = INTERMEDIATE_FRAGMENT == next_fragmentation_info;
                (*fragments)++;
                next = uxr_seq_num_add(next, 1);
            }
        }
    }

    return !intermediate_fragment_found;
}

bool on_full_input_buffer(ucdrBuffer* ub, void* args)
{
    uxrInputReliableStream* stream = (uxrInputReliableStream*) args;

    uint8_t* buffer = ub->init + stream->size / stream->history;
    uint32_t size = (uint32_t)uxr_get_input_buffer_size(stream);
    ucdr_init_buffer(ub, buffer, size);
    ucdr_set_on_full_buffer_callback(ub, on_full_input_buffer, stream);

    return false;
}

