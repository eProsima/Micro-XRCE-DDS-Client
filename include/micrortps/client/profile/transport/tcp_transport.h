// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef _MICRORTPS_CLIENT_TCP_TRANSPORT_H_
#define _MICRORTPS_CLIENT_TCP_TRANSPORT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct TCPProperties TCPProperties;

typedef struct TCPTransport TCPTransport;
struct TCPTransport
{
    intmax_t (*send_data)(TCPTransport* transport, const void* buf, size_t len);
    intmax_t (*recv_data)(TCPTransport* transport, void** buf, size_t* len, int timeout);
    TCPProperties* properties;
};

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_TCP_TRANSPORT_H_
