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

#include <unistd.h>
#include <fcntl.h>

void on_status(Session* session, mrObjectId id, uint16_t request_id, uint8_t status, void* args)
{
    (void) session; (void) id; (void) request_id; (void) status; (void) args;
    //printf("Status received!\n");
}

void on_topic(Session* session, mrObjectId id, uint16_t request_id, MicroBuffer* mb, StreamId stream_id, void* args)
{
    (void) session; (void) id; (void) request_id; (void) mb; (void) stream_id; (void) args;
    //printf("Topic received!\n");
}

#define BUFFER_SIZE     UDP_TRANSPORT_MTU * 8
int main(int args, char** argv)
{
    (void) args; (void) argv;

    // Transport
    //UARTTransport transport;
    //int fd = open("/dev/pts/8", O_RDWR | O_NOCTTY | O_NONBLOCK);
    //init_uart_transport_fd(&transport, fd, 0x00, 0x01);

    UDPTransport transport;
    init_udp_transport(&transport, "127.0.0.1", 2019);

    //TCPTransport transport;
    //init_tcp_transport(&transport, "127.0.0.1", 2019);

    // Session
    Session session;
    create_session(&session, 129, 0xAABBCCDD, &transport.comm);
    set_status_callback(&session, on_status, NULL);
    set_topic_callback(&session, on_topic, NULL);

    // Streams
    //uint8_t output_best_effort_stream_buffer[256];
    //StreamId best_effort_out = create_output_best_effort_stream(&session, output_best_effort_stream_buffer, 256);

    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    StreamId reliable_out = create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE, 8);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, 8);

    // Create entities
    mrObjectId participant_id = create_object_id(0x01, PARTICIPANT_ID);
    char* participant_ref = "default participant";
    write_create_participant_ref(&session, reliable_out, participant_id, participant_ref, 0);

    mrObjectId topic_id = create_object_id(0x01, TOPIC_ID);
    char* topic_xml = "<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>";
    write_configure_topic_xml(&session, reliable_out, topic_id, participant_id, topic_xml, 0);

    mrObjectId subscriber_id = create_object_id(0x01, SUBSCRIBER_ID);
    char* subscriber_xml = {"<subscriber name=\"MySubscriber\""};
    write_configure_subscriber_xml(&session, reliable_out, subscriber_id, participant_id, subscriber_xml, 0);

    mrObjectId datareader_id = create_object_id(0x01, DATAREADER_ID);
    char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
    write_configure_datareader_xml(&session, reliable_out, datareader_id, subscriber_id, datareader_xml, 0);

    // Configure read
    DeliveryControl delivery_control;
    delivery_control.max_bytes_per_second = 1024;
    delivery_control.max_elapsed_time = 100;
    delivery_control.max_samples = 50;
    delivery_control.min_pace_period = 1;
    write_read_data(&session, reliable_out, datareader_id, MR_FORMAT_DATA, NULL);

    // Send and receive messages
    run_session(&session, 20, 50);

    // Delete resources
    delete_session(&session);

    return 0;
}

/*void check_and_print_error(Session* session, const char* where)
{
    if(session->last_status_received)
    {
        if(session->last_status.status == STATUS_OK)
        {
            return; //All things go well
        }
        else
        {
            printf("%sStatus error (%i)%s", "\x1B[1;31m", session->last_status.status, "\x1B[0m");
        }
    }
    else
    {
        printf("%sConnection error", "\x1B[1;31m");
    }

    printf(" at %s%s\n", where, "\x1B[0m");
    exit(1);
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

        uint16_t port = (uint16_t)atoi(argv[3]);
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

    init_session_sync(&my_session);
    check_and_print_error(&my_session, "init_session");

    ObjectId participant_id = {{0x00, OBJK_PARTICIPANT}};
    create_participant_sync_by_ref(&my_session, participant_id, "default_participant", false, false);
    check_and_print_error(&my_session, "create participant");

    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId topic_id = {{0x00, OBJK_TOPIC}};
    create_topic_sync_by_xml(&my_session, topic_id, topic_xml, participant_id, false, false);
    check_and_print_error(&my_session, "create topic");

    const char* subscriber_xml = {"<subscriber name=\"MySubscriber\""};
    ObjectId subscriber_id = {{HELLO_WORLD_TOPIC, OBJK_SUBSCRIBER}};
    create_subscriber_sync_by_xml(&my_session, subscriber_id, subscriber_xml, participant_id, false, false);
    check_and_print_error(&my_session, "create subscriber");

    const char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
    ObjectId datareader_id = {{HELLO_WORLD_TOPIC, OBJK_DATAREADER}};
    create_datareader_sync_by_xml(&my_session, datareader_id, datareader_xml, subscriber_id, false, false);
    check_and_print_error(&my_session, "create datareader");

    while(true)
    {
        read_data_sync(&my_session, datareader_id, STREAMID_BUILTIN_RELIABLE);
        check_and_print_error(&my_session, "read_data");

        run_communication(&my_session);

        ms_sleep(1000);
    }

    close_session_sync(&my_session);
    check_and_print_error(&my_session, "close session");

    free_session(&my_session);

    return 0;
}
*/
