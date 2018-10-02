#include <microxrce/client/core/session/stream_storage.h>

//==================================================================
//                             PUBLIC
//==================================================================
void init_stream_storage(uxrStreamStorage* storage)
{
    storage->output_best_effort_size = 0;
    storage->output_reliable_size = 0;
    storage->input_best_effort_size = 0;
    storage->input_reliable_size = 0;
}

void reset_stream_storage(uxrStreamStorage* storage)
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

uxrStreamId add_output_best_effort_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint8_t header_offset)
{
    uint8_t index = storage->output_best_effort_size++;
    // assert for index
    uxrOutputBestEffortStream* stream = &storage->output_best_effort[index];
    init_output_best_effort_stream(stream, buffer, size, header_offset);
    return uxr_stream_id(index, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
}

uxrStreamId add_output_reliable_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history, uint8_t header_offset)
{
    uint8_t index = storage->output_reliable_size++;
    // assert for index
    uxrOutputReliableStream* stream = &storage->output_reliable[index];
    init_output_reliable_stream(stream, buffer, size, history, header_offset);
    return uxr_stream_id(index, UXR_RELIABLE_STREAM, UXR_OUTPUT_STREAM);
}

uxrStreamId add_input_best_effort_buffer(uxrStreamStorage* storage)
{
    uint8_t index = storage->input_best_effort_size++;
    // assert for index
    uxrInputBestEffortStream* stream = &storage->input_best_effort[index];
    init_input_best_effort_stream(stream);
    return uxr_stream_id(index, UXR_BEST_EFFORT_STREAM, UXR_INPUT_STREAM);
}

uxrStreamId add_input_reliable_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history)
{
    uint8_t index = storage->input_reliable_size++;
    // assert for index
    uxrInputReliableStream* stream = &storage->input_reliable[index];
    init_input_reliable_stream(stream, buffer, size, history);
    return uxr_stream_id(index, UXR_RELIABLE_STREAM, UXR_INPUT_STREAM);
}

uxrOutputBestEffortStream* get_output_best_effort_stream(uxrStreamStorage* storage, uint8_t index)
{
    if(index < storage->output_best_effort_size)
    {
        return &storage->output_best_effort[index];
    }
    return NULL;
}

uxrOutputReliableStream* get_output_reliable_stream(uxrStreamStorage* storage, uint8_t index)
{
    if(index < storage->output_reliable_size)
    {
        return &storage->output_reliable[index];
    }
    return NULL;
}

uxrInputBestEffortStream* get_input_best_effort_stream(uxrStreamStorage* storage, uint8_t index)
{
    if(index < storage->input_best_effort_size)
    {
        return &storage->input_best_effort[index];
    }
    return NULL;
}

uxrInputReliableStream* get_input_reliable_stream(uxrStreamStorage* storage, uint8_t index)
{
    if(index < storage->input_reliable_size)
    {
        return &storage->input_reliable[index];
    }
    return NULL;
}

bool prepare_stream_to_write(uxrStreamStorage* storage, uxrStreamId stream_id, size_t size, struct ucdrBuffer* mb)
{
    bool available = false;
    switch(stream_id.type)
    {
        case UXR_BEST_EFFORT_STREAM:
        {
            uxrOutputBestEffortStream* stream = get_output_best_effort_stream(storage, stream_id.index);
            available = stream && prepare_best_effort_buffer_to_write(stream, size, mb);
            break;
        }
        case UXR_RELIABLE_STREAM:
        {
            uxrOutputReliableStream* stream = get_output_reliable_stream(storage, stream_id.index);
            available = stream && prepare_reliable_buffer_to_write(stream, size, mb);
            break;
        }
        default:
            break;
    }

    return available;
}

bool output_streams_confirmed(const uxrStreamStorage* storage)
{
    bool busy = false;
    for(unsigned i = 0; i < storage->output_reliable_size && !busy; ++i)
    {
        busy = is_output_reliable_stream_busy(&storage->output_reliable[i]);
    }
    return !busy;
}
