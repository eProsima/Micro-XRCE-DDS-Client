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

#include "ByteArray.h"

#include <uxr/client/client.h>
#include <ucdr/microcdr.h>

// #include <string.h> //strcmp
// #include <stdlib.h> //atoi

#define USE_XML_CREATION

#ifdef USE_XML_CREATION
// #define MIN_BUFFER_SIZE 390 // 384 Bytes aprox
#define MIN_BUFFER_SIZE 1000 // 384 Bytes aprox
#else
#define MIN_BUFFER_SIZE 150 //144 Bytes aprox
#endif

int main(int args, char** argv)
{
    char* ip = "localhost";
    char* port = "8888";
    uint32_t topic_length = (uint32_t)atoi(argv[1]);

    // Transport
    uxrUDPTransport transport;
    uxrUDPPlatform udp_platform;
    if(!uxr_init_udp_transport(&transport, &udp_platform, UXR_IPv4, ip, port))
    {
        return 1;
    }

    // Session
    uxrSession session;
    uxr_init_session(&session, &transport.comm, 0xAAAABBBB);
    if(!uxr_create_session(&session))
    {
        return 1;
    }

    // Streams
    uint32_t buffer_length = ((topic_length + 24) < MIN_BUFFER_SIZE) ? MIN_BUFFER_SIZE : topic_length + 24;

    uint8_t * output_reliable_stream_buffer = (uint8_t*)malloc((buffer_length)*sizeof(uint8_t));
    uxrStreamId reliable_out = uxr_create_output_reliable_stream(&session, output_reliable_stream_buffer, buffer_length, 1);

    uint8_t * input_reliable_stream_buffer = (uint8_t*)malloc((25)*sizeof(uint8_t));
    uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, buffer_length, 1);

#ifdef USE_XML_CREATION
    // Create entities
    uxrObjectId participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* participant_xml = "<dds>"
                                      "<participant>"
                                          "<rtps>"
                                              "<name>default_xrce_participant</name>"
                                          "</rtps>"
                                      "</participant>"
                                  "</dds>";
    uint16_t participant_req = uxr_buffer_create_participant_xml(&session, reliable_out, participant_id, 0, participant_xml, UXR_REPLACE);

    uxrObjectId topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    const char* topic_xml = "<dds>"
                                "<topic>"
                                    "<name>ByteArrayTopic</name>"
                                    "<dataType>ByteArray</dataType>"
                                "</topic>"
                            "</dds>";
    uint16_t topic_req = uxr_buffer_create_topic_xml(&session, reliable_out, topic_id, participant_id, topic_xml, UXR_REPLACE);

    uxrObjectId publisher_id = uxr_object_id(0x01, UXR_PUBLISHER_ID);
    const char* publisher_xml = "";
    uint16_t publisher_req = uxr_buffer_create_publisher_xml(&session, reliable_out, publisher_id, participant_id, publisher_xml, UXR_REPLACE);

    uxrObjectId datawriter_id = uxr_object_id(0x01, UXR_DATAWRITER_ID);
    const char* datawriter_xml = "<dds>"
                                     "<data_writer>"
                                         "<topic>"
                                             "<kind>NO_KEY</kind>"
                                             "<name>ByteArrayTopic</name>"
                                             "<dataType>ByteArray</dataType>"
                                         "</topic>"
                                     "</data_writer>"
                                 "</dds>";
    uint16_t datawriter_req = uxr_buffer_create_datawriter_xml(&session, reliable_out, datawriter_id, publisher_id, datawriter_xml, UXR_REPLACE);
#else 
    uxrObjectId participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* participant_ref = "default_xrce_participant";
    uint16_t participant_req = uxr_buffer_create_participant_ref(&session, reliable_out, participant_id, 0, participant_ref, UXR_REPLACE);

    uxrObjectId topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    const char* topic_ref = "valgrind_topic";
    uint16_t topic_req = uxr_buffer_create_topic_ref(&session, reliable_out, topic_id, participant_id, topic_ref, UXR_REPLACE);

    uxrObjectId publisher_id = uxr_object_id(0x01, UXR_PUBLISHER_ID);
    const char* publisher_xml = "";
    uint16_t publisher_req = uxr_buffer_create_publisher_xml(&session, reliable_out, publisher_id, participant_id, publisher_xml, UXR_REPLACE);

    uxrObjectId datawriter_id = uxr_object_id(0x01, UXR_DATAWRITER_ID);
    const char* datawriter_ref = "valgrind_data_writer";
    uint16_t datawriter_req = uxr_buffer_create_datawriter_ref(&session, reliable_out, datawriter_id, publisher_id, datawriter_ref, UXR_REPLACE);
#endif

    // Send create entities message and wait its status
    uint8_t status[4];
    uint16_t requests[4] = {participant_req, topic_req, publisher_req, datawriter_req};
    if(!uxr_run_session_until_all_status(&session, 1000, requests, status, 4))
    {
        return 1;
    }

    // Write topics
    ByteArray topic;
    topic.message = (char*)malloc((topic_length)*sizeof(char));
    memset(topic.message, 'A', topic_length);
    topic.message[topic_length] = '\0';

    int8_t count = 0;
    while(count++ < 30)
    {   
        ucdrBuffer ub;
        uint32_t topic_size = ByteArray_size_of_topic(&topic, 0);
        uxr_prepare_output_stream(&session, reliable_out, datawriter_id, &ub, topic_size);
        ByteArray_serialize_topic(&ub, &topic);

        uxr_run_session_time(&session, 5);
    }

    // Delete resources
    uxr_delete_session(&session);
    uxr_close_udp_transport(&transport);

    return 0;
}
