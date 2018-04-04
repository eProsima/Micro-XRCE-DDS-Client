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

#define HELLO_WORLD_TOPIC 0x01

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

void on_topic(ObjectId id, MicroBuffer* serialized_topic, void* args)
{
    if(HELLO_WORLD_TOPIC == id.data[0])
    {
        HelloWorld topic;
        deserialize_HelloWorld_topic(serialized_topic, &topic);

        printf("Receive topic: %s, count: %i\n", topic.m_message, topic.m_index);
    }
}

int main()
{
    /* Init session. */
    Session my_session;
    ClientKey key = {{0xAA, 0xBB, 0xCC, 0xDD}};
    micrortps_locator_t locator;
    uint8_t ip[] = {127, 0, 0, 1};
    if (MICRORTPS_STATUS_OK != new_udp_session(&my_session, 0x01, key, 2019, ip, &locator, on_topic, NULL))
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

    const char* subscriber_xml = {"<publisher name=\"MySubscriber\""};
    ObjectId subscriber_id = {{HELLO_WORLD_TOPIC, 0x04}};
    create_subscriber_sync_by_xml(&my_session, subscriber_id, subscriber_xml, participant_id, false, false);

    const char* datareader_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datareader_id = {{0x00, 0x06}};
    create_datawriter_sync_by_xml(&my_session, datareader_id, datareader_xml, subscriber_id, false, false);


    /* Request data */
    read_data_sync(&my_session, datareader_id);


    /* Main loop */
    while(true)
    {
        run_communication(&my_session);

        sleep(1);
    }


    close_session(&my_session);
}
