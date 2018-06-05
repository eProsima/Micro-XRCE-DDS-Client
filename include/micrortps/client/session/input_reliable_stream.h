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

#ifndef _MICRORTPS_CLIENT_INPUT_RELIABLE_STREAM_H_
#define _MICRORTPS_CLIENT_INPUT_RELIABLE_STREAM_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct InputReliableStream
{
    StreamId id;
    FIFOStructureHere fifo;
    uint16_t last_handle;
    uint16_t last_announced;

} InputReliableStream;

void init_input_reliable_stream(InputReliableStream* stream, StreamId id, uint8_t* buffer);
bool prepare_reliable_stream_to_receive(InputReliableStream* stream, uint8_t* buffer, size_t length, int seq_num);
bool next_input_reliable_buffer_avaliable(InputReliableStream* stream, Microbuffer* mb);
bool update_input_reliable_stream_confirmation(InputReliableStream* stream);
int write_acknack(InputReliableStream* stream, MicroBuffer* mb);
void process_heartbeat(InputReliableStream* stream, uint16_t* first_seq_num, uint16_t last_seq_num);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_INPUT_RELIABLE_STREAM_H_
