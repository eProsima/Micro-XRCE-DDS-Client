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

#ifndef _UXR_CLIENT_CORE_COMMUNICATION_SERIAL_PROTOCOL_H_
#define _UXR_CLIENT_CORE_COMMUNICATION_SERIAL_PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <uxr/client/config.h>

#define UXR_FRAMING_BEGIN_FLAG 0x7E
#define UXR_FRAMING_ESC_FLAG 0x7D
#define UXR_FRAMING_XOR_FLAG 0x20

#define UXR_SERIAL_MTU          UXR_CONFIG_SERIAL_TRANSPORT_MTU //TODO: Change the name for a generic serial mtu.

typedef enum uxrSerialInputBufferState
{
    UXR_SERIAL_UNINITIALIZED,
    UXR_SERIAL_READING_SRC_ADDR,
    UXR_SERIAL_READING_DST_ADDR,
    UXR_SERIAL_READING_LEN_LSB,
    UXR_SERIAL_READING_LEN_MSB,
    UXR_SERIAL_READING_PAYLOAD,
    UXR_SERIAL_READING_CRC_LSB,
    UXR_SERIAL_READING_CRC_MSB,

} uxrSerialInputBufferState;

typedef struct uxrSerialInputBuffer
{
    uint8_t buffer[UXR_CONFIG_SERIAL_TRANSPORT_MTU];
    uxrSerialInputBufferState state;
    uint8_t rb[42];
    uint8_t rb_head;
    uint8_t rb_tail;
    uint8_t src_addr;
    uint8_t dst_addr;
    uint16_t msg_len;
    uint16_t msg_pos;
    uint16_t msg_crc;
    uint16_t crc;

} uxrSerialInputBuffer;

typedef struct uxrSerialOutputBuffer
{
    uint8_t wb[42];
    uint8_t wb_pos;

} uxrSerialOutputBuffer;

typedef struct uxrSerialIO
{
    uxrSerialInputBuffer input;
    uxrSerialOutputBuffer output;

} uxrSerialIO;

typedef uint16_t (*uxr_write_cb)(void*, uint8_t*, uint16_t);
typedef uint16_t (*uxr_read_cb)(void*, uint8_t*, uint16_t, int);

#ifdef __cplusplus
}
#endif

#endif //_UXR_CLIENT_CORE_COMMUNICATION_SERIAL_PROTOCOL_H_
