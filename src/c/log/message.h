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
#include <micrortps/client/client.h>
#include <stdint.h>
#include <stdio.h>

#define SEND 1
#define RECV 2

#ifndef MESSAGE_LOGS
#define INLINE_MES inline
#else
#define INLINE_MES
#endif


INLINE_MES const char* DATA_TO_STRING(const uint8_t* data, uint32_t size)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES void PRINT_MESSAGE(int dir, uint8_t* buffer, uint32_t size)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_C_LOG_MESSAGE_PRIVATE_H_
