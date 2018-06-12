#include <micrortps/client/session/input_reliable_stream.h>
#include <microcdr/microcdr.h>
#include <string.h>

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
}

bool receive_reliable_message(InputReliableStream* stream, uint16_t seq_num, uint8_t* buffer, size_t length)
{
    bool result = false;
    SeqNum last_history = seq_num_add(stream->last_handled, stream->history);
    if(0 > seq_num_cmp(stream->last_handled, seq_num) || 0 <= seq_num_cmp(last_history, seq_num))
    {
        size_t current_buffer_pos = (stream->size / stream->history) * (stream->last_handled % stream->history);
        if(is_input_buffer_empty(stream, current_buffer_pos))
        {
            SeqNum next = seq_num_add(stream->last_handled, 1);
            if(seq_num == next) //TODO: and is not fragment (except last)
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
    size_t current_buffer_pos = (stream->size / stream->history) * (stream->last_handled % stream->history);
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
    //TODO
    (void) stream;
    return false;
}

int write_acknack(InputReliableStream* stream, MicroBuffer* mb)
{
    //TODO
    (void) stream; (void) mb;
    return 0;
}

void process_heartbeat(InputReliableStream* stream, uint16_t first_seq_num, uint16_t last_seq_num)
{
    //TODO
    (void) stream; (void) first_seq_num; (void)last_seq_num;
}

//==================================================================
//                             PRIVATE
//==================================================================
bool is_buffer_empty(InputReliableStream* stream, size_t position)
{
    return 0 == stream->buffer[position + 1];
}

void set_buffer_empty(InputReliableStream* stream, size_t position)
{
    stream->buffer[position + 1] = 0;
}
