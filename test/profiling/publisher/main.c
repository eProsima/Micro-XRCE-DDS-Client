// Copyright 2020 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include "ByteArray.h"
#include <uxr/client/client.h>
#include <ucdr/microcdr.h>
#include "profile_config.h"

#include <stdio.h> //printf
#include <string.h> //strcmp
#include <stdlib.h> //atoi

#define OUTPUT_BUFFER_LENGTH    UXR_CONFIG_UDP_TRANSPORT_MTU

#ifdef UCLIENT_PROFILING_XML
#define PROFILING_PUB_MIN_OUTPUT_BUFFER_SIZE  400
#endif
#ifdef UCLIENT_PROFILING_REF
#define PROFILING_PUB_MIN_OUTPUT_BUFFER_SIZE  150
#endif

#define MINIMUM_BUFFER_SIZE 32
#define CALCULATE_OUTPUT_BUFFER_SIZE(TOPIC_SIZE) \
    ((TOPIC_SIZE + MINIMUM_BUFFER_SIZE) < PROFILING_PUB_MIN_OUTPUT_BUFFER_SIZE) ? \
        PROFILING_PUB_MIN_OUTPUT_BUFFER_SIZE : TOPIC_SIZE + MINIMUM_BUFFER_SIZE;

char topic_data[10240];

int main(
        int argc,
        char** argv)
{
    // Parse CLI arguments
    const char* ip = "localhost";
    const char* port = "8888";

    if (3 != argc)
    {
        printf("Usage: <topic_size> <publisher_number>\n");
        return 1;
    }
    uint32_t topic_size = (uint32_t)atoi(argv[1]);
    uint16_t pub_number = (uint16_t)atoi(argv[2]);

    // Init transport
    uxrUDPTransport transport;
    uxrUDPPlatform udp_platform;
    if(!uxr_init_udp_transport(&transport, &udp_platform, UXR_IPv4, ip, port))
    {
        printf("Error initializing UDP transport!\n");
        return 1;
    }

    // Create session
    uxrSession session;
    uxr_init_session(&session, &transport.comm, 0xAAAABBBB);
    if(!uxr_create_session(&session))
    {
        printf("Error creating uXRCE session!\n");
        return 1;
    }

    // Create streams
#ifdef UCLIENT_PROFILING_BEST_EFFORT
    uint32_t buffer_length = CALCULATE_OUTPUT_BUFFER_SIZE(topic_size);

    uint8_t* output_besteffort_stream_buffer = (uint8_t*)malloc(buffer_length * sizeof(uint8_t));
    uxrStreamId stream_out = uxr_create_output_best_effort_stream(&session, output_besteffort_stream_buffer, buffer_length);

#endif

#ifdef UCLIENT_PROFILING_RELIABLE
    uint32_t buffer_length = CALCULATE_OUTPUT_BUFFER_SIZE(topic_size);

    uint8_t* output_reliable_stream_buffer = (uint8_t*)malloc(buffer_length * sizeof(uint8_t));
    uxrStreamId stream_out = uxr_create_output_reliable_stream(&session, output_reliable_stream_buffer, buffer_length, 1);

    uint8_t* input_reliable_stream_buffer = (uint8_t*)malloc(MINIMUM_BUFFER_SIZE * sizeof(uint8_t));
    uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, MINIMUM_BUFFER_SIZE, 1);
#endif

    uxrObjectId* datawriter_ids = (uxrObjectId*)malloc(pub_number * sizeof(uxrObjectId));

#ifdef UCLIENT_PROFILING_XML
    // Create DDS entities
    uxrObjectId participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* participant_xml = "<dds>"
                                      "<participant>"
                                          "<rtps>"
                                              "<name>default_xrce_participant</name>"
                                          "</rtps>"
                                      "</participant>"
                                  "</dds>";
    uint16_t participant_req = uxr_buffer_create_participant_xml(&session, stream_out, participant_id, 0, participant_xml, UXR_REPLACE);

    uxrObjectId topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    const char* topic_xml = "<dds>"
                                "<topic>"
                                    "<name>ByteArrayTopic</name>"
                                    "<dataType>ByteArray</dataType>"
                                "</topic>"
                            "</dds>";
    uint16_t topic_req = uxr_buffer_create_topic_xml(&session, stream_out, topic_id, participant_id, topic_xml, UXR_REPLACE);

#ifdef UCLIENT_PROFILING_RELIABLE
    uint8_t status[2];
    uint16_t requests[2] = {participant_req, topic_req};
    if (!uxr_run_session_until_all_status(&session, 1000, requests, status, 2))
    {
        return 1;
    }
    uxr_run_session_time(&session, 5);
#endif
#ifdef UCLIENT_PROFILING_BEST_EFFORT
    uxr_flash_output_streams(&session);
    usleep(10000);
#endif

    for (size_t i = 0; i < pub_number; i++)
    {
        uxrObjectId publisher_id = uxr_object_id(i + 1, UXR_PUBLISHER_ID);
        const char* publisher_xml = "";
        uint16_t publisher_req = uxr_buffer_create_publisher_xml(&session, stream_out, publisher_id, participant_id, publisher_xml, UXR_REPLACE);

        uxrObjectId datawriter_id = uxr_object_id(i + 1, UXR_DATAWRITER_ID);
        const char* datawriter_xml = "<dds>"
                                        "<data_writer>"
                                            "<historyMemoryPolicy>PREALLOCATED_WITH_REALLOC</historyMemoryPolicy>"
                                            "<topic>"
                                                "<kind>NO_KEY</kind>"
                                                "<name>ByteArrayTopic</name>"
                                                "<dataType>ByteArray</dataType>"
                                            "</topic>"
                                        "</data_writer>"
                                    "</dds>";

        datawriter_ids[i] = datawriter_id;
        uint16_t datawriter_req = uxr_buffer_create_datawriter_xml(&session, stream_out, datawriter_id, publisher_id, datawriter_xml, UXR_REPLACE);

#ifdef UCLIENT_PROFILING_RELIABLE
        uint8_t status[2];
        uint16_t requests[2] = {publisher_req, datawriter_req};
        if (!uxr_run_session_until_all_status(&session, 1000, requests, status, 2))
        {
            return 1;
        }
        uxr_run_session_time(&session, 5);
#endif
#ifdef UCLIENT_PROFILING_BEST_EFFORT
        uxr_flash_output_streams(&session);
        usleep(10000);
#endif
    }
#endif
#ifdef UCLIENT_PROFILING_REF
    uxrObjectId participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* participant_ref = "default_xrce_participant";
    uint16_t participant_req = uxr_buffer_create_participant_ref(&session, stream_out, participant_id, 0, participant_ref, UXR_REPLACE);

    uxrObjectId topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    const char* topic_ref = "profiling_topic";
    uint16_t topic_req = uxr_buffer_create_topic_ref(&session, stream_out, topic_id, participant_id, topic_ref, UXR_REPLACE);

#ifdef UCLIENT_PROFILING_RELIABLE
    uint8_t status[2];
    uint16_t requests[2] = {participant_req, topic_req};
    if (!uxr_run_session_until_all_status(&session, 1000, requests, status, 2))
    {
        return 1;
    }
    uxr_run_session_time(&session, 5);
#endif
#ifdef UCLIENT_PROFILING_BEST_EFFORT
    uxr_flash_output_streams(&session);
    usleep(10000);
#endif

    for (size_t i = 0; i < pub_number; i++)
    {
        uxrObjectId publisher_id = uxr_object_id(i + 1, UXR_PUBLISHER_ID);
        const char* publisher_xml = "";
        uint16_t publisher_req = uxr_buffer_create_publisher_xml(&session, stream_out, publisher_id, participant_id, publisher_xml, UXR_REPLACE);

        uxrObjectId datawriter_id = uxr_object_id(i + 1, UXR_DATAWRITER_ID);
        const char* datawriter_ref = "profiling_data_writer";
        uint16_t datawriter_req = uxr_buffer_create_datawriter_ref(&session, stream_out, datawriter_id, publisher_id, datawriter_ref, UXR_REPLACE);

        datawriter_ids[i] = datawriter_id;

#ifdef UCLIENT_PROFILING_RELIABLE
        uint8_t pub_dw_status[2];
        uint16_t pub_dw_requests[2] = {publisher_req, datawriter_req};
        if(!uxr_run_session_until_all_status(&session, 1000, pub_dw_requests, pub_dw_status, 2))
        {
            return 1;
        }
        uxr_run_session_time(&session, 5);
#endif
#ifdef UCLIENT_PROFILING_BEST_EFFORT
        uxr_flash_output_streams(&session);
        usleep(10000);
#endif
    }
#endif

    // Write topic info
    ByteArray topic;
    topic.message = topic_data;
    memset(topic.message, 'A', topic_size);
    topic.message[topic_size] = '\0';

    uint8_t count = 0;
#ifdef UCLIENT_PROFILING_INFINITE_LOOP
    while (true)
#else
    while (50 > count++)
#endif
    {
        for (size_t i = 0; i < pub_number; i++)
        {
            ucdrBuffer ub;
            uint32_t ba_topic_size = ByteArray_size_of_topic(&topic, 0);
            uxr_prepare_output_stream(&session, stream_out, datawriter_ids[i], &ub, ba_topic_size);
            ByteArray_serialize_topic(&ub, &topic);

#ifdef UCLIENT_PROFILING_BEST_EFFORT
            uxr_flash_output_streams(&session);
            usleep(10000);
#endif
#ifdef UCLIENT_PROFILING_RELIABLE
            uxr_run_session_time(&session, 5);
#endif
        }
    }

    // Delete resources
#ifdef UCLIENT_PROFILING_RELIABLE
    uxr_delete_session(&session);
#endif
    uxr_close_udp_transport(&transport);

    return 0;
}
