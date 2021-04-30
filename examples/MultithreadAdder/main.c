// Copyright 2021-present Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <uxr/client/client.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>

#define STREAM_HISTORY  8
#define BUFFER_SIZE     UXR_CONFIG_UDP_TRANSPORT_MTU* STREAM_HISTORY

// Global objects

uxrSession session;

uxrStreamId reliable_out;
uxrStreamId reliable_in;

uxrObjectId participant_id;
uxrObjectId replier_id;


bool create_requester(
        uint8_t id)
{
    printf("Creating req %d\n", id);
    uxrObjectId requester_id = uxr_object_id(id, UXR_REQUESTER_ID);
    const char* requester_xml = "<dds>"
            "<requester profile_name=\"my_requester\""
            "service_name=\"service_name\""
            "request_type=\"request_type\""
            "reply_type=\"reply_type\">"
            "</requester>"
            "</dds>";
    uint16_t requester_req = uxr_buffer_create_requester_xml(&session, reliable_out, requester_id, participant_id,
                    requester_xml, UXR_REPLACE);

    uint16_t requests[1] = {
        requester_req
    };
    uint8_t status[1];

    if (!uxr_run_session_until_all_status(&session, 1000, requests, status, sizeof(status)))
    {
        printf("Error at create req: %i\n", status[0]);
        return NULL;
    }
}

// Publish thread

void* request(
        void* args)
{
    uint8_t id = *(uint8_t*) args;
    printf("Running req %d\n", id);

    uxrObjectId requester_id = uxr_object_id(id, UXR_REQUESTER_ID);

    // Write requests
    bool connected = true;
    uint32_t count = 0;

    while (connected)
    {
        uint8_t request[2 * 4] = {
            0
        };
        ucdrBuffer ub;

        ucdr_init_buffer(&ub, request, sizeof(request));

        // Method 1
        uxr_prepare_output_stream(&session, reliable_out, requester_id, &ub, sizeof(request));
        ucdr_serialize_uint32_t(&ub, count);
        ucdr_serialize_uint32_t(&ub, count);
        /*
           // Method 2
           ucdr_serialize_uint32_t(&ub, count);
           ucdr_serialize_uint32_t(&ub, count);
           uint16_t request_id = uxr_buffer_request(&session, reliable_out, requester_id, request, sizeof(request));
         */
        printf("Request sent: (%d + %d) [id: %d]\n", count, count, requester_id.id);
        connected = uxr_run_session_time(&session, 1000);

        ++count;
    }
}

void on_reply(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uint16_t reply_id,
        ucdrBuffer* ub,
        uint16_t length,
        void* args)
{
    (void) object_id;
    (void) request_id;
    (void) length;

    uint64_t result;
    ucdr_deserialize_uint64_t(ub, &result);

#ifdef WIN32
    printf("Reply received: %I64u [id: %d]\n", result, reply_id);
#else
    printf("Reply received: %" PRIu64 " [id: %d]\n", result, reply_id);
#endif /* ifdef WIN32 */
}

void on_request(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        SampleIdentity* sample_id,
        ucdrBuffer* ub,
        uint16_t length,
        void* args)
{
    (void) object_id;
    (void) request_id;
    (void) length;

    uint32_t rhs;
    uint32_t lhs;
    ucdr_deserialize_uint32_t(ub, &rhs);
    ucdr_deserialize_uint32_t(ub, &lhs);

    printf("Request received: (%d + %d)\n", rhs, lhs);

    uint8_t reply_buffer[8] = {
        0
    };
    ucdrBuffer reply_ub;
    ucdr_init_buffer(&reply_ub, reply_buffer, sizeof(reply_buffer));

    // Method 1
    uxr_prepare_output_stream(session, reliable_out, replier_id, &reply_ub, sizeof(reply_buffer));
    uxr_serialize_SampleIdentity(ub, sample_id);
    ucdr_serialize_uint64_t(&reply_ub, rhs + lhs);
    /*
       // Method 2
       ucdr_serialize_uint64_t(&reply_ub, rhs + lhs);
       uxr_buffer_reply(session, reliable_out, replier_id, sample_id, reply_buffer, sizeof(reply_buffer));
     */
#ifdef WIN32
    printf("Reply send: %I64u\n", (uint64_t)(rhs + lhs));
#else
    printf("Reply send: %" PRIu64 "\n", (uint64_t)(rhs + lhs));
#endif /* ifdef WIN32 */
}

int main(
        int args,
        char** argv)
{
    // CLI
    if (3 > args || 0 == atoi(argv[2]))
    {
        printf("usage: program [-h | --help] | ip port\n");
        return 0;
    }

    char* ip = argv[1];
    char* port = argv[2];

    // Transport
    uxrUDPTransport transport;
    if (!uxr_init_udp_transport(&transport, UXR_IPv4, ip, port))
    {
        printf("Error at create transport.\n");
        return 1;
    }

    // Session
    uxr_init_session(&session, &transport.comm, 0xABCDABCD);
    uxr_set_reply_callback(&session, on_reply, false);
    uxr_set_request_callback(&session, on_request, 0);
    if (!uxr_create_session(&session))
    {
        printf("Error at create session.\n");
        return 1;
    }

    // Streams
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    reliable_out = uxr_create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE,
                    STREAM_HISTORY);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    reliable_in = uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    // Create entities
    participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* participant_xml = "<dds>"
            "<participant>"
            "<rtps>"
            "<name>default_xrce_participant</name>"
            "</rtps>"
            "</participant>"
            "</dds>";
    uint16_t participant_req = uxr_buffer_create_participant_xml(&session, reliable_out, participant_id, 0,
                    participant_xml, UXR_REPLACE);

    replier_id = uxr_object_id(0x01, UXR_REPLIER_ID);
    const char* replier_xml = "<dds>"
            "<replier profile_name=\"my_requester\""
            "service_name=\"service_name\""
            "request_type=\"request_type\""
            "reply_type=\"reply_type\">"
            "</replier>"
            "</dds>";
    uint16_t replier_req = uxr_buffer_create_replier_xml(&session, reliable_out, replier_id, participant_id,
                    replier_xml, UXR_REPLACE);

    uint16_t requests[2] = {
        participant_req, replier_req
    };
    uint8_t status[2];
    if (!uxr_run_session_until_all_status(&session, 1000, requests, status, sizeof(status)))
    {
        printf("Error at create entities: participant: %i requester: %i\n", status[0], status[1]);
        return 1;
    }

    // Request topics
    uxrDeliveryControl delivery_control = {
        0
    };
    delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    uint16_t read_data_req =
            uxr_buffer_request_data(&session, reliable_out, replier_id, reliable_in, &delivery_control);

    pthread_t tid[1];
    uint8_t* thread_args = (uint8_t*) malloc(sizeof(tid) / sizeof(pthread_t));

    for (size_t i = 0; i < sizeof(tid) / sizeof(pthread_t); i++)
    {
        create_requester(i);
    }

    for (size_t i = 0; i < sizeof(tid) / sizeof(pthread_t); i++)
    {
        thread_args[i] = i;
        pthread_create(&tid[i], NULL, request, (void*) &thread_args[i]);
    }

    uxr_run_session_until_timeout(&session, UXR_TIMEOUT_INF);

    for (size_t i = 0; i < sizeof(tid) / sizeof(pthread_t); i++)
    {
        pthread_join(tid[i], NULL);
    }

    // Delete resources
    uxr_delete_session(&session);
    uxr_close_udp_transport(&transport);

    return 0;
}
