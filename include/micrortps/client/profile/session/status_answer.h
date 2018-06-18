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

#ifndef _MICRORTPS_CLIENT_PROFILE_SESSION_STATUS_ANSWER_H_
#define _MICRORTPS_CLIENT_PROFILE_SESSION_STATUS_ANSWER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/session.h>
#include <micrortps/client/core/session/stream_id.h>
#include <micrortps/client/core/session/object_id.h>
#include <stdint.h>

// TODO (julian): move to session.h
typedef void (*StatusCallback)(int status, ObjectId object_id, StreamId stream_id, void* args);
void set_status_callback(StatusCallback callback, void* args);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_PROFILE_SESSION_STATUS_ANSWER_H_
