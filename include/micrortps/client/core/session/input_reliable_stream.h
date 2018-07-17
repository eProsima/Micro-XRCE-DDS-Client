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

#ifndef _MICRORTPS_CLIENT_CORE_SESSION_INPUT_RELIABLE_STREAM_H_
#define _MICRORTPS_CLIENT_CORE_SESSION_INPUT_RELIABLE_STREAM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/seq_num.h>

#include <stdbool.h>
#include <stddef.h>

struct MicroBuffer;

typedef struct mrInputReliableStream
{
    uint8_t* buffer;
    size_t size;
    size_t history;

    mrSeqNum last_handled;
    mrSeqNum last_announced;

} mrInputReliableStream;

void init_input_reliable_stream(mrInputReliableStream* stream, uint8_t* buffer, size_t size, size_t history);
void reset_input_reliable_stream(mrInputReliableStream* stream);
bool receive_reliable_message(mrInputReliableStream* stream, uint16_t seq_num, uint8_t* buffer, size_t length);
bool next_input_reliable_buffer_available(mrInputReliableStream* stream, struct MicroBuffer* mb);

void write_acknack(const mrInputReliableStream* stream, struct MicroBuffer* mb);
void read_heartbeat(mrInputReliableStream* stream, struct MicroBuffer* payload);

bool is_input_reliable_stream_busy(mrInputReliableStream* stream);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_INPUT_RELIABLE_STREAM_H_
