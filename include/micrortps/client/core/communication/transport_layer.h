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

#ifndef _MICRORTPS_CLIENT_CORE_COMMUNICATION_TRANSPORT_LAYER_H_
#define _MICRORTPS_CLIENT_CORE_COMMUNICATION_TRANSPORT_LAYER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/communication/communication.h>

typedef struct TransportLayer TransportLayer;
struct TransportLayer
{
    Communication* communication;
    int (*open_transport)(TransportLayer* transport_layer, const char* ip, uint16_t port);
    int (*close_transport)(TransportLayer* transport_layer);
};

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_CORE_COMMUNICATION_TRANSPORT_LAYER_H_
