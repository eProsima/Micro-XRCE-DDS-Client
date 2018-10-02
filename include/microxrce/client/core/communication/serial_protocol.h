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

#ifndef _MICROXRCE_CLIENT_CORE_COMMUNICATION_SERIAL_PROTOCOL_H_
#define _MICROXRCE_CLIENT_CORE_COMMUNICATION_SERIAL_PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <microxrce/client/config.h>

#define UXR_FRAMING_END_FLAG 0x7E
#define UXR_FRAMING_ESC_FLAG 0x7D
#define UXR_FRAMING_XOR_FLAG 0x20

#define UXR_SERIAL_MTU          UXR_CONFIG_SERIAL_TRANSPORT_MTU //TODO: Change the name for a generic serial mtu.
#define UXR_SERIAL_OVERHEAD     5
#define UXR_SERIAL_BUFFER_SIZE  (2 * (UXR_SERIAL_MTU + UXR_SERIAL_OVERHEAD))

typedef struct mrSerialInputBuffer
{
    uint8_t buffer[UXR_SERIAL_BUFFER_SIZE];
    uint16_t head;
    uint16_t marker;
    uint16_t tail;
    bool stream_init;

} mrSerialInputBuffer;

typedef struct mrSerialOutputBuffer
{
    uint8_t buffer[UXR_SERIAL_BUFFER_SIZE];

} mrSerialOutputBuffer;

typedef struct mrSerialIO
{
    mrSerialInputBuffer input;
    mrSerialOutputBuffer output;

} mrSerialIO;

typedef uint16_t (*mr_read_cb)(void*, uint8_t*, size_t, int);

void init_serial_io(mrSerialIO* serial_io);
uint16_t write_serial_msg(mrSerialIO* serial_io, const uint8_t* buf, size_t len, uint8_t src_addr, uint8_t rmt_addr);
uint16_t read_serial_msg(mrSerialIO* serial_io,
                        mr_read_cb cb,
                        void* cb_arg,
                        uint8_t* buf,
                        size_t len,
                        uint8_t* src_addr,
                        uint8_t* rmt_addr,
                        int timeout);

#ifdef __cplusplus
}
#endif

#endif //_MICROXRCE_CLIENT_CORE_COMMUNICATION_SERIAL_PROTOCOL_H_
