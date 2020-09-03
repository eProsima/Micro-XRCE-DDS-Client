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

#include <uxr/client/client.h>
#include <uxr/client/brokerless/brokerless.h>
#include <ucdr/microcdr.h>

#include <stdio.h> //printf
#include <string.h> //strcmp
#include <stdlib.h> //atoi
#include <unistd.h> //sleep

#define BUFFER_SIZE     UXR_CONFIG_UDP_TRANSPORT_MTU

int main(int args, char** argv)
{
    // Session
    uxrSession session;
    uxr_init_session(&session, &brokerless_comm_stub, 0xAAAABBBB);

    // Streams
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    uxrStreamId best_effort_output = uxr_create_output_best_effort_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE);

    // Create entities
    uxrObjectId participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* participant_xml = "<dds>"
                                      "<participant>"
                                          "<rtps>"
                                              "<name>default_xrce_participant</name>"
                                          "</rtps>"
                                      "</participant>"
                                  "</dds>";
    uint16_t participant_req = uxr_buffer_create_participant_xml(&session, best_effort_output, participant_id, 0, participant_xml, UXR_REPLACE);

    uxrObjectId topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    const char* topic_xml = "<dds>"
                                "<topic>"
                                    "<name>HelloWorldTopic</name>"
                                    "<dataType>HelloWorld</dataType>"
                                "</topic>"
                            "</dds>";
    uint16_t topic_req = uxr_buffer_create_topic_xml(&session, best_effort_output, topic_id, participant_id, topic_xml, UXR_REPLACE);

    uxrObjectId publisher_id = uxr_object_id(0x01, UXR_PUBLISHER_ID);
    const char* publisher_xml = "";
    uint16_t publisher_req = uxr_buffer_create_publisher_xml(&session, best_effort_output, publisher_id, participant_id, publisher_xml, UXR_REPLACE);

    uxrObjectId datawriter_id = uxr_object_id(0x01, UXR_DATAWRITER_ID);
    const char* datawriter_xml = "<dds>"
                                     "<data_writer>"
                                         "<topic>"
                                             "<kind>NO_KEY</kind>"
                                             "<name>HelloWorldTopic</name>"
                                             "<dataType>HelloWorld</dataType>"
                                         "</topic>"
                                     "</data_writer>"
                                 "</dds>";
    uint16_t datawriter_req = uxr_buffer_create_datawriter_xml(&session, best_effort_output, datawriter_id, publisher_id, datawriter_xml, UXR_REPLACE);

    // Write topics
    char data[50];  
    data[49] = '\0';
    memset(data, 96+1, 49);
  
    while(1)
    {
        ucdrBuffer ub;

        memset(data, 96+((data[0]-96+1)%25), 49);

        uxr_prepare_output_stream(&session, best_effort_output, datawriter_id, &ub, 50);
        ucdr_serialize_array_char(&ub, data, 50);

        printf("Send topic: %s\n", data);
        
        uxr_run_session_timeout(&session, 10);
        sleep(1);
    }

    // Delete resources
    uxr_delete_session(&session);

    return 0;
}
