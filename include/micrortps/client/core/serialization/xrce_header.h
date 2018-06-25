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

#ifndef _MICRORTPS_CLIENT_CORE_SERIALIZATION_HEADER_H_
#define _MICRORTPS_CLIENT_CORE_SERIALIZATION_HEADER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <microcdr/microcdr.h>

#define CLIENT_KEY_SIZE 4

#define MIN_HEADER_SIZE 4
#define MAX_HEADER_SIZE (MIN_HEADER_SIZE + CLIENT_KEY_SIZE)

#define SESSION_ID_WITH_CLIENT_KEY 0x00
#define SESSION_ID_WITHOUT_CLIENT_KEY 0x80

void serialize_message_header(MicroBuffer* mb, uint8_t session_id, uint8_t stream_id, uint16_t seq_num, const uint8_t* key);
void deserialize_message_header(MicroBuffer* mb, uint8_t* session_id, uint8_t* stream_id, uint16_t* seq_num, uint8_t* key);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SERIALIZATION_HEADER_H_
