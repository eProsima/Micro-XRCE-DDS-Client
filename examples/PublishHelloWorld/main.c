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
#include <stdio.h>
#include <unistd.h> //for sleep function in linux

#define STREAM_HISTORY  8
#define BUFFER_SIZE     UDP_TRANSPORT_MTU * STREAM_HISTORY

int main(int args, char** argv)
{
    (void) args; (void) argv;

    // Transport
    UDPTransport transport;
    if(!init_udp_transport(&transport, "127.0.0.1", 2019))
    {
        printf("Error at create transport.\n");
        return 1;
    }

    // Session
    Session session;
    if(!create_session(&session, 0x01, 0xAAAABBBB, &transport.comm))
    {
        printf("Error at create session.\n");
        return 1;
    }

    // Streams
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    StreamId reliable_out = create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    // Create entities
    mrObjectId participant_id = create_object_id(0x01, PARTICIPANT_ID);
    char* participant_ref = "default participant";
    uint16_t participant_req = write_create_participant_ref(&session, reliable_out, participant_id, participant_ref, 0);

    mrObjectId topic_id = create_object_id(0x01, TOPIC_ID);
    char* topic_xml = "<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>";
    uint16_t topic_req = write_configure_topic_xml(&session, reliable_out, topic_id, participant_id, topic_xml, 0);

    mrObjectId publisher_id = create_object_id(0x01, PUBLISHER_ID);
    char* publisher_xml = "<publisher name=\"MyPublisher\"";
    uint16_t publisher_req = write_configure_publisher_xml(&session, reliable_out, publisher_id, participant_id, publisher_xml, 0);

    mrObjectId datawriter_id = create_object_id(0x01, DATAWRITER_ID);
    char* datawriter_xml = "<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>";
    uint16_t datawriter_req = write_configure_datawriter_xml(&session, reliable_out, datawriter_id, publisher_id, datawriter_xml, 0);

    // Send create entities message and wait its status
    uint8_t status[4];
    uint16_t requests[4] = {participant_req, topic_req, publisher_req, datawriter_req};
    if(!run_session_until_status(&session, 1000, requests, status, 4))
    {
        printf("Error at create entities: participant: %i topic: %i publisher: %i darawriter: %i\n", status[0], status[1], status[2], status[3]);
        return 1;
    }

    // Write 10 topics
    for(unsigned i = 0; i < 10; ++i)
    {
        HelloWorld topic = {i, "Hello DDS world!"};

        if(!write_HelloWorld_topic(&session, reliable_out, datawriter_id, &topic))
        {
            printf("Error writing the submessage 'write data topic', stream is full\n");
            i -= 1; //try again
        }

        if(run_session_until_confirm_delivery(&session, 10))
        {
            printf("Sent topic: %s, count: %i\n", topic.message, topic.index);
        }

        sleep(1);
    }

    // Delete resources
    delete_session(&session);
    //TODO: add the close transport functions

    return 0;
}
