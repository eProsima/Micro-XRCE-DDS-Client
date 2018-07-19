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

#include "HelloWorld.h"

#include <micrortps/client/client.h>
#include <string.h> //strcmp
#include <stdlib.h> //atoi
#include <stdio.h>

#define STREAM_HISTORY  8
#define BUFFER_SIZE     MR_CONFIG_UDP_TRANSPORT_MTU * STREAM_HISTORY

void on_topic(mrSession* session, mrObjectId id, uint16_t request_id, mrStreamId stream_id, struct MicroBuffer* mb, void* args)
{
    (void) session; (void) id; (void) request_id; (void) stream_id; (void) args;

    HelloWorld topic;
    mr_deserialize_HelloWorld_topic(mb, &topic);

    printf("Received topic: %s, id: %i\n", topic.message, topic.index);

    uint32_t* count_ptr = (uint32_t*) args;
    (*count_ptr)++;
}

int main(int args, char** argv)
{
    if(args >= 2 && (0 == strcmp("-h", argv[1]) || 0 == strcmp("--help", argv[1]) || 0 == atoi(argv[1])))
    {
        printf("usage: program [-h | --help | <topics>]\n");
        return 0;
    }

    uint32_t count = 0;
    uint32_t max_topics = (args == 2) ? (uint32_t)atoi(argv[1]) : UINT32_MAX;

    // Transport
    mrUDPTransport transport;
    if(!mr_init_udp_transport(&transport, "127.0.0.1", 2019))
    {
        printf("Error at create transport.\n");
        return 1;
    }

    // Session
    mrSession session;
    mr_init_session(&session, &transport.comm, 0xCCCCDDDD);
    mr_set_topic_callback(&session, on_topic, &count);
    if(!mr_create_session(&session))
    {
        printf("Error at create session.\n");
        return 1;
    }

    // Streams
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    mrStreamId reliable_out = mr_create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    mrStreamId reliable_in = mr_create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    // Create entities
    mrObjectId participant_id = mr_object_id(0x01, MR_PARTICIPANT_ID);
    const char* participant_ref = "default participant";
    uint16_t participant_req = mr_write_create_participant_ref(&session, reliable_out, participant_id, participant_ref, MR_REPLACE);

    mrObjectId topic_id = mr_object_id(0x01, MR_TOPIC_ID);
    const char* topic_xml = "<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>";
    uint16_t topic_req = mr_write_configure_topic_xml(&session, reliable_out, topic_id, participant_id, topic_xml, MR_REPLACE);

    mrObjectId subscriber_id = mr_object_id(0x01, MR_SUBSCRIBER_ID);
    const char* subscriber_xml = "<subscriber name=\"MySubscriber\"";
    uint16_t subscriber_req = mr_write_configure_subscriber_xml(&session, reliable_out, subscriber_id, participant_id, subscriber_xml, MR_REPLACE);

    mrObjectId datareader_id = mr_object_id(0x01, MR_DATAREADER_ID);
    const char* datareader_xml = "<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>";
    uint16_t datareader_req = mr_write_configure_datareader_xml(&session, reliable_out, datareader_id, subscriber_id, datareader_xml, MR_REPLACE);

    // Send create entities message and wait its status
    uint8_t status[4];
    uint16_t requests[4] = {participant_req, topic_req, subscriber_req, datareader_req};
    if(!mr_run_session_until_status(&session, 1000, requests, status, 4))
    {
        printf("Error at create entities: participant: %i topic: %i subscriber: %i datareader: %i\n", status[0], status[1], status[2], status[3]);
        return 1;
    }

    // Request topics
    mrDeliveryControl delivery_control = {0};
    delivery_control.max_samples = MR_MAX_SAMPLES_UNLIMITED;
    uint16_t read_data_req = mr_write_request_data(&session, reliable_out, datareader_id, reliable_in, &delivery_control);

    // Read topics
    bool connected = true;
    while(connected && count < max_topics)
    {
        uint8_t read_data_status;
        connected = mr_run_session_until_status(&session, MR_TIMEOUT_INF, &read_data_req, &read_data_status, 1);
    }

    // Delete resources
    mr_delete_session(&session);
    //TODO: add the close transport functions

    return 0;
}
