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

#ifndef _MICRORTPS_CLIENT_OUTPUT_RELIABLE_STREAM_H_
#define _MICRORTPS_CLIENT_OUTPUT_RELIABLE_STREAM_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct OutputReliableStream
{
    StreamId id;
    FIFOStrutureHere fifo;
    int last_sent;
    int last_acknown;
    int next_heartbeat_time_stamp;

} OutputReliableStream;

void init_output_reliable_stream(OutputReliableStream* stream, int id, uint8_t* buffer);
void prepare_reliable_stream_to_write(OutputReliableStream* stream, MicroBuffer* mb, int size);
void prepare_reliable_next_buffer_to_send(OutputReliableStream* stream, MicroBuffer* mb);
size_t reliable_buffer_to_send(OutputReliableStream* stream, char* buffer);

bool update_output_reliable_stream_time(OutputReliableStream* stream, int current_timestamp);
void process_acknack(OutputReliableStream* stream, uint16_t bitmap, uint16_t first_unacked_seq_num);
int write_heartbeat(OutputReliableStream* stream, Microbuffer* mb);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_OUTPUT_RELIABLE_STREAM_H_
