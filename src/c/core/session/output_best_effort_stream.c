#include <micrortps/client/core/session/output_best_effort_stream.h>
#include <microcdr/microcdr.h>

//==================================================================
//                              PUBLIC
//==================================================================
void init_output_best_effort_stream(OutputBestEffortStream* stream, uint8_t* buffer, size_t size, uint8_t offset)
{
    stream->buffer = buffer;
    stream->writer = offset;
    stream->size = size;
    stream->offset = offset;

    stream->last_send = UINT16_MAX;
}

bool prepare_best_effort_buffer_to_write(OutputBestEffortStream* stream, size_t size, MicroBuffer* mb)
{
    bool available_to_write = stream->writer + size <= stream->size;
    if(available_to_write)
    {
        init_micro_buffer_offset(mb, stream->buffer, stream->writer + size, stream->writer);
        stream->writer += size;
    }

    return available_to_write;
}

bool prepare_best_effort_buffer_to_send(OutputBestEffortStream* stream, uint8_t** buffer, size_t* length, uint16_t* seq_num)
{
    bool data_to_send = stream->writer > stream->offset;
    if(data_to_send)
    {
        stream->last_send = seq_num_add(stream->last_send, 1);

        *seq_num = stream->last_send;
        *buffer = stream->buffer;
        *length = stream->writer;

        stream->writer = stream->offset;
    }

    return data_to_send;
}

