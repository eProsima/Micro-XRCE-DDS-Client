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

#ifndef _MICRORTPS_C_CLIENT_PRIVATE_H_
#define _MICRORTPS_C_CLIENT_PRIVATE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "client.h"
#include "input_message.h"
#include "output_message.h"

#include <transport/ddsxrce_transport.h>

typedef struct CallbackData
{
    DataFormat format;
    OnMessageReceived on_message;
    void *callback_args;

} CallbackData;

typedef struct CallbackDataStorage
{
    uint32_t size;
    uint16_t initial_id;
    bool* existence;
    CallbackData* callbacks;

} CallbackDataStorage;

typedef struct ClientState
{
    locator_id_t transport_id;

    uint32_t buffer_size;
    uint8_t* input_buffer;
    uint8_t* output_buffer;

    uint16_t output_sequence_number;
    uint16_t input_sequence_number;

    uint8_t session_id;
    uint8_t stream_id;
    ClientKey key;

    OutputMessage output_message;
    InputMessage input_message;

    uint16_t next_request_id;
    uint16_t next_object_id;

    CallbackDataStorage callback_data_storage;

    OnStatusReceived on_status_received;
    void* on_status_received_args;

} ClientState;


// ----------------------------------------------------------------------------------------------
//    Data callback map funcions
// ----------------------------------------------------------------------------------------------
void init_callback_data_storage(CallbackDataStorage* store, int16_t initial_id);
void free_callback_data_storage(CallbackDataStorage* store);

uint16_t register_callback_data(CallbackDataStorage* store, uint8_t format, OnMessageReceived on_message, void *callback_args);
void remove_callback_data(CallbackDataStorage* store, uint16_t id);
CallbackData* get_callback_data(const CallbackDataStorage* store, uint16_t id);

// ----------------------------------------------------------------------------------------------
//    Internal state functions
// ----------------------------------------------------------------------------------------------
ClientState* new_client_state(uint32_t buffer_size, locator_id_t transport_id);

uint16_t get_num_request_id(RequestId request_id);
RequestId get_raw_request_id(uint16_t request_id);

uint16_t get_num_object_id(ObjectId object_id);
ObjectId get_raw_object_id(uint16_t object_id);

XRCEInfo create_xrce_info(RequestId request_id, ObjectId object_id);

// ----------------------------------------------------------------------------------------------
//    Callbacks
// ----------------------------------------------------------------------------------------------
void on_initialize_message(MessageHeader* header, ClientKey* key, void* args);

bool on_message_header(const MessageHeader* header, const ClientKey* key, void* args);

void on_status_submessage(const StatusPayload* payload, void* args);
DataFormat on_data_submessage(const BaseObjectReply* data_reply, void* args);

void on_data_payload(const BaseObjectReply* reply, const SampleData* data, void* args, Endianness endianness);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_C_CLIENT_PRIVATE_H_
