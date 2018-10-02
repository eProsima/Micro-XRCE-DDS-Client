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

#ifndef _MICROXRCE_CLIENT_CORE_SESSION_OBJECT_ID_H_
#define _MICROXRCE_CLIENT_CORE_SESSION_OBJECT_ID_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microxrce/client/dll.h>
#include <stdint.h>

extern const uint8_t UXR_PARTICIPANT_ID;
extern const uint8_t UXR_TOPIC_ID;
extern const uint8_t UXR_PUBLISHER_ID;
extern const uint8_t UXR_SUBSCRIBER_ID;
extern const uint8_t UXR_DATAWRITER_ID;
extern const uint8_t UXR_DATAREADER_ID;

typedef struct mrObjectId
{
    uint16_t id;
    uint8_t type;

} mrObjectId;

MRDLLAPI mrObjectId uxr_object_id(uint16_t id, uint8_t type);

mrObjectId object_id_from_raw(const uint8_t* raw);
void object_id_to_raw(mrObjectId object_id, uint8_t* raw);

#ifdef __cplusplus
}
#endif

#endif //_MICROXRCE_CLIENT_CORE_SESSION_OBJECT_ID_H_
