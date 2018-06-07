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
//#include <micrortps/client/client.h>
#include <stdint.h>
#include <stdio.h>

#define SEND 1
#define RECV 2

#ifdef MESSAGE_LOGS
#define MESSAGE_LOGS_AVAILABLE 1
#else
#define MESSAGE_LOGS_AVAILABLE 0
#endif

#ifdef SERIALIZATION_LOGS
#define SERIALIZATION_LOGS_AVAILABLE 1
#else
#define SERIALIZATION_LOGS_AVAILABLE 0
#endif

void print_message(int direction, uint8_t* buffer, uint32_t size);
void print_serialization(int direction, const uint8_t* buffer, uint32_t size);

#define DEBUG_PRINT_MESSAGE(direction, buffer, size) \
    do { \
        if (MESSAGE_LOGS_AVAILABLE) print_message(direction, buffer, size); \
        if (SERIALIZATION_LOGS_AVAILABLE) print_serialization(direction, buffer, size); \
    } while (0);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_C_LOG_MESSAGE_PRIVATE_H_
