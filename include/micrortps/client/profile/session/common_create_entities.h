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

#ifndef _MICRORTPS_CLIENT_PROFILE_SESSION_COMMON_CREATE_ENTITIES_H_
#define _MICRORTPS_CLIENT_PROFILE_SESSION_COMMON_CREATE_ENTITIES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/session.h>

const uint8_t ENTITY_REPLACE;
const uint8_t ENTITY_REUSE;

typedef struct CREATE_Payload CREATE_Payload;

uint16_t common_create_entity(Session* session, StreamId stream_id,
                              mrObjectId object_id, size_t xml_ref_size, uint8_t flags,
                              CREATE_Payload* payload);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_PROFILE_SESSION_COMMON_CREATE_ENTITIES_H_
