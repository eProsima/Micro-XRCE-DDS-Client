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

#ifndef _MICRORTPS_CLIENT_PROFILE_SESSION_CREATE_ENTITIES_BIN_H_
#define _MICRORTPS_CLIENT_PROFILE_SESSION_CREATE_ENTITIES_BIN_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/session.h>
#include <micrortps/client/core/session/stream_id.h>
#include <micrortps/client/core/session/object_id.h>
#include <stdint.h>

typedef struct XrceString
{
    char* buffer;
    uint32_t len;

} XrceString;

typedef XrceString XrceReference;
typedef XrceString ContentFilter;

typedef struct TypeIdentifier TypeIdentifier;
typedef struct OBJK_PUBLISHER_QosBinary OBJK_PUBLISHER_QosBinary;
typedef struct OBJK_SUBSCRIBER_QosBinary OBJK_SUBSCRIBER_QosBinary;
typedef struct OBJK_Endpoint_QosBinary OBJK_Endpoint_QosBinary;

int write_configure_participant_bin(Session* session,
                                    StreamId stream_id,
                                    ObjectId object_id,
                                    int16_t domain_id,
                                    XrceReference* domain_reference,
                                    XrceReference* qos_profile_reference,
                                    uint8_t flags);

int write_configure_topic_bin(Session* session,
                              StreamId stream_id,
                              ObjectId object_id,
                              XrceReference* topic_name,
                              XrceReference* type_reference,
                              TypeIdentifier* type_identifier,
                              uint8_t flags);

int write_configure_publisher_bin(Session* session,
                                  StreamId stream_id,
                                  ObjectId object_id,
                                  XrceReference* publisher_name,
                                  OBJK_PUBLISHER_QosBinary* qos,
                                  uint8_t flags);

int write_configure_subscriber_bin(Session* session,
                                   StreamId stream_id,
                                   ObjectId object_id,
                                   XrceReference* subscriber_name,
                                   OBJK_SUBSCRIBER_QosBinary* qos,
                                   uint8_t flags);

int write_configure_datawriter_bin(Session* session,
                                   StreamId stream_id,
                                   ObjectId object_id,
                                   XrceReference* topic_name,
                                   OBJK_Endpoint_QosBinary* endpoint_qos,
                                   uint32_t ownership_strength,
                                   uint8_t flags);

int write_configure_datareader_bin(Session* session,
                                   StreamId stream_id,
                                   ObjectId object_id,
                                   XrceReference* topic_name,
                                   OBJK_Endpoint_QosBinary* endpoint_qos,
                                   uint32_t timebasedfilter_msec,
                                   ContentFilter* content_filter_expression,
                                   uint8_t flags);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_PROFILE_SESSION_CREATE_ENTITIES_BIN_H_
