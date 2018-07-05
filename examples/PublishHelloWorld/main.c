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

#include <micrortps/client/client.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE     UDP_TRANSPORT_MTU * 8

//printf("%sStatus error (%i)", "\x1B[1;31m", status);
//printf(" at entity %04X with request: %04X%s\n", (object_id.id << 4) || object_id.type, request_id, "\x1B[0m");

int main(int args, char** argv)
{
    (void) args; (void) argv;
    //UserEntityRequest request;

    // Transport
    UDPTransport udp;
    init_udp_transport(&udp, "127.0.0.1", 2019);

    // Session
    Session session;
    create_session(&session, 129, 0xAABBCCDD, &udp.comm);

    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    StreamId reliable_out = create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE, 8);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, 8);

    // Create entities
    mrObjectId participant_id = create_object_id(0x01, PARTICIPANT_ID);
    char* participant_ref = "default participant";
    uint16_t participant_request = write_create_participant_ref(&session, reliable_out, participant_id, participant_ref, 0);

    mrObjectId topic_id = create_object_id(0x01, TOPIC_ID);
    char* topic_xml = "<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>";
    uint16_t topic_request = write_configure_topic_xml(&session, reliable_out, topic_id, participant_id, topic_xml, 0);

    mrObjectId publisher_id = create_object_id(0x01, PUBLISHER_ID);
    char* publisher_xml = "<publisher name=\"MyPublisher\"";
    uint16_t publisher_request = write_configure_publisher_xml(&session, reliable_out, publisher_id, participant_id, publisher_xml, 0);

    mrObjectId datawriter_id = create_object_id(0x01, DATAWRITER_ID);
    char* datawriter_xml = "<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>";
    uint16_t datawriter_request = write_configure_datawriter_xml(&session, reliable_out, datawriter_id, publisher_id, datawriter_xml, 0);


    uint16_t request[4] = {participant_request, topic_request, publisher_request, datawriter_request};
    //uint8_t answers[4] = {};
    //if(run_session_until_status(&session, 10000, request, answers, 4))
    {
        //TODO ha ido bien.
    }
    //else
    {
        //Algo ha ido mal, mirar en answers el valor del status.
    }

    // Send and receive messages
    //while(run_session(&session, 100) /* || things not created yet */);

    if(/* All things created */true)
    {
        for(int i = 0; i < 100; i++)
        {
            /*HelloWorld topic;
            topic.index = i;
            topic.message = "Hello DDS world!";
            write_HelloWorld(&my_session, datawriter_id, STREAMID_BUILTIN_RELIABLE, &topic);*/
            //while(run_session(&session, 5));
        }
    }

    // Delete resources
    delete_session(&session);

    return 0;
}

/*
    // Main loop
    int32_t count = 0;
    while(true)
    {
        // Write HelloWorld topic
        HelloWorld topic;
        topic.index = ++count;
        topic.message = "Hello DDS world!";
        write_HelloWorld(&my_session, datawriter_id, STREAMID_BUILTIN_RELIABLE, &topic);
        printf("Send topic: %s, count: %i\n", topic.message, topic.index);

        run_communication(&my_session);

        ms_sleep(1000);
    }

*/
