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

#ifndef _SRC_C_BROKERLESS_BROKERLESS_TRANSPORT_H_
#define _SRC_C_BROKERLESS_BROKERLESS_TRANSPORT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

bool brokerless_init_transport();

bool brokerless_close_transport();

size_t brokerless_broadcast_send(
        const uint8_t* buf,
        size_t len);

size_t brokerless_broadcast_recv(
        uint8_t* buf,
        size_t len,
        int timeout);

#ifdef __cplusplus
}
#endif

#endif // _SRC_C_BROKERLESS_BROKERLESS_TRANSPORT_H_
