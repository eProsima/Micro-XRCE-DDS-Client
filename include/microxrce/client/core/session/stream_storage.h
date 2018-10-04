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

#ifndef _MICROXRCE_CLIENT_CORE_SESSION_STREAM_STORAGE_H_
#define _MICROXRCE_CLIENT_CORE_SESSION_STREAM_STORAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microxrce/client/core/session/stream/output_best_effort_stream.h>
#include <microxrce/client/core/session/stream/output_reliable_stream.h>
#include <microxrce/client/core/session/stream/input_best_effort_stream.h>
#include <microxrce/client/core/session/stream/input_reliable_stream.h>
#include <microxrce/client/core/session/stream_id.h>
#include <microxrce/client/config.h>

typedef struct uxrStreamStorage
{
    uxrOutputBestEffortStream output_best_effort[UXR_CONFIG_MAX_OUTPUT_BEST_EFFORT_STREAMS];
    uint8_t output_best_effort_size;
    uxrOutputReliableStream output_reliable[UXR_CONFIG_MAX_OUTPUT_RELIABLE_STREAMS];
    uint8_t output_reliable_size;
    uxrInputBestEffortStream input_best_effort[UXR_CONFIG_MAX_INPUT_BEST_EFFORT_STREAMS];
    uint8_t input_best_effort_size;
    uxrInputReliableStream input_reliable[UXR_CONFIG_MAX_INPUT_RELIABLE_STREAMS];
    uint8_t input_reliable_size;

} uxrStreamStorage;

void init_stream_storage(uxrStreamStorage* storage);
void reset_stream_storage(uxrStreamStorage* storage);

uxrStreamId add_output_best_effort_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint8_t header_offset);
uxrStreamId add_output_reliable_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history, uint8_t header_offset);
uxrStreamId add_input_best_effort_buffer(uxrStreamStorage* storage);
uxrStreamId add_input_reliable_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history);

uxrOutputBestEffortStream* get_output_best_effort_stream_id(uxrStreamStorage* storage, uint8_t index);
uxrOutputReliableStream* get_output_reliable_stream_id(uxrStreamStorage* storage, uint8_t index);
uxrInputBestEffortStream* get_input_best_effort_stream_id(uxrStreamStorage* storage, uint8_t index);
uxrInputReliableStream* get_input_reliable_stream_id(uxrStreamStorage* storage, uint8_t index);

uxrOutputBestEffortStream* get_output_best_effort_stream(uxrStreamStorage* storage, uint8_t index);
uxrOutputReliableStream* get_output_reliable_stream(uxrStreamStorage* storage, uint8_t index);
uxrInputBestEffortStream* get_input_best_effort_stream(uxrStreamStorage* storage, uint8_t index);
uxrInputReliableStream* get_input_reliable_stream(uxrStreamStorage* storage, uint8_t index);

UXRDLLAPI bool prepare_stream_to_write(uxrStreamStorage* storage, uxrStreamId stream_id, size_t size, struct ucdrBuffer* mb);
bool output_streams_confirmed(const uxrStreamStorage* storage);

#ifdef __cplusplus
}
#endif

#endif // _MICROXRCE_CLIENT_CORE_SESSION_STREAM_STORAGE_H
