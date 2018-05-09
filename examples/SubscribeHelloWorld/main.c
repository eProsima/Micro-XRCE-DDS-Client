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

#include <stdio.h>

#define HELLO_WORLD_TOPIC 0x01

void check_and_print_error(Session* session)
{
    if(session->last_status_received)
    {
        if(session->last_status.status != STATUS_OK)
        {
            printf("%sStatus error (%i)%s\n", "\x1B[1;31m", session->last_status.status, "\x1B[0m");
        }
        else
        {
            //All things go well
        }
    }
    else
    {
        printf("%sConnection error%s\n", "\x1B[1;31m", "\x1B[0m");
    }
}

void on_topic(ObjectId id, MicroBuffer* serialized_topic, void* args)
{
    (void) args;
    if(HELLO_WORLD_TOPIC == id.data[0])
    {
        HelloWorld topic;
        deserialize_HelloWorld_topic(serialized_topic, &topic);

        printf("Receive topic: %s, count: %i\n", topic.message, topic.index);
    }
}

int main(int args, char** argv)
{
    Session my_session;
    ClientKey key = {{0xBB, 0xBB, 0xBB, 0xBB}};
    if(args == 3 && strcmp(argv[1], "serial") == 0)
    {
        const char* device = argv[2];
        if(!new_serial_session(&my_session, 0x01, key, device, on_topic, NULL))
        {
            printf("%sCan not create serial connection%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        printf("<< Serial mode => dev: %s >>\n", device);
    }
    else if(args == 4 && strcmp(argv[1], "udp") == 0)
    {
        uint8_t ip[4];
        int length = sscanf(argv[2], "%d.%d.%d.%d", (int*)&ip[0], (int*)&ip[1], (int*)&ip[2], (int*)&ip[3]);
        if(length != 4)
        {
            printf("%sIP must have th format a.b.c.d%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }

        uint16_t port = atoi(argv[3]);
        if(!new_udp_session(&my_session, 0x01, key, ip, port, on_topic, NULL))
        {
            printf("%sCan not create a socket%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        printf("<< UDP mode => ip: %s - port: %hu >>\n", argv[2], port);
    }
    else
    {
        printf("Usage: program <command>\n");
        printf("List of commands:\n");
        printf("    serial device\n");
        printf("    udp agent_ip agent_port\n");
        printf("    help\n");
        return 1;
    }

    /* Init session. */
    init_session_sync(&my_session);
    check_and_print_error(&my_session);

    /* Init XRCE objects. */
    ObjectId participant_id = {{0x00, OBJK_PARTICIPANT}};
    create_participant_sync_by_ref(&my_session, participant_id, "default_participant", false, false);
    check_and_print_error(&my_session);

    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId topic_id = {{0x00, OBJK_TOPIC}};
    create_topic_sync_by_xml(&my_session, topic_id, topic_xml, participant_id, false, false);
    check_and_print_error(&my_session);

    const char* subscriber_xml = {"<subscriber name=\"MySubscriber\""};
    ObjectId subscriber_id = {{HELLO_WORLD_TOPIC, OBJK_SUBSCRIBER}};
    create_subscriber_sync_by_xml(&my_session, subscriber_id, subscriber_xml, participant_id, false, false);
    check_and_print_error(&my_session);

    const char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
    ObjectId datareader_id = {{HELLO_WORLD_TOPIC, OBJK_DATAREADER}};
    create_datareader_sync_by_xml(&my_session, datareader_id, datareader_xml, subscriber_id, false, false);
    check_and_print_error(&my_session);


    /* Main loop */
    while(true)
    {
        /* Request data */
        read_data_sync(&my_session, datareader_id, STREAMID_BUILTIN_RELIABLE);
        check_and_print_error(&my_session);

        run_communication(&my_session);

        ms_sleep(1000);
    }

    close_session_sync(&my_session);
    free_session(&my_session);

    return 0;
}
