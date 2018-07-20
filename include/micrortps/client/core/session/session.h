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

#define MR_INVALID_REQUEST_ID 0
#define MR_TIMEOUT_INF       -1

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

struct mrSession;
struct mrCommunication;
struct BaseObjectRequest;

typedef void (*mrOnStatusFunc) (struct mrSession* session, mrObjectId object_id, uint16_t request_id,
                             uint8_t status, void* args);
typedef void (*mrOnTopicFunc) (struct mrSession* session, mrObjectId object_id, uint16_t request_id,
                             mrStreamId stream_id, struct MicroBuffer* mb, void* args);

typedef struct mrSession
{
    mrSessionInfo info;
    mrStreamStorage streams;
    struct mrCommunication* comm;
    uint16_t last_request_id;

    const uint16_t* request_list;
    uint8_t* status_list;
    size_t request_status_list_size;

    mrOnStatusFunc on_status;
    void* on_status_args;

    mrOnTopicFunc on_topic;
    void* on_topic_args;

} mrSession;

//==================================================================
//                         PUBLIC FUNCTIONS
//==================================================================
void mr_init_session(mrSession* session, struct mrCommunication* comm, uint32_t key);
void mr_set_status_callback(mrSession* session, mrOnStatusFunc on_status_func, void* args);
void mr_set_topic_callback(mrSession* session, mrOnTopicFunc on_topic_func, void* args);

bool mr_create_session(mrSession* session);
bool mr_delete_session(mrSession* session);

mrStreamId mr_create_output_best_effort_stream(mrSession* session, uint8_t* buffer, size_t size);
mrStreamId mr_create_output_reliable_stream(mrSession* session, uint8_t* buffer, size_t size, uint16_t history);
mrStreamId mr_create_input_best_effort_stream(mrSession* session);
mrStreamId mr_create_input_reliable_stream(mrSession* session, uint8_t* buffer, size_t size, uint16_t history);

bool mr_run_session_until_timeout(mrSession* session, int tiemout);
bool mr_run_session_until_confirm_delivery(mrSession* session, int timeout);
bool mr_run_session_until_status(mrSession* session, int timeout, const uint16_t* request_list, uint8_t* status_list, size_t list_size);

//==================================================================
//                        INTERNAL FUNCTIONS
//==================================================================
uint16_t init_base_object_request(mrSession* session, mrObjectId object_id, struct BaseObjectRequest* base);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_SESSION_H
