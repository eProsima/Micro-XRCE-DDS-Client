// Copyright 2021-present Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include "HelloWorld.h"

#include <uxr/client/client.h>
#include <ucdr/microcdr.h>

#include <stdio.h> //printf
#include <string.h> //strcmp
#include <stdlib.h> //atoi
#include <pthread.h>
#include <unistd.h>

#define STREAM_HISTORY  8
#define BUFFER_SIZE     UXR_CONFIG_UDP_TRANSPORT_MTU * STREAM_HISTORY

// Global objects

uxrSession session;
uxrStreamId reliable_out;
uxrStreamId reliable_in;
uxrObjectId participant_id;

// Create publisher function

bool create_publisher(uint8_t id)
{   
    printf("Creating pub %d\n", id);
    uxrObjectId publisher_id = uxr_object_id(id, UXR_PUBLISHER_ID);
    const char* publisher_xml = "";
    uint16_t publisher_req = uxr_buffer_create_publisher_xml(&session, reliable_out, publisher_id, participant_id, publisher_xml, UXR_REPLACE);

    uxrObjectId datawriter_id = uxr_object_id(id, UXR_DATAWRITER_ID);
    const char* datawriter_xml = "<dds>"
                                     "<data_writer>"
                                         "<topic>"
                                             "<kind>NO_KEY</kind>"
                                             "<name>HelloWorldTopic</name>"
                                             "<dataType>HelloWorld</dataType>"
                                         "</topic>"
                                     "</data_writer>"
                                 "</dds>";
    uint16_t datawriter_req = uxr_buffer_create_datawriter_xml(&session, reliable_out, datawriter_id, publisher_id, datawriter_xml, UXR_REPLACE);
    
    uint16_t requests[] = {publisher_req, datawriter_req};
    uint8_t status[sizeof(requests)/2];

    if(!uxr_run_session_until_all_status(&session, 1000, requests, status, sizeof(status)))
    {
        printf("Error at create pub: participant: %i topic: %i\n", status[0], status[1]);
        return NULL;
    }
}

// Publish thread

void * publish(void * args)
{ 
    uint8_t id = *(uint8_t*) args;
    printf("Running pub %d\n", id);

    uxrObjectId publisher_id = uxr_object_id(id, UXR_PUBLISHER_ID);
    uxrObjectId datawriter_id = uxr_object_id(id, UXR_DATAWRITER_ID);

    // Write topics
    HelloWorld topic;
    topic.index = 0;
    sprintf(topic.message, "Thread %d - topic %d", id, topic.index);

    uint32_t period_us = ((1 + rand()) % 10) * 1e5;

    while(1)
    {
        uint32_t topic_size = HelloWorld_size_of_topic(&topic, 0);

        ucdrBuffer ub;
        uint8_t * buffer = (uint8_t*) malloc(topic_size);
        ucdr_init_buffer(&ub, buffer, topic_size);
        HelloWorld_serialize_topic(&ub, &topic); 
        
        uint16_t req = uxr_buffer_topic(&session, reliable_out, datawriter_id, buffer, topic_size);

        if (0 == req)
        {
            printf("Error\n");
        }
        else
        {
            printf("Thread %d - topic %d\n", id, topic.index);

            sprintf(topic.message, "Thread %d - topic %d", id, ++topic.index);

            if(!uxr_run_session_until_confirm_delivery(&session, UXR_TIMEOUT_INF)){
                printf("Error\n");
            }

            usleep(period_us);
        }
    }
}

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

    HelloWorld topic;
    HelloWorld_deserialize_topic(ub, &topic);

    printf("Received topic: %s, id: %i\n", topic.message, topic.index);
}

int main(int args, char** argv)
{
    // CLI
    if(3 > args || 0 == atoi(argv[2]))
    {
        printf("usage: program [-h | --help] | ip port\n");
        return 0;
    }

    char* ip = argv[1];
    char* port = argv[2];

    // Transport
    uxrUDPTransport transport;
    if(!uxr_init_udp_transport(&transport, UXR_IPv4, ip, port))
    {
        printf("Error at create transport.\n");
        return 1;
    }

    // Session
    uxr_init_session(&session, &transport.comm, 0xABCDABCD);
    uxr_set_topic_callback(&session, on_topic, NULL);
    if(!uxr_create_session(&session))
    {
        printf("Error at create session.\n");
        return 1;
    }

    // Streams
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    reliable_out = uxr_create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    reliable_in = uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    // Create entities
    participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
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
                                    "<name>HelloWorldTopic</name>"
                                    "<dataType>HelloWorld</dataType>"
                                "</topic>"
                            "</dds>";
    uint16_t topic_req = uxr_buffer_create_topic_xml(&session, reliable_out, topic_id, participant_id, topic_xml, UXR_REPLACE);
    
    uxrObjectId subscriber_id = uxr_object_id(0x01, UXR_SUBSCRIBER_ID);
    const char* subscriber_xml = "";
    uint16_t subscriber_req = uxr_buffer_create_subscriber_xml(&session, reliable_out, subscriber_id, participant_id, subscriber_xml, UXR_REPLACE);

    uxrObjectId datareader_id = uxr_object_id(0x01, UXR_DATAREADER_ID);
    const char* datareader_xml = "<dds>"
                                     "<data_reader>"
                                         "<topic>"
                                             "<kind>NO_KEY</kind>"
                                             "<name>HelloWorldTopic</name>"
                                             "<dataType>HelloWorld</dataType>"
                                         "</topic>"
                                     "</data_reader>"
                                 "</dds>";
    uint16_t datareader_req = uxr_buffer_create_datareader_xml(&session, reliable_out, datareader_id, subscriber_id, datareader_xml, UXR_REPLACE);

    uint16_t requests[] = {participant_req, topic_req, subscriber_req, datareader_req};
    uint8_t status[sizeof(requests)/2];
    if(!uxr_run_session_until_all_status(&session, 1000, requests, status, sizeof(status)))
    {
        printf("Error at create entities: participant: %i topic: %i\n", status[0], status[1]);
        return 1;
    }

    // Request topics
    uxrDeliveryControl delivery_control = {0};
    delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    uint16_t read_data_req = uxr_buffer_request_data(&session, reliable_out, datareader_id, reliable_in, &delivery_control);

    pthread_t tid[3];
    uint8_t * thread_args = (uint8_t*) malloc(sizeof(tid)/sizeof(pthread_t));

    for (size_t i = 0; i < sizeof(tid)/sizeof(pthread_t); i++)
    {
        create_publisher(i);
    }

    for (size_t i = 0; i < sizeof(tid)/sizeof(pthread_t); i++)
    {
        thread_args[i] = i;
        pthread_create(&tid[i], NULL, publish, (void *) &thread_args[i]);
    }

    uxr_run_session_until_timeout(&session, UXR_TIMEOUT_INF);

    for (size_t i = 0; i < sizeof(tid)/sizeof(pthread_t); i++)
    {
        pthread_join(tid[i], NULL);
    }

    // Delete resources
    uxr_delete_session(&session);
    uxr_close_udp_transport(&transport);

    return 0;
}
