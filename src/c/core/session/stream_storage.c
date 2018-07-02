#include <micrortps/client/core/session/stream_storage.h>

//==================================================================
//                             PUBLIC
//==================================================================
void init_stream_storage(StreamStorage* storage)
{
    storage->output_best_effort_size = 0;
    storage->output_reliable_size = 0;
    storage->input_best_effort_size = 0;
    storage->input_reliable_size = 0;
}

StreamId add_output_best_effort_buffer(StreamStorage* storage, uint8_t* buffer, size_t size, uint8_t header_offset)
{
    uint8_t index = storage->output_best_effort_size++;
    // assert for index
    OutputBestEffortStream* stream = &storage->output_best_effort[index];
    init_output_best_effort_stream(stream, buffer, size, header_offset);
    return create_stream_id(index, BEST_EFFORT_STREAM, OUTPUT_STREAM);
}

StreamId add_output_reliable_buffer(StreamStorage* storage, uint8_t* buffer, size_t size, size_t history, uint8_t header_offset)
{
    uint8_t index = storage->output_reliable_size++;
    // assert for index
    OutputReliableStream* stream = &storage->output_reliable[index];
    init_output_reliable_stream(stream, buffer, size, history, header_offset);
    return create_stream_id(index, RELIABLE_STREAM, OUTPUT_STREAM);
}

StreamId add_input_best_effort_buffer(StreamStorage* storage)
{
    uint8_t index = storage->input_best_effort_size++;
    // assert for index
    InputBestEffortStream* stream = &storage->input_best_effort[index];
    init_input_best_effort_stream(stream);
    return create_stream_id(index, BEST_EFFORT_STREAM, INPUT_STREAM);
}

StreamId add_input_reliable_buffer(StreamStorage* storage, uint8_t* buffer, size_t size, size_t history)
{
    uint8_t index = storage->input_reliable_size++;
    // assert for index
    InputReliableStream* stream = &storage->input_reliable[index];
    init_input_reliable_stream(stream, buffer, size, history);
    return create_stream_id(index, RELIABLE_STREAM, INPUT_STREAM);
}

OutputBestEffortStream* get_output_best_effort_stream(StreamStorage* storage, uint8_t index)
{
    if(index < storage->output_best_effort_size)
    {
        return &storage->output_best_effort[index];
    }
    return NULL;
}

OutputReliableStream* get_output_reliable_stream(StreamStorage* storage, uint8_t index)
{
    if(index < storage->output_reliable_size)
    {
        return &storage->output_reliable[index];
    }
    return NULL;
}

InputBestEffortStream* get_input_best_effort_stream(StreamStorage* storage, uint8_t index)
{
    if(index < storage->input_best_effort_size)
    {
        return &storage->input_best_effort[index];
    }
    return NULL;
}

InputReliableStream* get_input_reliable_stream(StreamStorage* storage, uint8_t index)
{
    if(index < storage->input_reliable_size)
    {
        return &storage->input_reliable[index];
    }
    return NULL;
}

bool prepare_stream_to_write(StreamStorage* storage, StreamId stream_id, size_t size, MicroBuffer* mb)
{
    bool available = false;
    switch(stream_id.type)
    {
        case BEST_EFFORT_STREAM:
        {
            OutputBestEffortStream* stream = get_output_best_effort_stream(storage, stream_id.index);
            available = stream && prepare_best_effort_buffer_to_write(stream, size, mb);
            break;
        }
        case RELIABLE_STREAM:
        {
            OutputReliableStream* stream = get_output_reliable_stream(storage, stream_id.index);
            available = stream && prepare_reliable_buffer_to_write(stream, size, mb);
            break;
        }
        default:
            break;
    }

    return available;
}

bool busy_streams(StreamStorage* storage)
{
    bool busy = false;
    for(unsigned i = 0; i < storage->output_reliable_size && !busy; ++i)
    {
        busy = is_output_reliable_stream_busy(&storage->output_reliable[i]);
    }

    for(unsigned i = 0; i < storage->input_reliable_size && !busy; ++i)
    {
        busy = is_input_reliable_stream_busy(&storage->input_reliable[i]);
    }

    return busy;
}
