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

#ifndef _MICRORTPS_C_LOG_MESSAGE_PRIVATE_H_
#define _MICRORTPS_C_LOG_MESSAGE_PRIVATE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/xrce_protocol_spec.h>
#include "../client_private.h"
#include <stdint.h>

//#if defined(SERIALIZATION_LOGS) || defined(MESSAGE_LOGS)
#include <stdio.h>
#define SEND "==> "
#define RECV "<== "
//#endif

#ifndef SERIALIZATION_LOGS
#define INLINE_SER inline
#else
#define INLINE_SER
#endif

#ifndef MESSAGE_LOGS
#define INLINE_MES inline
#else
#define INLINE_MES
#endif

INLINE_SER void PRINTL_SERIALIZATION(const char* pre, const uint8_t* buffer, uint32_t size)
#ifndef SERIALIZATION_LOGS
{}
#else
;
#endif

INLINE_MES void PRINT_SEQUENCE_NUMBER(uint16_t message_sequence_number, uint16_t local_sequence_number)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_CREATE_CLIENT_SUBMESSAGE(const CREATE_CLIENT_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_CREATE_RESOURCE_SUBMESSAGE(const CREATE_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_GET_INFO_SUBMESSAGE(const GET_INFO_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_DELETE_RESOURCE_SUBMESSAGE(const DELETE_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_STATUS_SUBMESSAGE(const STATUS_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_INFO_SUBMESSAGE(const INFO_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_READ_DATA_SUBMESSAGE(const READ_DATA_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_WRITE_DATA_DATA_SUBMESSAGE(const WRITE_DATA_Payload_Data* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_DATA_DATA_SUBMESSAGE(const DATA_Payload_Data* reply)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_ACKNACK_SUBMESSAGE(const ACKNACK_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINTL_HEARTBEAT_SUBMESSAGE(const HEARTBEAT_Payload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_C_LOG_MESSAGE_PRIVATE_H_
