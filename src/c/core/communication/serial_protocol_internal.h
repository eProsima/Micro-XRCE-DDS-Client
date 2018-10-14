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

#ifndef _SRC_C_CORE_COMMUNICATION_SERIAL_PROTOCOL_INTERNAL_H_
#define _SRC_C_CORE_COMMUNICATION_SERIAL_PROTOCOL_INTERNAL_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <uxr/client/core/communication/serial_protocol.h>

void uxr_init_serial_io(uxrSerialIO* serial_io);
uint16_t uxr_write_serial_msg(uxrSerialOutputBuffer* output,
                              uxr_write_cb write_cb,
                              void* cb_arg,
                              const uint8_t* buf,
                              uint16_t len,
                              uint8_t src_addr,
                              uint8_t rmt_addr);
uint16_t uxr_read_serial_msg(uxrSerialInputBuffer* input, uxr_read_cb read_cb, void* cb_arg, int timeout);

#ifdef __cplusplus
}
#endif

#endif //_SRC_C_CORE_COMMUNICATION_SERIAL_PROTOCOL_H_
