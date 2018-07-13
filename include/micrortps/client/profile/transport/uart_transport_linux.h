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

#ifndef _MICRORTPS_CLIENT_UART_TRANSPORT_LINUX_H_
#define _MICRORTPS_CLIENT_UART_TRANSPORT_LINUX_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/communication/communication.h>
#include <micrortps/client/core/communication/serial_protocol.h>
#include <sys/poll.h>

#define UART_TRANSPORT_MTU 256

typedef struct UARTTransport UARTTransport;
struct UARTTransport
{
    uint8_t buffer[UART_TRANSPORT_MTU];
    uint8_t remote_addr;
    uint8_t local_addr;
    struct pollfd poll_fd;
    SerialIO serial_io;
    mrCommunication comm;
};

bool init_uart_transport(UARTTransport* transport, const char* device, uint8_t remote_addr, uint8_t local_addr);
bool init_uart_transport_fd(UARTTransport* transport, const int fd, uint8_t remote_addr, uint8_t local_addr);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_UART_TRANSPORT_LINUX_H_
