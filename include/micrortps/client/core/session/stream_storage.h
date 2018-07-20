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

#ifndef _MICRORTPS_CLIENT_CORE_SESSION_STREAM_STORAGE_H_
#define _MICRORTPS_CLIENT_CORE_SESSION_STREAM_STORAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/output_best_effort_stream.h>
#include <micrortps/client/core/session/output_reliable_stream.h>
#include <micrortps/client/core/session/input_best_effort_stream.h>
#include <micrortps/client/core/session/input_reliable_stream.h>
#include <micrortps/client/core/session/stream_id.h>
#include <micrortps/client/config.h>

typedef struct mrStreamStorage
{
    mrOutputBestEffortStream output_best_effort[MR_CONFIG_MAX_OUTPUT_BEST_EFFORT_STREAMS];
    uint8_t output_best_effort_size;
    mrOutputReliableStream output_reliable[MR_CONFIG_MAX_OUTPUT_RELIABLE_STREAMS];
    uint8_t output_reliable_size;
    mrInputBestEffortStream input_best_effort[MR_CONFIG_MAX_INPUT_BEST_EFFORT_STREAMS];
    uint8_t input_best_effort_size;
    mrInputReliableStream input_reliable[MR_CONFIG_MAX_INPUT_RELIABLE_STREAMS];
    uint8_t input_reliable_size;

} mrStreamStorage;

void init_stream_storage(mrStreamStorage* storage);
void reset_stream_storage(mrStreamStorage* storage);

mrStreamId add_output_best_effort_buffer(mrStreamStorage* storage, uint8_t* buffer, size_t size, uint8_t header_offset);
mrStreamId add_output_reliable_buffer(mrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history, uint8_t header_offset);
mrStreamId add_input_best_effort_buffer(mrStreamStorage* storage);
mrStreamId add_input_reliable_buffer(mrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history);

mrOutputBestEffortStream* get_output_best_effort_stream_id(mrStreamStorage* storage, uint8_t index);
mrOutputReliableStream* get_output_reliable_stream_id(mrStreamStorage* storage, uint8_t index);
mrInputBestEffortStream* get_input_best_effort_stream_id(mrStreamStorage* storage, uint8_t index);
mrInputReliableStream* get_input_reliable_stream_id(mrStreamStorage* storage, uint8_t index);

mrOutputBestEffortStream* get_output_best_effort_stream(mrStreamStorage* storage, uint8_t index);
mrOutputReliableStream* get_output_reliable_stream(mrStreamStorage* storage, uint8_t index);
mrInputBestEffortStream* get_input_best_effort_stream(mrStreamStorage* storage, uint8_t index);
mrInputReliableStream* get_input_reliable_stream(mrStreamStorage* storage, uint8_t index);

bool prepare_stream_to_write(mrStreamStorage* storage, mrStreamId stream_id, size_t size, struct MicroBuffer* mb);
bool output_streams_confirmed(const mrStreamStorage* storage);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_STREAM_STORAGE_H
