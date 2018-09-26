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

#ifndef _MICRORTPS_CLIENT_CORE_SESSION_OUTPUT_RELIABLE_STREAM_H_
#define _MICRORTPS_CLIENT_CORE_SESSION_OUTPUT_RELIABLE_STREAM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/seq_num.h>

#include <stddef.h>
#include <stdbool.h>

struct mcMicroBuffer;

typedef struct mrOutputReliableStream
{
    uint8_t* buffer;
    size_t size;
    uint16_t history;
    uint8_t offset;

    mrSeqNum last_written;
    mrSeqNum last_sent;
    mrSeqNum last_acknown;

    int64_t next_heartbeat_timestamp;
    uint8_t next_heartbeat_tries;
    bool send_lost;

} mrOutputReliableStream;

void init_output_reliable_stream(mrOutputReliableStream* stream, uint8_t* buffer, size_t size, uint16_t history, uint8_t header_offset);
void reset_output_reliable_stream(mrOutputReliableStream* stream);
bool prepare_reliable_buffer_to_write(mrOutputReliableStream* stream, size_t size, struct mcMicroBuffer* mb);
bool prepare_next_reliable_buffer_to_send(mrOutputReliableStream* stream, uint8_t** buffer, size_t* length, mrSeqNum* seq_num);

bool update_output_stream_heartbeat_timestamp(mrOutputReliableStream* stream, int64_t current_timestamp);
mrSeqNum begin_output_nack_buffer_it(const mrOutputReliableStream* stream);
bool next_reliable_nack_buffer_to_send(mrOutputReliableStream* stream, uint8_t** buffer, size_t *length, mrSeqNum* seq_num_it);
void write_heartbeat(const mrOutputReliableStream* stream, struct mcMicroBuffer* mb);
void read_acknack(mrOutputReliableStream* stream, struct mcMicroBuffer* payload);

bool is_output_reliable_stream_busy(const mrOutputReliableStream* stream);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_OUTPUT_RELIABLE_STREAM_H_
