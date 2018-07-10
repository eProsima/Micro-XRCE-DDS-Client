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

#define GREEN_CONSOLE_COLOR "\x1B[1;32m"
#define RED_CONSOLE_COLOR   "\x1B[1;31m"
#define RESTORE_COLOR       "\x1B[0m"

#define XML_BUFFER_SIZE 1024
#define SHAPE_TOPIC 0x01

// Stream buffers
#define MAX_TRANSPORT_MTU 512
#define MAX_HISTORY 16
#define MAX_BUFFER_SIZE    MAX_TRANSPORT_MTU * MAX_HISTORY

static bool compute_command(const char* command, Session* session);
static void on_topic(Session* session, mrObjectId object_id, uint16_t request_id, StreamId stream_id, MicroBuffer* serialization, void* args);
static void print_status(uint8_t status);
static void help(void);
static void list_commands(void);
static int check_input(void);

int main(int args, char** argv)
{
    printf("SHAPES DEMO XRCE CLIENT\n");

    Session session;
    UARTTransport uart;
    UDPTransport udp;
    TCPTransport tcp;

    Communication* comm;

    int args_index = 0;
    if(args >= 3 && strcmp(argv[1], "serial") == 0)
    {
        char* device = argv[2];
        int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if(!init_uart_transport_fd(&uart, fd, 0x00, 0x01))
        {
            printf("%sCan not create serial connection%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        comm = &uart.comm;
        printf("Serial mode => dev: %s\n", device);
        args_index = 3;
    }
    else if(args >= 4 && strcmp(argv[1], "udp") == 0)
    {
        char* ip = argv[2];
        uint16_t port = (uint16_t)atoi(argv[3]);
        if(!init_udp_transport(&udp, ip, port))
        {
            printf("%sCan not create an udp connection%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        comm = &udp.comm;
        printf("UDP mode => ip: %s - port: %hu\n", argv[2], port);
        args_index = 4;
    }
    else if(args >= 4 && strcmp(argv[1], "tcp") == 0)
    {
        char* ip = argv[2];
        uint16_t port = (uint16_t)atoi(argv[3]);
        if(!init_tcp_transport(&tcp, ip, port))
        {
            printf("%sCan not create a tcp connection%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        comm = &tcp.comm;
        printf("<< TCP mode => ip: %s - port: %hu >>\n", argv[2], port);
        args_index = 4;
    }
    else
    {
        help();
        return 1;
    }

    // Session
    uint32_t key = 0xAABBCCDD;
    uint8_t session_id = 0x81;
    if(args_index < args)
    {
        session_id = (uint8_t) atoi(argv[args_index++]);
    }

    if(!create_session(&session, session_id, key, comm))
    {
        printf("%sCan not create session with the agent%s\n", "\x1B[1;31m", "\x1B[0m");
        return 1;
    }

    set_topic_callback(&session, on_topic, NULL);

    // Streams
    size_t history = 8;
    if(args_index < args)
    {
        history = atoi(argv[args_index++]);
    }

    uint8_t output_best_effort_stream_buffer[MAX_BUFFER_SIZE];
    uint8_t output_reliable_stream_buffer[MAX_BUFFER_SIZE];
    uint8_t input_reliable_stream_buffer[MAX_BUFFER_SIZE];
    (void) create_output_best_effort_stream(&session, output_best_effort_stream_buffer, comm->mtu);
    (void) create_output_reliable_stream(&session, output_reliable_stream_buffer, comm->mtu * history, history);
    (void) create_input_best_effort_stream(&session);
    (void) create_input_reliable_stream(&session, input_reliable_stream_buffer, comm->mtu * history, history);

    // Waiting user commands
    char command_stdin_line[256];
    bool running = true;
    while (running)
    {
        if (!check_input())
        {
            (void) run_session_until_timeout(&session, 100);
        }
        else if (fgets(command_stdin_line, 256, stdin))
        {
            running = compute_command(command_stdin_line, &session);
        }
    }

    delete_session(&session);
    //TODO: add the close transport functions

    return 0;
}

bool compute_command(const char* command, Session* session)
{
    char name[128];
    uint8_t arg1 = 0;
    uint8_t arg2 = 0;
    char color[128];
    uint32_t x;
    uint32_t y;
    uint32_t shapesize;
    int length = sscanf(command, "%s %hhu %hhu %s %u %u %u", name, &arg1, &arg2, color, &x, &y, &shapesize);
    if(length == 4 && color[0] == '\0')
    {
        length = 3; //some implementations of sscanfs add 1 to length if color is empty.
    }

    else if(strcmp(name, "create_participant") == 0 && length == 2)
    {
        mrObjectId participant_id = create_object_id(arg1, PARTICIPANT_ID);
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        char* participant_ref = "default participant";
        uint16_t request = write_create_participant_ref(session, stream_id, participant_id, participant_ref, 0);

        uint8_t status;
        (void) run_session_until_status(session, 1000, &request, &status, 1);
        print_status(status);
    }
    else if(strcmp(name, "create_topic") == 0 && length == 3)
    {
        mrObjectId topic_id = create_object_id(arg1, TOPIC_ID);
        mrObjectId participant_id = create_object_id(arg2, PARTICIPANT_ID);
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        char* topic_xml = "<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>";
        uint16_t request = write_configure_topic_xml(session, stream_id, topic_id, participant_id, topic_xml, 0);

        uint8_t status;
        (void) run_session_until_status(session, 1000, &request, &status, 1);
        print_status(status);
    }
    else if(strcmp(name, "create_publisher") == 0 && length == 3)
    {
        mrObjectId publisher_id = create_object_id(arg1, PUBLISHER_ID);
        mrObjectId participant_id = create_object_id(arg2, PARTICIPANT_ID);
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        char* publisher_xml = "<publisher name=\"MyPublisher\"";
        uint16_t request = write_configure_publisher_xml(session, stream_id, publisher_id, participant_id, publisher_xml, 0);

        uint8_t status;
        (void) run_session_until_status(session, 1000, &request, &status, 1);
        print_status(status);
    }
    else if(strcmp(name, "create_subscriber") == 0 && length == 3)
    {
        mrObjectId subscriber_id = create_object_id(arg1, SUBSCRIBER_ID);
        mrObjectId participant_id = create_object_id(arg2, PARTICIPANT_ID);
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        char* subscriber_xml = {"<subscriber name=\"MySubscriber\""};
        uint16_t request = write_configure_subscriber_xml(session, stream_id, subscriber_id, participant_id, subscriber_xml, 0);

        uint8_t status;
        (void) run_session_until_status(session, 1000, &request, &status, 1);
        print_status(status);
    }
    else if(strcmp(name, "create_datawriter") == 0 && length == 3)
    {
        mrObjectId datawriter_id = create_object_id(arg1, DATAWRITER_ID);
        mrObjectId publisher_id = create_object_id(arg2, PUBLISHER_ID);
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        char* datawriter_xml = "<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>";
        uint16_t request = write_configure_datawriter_xml(session, stream_id, datawriter_id, publisher_id, datawriter_xml, 0);

        uint8_t status;
        (void) run_session_until_status(session, 1000, &request, &status, 1);
        print_status(status);
    }
    else if(strcmp(name, "create_datareader") == 0 && length == 3)
    {
        mrObjectId datareader_id = create_object_id(arg1, DATAREADER_ID);
        mrObjectId subscriber_id = create_object_id(arg2, SUBSCRIBER_ID);
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
        uint16_t request = write_configure_datareader_xml(session, stream_id, datareader_id, subscriber_id, datareader_xml, 0);

        uint8_t status;
        (void) run_session_until_status(session, 1000, &request, &status, 1);
        print_status(status);
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
        mrObjectId datareader_id = create_object_id(arg1, DATAREADER_ID);
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        StreamId data_stream_id = create_stream_id(0, (arg2) ? RELIABLE_STREAM : BEST_EFFORT_STREAM, INPUT_STREAM);
        uint16_t request = write_read_data(session, stream_id, datareader_id, data_stream_id, MR_FORMAT_DATA, NULL);

        uint8_t status;
        (void) run_session_until_status(session, 1000, &request, &status, 1);
        print_status(status);
    }
    else if(strcmp(name, "delete") == 0 && length == 3)
    {
        StreamId stream_id = create_stream_id(0, RELIABLE_STREAM, OUTPUT_STREAM);
        mrObjectId entity_id = create_object_id((arg1 & 0xFFF0) >> 4, arg1 & 0x0F);
        uint16_t request = write_delete_entity(session, stream_id, entity_id);

        uint8_t status;
        (void) run_session_until_status(session, 1000, &request, &status, 1);
        print_status(status);
    }
    else if(strcmp(name, "exit") == 0)
    {
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

void on_topic(Session* session, mrObjectId object_id, uint16_t request_id, StreamId stream_id, MicroBuffer* serialization, void* args)
{
    (void) session; (void) object_id; (void) request_id; (void) stream_id; (void) serialization; (void) args;
    // Only can be ShapeType type
    /*ShapeType topic;
    deserialize_ShapeType_topic(serialization, &topic);
    printf("Receiving... ");
    printl_ShapeType_topic(&topic);*/
}

void print_status(uint8_t status)
{
    char status_name[128];
    if(MR_STATUS_OK == status)
    {
        strcpy(status_name, "OK");
    }
    else if(MR_STATUS_OK_MATCHED == status)
    {
        strcpy(status_name, "OK_MATCHED");
    }
    else if(MR_STATUS_ERR_DDS_ERROR == status)
    {
        strcpy(status_name, "ERR_DDS_ERROR");
    }
    else if(MR_STATUS_ERR_MISMATCH == status)
    {
        strcpy(status_name, "ERR_MISMATCH");
    }
    else if(MR_STATUS_ERR_ALREADY_EXISTS == status)
    {
        strcpy(status_name, "ERR_ALREADY_EXISTS");
    }
    else if(MR_STATUS_ERR_DENIED == status)
    {
        strcpy(status_name, "ERR_DDS_DENIED");
    }
    else if(MR_STATUS_ERR_UNKNOWN_REFERENCE == status)
    {
        strcpy(status_name, "UNKNOWN_REFERENCE");
    }
    else if(MR_STATUS_ERR_INVALID_DATA == status)
    {
        strcpy(status_name, "ERR_INVALID_DATA");
    }
    else if(MR_STATUS_ERR_INCOMPATIBLE == status)
    {
        strcpy(status_name, "ERR_INCOMPATIBLE");
    }
    else if(MR_STATUS_ERR_RESOURCES == status)
    {
        strcpy(status_name, "ERR_RESOURCES");
    }
    else if(MR_STATUS_NONE == status)
    {
        strcpy(status_name, "NONE");
    }

    const char* color = (MR_STATUS_OK == status || MR_STATUS_OK_MATCHED == status) ? GREEN_CONSOLE_COLOR : RED_CONSOLE_COLOR;
    printf("%sStatus: %s%s\n", color, status_name, RESTORE_COLOR);
}

void help(void)
{
    printf("Usage: program <command>\n");
    printf("List of commands:\n");
    printf("    serial device\n");
    printf("    udp agent-ip agent-port\n");
    printf("    tcp agent-ip agent-port\n");
    printf("    help\n");
}

void list_commands(void)
{
    printf("usage: <command> [<args>]\n");
    printf("    - create_session [<reliable stream history>]\n");
    printf("        Creates a Session. The reliable stream history can be set optionally\n\n");
    printf("    create_participant <participant id>:                             Creates a new Participant on the current session\n");
    printf("    create_topic       <topic id> <participant id>:                  Register new Topic using <participant id> participant\n");
    printf("    create_publisher   <publisher id> <participant id>:              Creates a Publisher on <participant id> participant\n");
    printf("    create_subscriber  <subscriber id> <participant id>:             Creates a Subscriber on <participant id> participant\n");
    printf("    create_datawriter  <datawriter id> <publisher id>:               Creates a DataWriter on the publisher <publisher id>\n");
    printf("    create_datareader  <datareader id> <subscriber id>:              Creates a DataReader on the subscriber <subscriber id>\n");
    printf("    write_data <datawriter id> <stream id> [<color> <x> <y> <size>]: Write data into a <stream id> using <data writer id> DataWriter\n");
    printf("    read_data          <datareader id> <stream id>:                  Read data from a <stream id> using <data reader id> DataReader\n");
    printf("    delete             <id>:                                         Removes object with <id> identifier\n");
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

