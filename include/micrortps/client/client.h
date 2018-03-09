/*
 * Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _MICRORTPS_CLIENT_CLIENT_H_
#define _MICRORTPS_CLIENT_CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define SESSION_CREATED				0x00
#define SESSION_INIT				0x01
#define SESSION_LOCATOR_OK 			0xF0
#define SESSION_LOCATOR_ERR 		0xF1
#define SESSION_SERIALIZATION_ERR	0xF2

#include <micrortps/client/config.h>
#include <micrortps/client/input_message.h>
#include <micrortps/client/output_message.h>

#include <transport/micrortps_transport.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct Session Session;
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

typedef struct Session
{
    locator_id_t transport_id;

    uint32_t buffer_size;
    uint8_t* buffer;

    uint16_t output_sequence_number;
    uint16_t input_sequence_number;

    uint8_t session_id;
    uint8_t stream_id;
    ClientKey key;

    OutputMessage output_message;
    InputMessage input_message;

    uint16_t next_request_id;
    uint16_t next_object_id;

    bool				existence[DATA_CALLBACK_SIZE];
    CallbackData 		callback_data[DATA_CALLBACK_SIZE];
    CallbackDataStorage callback_data_storage;

    OnStatusReceived on_status_received;
    void* on_status_received_args;

} Session;

/* ----------------------------------------------------------------------------------------------
      Session functions
 ---------------------------------------------------------------------------------------------- */
uint8_t new_udp_session(Session* session,
                     uint8_t* buf,
                     uint32_t buf_size,
                     uint16_t send_port,
                     uint16_t recv_port,
                     uint16_t remote_port,
                     const char* server_ip);
uint8_t init_session(Session* session, XRCEInfo* info, OnStatusReceived on_status, void* on_status_args);
void close_session(Session* session);

/* ----------------------------------------------------------------------------------------------
      XRCE Client API funcions
 ---------------------------------------------------------------------------------------------- */
XRCEInfo create_participant(Session* session);
XRCEInfo create_topic(Session* session, uint16_t participant_id, String xml);
XRCEInfo create_publisher(Session* session, uint16_t participant_id);
XRCEInfo create_subscriber(Session* session, uint16_t participant_id);
XRCEInfo create_data_writer(Session* session, uint16_t participant_id, uint16_t publisher_id, String xml);
XRCEInfo create_data_reader(Session* session, uint16_t participant_id, uint16_t subscriber_id, String xml);

XRCEInfo delete_resource(Session* session, uint16_t resource_id);

XRCEInfo write_data(Session* session, uint16_t data_writer_id, SerializeTopic serialization, void* topic);
bool read_data(Session* session, XRCEInfo* info, uint16_t data_reader_id, OnMessageReceived on_message, void *callback_args);

/* ----------------------------------------------------------------------------------------------
      Comunication functions
 ---------------------------------------------------------------------------------------------- */
bool send_to_agent(Session* session);
bool receive_from_agent(Session* session);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_CLIENT_H_
