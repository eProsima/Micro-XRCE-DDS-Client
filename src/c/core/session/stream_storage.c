#include <micrortps/client/core/session/stream_storage.h>

//==================================================================
//                             PUBLIC
//==================================================================
void init_stream_storage(mrStreamStorage* storage)
{
    storage->output_best_effort_size = 0;
    storage->output_reliable_size = 0;
    storage->input_best_effort_size = 0;
    storage->input_reliable_size = 0;
}

void reset_stream_storage(mrStreamStorage* storage)
{
    for(unsigned i = 0; i < storage->output_best_effort_size; ++i)
    {
        reset_output_best_effort_stream(&storage->output_best_effort[i]);
    }

    for(unsigned i = 0; i < storage->input_best_effort_size; ++i)
    {
        reset_input_best_effort_stream(&storage->input_best_effort[i]);
    }

    for(unsigned i = 0; i < storage->output_reliable_size; ++i)
    {
        reset_output_reliable_stream(&storage->output_reliable[i]);
    }

    for(unsigned i = 0; i < storage->input_reliable_size; ++i)
    {
        reset_input_reliable_stream(&storage->input_reliable[i]);
    }
}

mrStreamId add_output_best_effort_buffer(mrStreamStorage* storage, uint8_t* buffer, size_t size, uint8_t header_offset)
{
    uint8_t index = storage->output_best_effort_size++;
    // assert for index
    mrOutputBestEffortStream* stream = &storage->output_best_effort[index];
    init_output_best_effort_stream(stream, buffer, size, header_offset);
    return mr_stream_id(index, MR_BEST_EFFORT_STREAM, MR_OUTPUT_STREAM);
}

mrStreamId add_output_reliable_buffer(mrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history, uint8_t header_offset)
{
    uint8_t index = storage->output_reliable_size++;
    // assert for index
    mrOutputReliableStream* stream = &storage->output_reliable[index];
    init_output_reliable_stream(stream, buffer, size, history, header_offset);
    return mr_stream_id(index, MR_RELIABLE_STREAM, MR_OUTPUT_STREAM);
}

mrStreamId add_input_best_effort_buffer(mrStreamStorage* storage)
{
    uint8_t index = storage->input_best_effort_size++;
    // assert for index
    mrInputBestEffortStream* stream = &storage->input_best_effort[index];
    init_input_best_effort_stream(stream);
    return mr_stream_id(index, MR_BEST_EFFORT_STREAM, MR_INPUT_STREAM);
}

mrStreamId add_input_reliable_buffer(mrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history)
{
    uint8_t index = storage->input_reliable_size++;
    // assert for index
    mrInputReliableStream* stream = &storage->input_reliable[index];
    init_input_reliable_stream(stream, buffer, size, history);
    return mr_stream_id(index, MR_RELIABLE_STREAM, MR_INPUT_STREAM);
}

mrOutputBestEffortStream* get_output_best_effort_stream(mrStreamStorage* storage, uint8_t index)
{
    if(index < storage->output_best_effort_size)
    {
        return &storage->output_best_effort[index];
    }
    return NULL;
}

mrOutputReliableStream* get_output_reliable_stream(mrStreamStorage* storage, uint8_t index)
{
    if(index < storage->output_reliable_size)
    {
        return &storage->output_reliable[index];
    }
    return NULL;
}

mrInputBestEffortStream* get_input_best_effort_stream(mrStreamStorage* storage, uint8_t index)
{
    if(index < storage->input_best_effort_size)
    {
        return &storage->input_best_effort[index];
    }
    return NULL;
}

mrInputReliableStream* get_input_reliable_stream(mrStreamStorage* storage, uint8_t index)
{
    if(index < storage->input_reliable_size)
    {
        return &storage->input_reliable[index];
    }
    return NULL;
}

bool prepare_stream_to_write(mrStreamStorage* storage, mrStreamId stream_id, size_t size, struct ucdrBuffer* mb)
{
    bool available = false;
    switch(stream_id.type)
    {
        case MR_BEST_EFFORT_STREAM:
        {
            mrOutputBestEffortStream* stream = get_output_best_effort_stream(storage, stream_id.index);
            available = stream && prepare_best_effort_buffer_to_write(stream, size, mb);
            break;
        }
        case MR_RELIABLE_STREAM:
        {
            mrOutputReliableStream* stream = get_output_reliable_stream(storage, stream_id.index);
            available = stream && prepare_reliable_buffer_to_write(stream, size, mb);
            break;
        }
        default:
            break;
    }

    return available;
}

bool output_streams_confirmed(const mrStreamStorage* storage)
{
    bool busy = false;
    for(unsigned i = 0; i < storage->output_reliable_size && !busy; ++i)
    {
        busy = is_output_reliable_stream_busy(&storage->output_reliable[i]);
    }
    return !busy;
}
