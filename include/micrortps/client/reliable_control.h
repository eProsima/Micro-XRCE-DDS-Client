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

#ifndef _MICRORTPS_CLIENT_RELIABLE_CONTROL_H_
#define _MICRORTPS_CLIENT_RELIABLE_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"

#include <micrortps/client/client.h>

void output_heartbeat(OutputReliableStream* reference_stream, HEARTBEAT_Payload* heartbeat);
void output_acknack(InputReliableStream* reference_stream, ACKNACK_Payload* acknack);

void input_heartbeat(Session* sesson, InputReliableStream* reference_stream, uint16_t first_seq_num, uint16_t last_seq_num);
void input_acknack(Session* session, OutputReliableStream* reference_stream, uint16_t first_unacked_seq_num, uint8_t bitmap[2]);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_INPUT_MESSAGE_H_
