/*
 * Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <micrortps/client/client.h>
#include <stdio.h>

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <sys/select.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define XML_BUFFER_SIZE 1024
#define SHAPE_TOPIC 0x01

#define BUFFER_SIZE 512
typedef struct StreamBuffers
{
    uint8_t output_best_effort_stream_buffer[BUFFER_SIZE];
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE * 8];
    uint8_t input_reliable_stream_buffer[BUFFER_SIZE * 8];

} StreamBuffers;

static void help(void);
static void list_commands(void);
static int check_input(void);
static void on_status(Session* session, mrObjectId id, uint16_t request_id, uint8_t status, void* args);
static void on_topic(Session* session, mrObjectId object_id, uint16_t request_id, StreamId stream_id, MicroBuffer* serialization, void* args);
static bool compute_command(const char* command, Session* session, Communication* comm, StreamBuffers* stream_buffers);

int main(int args, char** argv)
{
    printf("<< SHAPES DEMO XRCE CLIENT >>\n");

    Session session;
    Communication* comm;

    if(args == 3 && strcmp(argv[1], "serial") == 0)
    {
        char* device = argv[2];
        UARTTransport uart;
        comm = &uart.comm;
        int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if(!init_uart_transport_fd(&uart, fd, 0x00, 0x01))
        {
            printf("%sCan not create serial connection%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        printf("<< Serial mode => dev: %s >>\n", device);
    }
    else if(args == 4 && strcmp(argv[1], "udp") == 0)
    {
        char* ip = argv[2];
        uint16_t port = (uint16_t)atoi(argv[3]);
        UDPTransport udp;
        comm = &udp.comm;
        if(!init_udp_transport(&udp, ip, port))
        {
            printf("%sCan not create an udp socket%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        printf("<< UDP mode => ip: %s - port: %hu >>\n", argv[2], port);
    }
    else if(args == 4 && strcmp(argv[1], "tcp") == 0)
    {
        char* ip = argv[2];
        uint16_t port = (uint16_t)atoi(argv[3]);
        TCPTransport tcp;
        comm = &tcp.comm;
        if(!init_tcp_transport(&tcp, ip, port))
        {
            printf("%sCan not create a tcp socket%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        printf("<< TCP mode => ip: %s - port: %hu >>\n", argv[2], port);
    }
    else
    {
        help();
        return 1;
    }

    StreamBuffers stream_buffers;

    // Waiting user commands
    char command_stdin_line[256];
    bool running = true;
    while (running)
    {
        if (!check_input())
        {
            (void) run_session(&session, 50);
        }
        else if (fgets(command_stdin_line, 256, stdin))
        {
            if (!compute_command(command_stdin_line, &session, comm, &stream_buffers))
            {
                running = false;
            }
        }
    }

    return 0;
}

bool compute_command(const char* command, Session* session, Communication* comm, StreamBuffers* stream_buffers)
{
    char name[128];
    uint8_t id_pre = 0;
    uint8_t id_related_pre = 0;
    char color[128];
    uint32_t x;
    uint32_t y;
    uint32_t shapesize;
    int length = sscanf(command, "%s %hhu %hhu %s %u %u %u", name, &id_pre, &id_related_pre, color, &x, &y, &shapesize);
    if(length == 4 && color[0] == '\0')
    {
        length = 3; //some implementations of sscanfs add 1 to length if color is empty.
    }

    if(strcmp(name, "create_session") == 0)
    {
        uint32_t key = 0xAABBCCDD;
        uint8_t session_id = 0x81;
        if(!create_session(session, session_id, key, comm))
        {
            //Error
            return true;
        }

        set_status_callback(session, on_status, NULL);
        set_topic_callback(session, on_topic, NULL);

        (void) create_output_best_effort_stream(session, stream_buffers->output_best_effort_stream_buffer, BUFFER_SIZE);
        (void) create_output_reliable_stream(session, stream_buffers->output_reliable_stream_buffer, BUFFER_SIZE * 8, 8);
        (void) create_input_best_effort_stream(session);
        (void) create_input_reliable_stream(session, stream_buffers->input_reliable_stream_buffer, BUFFER_SIZE * 8, 8);
    }
    else if(strcmp(name, "create_participant") == 0 && length == 2)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        mrObjectId participant_id = create_object_id(id_pre, PARTICIPANT_ID);
        char* participant_ref = "default participant";
        write_create_participant_ref(session, stream_id, participant_id, participant_ref, 0);
    }
    else if(strcmp(name, "create_topic") == 0 && length == 3)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        mrObjectId participant_id = create_object_id(id_related_pre, PARTICIPANT_ID);
        mrObjectId topic_id = create_object_id(id_pre, TOPIC_ID);
        char* topic_xml = "<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>";
        write_configure_topic_xml(session, stream_id, topic_id, participant_id, topic_xml, 0);
    }
    else if(strcmp(name, "create_publisher") == 0 && length == 3)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        mrObjectId participant_id = create_object_id(id_related_pre, PARTICIPANT_ID);
        mrObjectId publisher_id = create_object_id(id_pre, PUBLISHER_ID);
        char* publisher_xml = "<publisher name=\"MyPublisher\"";
        write_configure_publisher_xml(session, stream_id, publisher_id, participant_id, publisher_xml, 0);
    }
    else if(strcmp(name, "create_subscriber") == 0 && length == 3)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        mrObjectId participant_id = create_object_id(id_related_pre, PARTICIPANT_ID);
        mrObjectId subscriber_id = create_object_id(id_pre, SUBSCRIBER_ID);
        char* subscriber_xml = {"<subscriber name=\"MySubscriber\""};
        write_configure_subscriber_xml(session, stream_id, subscriber_id, participant_id, subscriber_xml, 0);
    }
    else if(strcmp(name, "create_datawriter") == 0 && length == 3)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        mrObjectId publisher_id = create_object_id(id_related_pre, PUBLISHER_ID);
        mrObjectId datawriter_id = create_object_id(0x01, DATAWRITER_ID);
        char* datawriter_xml = "<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>";
        write_configure_datawriter_xml(session, stream_id, datawriter_id, publisher_id, datawriter_xml, 0);
    }
    else if(strcmp(name, "create_datareader") == 0 && length == 3)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        mrObjectId subscriber_id = create_object_id(id_related_pre, SUBSCRIBER_ID);
        mrObjectId datareader_id = create_object_id(id_pre, DATAREADER_ID);
        char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
        write_configure_datareader_xml(session, stream_id, datareader_id, subscriber_id, datareader_xml, 0);
    }
    else if(strcmp(name, "write_data") == 0 && length >= 3)
    {
        /*ShapeType topic = {"GREEN", 100 , 100, 50};
        if (length == 7)
        {
            topic.color = color;
            topic.x = x;
            topic.y = y;
            topic.shapesize = shapesize;
        }*/
    }
    else if(strcmp(name, "read_data") == 0 && length == 3)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        StreamId data_stream_id = create_stream_id(0, (id_related_pre) ? RELIABLE_STREAM : BEST_EFFORT_STREAM, INPUT_STREAM);
        mrObjectId datareader_id = create_object_id(id_pre, DATAREADER_ID);
        write_read_data(session, stream_id, datareader_id, data_stream_id, MR_FORMAT_DATA, NULL);
    }
    else if(strcmp(name, "delete") == 0 && length == 3)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        mrObjectId entity_id = create_object_id((id_pre & 0xFFF0) >> 4, id_pre & 0x0F);
        write_delete_entity(session, stream_id, entity_id);
    }
    else if(strcmp(name, "delete_session") == 0)
    {
        delete_session(session);
    }
    else if(strcmp(name, "exit") == 0)
    {
        //TODO (Julian): add a close transport function
        return false;
    }
    else if(strcmp(name, "h") == 0 || strcmp(name, "help") == 0)
    {
        list_commands();
    }
    else
    {
        printf("%sUnknown command error%s\n", "\x1B[1;31m", "\x1B[0m");
        list_commands();
    }

    return true;
}


void on_status(Session* session, mrObjectId id, uint16_t request_id, uint8_t status, void* args)
{
    (void) session; (void) id; (void) request_id; (void) status; (void) args;
    //printf("Status received!\n");
}

void on_topic(Session* session, mrObjectId object_id, uint16_t request_id, StreamId stream_id, MicroBuffer* serialization, void* args)
{
    (void) session; (void) request_id; (void) stream_id; (void) serialization; (void) args;
    if(SHAPE_TOPIC ==  object_id.id)
    {
        /*ShapeType topic;
        deserialize_ShapeType_topic(serialization, &topic);
        printf("Receiving... ");
        printl_ShapeType_topic(&topic);*/
    }
}

void help(void)
{
    printf("Usage: program <command>\n");
    printf("List of commands:\n");
    printf("    serial device\n");
    printf("    udp agent_ip agent_port\n");
    printf("    help\n");
}

void list_commands(void)
{
    printf("usage: <command> [<args>]\n");
    printf("    create_session:                                                  Creates a Session\n");
    printf("    create_participant <participant id>:                             Creates a new Participant on the current session\n");
    printf("    create_topic       <topic id> <participant id>:                  Register new Topic using <participant id> participant\n");
    printf("    create_publisher   <publisher id> <participant id>:              Creates a Publisher on <participant id> participant\n");
    printf("    create_subscriber  <subscriber id> <participant id>:             Creates a Subscriber on <participant id> participant\n");
    printf("    create_datawriter  <datawriter id> <publisher id>:               Creates a DataWriter on the publisher <publisher id>\n");
    printf("    create_datareader  <datareader id> <subscriber id>:              Creates a DataReader on the subscriber <subscriber id>\n");
    printf("    write_data <datawriter id> <stream id> [<color> <x> <y> <size>]: Write data into a <stream id> using <data writer id> DataWriter\n");
    printf("    read_data <datareader id> <stream id>:                           Read data from a <stream id> using <data reader id> DataReader\n");
    printf("    delete <id>:                                                     Removes object with <id> identifier\n");
    printf("    delete_session:                                                  Deletes a session\n");
    printf("    exit:                                                            Close program\n");
    printf("    h, help:                                                         Shows this message\n");
}

int check_input(void)
{
    struct timeval tv = {0, 0};
    fd_set fds = {0};
    FD_ZERO(&fds);
    FD_SET(0, &fds); //STDIN 0
    select(1, &fds, NULL, NULL, &tv);
    return FD_ISSET(0, &fds);
}

/*void check_and_print_error(Session* session)
{
    if(session->last_status_received)
    {
        if(session->last_status.status == STATUS_OK)
        {
            printf("%s[OK]%s\n", "\x1B[1;32m", "\x1B[0m");
        }
        else
        {
            printf("%s[Status error: %i]%s\n", "\x1B[1;31m", session->last_status.status, "\x1B[0m");
        }
    }
    else
    {
        printf("%s[Connection error]%s\n", "\x1B[1;31m", "\x1B[0m");
    }
} */

/*void printl_ShapeType_topic(const ShapeType* shape_topic)
{
    printf("%s[<SHAPE> | %s | x: %u | y: %u | size: %u]%s\n",
            "\x1B[1;34m",
            shape_topic->color,
            shape_topic->x,
            shape_topic->y,
            shape_topic->shapesize,
            "\x1B[0m");
}*/

