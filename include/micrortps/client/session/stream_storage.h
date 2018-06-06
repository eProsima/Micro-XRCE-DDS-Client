// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _MICRORTPS_CLIENT_STREAM_STORAGE_H_
#define _MICRORTPS_CLIENT_STREAM_STORAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/session/output_best_effort_stream.h>
#include <micrortps/client/session/output_reliable_stream.h>
#include <micrortps/client/session/input_best_effort_stream.h>
#include <micrortps/client/session/input_reliable_stream.h>
#include <micrortps/client/session/stream_id.h>

#ifndef MAX_OUTPUT_BEST_EFFORT_STREAMS
#define MAX_OUTPUT_BEST_EFFORT_STREAMS 1
#endif

#ifndef MAX_OUTPUT_RELIABLE_STREAMS
#define MAX_OUTPUT_RELIABLE_STREAMS 1
#endif

#ifndef MAX_INPUT_BEST_EFFORT_STREAMS
#define MAX_INPUT_BEST_EFFORT_STREAMS 1
#endif

#ifndef MAX_INPUT_RELIABLE_STREAMS
#define MAX_INPUT_RELIABLE_STREAMS 1
#endif

typedef struct StreamStorage
{
    OutputBestEffortStream output_best_effort[MAX_OUTPUT_BEST_EFFORT_STREAMS];
    uint8_t output_best_effort_size;
    OutputReliableStream output_reliable[MAX_OUTPUT_RELIABLE_STREAMS];
    uint8_t output_reliable_size;
    InputBestEffortStream input_best_effort[MAX_INPUT_BEST_EFFORT_STREAMS];
    uint8_t input_best_effort_size;
    InputReliableStream input_reliable[MAX_INPUT_RELIABLE_STREAMS];
    uint8_t input_reliable_size;

} StreamStorage;

StreamId add_output_best_effort_buffer(uint8_t* buffer, size_t size);
StreamId add_output_reliable_buffer(uint8_t* buffer, size_t size);
StreamId add_input_best_effort_buffer(uint8_t* buffer, size_t size);
StreamId add_input_reliable_buffer(uint8_t* buffer, size_t size);

OutputBestEffortStream* get_output_best_effort_stream(StreamStorage* storage, uint8_t index);
OutputReliableStream* get_output_reliable_stream(StreamStorage* storage, uint8_t index);
InputBestEffortStream* get_input_best_effort_stream(StreamStorage* storage, uint8_t index);
InputReliableStream* get_input_reliable_stream(StreamStorage* storage, uint8_t index);

bool read_stream_header(StreamStorage* storage, MicroBuffer* mb, StreamId* stream_id, uint16_t* seq_num);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_STREAM_STORAGE_H
