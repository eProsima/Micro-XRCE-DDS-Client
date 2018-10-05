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

#ifndef _MICROXRCE_CLIENT_TCP_TRANSPORT_WINDOWS_H_
#define _MICROXRCE_CLIENT_TCP_TRANSPORT_WINDOWS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <uxr/client/core/communication/communication.h>
#include <uxr/client/dll.h>
#include <winsock2.h>
#include <stdint.h>
#include <stddef.h>

#define TCP_TRANSPORT_MTU 512

typedef struct uxrTCPTransport
{
    uint8_t buffer[TCP_TRANSPORT_MTU];
    SOCKET socket_fd;
    struct sockaddr remote_addr;
    WSAPOLLFD poll_fd;
    uxrCommunication comm;
} uxrTCPTransport;


UXRDLLAPI bool uxr_init_tcp_transport(uxrTCPTransport* transport, const char* ip, uint16_t port);
UXRDLLAPI bool uxr_close_tcp_transport(uxrTCPTransport* transport);

#ifdef __cplusplus
}
#endif

#endif //_MICROXRCE_CLIENT_TCP_TRANSPORT_WINDOWS_H_
