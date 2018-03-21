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

#ifndef _MICRORTPS_CLIENT_INPUT_MESSAGE_H_
#define _MICRORTPS_CLIENT_INPUT_MESSAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"

#include <micrortps/client/client.h>
#include <microcdr/microcdr.h>

void process_message(Session* session, MicroBuffer* input_buffer);
void process_submessages(Session* session, MicroBuffer* micro_buffer);

void process_status_submessage(Session* session, MicroBuffer* micro_buffer);
void process_info_submessage(Session* session, MicroBuffer* input_buffer);
void process_heartbeat_submessage(Session* session, MicroBuffer* input_buffer);
void process_acknack_submessage(Session* session, MicroBuffer* input_buffer);
void process_data_submessage(Session* session, MicroBuffer* input_buffer);

bool receive_best_effort_message(BestEffortStream* input_stream, const uint16_t seq_num);
bool receive_reliable_message(ReliableStream* input_stream, MicroBuffer* submessages, uint16_t seq_num);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_INPUT_MESSAGE_H_
