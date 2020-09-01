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

#include <uxr/client/client.h>

#include <stdio.h> //printf
#include <string.h> //strcmp
#include <stdlib.h> //atoi
#include <pthread.h> 

#define TEST_TIME 10
uint32_t test_count = 0;
float samples[TEST_TIME];

// State
uint32_t count = 0;
uint32_t topic_size = 0;

pthread_mutex_t lock;


void *troughputCalc(void *vargp) 
{ 
    sleep(2);
    count=0;
    sleep(1);
    while (test_count < TEST_TIME)
    {
        pthread_mutex_lock(&lock);
        float rate = (count*topic_size*8)/1e6;
        uint32_t aux_count = count;
        count = 0;
        pthread_mutex_unlock(&lock);
        samples[test_count] = rate;
        fprintf(stderr,"Count %d, Size: %d, Rate %0.8f Mbps\n", aux_count, topic_size, rate);
        
        test_count++;
        sleep(1);
    }
} 

void on_topic(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uxrStreamId stream_id,
        struct ucdrBuffer* ub,
        uint16_t length,
        void* args)
{
    (void) session; (void) object_id; (void) request_id; (void) stream_id; (void) length;

    pthread_mutex_lock(&lock);
    count += 1;
    pthread_mutex_unlock(&lock);

}

int main(int args, char** argv)
{
    // CLI
    if(3 > args || 0 == atoi(argv[2]))
    {
        printf("usage: program [-h | --help] | ip port [<max_topics>]\n");
        return 0;
    }

    char* ip = argv[1];
    char* port = argv[2];
    topic_size = (uint32_t)atoi(argv[3]);
    uint32_t buffer_size = (topic_size < 1000) ? 1000 : topic_size;

    // Transport
    uxrUDPTransport transport;
    uxrUDPPlatform udp_platform;
    if(!uxr_init_udp_transport(&transport, &udp_platform, UXR_IPv4, ip, port))
    {
        printf("Error at create transport.\n");
        return 1;
    }

    // Session
    uxrSession session;
    uxr_init_session(&session, &transport.comm, 0xCCCCDDDD);
    uxr_set_topic_callback(&session, on_topic, &topic_size);
    if(!uxr_create_session(&session))
    {
        printf("Error at create session.\n");
        return 1;
    }

    // Streams

    uint8_t * output_best_effort_buffer = (uint8_t*)malloc(buffer_size*sizeof(uint8_t));
    uxrStreamId best_effort_out = uxr_create_output_best_effort_stream(&session, output_best_effort_buffer, buffer_size);
    
    static uint8_t input_reliable_stream_buffer[1000];
    uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, 1000, 1);

    uxrStreamId best_effort_in = uxr_create_input_best_effort_stream(&session);
    
    // Create entities
    uxrObjectId participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    uint16_t participant_req = uxr_buffer_create_participant_ref(&session, best_effort_out, participant_id, 0, "default_xrce_participant", UXR_REPLACE);

    uxrObjectId topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    uint16_t topic_req = uxr_buffer_create_topic_ref(&session, best_effort_out, topic_id, participant_id, "topic", UXR_REPLACE);

    uxrObjectId subscriber_id = uxr_object_id(0x01, UXR_SUBSCRIBER_ID);
    const char* subscriber_xml = "";
    uint16_t subscriber_req = uxr_buffer_create_subscriber_xml(&session, best_effort_out, subscriber_id, participant_id, subscriber_xml, UXR_REPLACE);

    uxrObjectId datareader_id = uxr_object_id(0x01, UXR_DATAREADER_ID);
    uint16_t datareader_req = uxr_buffer_create_datareader_ref(&session, best_effort_out, datareader_id, subscriber_id, "topic", UXR_REPLACE);

    // Send create entities message and wait its status
    uxr_run_session_until_timeout(&session, 100);

    // Request topics
    uxrDeliveryControl delivery_control = {0};
    delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    uint16_t read_data_req = uxr_buffer_request_data(&session, best_effort_out, datareader_id, best_effort_in, &delivery_control);
    
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, troughputCalc, NULL); 
    
    // Read topics
    while(test_count < TEST_TIME)
    {
        uxr_run_session_until_timeout(&session, 1000);
    }

    // Delete resources
    uxr_delete_session(&session);
    uxr_close_udp_transport(&transport);
    pthread_join(thread_id, NULL);

    float sum = 0;
    for (size_t i = 0; i < TEST_TIME; i++){
        sum += samples[i];
    }
    
    printf("%f\n",sum/TEST_TIME);
    return 0;
}
