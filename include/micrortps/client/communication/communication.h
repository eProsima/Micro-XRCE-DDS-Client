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

#ifndef _MICRORTPS_CLIENT_COMMUNICATION_COMMUNICATION_H_
#define _MICRORTPS_CLIENT_COMMUNICATION_COMMUNICATION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define RECV_DATA_OK 0
#define RECV_DATA_TIMEOUT 1

typedef struct Communication Communication;
struct Communication
{
    size_t (*send_data)(Communication* communication, uint8_t* buffer, size_t length);
    int (*recv_data)(Communication* communication, uint8_t** buffer, size_t* length, uint32_t poll_ms);
    bool (*is_reliable)(Communication* communication);
    size_t (*mtu)(Communication* communication);
};

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_COMMUNICATION_COMMUNICATION_H
