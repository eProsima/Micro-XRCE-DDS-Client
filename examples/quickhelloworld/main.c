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

String read_file(char* file_name)
{
    printf("READ FILE\n");
    const size_t MAXBUFLEN = 4096;
    char data[MAXBUFLEN];
    String xml = {data, 0};
    FILE *fp = fopen(file_name, "r");
    if (fp != NULL)
    {
        xml.length = fread(xml.data, sizeof(char), MAXBUFLEN, fp);
        if (xml.length == 0)
        {
            printf("Error reading %s\n", file_name);
        }
        fclose(fp);
    }
    else
    {
        printf("Error opening %s\n", file_name);
    }

     return xml;
}

// User callback for receiving status messages from the Agent.
void on_status(XRCEInfo info, uint8_t operation, uint8_t status, void* args)
{
    (void)info;
    (void)operation;
    (void)status;
    (void)args;
    // Process status message.
}

int main(int args, char** argv)
{
    (void)args;
    (void)argv;
    // Creates a client state.
    ClientState* state = new_udp_client_state(4096, "127.0.0.1", 2019, 2020);

    // Creates this client on the Agent.
    create_client(state, on_status, NULL);
    // Creates a participant on the Agent.
    XRCEInfo participant_info = create_participant(state);

    // Register a topic on the given participant. Uses a topic configuration read from xml file
    String topic_profile = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>", 86+1};
    create_topic(state, participant_info.object_id, topic_profile);


    // Creates a publisher on the given participant
    XRCEInfo publisher_info = create_publisher(state, participant_info.object_id);


    String data_writer_profile = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>",
    300+1};

    XRCEInfo data_writer_info = create_data_writer(state, participant_info.object_id, publisher_info.object_id, data_writer_profile); //read_file("hello_data_writer_profile.xml"));

    // Prepare and write the user data to be sent.
    char message[] = "Hello DDS world!";
    HelloWorld hello_topic = {1, message};
    // Write user type data.
    write_data(state, data_writer_info.object_id, serialize_HelloWorld_topic, &hello_topic);

    // Send the data through the UDP transport.
    send_to_agent(state);

    // Free all the ClientState resources.
    free_client_state(state);
}
