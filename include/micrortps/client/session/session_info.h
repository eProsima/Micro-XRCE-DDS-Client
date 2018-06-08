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

#ifndef _MICRORTPS_CLIENT_SESSION_INFO_H_
#define _MICRORTPS_CLIENT_SESSION_INFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microcdr/microcdr.h>
#include <stddef.h>

typedef struct SessionInfo
{
    uint8_t id;
    uint8_t* key;

} SessionInfo;


void init_session_info(SessionInfo* info, uint8_t id, const char* key);

void write_create_session_message(SessionInfo* info, MicroBuffer* mb, uint32_t time_stamp);
void write_delete_session_message(SessionInfo* info, MicroBuffer* mb);
bool read_status_agent_message(SessionInfo* info, MicroBuffer* buffer, int* status_agent);

void stamp_session_header(SessionInfo* info, uint8_t* buffer);
bool read_session_header(SessionInfo* info, MicroBuffer* bm);

bool session_matching(SessionInfo* info, SessionInfo* other);

size_t session_header_offset(SessionInfo* info);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_SESSION_INFO_H
