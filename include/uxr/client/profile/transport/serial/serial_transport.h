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

#ifndef _UXR_CLIENT_SERIAL_TRANSPORT_H_
#define _UXR_CLIENT_SERIAL_TRANSPORT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <uxr/client/core/communication/communication.h>
#include <uxr/client/core/communication/serial_protocol.h>
#include <uxr/client/dll.h>

struct uxrSerialPlatform;

typedef struct uxrSerialTransport
{
    uxrSerialIO serial_io;
    uint8_t remote_addr;
    uint8_t local_addr;
    uxrCommunication comm;
    struct uxrSerialPlatform* platform;

} uxrSerialTransport;

typedef size_t (*uxr_write_cb)(struct uxrSerialPlatform*, uint8_t*, size_t);
typedef size_t (*uxr_read_cb)(struct uxrSerialPlatform*, uint8_t*, size_t, int);

UXRDLLAPI bool uxr_init_serial_transport(uxrSerialTransport* transport,
                                         struct uxrSerialPlatform* platform,
                                         const int fd,
                                         uint8_t remote_addr,
                                         uint8_t local_addr);
UXRDLLAPI bool uxr_close_serial_transport(uxrSerialTransport* transport);
size_t uxr_write_serial_msg(uxrSerialOutputBuffer* output,
                            uxr_write_cb write_cb,
                            void* cb_arg,
                            const uint8_t* buf,
                            size_t len,
                            uint8_t src_addr,
                            uint8_t dst_addr);
size_t uxr_read_serial_msg(uxrSerialInputBuffer* input, uxr_read_cb read_cb, void* cb_arg, int timeout);

bool uxr_init_serial_platform(struct uxrSerialPlatform* platform, const int fd, uint8_t remote_addr, uint8_t local_addr);
bool uxr_close_serial_platform(struct uxrSerialPlatform* platform);
size_t uxr_write_serial_data_platform(struct uxrSerialPlatform* platform, uint8_t* buf, size_t len);
size_t uxr_read_serial_data_platform(struct uxrSerialPlatform* platform, uint8_t* buf, size_t len, int timeout);

#ifdef __cplusplus
}
#endif

#endif //_UXR_CLIENT_SERIAL_TRANSPORT_H_
