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

#ifndef _MICRORTPS_CLIENT_SESSION_SESSION_INFO_H_
#define _MICRORTPS_CLIENT_SESSION_SESSION_INFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct MicroBuffer MicroBuffer;
typedef struct SessionInfo
{
    uint8_t id;
    uint8_t key[4];

} SessionInfo;


void init_session_info(SessionInfo* info, uint8_t id, uint32_t key);

void write_create_session(SessionInfo* info, MicroBuffer* mb, uint32_t nanoseconds);
void write_delete_session(SessionInfo* info, MicroBuffer* mb);
bool read_status_agent(SessionInfo* info, MicroBuffer* buffer, int* status_agent);

void stamp_first_session_header(SessionInfo* info, uint8_t* buffer);
void stamp_session_header(SessionInfo* info, uint8_t stream_id_raw, uint16_t seq_num, uint8_t* buffer);
bool read_session_header(SessionInfo* info, MicroBuffer* mb, uint8_t* stream_id_raw, uint16_t* seq_num);

uint8_t session_header_offset(SessionInfo* info);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_SESSION_SESSION_INFO_H
