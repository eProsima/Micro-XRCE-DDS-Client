#include <micrortps/client/session/output_reliable_stream.h>
#include <microcdr/microcdr.h>

void init_output_reliable_stream(OutputReliableStream* stream, uint8_t* buffer, size_t size, size_t history, uint8_t offset)
{
    stream->buffer = buffer;
    stream->writer = offset;
    stream->size = size;
    stream->history = history;
    stream->offset = offset;

    stream->last_written = UINT16_MAX;
    stream->last_sent = UINT16_MAX;
    stream->last_acknown = UINT16_MAX;

    stream->nack_bitmap = 0;
    stream->next_heartbeat_time_stamp = 0;
}

bool prepare_next_reliable_buffer_to_send(OutputReliableStream* stream, uint8_t** buffer, size_t* length, uint16_t* seq_num)
{
    bool data_to_send = (0 >= seq_num_cmp(stream->last_sent, stream->last_written));
    if(data_to_send)
    {
        stream->last_sent = seq_num_add(stream->last_sent, 1);

        *seq_num = stream->last_sent;
        *buffer = stream->buffer + (stream->size / stream->history) * (stream->last_sent % stream->history);
        *length = stream->writer;

        stream->writer = stream->offset;
    }

    return data_to_send;
}

bool prepare_reliable_stream_to_write(OutputReliableStream* stream, int size, MicroBuffer* mb)
{
    bool available_to_write = stream->writer + size <= (stream->size / stream->history);
    if(available_to_write)
    {
        size_t current_buffer_pos = (stream->size / stream->history) * (stream->last_sent % stream->history);
        init_micro_buffer_offset(mb, stream->buffer + current_buffer_pos, stream->writer + size, stream->writer);

        stream->last_written = seq_num_add(stream->last_written, 1);
    }

    return available_to_write;
}

bool output_reliable_stream_must_notify(OutputReliableStream* stream, uint32_t current_timestamp)
{
    //TODO
    (void) stream; (void) current_timestamp;
    return false;
}

bool output_reliable_stream_must_send(OutputReliableStream* stream, uint8_t** buffer, size_t *length)
{
    //TODO
    (void) stream; (void) buffer; (void) length;
    return false;
}

void process_acknack(OutputReliableStream* stream, uint16_t bitmap, uint16_t first_unacked_seq_num)
{
    //TODO
    (void) stream; (void) bitmap; (void) first_unacked_seq_num;
}

int write_heartbeat(OutputReliableStream* stream, MicroBuffer* mb)
{
    //TODO
    (void) stream; (void) mb;
    return 0;
}
