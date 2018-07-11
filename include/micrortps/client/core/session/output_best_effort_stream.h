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

#ifndef _MICRORTPS_CLIENT_CORE_SESSION_OUTPUT_BEST_EFFORT_STREAM_H_
#define _MICRORTPS_CLIENT_CORE_SESSION_OUTPUT_BEST_EFFORT_STREAM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/seq_num.h>

#include <stddef.h>
#include <stdbool.h>

typedef struct MicroBuffer MicroBuffer;
typedef struct OutputBestEffortStream
{
    uint8_t* buffer;
    size_t writer;
    size_t size;
    uint8_t offset;

    SeqNum last_send;

} OutputBestEffortStream;

void init_output_best_effort_stream(OutputBestEffortStream* stream, uint8_t* buffer, size_t size, uint8_t offset);
bool prepare_best_effort_buffer_to_send(OutputBestEffortStream* stream, uint8_t** buffer, size_t* length, uint16_t* seq_num);
bool prepare_best_effort_buffer_to_write(OutputBestEffortStream* stream, size_t size, MicroBuffer* mb);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_OUTPUT_BEST_EFFORT_STREAM_H
