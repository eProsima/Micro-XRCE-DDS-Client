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

#ifndef _MICRORTPS_CLIENT_TCP_TRANSPORT_FREERTOS_H_
#define _MICRORTPS_CLIENT_TCP_TRANSPORT_FREERTOS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <FreeRTOS_Sockets.h>
#include <stdint.h>
#include <stddef.h>

#define TCP_TRANSPORT_MTU 512

typedef struct TCPTransport TCPTransport;
struct TCPTransport
{
    uint8_t buffer[TCP_TRANSPORT_MTU];
    Socket_t socket_fd;
    struct freertos_sockaddr remote_addr;
};

int init_tcp_transport(TCPTransport* transport, const char* ip, uint16_t port);
intmax_t send_tcp_msg(TCPTransport* transport, const uint8_t* buf, size_t len);
intmax_t recv_tcp_msg(TCPTransport* transport, uint8_t** buf,  size_t* len, int timeout);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_TCP_TRANSPORT_FREERTOS_H_
