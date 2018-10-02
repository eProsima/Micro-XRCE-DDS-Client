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

#ifndef _MICROXRCE_CLIENT_UDP_TRANSPORT_LINUX_H_
#define _MICROXRCE_CLIENT_UDP_TRANSPORT_LINUX_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microxrce/client/core/communication/communication.h>
#include <microxrce/client/config.h>
#include <sys/socket.h>
#include <poll.h>

typedef struct mrUDPTransport
{
    uint8_t buffer[UXR_CONFIG_UDP_TRANSPORT_MTU];
    int socket_fd;
    struct sockaddr remote_addr;
    struct pollfd poll_fd;
    mrCommunication comm;
} mrUDPTransport;

bool mr_init_udp_transport(mrUDPTransport* transport, const char* ip, uint16_t port);
bool mr_close_udp_transport(mrUDPTransport* transport);

#ifdef __cplusplus
}
#endif

#endif //_MICROXRCE_CLIENT_UDP_TRANSPORT_LINUX_H_
