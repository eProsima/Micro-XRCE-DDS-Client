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

#ifndef _MICROXRCE_CLIENT_CORE_SESSION_SESSION_INFO_H_
#define _MICROXRCE_CLIENT_CORE_SESSION_SESSION_INFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microxrce/client/core/session/seq_num.h>
#include <microxrce/client/core/session/object_id.h>
#include <stdbool.h>

#define UXR_STATUS_OK                     0x00
#define UXR_STATUS_OK_MATCHED             0x01
#define UXR_STATUS_ERR_DDS_ERROR          0x80
#define UXR_STATUS_ERR_MISMATCH           0x81
#define UXR_STATUS_ERR_ALREADY_EXISTS     0x82
#define UXR_STATUS_ERR_DENIED             0x83
#define UXR_STATUS_ERR_UNKNOWN_REFERENCE  0x84
#define UXR_STATUS_ERR_INVALID_DATA       0x85
#define UXR_STATUS_ERR_INCOMPATIBLE       0x86
#define UXR_STATUS_ERR_RESOURCES          0x87
#define UXR_STATUS_NONE                   0xFF //Never sent. It corresponds as an unknown status

#define UXR_REUSE            0x01 << 1 //Not supported yet
#define UXR_REPLACE          0x01 << 2

#define UXR_INVALID_REQUEST_ID 0

#define UXR_REQUEST_NONE     0x00
#define UXR_REQUEST_LOGIN    0x01
#define UXR_REQUEST_LOGOUT   0x02

struct ucdrBuffer;
struct BaseObjectRequest;

typedef struct uxrSessionInfo
{
    uint8_t id;
    uint8_t key[4];
    uint8_t last_requested_status;
    uint16_t last_request_id;

} uxrSessionInfo;


void init_session_info(uxrSessionInfo* info, uint8_t id, uint32_t key);

void write_create_session(const uxrSessionInfo* info, struct ucdrBuffer* mb, int64_t nanoseconds);
void write_delete_session(const uxrSessionInfo* info, struct ucdrBuffer* mb);
void read_create_session_status(uxrSessionInfo* info, struct ucdrBuffer* mb);
void read_delete_session_status(uxrSessionInfo* info, struct ucdrBuffer* mb);

void stamp_create_session_header(const uxrSessionInfo* info, uint8_t* buffer);
void stamp_session_header(const uxrSessionInfo* info, uint8_t stream_id_raw, uxrSeqNum seq_num, uint8_t* buffer);
bool read_session_header(const uxrSessionInfo* info, struct ucdrBuffer* mb, uint8_t* stream_id_raw, uxrSeqNum* seq_num);

uint8_t session_header_offset(const uxrSessionInfo* info);

UXRDLLAPI uint16_t init_base_object_request(uxrSessionInfo* info, uxrObjectId object_id, struct BaseObjectRequest* base);
void parse_base_object_request(const struct BaseObjectRequest* base, uxrObjectId* object_id, uint16_t* request_id);

#ifdef __cplusplus
}
#endif

#endif // _MICROXRCE_CLIENT_CORE_SESSION_SESSION_INFO_H
