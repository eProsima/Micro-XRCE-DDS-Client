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

#ifndef _MICRORTPS_CLIENT_COMMUNICATION_SERIAL_PROTOCOL_H_
#define _MICRORTPS_CLIENT_COMMUNICATION_SERIAL_PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>

#define MICRORTPS_FRAMING_FLAG 0x7E
#define MICRORTPS_FRAMING_ESP 0x7D
#define MICRORTPS_FRAMING_XOR 0x20

#define MICRORTPS_SERIAL_OVERHEAD 5
#define MICRORTPS_SERIAL_BUFFER_SIZE 517

typedef struct SerialIOProperties SerialIOProperties;

typedef struct SerialIO SerialIO;
struct SerialIO
{
    uint8_t input_buffer[MICRORTPS_SERIAL_BUFFER_SIZE];
    uint8_t output_buffer[MICRORTPS_SERIAL_BUFFER_SIZE];
    uint16_t crc;
    SerialIOProperties* properties;
};

typedef intmax_t (*read_callback)(void*, uint8_t*, size_t);

uint16_t calculate_crc(const uint8_t* buffer, size_t len);
void update_crc(uint16_t* crc, const uint8_t data);
intmax_t write_serial_msg(SerialIO* serial_io, const uint8_t* input_buffer, const size_t input_len, const uint8_t addr);
intmax_t read_serial_msg(SerialIO* serial_io, read_callback cb, uint8_t* output_buffer, const size_t output_len);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_COMMUNICATION_SERIAL_PROTOCOL_H_
