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

#ifndef _MICRORTPS_CLIENT_CORE_SESSION_SESSION_H_
#define _MICRORTPS_CLIENT_CORE_SESSION_SESSION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/session_info.h>
#include <micrortps/client/core/session/stream_storage.h>
#include <micrortps/client/core/session/object_id.h>

#define INVALID_REQUEST_ID 0

typedef struct Communication Communication;
typedef struct Session Session;
typedef struct BaseObjectRequest BaseObjectRequest;

typedef void (*OnStatusFunc) (Session* session, mrObjectId object_id, uint16_t request_id,
                              uint8_t status, void* args);

#ifdef PROFILE_READ_ACCESS
typedef void (*OnTopicFunc) (Session* session, mrObjectId object_id, uint16_t request_id,
                             MicroBuffer* mb, StreamId stream_id, void* args);
#endif

struct Session
{
    SessionInfo info;
    StreamStorage streams;
    Communication* comm;
    uint16_t last_request_id;

    OnStatusFunc on_status;
    void* on_status_args;

#ifdef PROFILE_READ_ACCESS
    OnTopicFunc on_topic;
    void* on_topic_args;
#endif

};

int create_session(Session* session, uint8_t session_id, uint32_t key, Communication* comm);
int delete_session(Session* session);

void set_status_callback(Session* session, OnStatusFunc on_status_func, void* args);

StreamId create_output_best_effort_stream(Session* session, uint8_t* buffer, size_t size);
StreamId create_output_reliable_stream(Session* session, uint8_t* buffer, size_t size, size_t history);
StreamId create_input_best_effort_stream(Session* session);
StreamId create_input_reliable_stream(Session* session, uint8_t* buffer, size_t size, size_t history);

size_t run_session(Session* session, size_t max_attempts, int poll_ms);

uint16_t init_base_object_request(Session* session, mrObjectId object_id, BaseObjectRequest* base);

#ifdef PROFILE_READ_ACCESS
void set_topic_callback(Session* session, OnTopicFunc on_topic_func, void* args);
void read_submessage_data(Session* session, MicroBuffer* submessage, StreamId id, uint8_t format);
#endif

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_SESSION_H
