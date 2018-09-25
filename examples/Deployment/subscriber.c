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

void on_topic(mrSession* session, mrObjectId object_id, uint16_t request_id, mrStreamId stream_id, struct MicroBuffer* mb, void* args)
{
    (void) session; (void) object_id; (void) request_id; (void) stream_id; (void) args;

    HelloWorld topic;
    HelloWorld_deserialize_topic(mb, &topic);

    printf("Received topic: %s, id: %i\n", topic.message, topic.index);
}

int main(int args, char** argv)
{
    // Args
    if(args < 5 || 0 == strcmp("-h", argv[1]) || 0 == strcmp("--help", argv[1])
                || 0 != strcmp("--key", argv[1]) || 0 != strcmp("--id", argv[3])
                || 0 == atoi(argv[2]) || 0 == atoi(argv[4]))
    {
        printf("usage: program [-h | --help | --key <number> --id <datareader-number>]\n");
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
    mr_set_topic_callback(&session, on_topic, NULL);
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

    // Request topics
    mrDeliveryControl delivery_control = {0};
    delivery_control.max_samples = MR_MAX_SAMPLES_UNLIMITED;

    mrObjectId datareader_id = mr_object_id((uint16_t)atoi(argv[4]), MR_DATAREADER_ID);
    uint16_t read_data_req = mr_write_request_data(&session, reliable_out, datareader_id, reliable_in, &delivery_control);

    // Read topics
    bool connected = true;
    while(connected)
    {
        uint8_t read_data_status;
        connected = mr_run_session_until_all_status(&session, MR_TIMEOUT_INF, &read_data_req, &read_data_status, 1);
    }

    // Delete resources
    mr_close_udp_transport(&transport);

    return 0;
}
