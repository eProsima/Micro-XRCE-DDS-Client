// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef _SRC_C_BROKERLESS_BROKERLESS_H_
#define _SRC_C_BROKERLESS_BROKERLESS_H_

#include <uxr/client/core/communication/communication.h>

#ifdef __cplusplus
extern "C"
{
#endif

bool brokerless_send_stub(void* instance, const uint8_t* buf, size_t len){
    return true;
}

bool brokerless_recv_stub(void* instance, uint8_t** buf, size_t* len, int timeout){
    return true;
}

uxrCommunication brokerless_comm_stub = {
    NULL,
    brokerless_send_stub,
    brokerless_recv_stub,
    NULL,
    0
};

#ifdef __cplusplus
}
#endif

#endif // _SRC_C_BROKERLESS_BROKERLESS_H_
