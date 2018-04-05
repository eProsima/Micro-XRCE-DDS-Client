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

size_t read_file(const char *file_name, char* data_file, size_t buf_size)
{
    printf("READ FILE\n");
    FILE *fp = fopen(file_name, "r");
    size_t length = 0;
    if (fp != NULL)
    {
        length = fread(data_file, sizeof(char), buf_size, fp);
        if (length == 0)
        {
            printf("Error reading %s\n", file_name);
        }
        fclose(fp);
    }
    else
    {
        printf("Error opening %s\n", file_name);
    }

    return length;
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Please, specify the agent port");
        return 1;
    }

    /* Init session. */
    Session my_session;
    ClientKey key = {{0xBB, 0xBB, 0xCC, 0xDD}};
    uint8_t ip[] = {127, 0, 0, 1};
    uint16_t port = atoi(argv[1]);
    if (!new_udp_session(&my_session, 0x01, key, ip, port, NULL, NULL))
    {
        return 1;
    }

    init_session_syn(&my_session);

    /* Init XRCE objects. */
    ObjectId participant_id = {{0x00, 0x01}};
    create_participant_sync_by_ref(&my_session, participant_id, "default_participant", false, false);

    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId topic_id = {{0x00, 0x02}};
    create_topic_sync_by_xml(&my_session, topic_id, topic_xml, participant_id, false, false);

    const char* publisher_xml = {"<publisher name=\"MyPublisher\""};
    ObjectId publisher_id = {{0x00, 0x03}};
    create_publisher_sync_by_xml(&my_session, publisher_id, publisher_xml, participant_id, false, false);

    const char* datawriter_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datawriter_id = {{0x00, 0x05}};
    create_datawriter_sync_by_xml(&my_session, datawriter_id, datawriter_xml, publisher_id, false, false);


    /* Main loop */
    int32_t count = 0;
    while(true)
    {
        /* Write HelloWorld topic */
        BestEffortStream* best_effort = &my_session.output_best_effort_stream;

        HelloWorld topic = {++count, "Hello DDS world!"};
        uint32_t topic_size = size_of_HelloWorld_topic(&topic);
        MicroBuffer* topic_buffer = prepare_best_effort_stream_for_topic(best_effort, datawriter_id, topic_size);
        if(topic_buffer != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer, &topic);
        }

        run_communication(&my_session);

        printf("Send topic: %s, count: %i\n", topic.m_message, topic.m_index);

        sleep(1);
    }


    close_session(&my_session);
}
