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

#ifndef _MICRORTPS_CLIENT_SERIALIZATION_SUBHEADER_H_
#define _MICRORTPS_CLIENT_SERIALIZATION_SUBHEADER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct MicroBuffer MicroBuffer;

typedef enum SubmessageId
{
    SUBMESSAGE_ID_CREATE_CLIENT = 0,
    SUBMESSAGE_ID_CREATE = 1,
    SUBMESSAGE_ID_GET_INFO = 2,
    SUBMESSAGE_ID_DELETE = 3,
    SUBMESSAGE_ID_STATUS = 4,
    SUBMESSAGE_ID_INFO = 5,
    SUBMESSAGE_ID_WRITE_DATA = 6,
    SUBMESSAGE_ID_READ_DATA = 7,
    SUBMESSAGE_ID_DATA = 8,
    SUBMESSAGE_ID_ACKNACK = 9,
    SUBMESSAGE_ID_HEARTBEAT = 10,
    SUBMESSAGE_ID_FRAGMENT = 12,
    SUBMESSAGE_ID_FRAGMENT_END = 13

} SubmessageId;

typedef enum SubmessageHeaderFlags
{
    FLAG_ENDIANNESS = 0x01 << 0,
    FLAG_REUSE = 0x01 << 1,
    FLAG_REPLACE = 0x01 << 2,
    FLAG_LAST_FRAGMENT = 0x01 << 1,
    FORMAT_DATA_F = 0x00,
    FORMAT_SAMPLE_F = 0x02,
    FORMAT_DATA_SEQ_F = 0x08,
    FORMAT_SAMPLE_SEQ_F = 0x0A,
    FORMAT_PACKED_SAMPLES_F = 0x0E

} SubmessageHeaderFlags;


typedef struct SubmessageHeader
{
    uint8_t id;
    uint8_t flags;
    uint16_t length;

} SubmessageHeader;

void serialize_submessage_header(MicroBuffer* buffer, const SubmessageHeader* input);
void deserialize_submessage_header(MicroBuffer* buffer, SubmessageHeader* output);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_SERIALIZATION_SUBHEADER_H_
