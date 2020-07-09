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

#include "../resources/ByteArray.h"
#include <uxr/client/client.h>
#include <ucdr/microcdr.h>
#include "profile_config.h"

#include <stdio.h> //printf
#include <string.h> //strcmp
#include <stdlib.h> //atoi

#define OUTPUT_BUFFER_LENGTH    UXR_CONFIG_UDP_TRANSPORT_MTU

#ifdef UCLIENT_PROFILING_XML
#define PROFILING_SUB_MIN_OUTPUT_BUFFER_SIZE  400
#endif
#ifdef UCLIENT_PROFILING_REF
#define PROFILING_SUB_MIN_OUTPUT_BUFFER_SIZE  150
#endif

#define MINIMUM_BUFFER_SIZE 32
#define MAX_CONTROL_MSG_PAYLOAD_SIZE 5 // heartbeat
#define CONTROL_MSGS_BUFFER_SIZE MINIMUM_BUFFER_SIZE + MAX_CONTROL_MSG_PAYLOAD_SIZE
#define CALCULATE_INPUT_BUFFER_SIZE(TOPIC_SIZE) \
    ((TOPIC_SIZE + MINIMUM_BUFFER_SIZE) < PROFILING_SUB_MIN_OUTPUT_BUFFER_SIZE) ? \
        PROFILING_SUB_MIN_OUTPUT_BUFFER_SIZE : TOPIC_SIZE + MINIMUM_BUFFER_SIZE;

char topic_data[10240];

void on_topic(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uxrStreamId stream_id,
        struct ucdrBuffer* ub,
        uint16_t length,
        void* args)
{
    (void) session; (void) object_id; (void) request_id; (void) stream_id; (void) length;

    ByteArray topic;
    topic.message = topic_data;
    memset(topic.message, 0, sizeof(topic.message));
    ByteArray_deserialize_topic(ub, &topic);

    printf("Received topic: %s\n", topic.message);

    uint32_t* count_ptr = (uint32_t*) args;
    (*count_ptr)++;
}

int main(
        int argc,
        char** argv)
{
    // Parse CLI arguments
    const char* ip = "localhost";
    const char* port = "8888";

    if (3 != argc)
    {
        printf("Usage: <topic_size> <subscriber_number>\n");
        return 1;
    }
    uint32_t topic_size = (uint32_t)atoi(argv[1]);
    uint16_t sub_number = (uint16_t)atoi(argv[2]);
    uint32_t count = 0;

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
    uxr_init_session(&session, &transport.comm, 0xAAAABBCC);
    uxr_set_topic_callback(&session, on_topic, &count);
    if(!uxr_create_session(&session))
    {
        printf("Error creating uXRCE session!\n");
        return 1;
    }

    // Create streams
#ifdef UCLIENT_PROFILING_BEST_EFFORT
    uint8_t* output_besteffort_stream_buffer = (uint8_t*)malloc(PROFILING_SUB_MIN_OUTPUT_BUFFER_SIZE * sizeof(uint8_t));
    uxrStreamId stream_out = uxr_create_output_best_effort_stream(&session, output_besteffort_stream_buffer, PROFILING_SUB_MIN_OUTPUT_BUFFER_SIZE);

    uint8_t* input_reliable_stream_buffer = (uint8_t*)malloc(CONTROL_MSGS_BUFFER_SIZE * sizeof(uint8_t));
    uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, CONTROL_MSGS_BUFFER_SIZE, 1);

    uxrStreamId stream_in = uxr_create_input_best_effort_stream(&session);

#endif

#ifdef UCLIENT_PROFILING_RELIABLE
    uint32_t buffer_length = CALCULATE_INPUT_BUFFER_SIZE(topic_size);

    uint8_t* output_reliable_stream_buffer = (uint8_t*)malloc(PROFILING_SUB_MIN_OUTPUT_BUFFER_SIZE * sizeof(uint8_t));
    uxrStreamId stream_out = uxr_create_output_reliable_stream(&session, output_reliable_stream_buffer, PROFILING_SUB_MIN_OUTPUT_BUFFER_SIZE, 1);

    uint32_t stream_history = buffer_length/UXR_CONFIG_UDP_TRANSPORT_MTU + 2;
    uint8_t* input_reliable_stream_buffer = (uint8_t*)malloc(stream_history * UXR_CONFIG_UDP_TRANSPORT_MTU * sizeof(uint8_t));
    uxrStreamId stream_in = uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, stream_history * UXR_CONFIG_UDP_TRANSPORT_MTU, stream_history);
#endif

    uxrObjectId* datareader_ids = (uxrObjectId*)malloc(sub_number * sizeof(uxrObjectId));

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

    for (size_t i = 0; i < sub_number; i++)
    {
        uxrObjectId subscriber_id = uxr_object_id(i + 1, UXR_SUBSCRIBER_ID);
        const char* subscriber_xml = "";
        uint16_t subscriber_req = uxr_buffer_create_subscriber_xml(&session, stream_out, subscriber_id, participant_id, subscriber_xml, UXR_REPLACE);

        uxrObjectId datareader_id = uxr_object_id(i + 1, UXR_DATAREADER_ID);
        const char* datareader_xml = "<dds>"
                                        "<data_reader>"
                                            "<historyMemoryPolicy>PREALLOCATED_WITH_REALLOC</historyMemoryPolicy>"
                                            "<topic>"
                                                "<kind>NO_KEY</kind>"
                                                "<name>ByteArrayTopic</name>"
                                                "<dataType>ByteArray</dataType>"
                                            "</topic>"
                                        "</data_reader>"
                                    "</dds>";

        datareader_ids[i] = datareader_id;
        uint16_t datareader_req = uxr_buffer_create_datareader_xml(&session, stream_out, datareader_id, subscriber_id, datareader_xml, UXR_REPLACE);

#ifdef UCLIENT_PROFILING_RELIABLE
        uint8_t status[2];
        uint16_t requests[2] = {subscriber_req, datareader_req};
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

    for (size_t i = 0; i < sub_number; i++)
    {
        uxrObjectId subscriber_id = uxr_object_id(i + 1, UXR_SUBSCRIBER_ID);
        const char* subscriber_xml = "";
        uint16_t subscriber_req = uxr_buffer_create_subscriber_xml(&session, stream_out, subscriber_id, participant_id, subscriber_xml, UXR_REPLACE);

        uxrObjectId datareader_id = uxr_object_id(i + 1, UXR_DATAREADER_ID);
        const char* datareader_ref = "profiling_data_reader";
        uint16_t datareader_req = uxr_buffer_create_datareader_ref(&session, stream_out, datareader_id, subscriber_id, datareader_ref, UXR_REPLACE);

        datareader_ids[i] = datareader_id;

        uint8_t sub_dw_status[2];
        uint16_t sub_dw_requests[2] = {subscriber_req, datareader_req};

#ifdef UCLIENT_PROFILING_RELIABLE
        if(!uxr_run_session_until_all_status(&session, 1000, sub_dw_requests, sub_dw_status, 2))
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

    uint16_t* read_data_reqs = (uint16_t*)malloc(sub_number * sizeof(uint16_t));
    for (size_t i = 0; i < sub_number; i++)
    {
        // Request topics
        uxrDeliveryControl delivery_control = {0};
        delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
        delivery_control.min_pace_period = 0;
        delivery_control.max_elapsed_time = UXR_MAX_ELAPSED_TIME_UNLIMITED;
        delivery_control.max_bytes_per_second = UXR_MAX_BYTES_PER_SECOND_UNLIMITED;
        uint16_t read_data_req = uxr_buffer_request_data(&session, stream_out, datareader_ids[i], stream_in, &delivery_control);
        read_data_reqs[i] = read_data_req;
    }

    for (size_t i = 0; i < sub_number; i++)
    {
        bool connected = true;
        while(connected && 30 > count)
        {
#ifdef UCLIENT_PROFILING_RELIABLE
            uint8_t read_data_status;
            connected = uxr_run_session_until_all_status(&session, UXR_TIMEOUT_INF, read_data_reqs, &read_data_status, 1);
#endif
#ifdef UCLIENT_PROFILING_BEST_EFFORT
            uxr_run_session_time(&session, 5);
#endif
        }
        count = 0;
        read_data_reqs++;
    }

    // Delete resources
#ifdef UCLIENT_PROFILING_RELIABLE
    uxr_delete_session(&session);
#endif
    uxr_close_udp_transport(&transport);

    return 0;
}
