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
#include <microcdr/microcdr.h>

#include <stdio.h>
#include <string.h> //strcmp
#include <stdlib.h> //atoi

#define STREAM_HISTORY  8
#define BUFFER_SIZE     MR_CONFIG_UDP_TRANSPORT_MTU * STREAM_HISTORY

int main(int args, char** argv)
{
    // Args
    if(args < 5 || 0 == strcmp("-h", argv[1]) || 0 == strcmp("--help", argv[1])
                || 0 != strcmp("--key", argv[1]) || 0 != strcmp("--id", argv[3])
                || 0 == atoi(argv[2]) || 0 == atoi(argv[4]))
    {
        printf("usage: program [-h | --help | --key <number> --id <datawriter-number>]\n");
        return 0;
    }

    // Transport
    mrUDPTransport transport;
    if(!mr_init_udp_transport(&transport, "127.0.0.1", 2018))
    {
        printf("Error at create transport.\n");
        return 1;
    }

    // Session
    mrSession session;
    mr_init_session(&session, &transport.comm, (uint32_t)atoi(argv[2]));
    if(!mr_create_session(&session))
    {
        printf("Error at create session.\n");
        return 1;
    }

    // Streams
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    mrStreamId reliable_out = mr_create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    mr_create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    mrObjectId datawriter_id = mr_object_id((uint16_t)atoi(argv[4]), MR_DATAWRITER_ID);

    // Write topics
    bool connected = true;
    uint32_t count = 0;
    while(connected)
    {
        HelloWorld topic = {count++, "Hello DDS world!"};

        mcBuffer mb;
        uint32_t topic_size = HelloWorld_size_of_topic(&topic, 0);
        mr_prepare_output_stream(&session, reliable_out, datawriter_id, &mb, topic_size);
        HelloWorld_serialize_topic(&mb, &topic);

        connected = mr_run_session_time(&session, 1000);
        if(connected)
        {
            printf("Sent topic: %s, id: %i\n", topic.message, topic.index);
        }
    }

    // Delete resources
    mr_close_udp_transport(&transport);

    return 0;
}
