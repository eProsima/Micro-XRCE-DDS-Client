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

#ifndef _MICRORTPS_CLIENT_CLIENT_H_
#define _MICRORTPS_CLIENT_CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct ClientState ClientState;
typedef struct MicroBuffer MicroBuffer;


typedef enum CreateModeFlags
{
    REUSE_MODE =   0x01 << 0,
    REPLACE_MODE = 0x01 << 1

} CreateModeFlags;

typedef struct String
{
    char* data;
    uint32_t length;

} String;

typedef struct AbstractTopic
{
    void* topic;

} AbstractTopic;

typedef struct XRCEInfo
{
    uint16_t request_id;
    uint16_t object_id;

} XRCEInfo;

typedef bool (*SerializeTopic)(MicroBuffer* writer, const AbstractTopic* topic_structure);
typedef bool (*DeserializeTopic)(MicroBuffer* reader, AbstractTopic* topic_structure);

typedef void (*OnMessageReceived)(XRCEInfo info, MicroBuffer *message, void *args);
typedef void (*OnStatusReceived)(XRCEInfo info, uint8_t operation, uint8_t status, void* args);

// ----------------------------------------------------------------------------------------------
//    State funcions
// ----------------------------------------------------------------------------------------------
ClientState* new_serial_client_state(uint32_t buffer_size, const char* device);
ClientState* new_udp_client_state(uint32_t buffer_size, const char* server_ip, uint16_t recv_port, uint16_t send_port);
void free_client_state(ClientState* state);

// ----------------------------------------------------------------------------------------------
//    XRCE Client API funcions
// ----------------------------------------------------------------------------------------------
XRCEInfo create_client(ClientState* state, OnStatusReceived on_status, void* on_status_args);
XRCEInfo create_participant(ClientState* state);
XRCEInfo create_topic(ClientState* state, uint16_t participant_id, String xml);
XRCEInfo create_publisher(ClientState* state, uint16_t participant_id);
XRCEInfo create_subscriber(ClientState* state, uint16_t participant_id);
XRCEInfo create_data_writer(ClientState* state, uint16_t participant_id, uint16_t publisher_id, String xml);
XRCEInfo create_data_reader(ClientState* state, uint16_t participant_id, uint16_t subscriber_id, String xml);

XRCEInfo delete_resource(ClientState* state, uint16_t resource_id);

XRCEInfo write_data(ClientState* state, uint16_t data_writer_id, SerializeTopic serialization, void* topic);
XRCEInfo read_data(ClientState* state, uint16_t data_reader_id, OnMessageReceived on_message, void *callback_args);

// ----------------------------------------------------------------------------------------------
//    Comunication functions
// ----------------------------------------------------------------------------------------------
bool send_to_agent(ClientState* state);
bool receive_from_agent(ClientState* state);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_CLIENT_H_
