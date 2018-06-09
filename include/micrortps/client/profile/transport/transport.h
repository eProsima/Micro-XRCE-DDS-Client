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

#ifndef _MICRORTPS_CLIENT_TRANSPORT_H_
#define _MICRORTPS_CLIENT_TRANSPORT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef PROFILE_UDP_TRANSPORT
#include <micrortps/client/profile/transport/udp_transport.h>
#endif //PROFILE_UDP_TRANSPORT

#ifdef PROFILE_TCP_TRANSPORT
#include <micrortps/client/profile/transport/tcp_transport.h>
#endif //PROFILE_TCP_TRANSPORT

#ifdef PROFILE_UART_TRANSPORT
#include <micrortps/client/profile/transport/uart_transport.h>
#endif //PROFILE_UART_TRANSPORT

typedef struct TransportContainer TransportContainer;
struct TransportContainer
{
#ifdef PROFILE_UDP_TRANSPORT
    UDPTransport* udp_transport;
#endif //PROFILE_UDP_TRANSPORT
#ifdef PROFILE_TCP_TRANSPORT
    TCPTransport* tcp_transport;
#endif //PROFILE_TCP_TRANSPORT
#ifdef PROFILE_UART_TRANSPORT
    UARTTransport* uart_transport;
#endif //PROFILE_UART_TRANSPORT
};

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_TRANSPORT_H_
