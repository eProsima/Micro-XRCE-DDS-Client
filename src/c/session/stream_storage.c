#include <micrortps/client/session/stream_storage.h>

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

StreamId add_output_best_effort_buffer(StreamStorage* storage, uint8_t* buffer, size_t size, uint8_t offset)
{
    uint8_t index = ++storage->output_best_effort_size;
    OutputBestEffortStream* stream = &storage->output_best_effort[index];
    init_output_best_effort_stream(stream, buffer, size, offset);
    return create_stream_id(index, BEST_EFFORT_STREAM, OUTPUT_STREAM);
}

StreamId add_output_reliable_buffer(StreamStorage* storage, uint8_t* buffer, size_t size, size_t history, uint8_t offset)
{
    uint8_t index = ++storage->output_reliable_size;
    OutputReliableStream* stream = &storage->output_reliable[index];
    init_output_reliable_stream(stream, buffer, size, history, offset);
    return create_stream_id(index, RELIABLE_STREAM, OUTPUT_STREAM);
}

StreamId add_input_best_effort_buffer(StreamStorage* storage)
{
    uint8_t index = ++storage->input_best_effort_size;
    InputBestEffortStream* stream = &storage->input_best_effort[index];
    init_input_best_effort_stream(stream);
    return create_stream_id(index, BEST_EFFORT_STREAM, INPUT_STREAM);
}

StreamId add_input_reliable_buffer(StreamStorage* storage, uint8_t* buffer, size_t size, size_t history)
{
    uint8_t index = ++storage->input_reliable_size;
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
