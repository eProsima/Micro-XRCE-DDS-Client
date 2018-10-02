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

#ifndef _MICROXRCE_CLIENT_SERIAL_TRANSPORT_LINUX_H_
#define _MICROXRCE_CLIENT_SERIAL_TRANSPORT_LINUX_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microxrce/client/core/communication/communication.h>
#include <microxrce/client/core/communication/serial_protocol.h>
#include <poll.h>

typedef struct mrSerialTransport
{
    uint8_t buffer[MR_CONFIG_SERIAL_TRANSPORT_MTU];
    uint8_t remote_addr;
    uint8_t local_addr;
    struct pollfd poll_fd;
    mrSerialIO serial_io;
    mrCommunication comm;
} mrSerialTransport;

bool mr_init_serial_transport(mrSerialTransport* transport, const char* device, uint8_t remote_addr, uint8_t local_addr);
bool mr_init_serial_transport_fd(mrSerialTransport* transport, const int fd, uint8_t remote_addr, uint8_t local_addr);
bool mr_close_serial_transport(mrSerialTransport* transport);

#ifdef __cplusplus
}
#endif

#endif //_MICROXRCE_CLIENT_SERIAL_TRANSPORT_LINUX_H_
