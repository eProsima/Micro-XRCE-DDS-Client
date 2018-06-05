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

#ifndef _MICRORTPS_CLIENT_SESSION_H_
#define _MICRORTPS_CLIENT_SESSION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/session/session_info.h>
#include <micrortps/client/session/stream_storage.h>
#include <micrortps/client/communication/communication.h>

// ===============================================================
//                            PUBLIC
// ===============================================================
typedef struct Session
{
    SessionInfo info;
    StreamStorage streams;
    Communication* communication;
    int last_request_id;
} Session;

int create_session(Session* session, StreamId id, const char* key, Communication* comm);
int delete_session(Session* session);
void run_session(Session* session, int max_attemps, int poll_ms);
int generate_request_id(Session* session);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_SESSION_H
