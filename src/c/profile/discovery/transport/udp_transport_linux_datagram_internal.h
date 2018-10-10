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

#ifndef _SRC_C_CLIENT_UDP_TRANSPORT_LINUX_DATAGRAM_INTERNAL_H_
#define _SRC_C_CLIENT_UDP_TRANSPORT_LINUX_DATAGRAM_INTERNAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <uxr/client/dll.h>
#include <uxr/client/config.h>
#include <sys/socket.h>
#include <poll.h>

#define UXR_UDP_TRANSPORT_MTU_DATAGRAM 128 //Adjust with the minimun necessary buffer to discovery

typedef struct uxrUDPTransportDatagram
{
    uint8_t buffer[UXR_UDP_TRANSPORT_MTU_DATAGRAM];
    struct pollfd poll_fd;

} uxrUDPTransportDatagram;

bool uxr_init_udp_transport_datagram(uxrUDPTransportDatagram* transport);
bool uxr_udp_send_datagram_to(uxrUDPTransportDatagram* transport, const uint8_t* buf, size_t len, const char* ip, uint16_t port);
bool uxr_udp_recv_datagram(uxrUDPTransportDatagram* instance, uint8_t** buf, size_t* len, int timeout);
void uxr_bytes_to_ip(const uint8_t* bytes, char* ip);

#ifdef __cplusplus
}
#endif

#endif //_SRC_C_CLIENT_UDP_TRANSPORT_LINUX_DATAGRAM_INTERNAL_H_
