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

#ifndef _MICRORTPS_CLIENT_CORE_SESSION_SESSION_INFO_H_
#define _MICRORTPS_CLIENT_CORE_SESSION_SESSION_INFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define STATE_LOGOUT   0x00
#define STATE_LOGIN    0x01

typedef struct MicroBuffer MicroBuffer;
typedef struct SessionInfo
{
    uint8_t id;
    uint8_t key[4];
    uint8_t state;
    uint8_t request;

} SessionInfo;


void init_session_info(SessionInfo* info, uint8_t id, uint32_t key);

void write_create_session(const SessionInfo* info, MicroBuffer* mb, uint64_t nanoseconds);
void write_delete_session(const SessionInfo* info, MicroBuffer* mb);
void read_create_session_status(SessionInfo* info, MicroBuffer* mb);
void read_delete_session_status(SessionInfo* info, MicroBuffer* mb);

void stamp_create_session_header(const SessionInfo* info, uint8_t* buffer);
void stamp_session_header(const SessionInfo* info, uint8_t stream_id_raw, uint16_t seq_num, uint8_t* buffer);
bool read_session_header(const SessionInfo* info, MicroBuffer* mb, uint8_t* stream_id_raw, uint16_t* seq_num);

uint8_t session_header_offset(const SessionInfo* info);

void set_session_info_request(SessionInfo* info, uint8_t request);
void reset_session_info_request(SessionInfo* info);
bool session_info_pending_request(const SessionInfo* info);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_SESSION_INFO_H
