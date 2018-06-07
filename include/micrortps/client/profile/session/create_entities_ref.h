
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

#ifndef _MICRORTPS_CLIENT_PROFILE_SESSION_CREATE_ENTITIES_REF_H_ 
#define _MICRORTPS_CLIENT_PROFILE_SESSION_CREATE_ENTITIES_REF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/session/session.h>
#include <micrortps/client/session/stream_id.h>
#include <micrortps/client/session/object_id.h>
#include <stdint.h>

int write_create_quo_profile_ref(Session* session,
                                 StreamId stream_id,
                                 ObjectId object_id,
                                 char* object_reference_str,
                                 uint32_t object_reference_size,
                                 uint8_t flags);

int write_create_type_ref(Session* session,
                          StreamId stream_id,
                          ObjectId object_id,
                          char* object_reference_str,
                          uint32_t object_reference_size,
                          uint8_t flags);

int write_create_domain_ref(Session* session,
                            StreamId stream_id,
                            ObjectId object_id,
                            char* object_reference_str,
                            uint32_t object_reference_size,
                            uint8_t flags);

int write_create_participant_ref(Session* session,
                                 StreamId stream_id,
                                 ObjectId object_id,
                                 char* object_reference_str,
                                 uint32_t object_reference_size,
                                 uint8_t flags);

int write_create_topic_ref(Session* session,
                           StreamId stream_id,
                           ObjectId object_id,
                           char* object_reference_str,
                           uint32_t object_reference_size,
                           ObjectId participant_id,
                           uint8_t flags);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_PROFILE_SESSION_CREATE_ENTITIES_REF_H_
