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

#define MR_REQUEST_NONE     0x00
#define MR_REQUEST_LOGIN    0x01
#define MR_REQUEST_LOGOUT   0x02

struct MicroBuffer;

typedef struct mrSessionInfo
{
    uint8_t id;
    uint8_t key[4];
    uint8_t request;
    uint8_t last_requested_status;

} mrSessionInfo;


void init_session_info(mrSessionInfo* info, uint8_t id, uint32_t key);

void write_create_session(const mrSessionInfo* info, struct MicroBuffer* mb, uint64_t nanoseconds, bool reset_streams);
void write_delete_session(const mrSessionInfo* info, struct MicroBuffer* mb);
void read_create_session_status(mrSessionInfo* info, struct MicroBuffer* mb);
void read_delete_session_status(mrSessionInfo* info, struct MicroBuffer* mb);

void stamp_create_session_header(const mrSessionInfo* info, uint8_t* buffer);
void stamp_session_header(const mrSessionInfo* info, uint8_t stream_id_raw, uint16_t seq_num, uint8_t* buffer);
bool read_session_header(const mrSessionInfo* info, struct MicroBuffer* mb, uint8_t* stream_id_raw, uint16_t* seq_num);

uint8_t session_header_offset(const mrSessionInfo* info);

void set_session_info_request(mrSessionInfo* info, uint8_t request);
void reset_session_info_request(mrSessionInfo* info);
bool session_info_pending_request(const mrSessionInfo* info);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_SESSION_INFO_H
