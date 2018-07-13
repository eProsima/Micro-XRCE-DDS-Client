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

#ifndef _MICRORTPS_CLIENT_TCP_TRANSPORT_LINUX_H_
#define _MICRORTPS_CLIENT_TCP_TRANSPORT_LINUX_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/communication/communication.h>
#include <arpa/inet.h>
#include <sys/poll.h>

#define TCP_TRANSPORT_MTU 512

typedef enum TCPInputBufferState
{
    TCP_BUFFER_EMPTY,
    TCP_SIZE_INCOMPLETE,
    TCP_SIZE_READ,
    TCP_MESSAGE_INCOMPLETE,
    TCP_MESSAGE_AVAILABLE
} TCPInputBufferState;

typedef struct TCPInputBuffer
{
    uint8_t buffer[TCP_TRANSPORT_MTU];
    uint16_t position;
    TCPInputBufferState state;
    uint16_t msg_size;
} TCPInputBuffer;

typedef struct TCPTransport
{
    TCPInputBuffer input_buffer;
    struct sockaddr remote_addr;
    struct pollfd poll_fd;
    mrCommunication comm;
} TCPTransport;

bool init_tcp_transport(TCPTransport* transport, const char* ip, uint16_t port);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_TCP_TRANSPORT_LINUX_H_
