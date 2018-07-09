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

extern const uint8_t MR_STATUS_OK;
extern const uint8_t MR_STATUS_OK_MATCHED;
extern const uint8_t MR_STATUS_ERR_DDS_ERROR;
extern const uint8_t MR_STATUS_ERR_MISMATCH;
extern const uint8_t MR_STATUS_ERR_ALREADY_EXISTS;
extern const uint8_t MR_STATUS_ERR_DENIED;
extern const uint8_t MR_STATUS_ERR_UNKNOWN_REFERENCE;
extern const uint8_t MR_STATUS_ERR_INVALID_DATA;
extern const uint8_t MR_STATUS_ERR_INCOMPATIBLE;
extern const uint8_t MR_STATUS_ERR_RESOURCES;
extern const uint8_t MR_STATUS_NONE;

typedef struct Communication Communication;
typedef struct Session Session;
typedef struct BaseObjectRequest BaseObjectRequest;

#ifdef PROFILE_READ_ACCESS
typedef void (*OnTopicFunc) (Session* session, mrObjectId object_id, uint16_t request_id,
                             StreamId stream_id, MicroBuffer* mb, void* args);
#endif

struct Session
{
    SessionInfo info;
    StreamStorage streams;
    Communication* comm;
    uint16_t last_request_id;

    const uint16_t* request_list;
    uint8_t* status_list;
    size_t request_status_list_size;

#ifdef PROFILE_READ_ACCESS
    OnTopicFunc on_topic;
    void* on_topic_args;
#endif

};

bool create_session(Session* session, uint8_t session_id, uint32_t key, Communication* comm);
bool delete_session(Session* session);

StreamId create_output_best_effort_stream(Session* session, uint8_t* buffer, size_t size);
StreamId create_output_reliable_stream(Session* session, uint8_t* buffer, size_t size, size_t history);
StreamId create_input_best_effort_stream(Session* session);
StreamId create_input_reliable_stream(Session* session, uint8_t* buffer, size_t size, size_t history);

void run_session_until_timeout(Session* session, int poll_ms);
bool run_session_until_confirm_delivery(Session* session, int poll_ms);
bool run_session_until_status(Session* session, int timeout_ms, const uint16_t* request_list, uint8_t* status_list, size_t list_size);

bool check_status_list_ok(uint8_t* status_list, size_t size);

uint16_t init_base_object_request(Session* session, mrObjectId object_id, BaseObjectRequest* base);

#ifdef PROFILE_READ_ACCESS
void set_topic_callback(Session* session, OnTopicFunc on_topic_func, void* args);
void read_submessage_data(Session* session, MicroBuffer* submessage, StreamId id, uint8_t format);
#endif

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_SESSION_H
