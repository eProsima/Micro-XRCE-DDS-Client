
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

#ifndef _MICRORTPS_CLIENT_XRCE_CLIENT_H_
#define _MICRORTPS_CLIENT_XRCE_CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/xrce_protocol_spec.h> //TODO: remove dependence
#include <micrortps/transport/micrortps_transport.h> //TODO: remove dependence
#include <microcdr/microcdr.h>

#define MICRORTPS_MTU_SIZE        512
#define MICRORTPS_MAX_MSG_NUM      16

typedef struct MessageBuffer
{
    uint8_t data[MICRORTPS_MTU_SIZE];
    MicroBuffer micro_buffer;

} MessageBuffer;

typedef struct InputBestEffortStream
{
    uint16_t last_handled;

} InputBestEffortStream;

typedef struct OutputBestEffortStream
{
    uint16_t last_sent;
    MessageBuffer buffer;

} OutputBestEffortStream;


typedef struct InputReliableStream
{
    uint16_t last_handled;
    uint16_t last_announced;
    MessageBuffer buffers[MICRORTPS_MAX_MSG_NUM];
    uint64_t last_acknack_timestamp;

} InputReliableStream;

typedef struct OutputReliableStream
{
    uint16_t last_sent;
    uint16_t last_acknown;
    MessageBuffer buffers[MICRORTPS_MAX_MSG_NUM];
    uint64_t last_heartbeat_timestamp;

} OutputReliableStream;


typedef void (*OnTopic)(ObjectId id, MicroBuffer* serialized_topic, void* args);

typedef struct Session
{
    SessionId id;
    ClientKey key;

    uint16_t request_id;

    locator_id_t transport_id;
    micrortps_locator_t locator;

    uint8_t header_offset;

    InputBestEffortStream input_best_effort_stream;
    OutputBestEffortStream output_best_effort_stream;
    InputReliableStream input_reliable_stream;
    OutputReliableStream output_reliable_stream;

    OnTopic on_topic_callback;
    void* on_topic_args;

    ResultStatus last_status;
    bool last_status_received;
    uint16_t last_status_request_id;

} Session;



bool new_udp_session(Session* const session,
                     SessionId id,
                     ClientKey key,
                     const uint8_t* const agent_ip,
                     uint16_t agent_port,
                     OnTopic on_topic_callback,
                     void* on_topic_args);

void free_udp_session(Session* session);

bool init_session_sync(Session* session);
bool close_session_sync(Session* session);

/**
 * @brief create_participant_by_ref
 *
 * @param session
 * @param object_id
 * @param ref
 * @param reuse
 * @param replace
 *
 * @return
 */
bool create_participant_sync_by_ref(Session* session,
                                    const ObjectId object_id,
                                    const char* ref,
                                    bool reuse,
                                    bool replace);

/**
 * @brief create_topic_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
bool create_topic_sync_by_xml(Session* session,
                              const ObjectId object_id,
                              const char* xml,
                              const ObjectId participant_id,
                              bool reuse,
                              bool replace);

/**
 * @brief create_publisher_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
bool create_publisher_sync_by_xml(Session* session,
                                  const ObjectId object_id,
                                  const char* xml,
                                  const ObjectId participant_id,
                                  bool reuse,
                                  bool replace);

/**
 * @brief create_subscriber_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
bool create_subscriber_sync_by_xml(Session* session,
                                   const ObjectId object_id,
                                   const char* xml,
                                   const ObjectId participant_id,
                                   bool reuse,
                                   bool replace);

/**
 * @brief create_datawriter_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param publisher_id
 * @param reuse
 * @param replace
 *
 * @return
 */
bool create_datawriter_sync_by_xml(Session* session,
                                   const ObjectId object_id,
                                   const char* xml,
                                   const ObjectId publisher_id,
                                   bool reuse,
                                   bool replace);

/**
 * @brief create_datareader_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param subscriber_id
 * @param reuse
 * @param replace
 *
 * @return
 */
bool create_datareader_sync_by_xml(Session* session,
                                   const ObjectId object_id,
                                   const char* xml,
                                   const ObjectId subscriber_id,
                                   bool reuse,
                                   bool replace);

bool delete_object_sync(Session* session, ObjectId object_id);

bool read_data_sync(Session* session, ObjectId object_id, StreamId id);

MicroBuffer* prepare_best_effort_stream_for_topic(OutputBestEffortStream* output_stream, ObjectId data_writer_id, uint16_t topic_size);
MicroBuffer* prepare_reliable_stream_for_topic(OutputReliableStream* output_stream, ObjectId data_writer_id, uint16_t topic_size);

void run_communication(Session* session);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_CLIENT_H_
