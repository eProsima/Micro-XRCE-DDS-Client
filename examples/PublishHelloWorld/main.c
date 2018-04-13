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
#include <unistd.h>

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

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("Usage: program agent_ip agent_port\n");
        return 1;
    }

    /* Init session. */
    Session my_session;
    ClientKey key = {{0xBB, 0xBB, 0xCC, 0xDD}};
    uint8_t ip[] = {atoi(strtok(argv[1], ".")), atoi(strtok(NULL, ".")),
                    atoi(strtok(NULL, ".")), atoi(strtok(NULL, "."))};
    uint16_t port = atoi(argv[2]);
    if (!new_udp_session(&my_session, 0x01, key, ip, port, NULL, NULL))
    {
        return 1;
    }

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

    const char* publisher_xml = {"<publisher name=\"MyPublisher\""};
    ObjectId publisher_id = {{0x00, OBJK_PUBLISHER}};
    create_publisher_sync_by_xml(&my_session, publisher_id, publisher_xml, participant_id, false, false);
    check_and_print_error(&my_session);

    const char* datawriter_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datawriter_id = {{0x00, OBJK_DATAWRITER}};
    create_datawriter_sync_by_xml(&my_session, datawriter_id, datawriter_xml, publisher_id, false, false);
    check_and_print_error(&my_session);


    /* Main loop */
    int32_t count = 0;
    while(true)
    {
        /* Write HelloWorld topic */
        HelloWorld topic = {++count, "Hello DDS world!"};
        write_HelloWorld(&my_session, datawriter_id, STREAMID_BUILTIN_RELIABLE, &topic);
        printf("Send topic: %s, count: %i\n", topic.message, topic.index);

        run_communication(&my_session);

        sleep(1);
    }

    close_session_sync(&my_session);

    return 0;
}
