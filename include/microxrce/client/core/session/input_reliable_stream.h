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

#ifndef _MICROXRCE_CLIENT_CORE_SESSION_INPUT_RELIABLE_STREAM_H_
#define _MICROXRCE_CLIENT_CORE_SESSION_INPUT_RELIABLE_STREAM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microxrce/client/core/session/seq_num.h>

#include <stdbool.h>
#include <stddef.h>

struct ucdrBuffer;

typedef struct uxrInputReliableStream
{
    uint8_t* buffer;
    size_t size;
    uint16_t history;

    uxrSeqNum last_handled;
    uxrSeqNum last_announced;

} uxrInputReliableStream;

void init_input_reliable_stream(uxrInputReliableStream* stream, uint8_t* buffer, size_t size, uint16_t history);
void reset_input_reliable_stream(uxrInputReliableStream* stream);
bool receive_reliable_message(uxrInputReliableStream* stream, uint16_t seq_num, uint8_t* buffer, size_t length);
bool next_input_reliable_buffer_available(uxrInputReliableStream* stream, struct ucdrBuffer* mb);

void write_acknack(const uxrInputReliableStream* stream, struct ucdrBuffer* mb);
void read_heartbeat(uxrInputReliableStream* stream, struct ucdrBuffer* payload);

bool is_input_reliable_stream_busy(uxrInputReliableStream* stream);

#ifdef __cplusplus
}
#endif

#endif // _MICROXRCE_CLIENT_CORE_SESSION_INPUT_RELIABLE_STREAM_H_
