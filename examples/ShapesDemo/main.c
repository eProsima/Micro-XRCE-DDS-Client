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

#include "WriteShapeType.h"

#include <micrortps/client/client.h>
#include <stdio.h>
#include <fcntl.h>  // O_RDWR, O_NOCTTY, O_NONBLOCK
#include <stdlib.h> // atoi
#include <string.h> // strcpy

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <sys/select.h>
#endif

// Colors for highlight the print
#define GREEN_CONSOLE_COLOR "\x1B[1;32m"
#define RED_CONSOLE_COLOR   "\x1B[1;31m"
#define RESTORE_COLOR       "\x1B[0m"

// Stream buffers
#define MAX_TRANSPORT_MTU  512
#define MAX_HISTORY 16
#define MAX_BUFFER_SIZE    MAX_TRANSPORT_MTU * MAX_HISTORY

static bool run_command(const char* command, mrSession* session, mrStreamId* stream_id);
static bool compute_command(mrSession* session, mrStreamId* stream_id, int length, const char* name,
                            uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, const char* topic_color);
static bool compute_print_command(mrSession* session, mrStreamId* stream_id, int length, const char* name,
                            uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, const char* topic_color);
static void on_topic(mrSession* session, mrObjectId object_id, uint16_t request_id, mrStreamId stream_id, MicroBuffer* serialization, void* args);
static void on_status(mrSession* session, mrObjectId object_id, uint16_t request_id, uint8_t status, void* args);
static void print_ShapeType_topic(const ShapeType* topic);
static void print_status(uint8_t status);
static void print_help(void);
static void print_commands(void);
static int check_input(void);

int main(int args, char** argv)
{
    mrSession session;
    mrUARTTransport uart;
    mrUDPTransport udp;
    mrTCPTransport tcp;

    mrCommunication* comm;

    int args_index = 0;
    if(args >= 3 && strcmp(argv[1], "--serial") == 0)
    {
        char* device = argv[2];
        int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if(!mr_init_uart_transport_fd(&uart, fd, 0, 1))
        {
            printf("%sCan not create a serial connection%s\n", RED_CONSOLE_COLOR, RESTORE_COLOR);
            return 1;
        }
        comm = &uart.comm;
        printf("Serial mode => dev: %s\n", device);
        args_index = 3;
    }
    else if(args >= 4 && strcmp(argv[1], "--udp") == 0)
    {
        char* ip = argv[2];
        uint16_t port = (uint16_t)atoi(argv[3]);
        if(!mr_init_udp_transport(&udp, ip, port))
        {
            printf("%sCan not create an udp connection%s\n", RED_CONSOLE_COLOR, RESTORE_COLOR);
            return 1;
        }
        comm = &udp.comm;
        printf("UDP mode => ip: %s - port: %hu\n", argv[2], port);
        args_index = 4;
    }
    else if(args >= 4 && strcmp(argv[1], "--tcp") == 0)
    {
        char* ip = argv[2];
        uint16_t port = (uint16_t)atoi(argv[3]);
        if(!mr_init_tcp_transport(&tcp, ip, port))
        {
            printf("%sCan not create a tcp connection%s\n", RED_CONSOLE_COLOR, RESTORE_COLOR);
            return 1;
        }
        comm = &tcp.comm;
        printf("<< TCP mode => ip: %s - port: %hu >>\n", argv[2], port);
        args_index = 4;
    }
    else
    {
        print_help();
        return 1;
    }

    printf("Running Shapes Demo XRCE Client...\n");

    uint32_t key = 0xAABBCCDD;
    size_t history = 8;
    if(args_index < args && 0 == strcmp(argv[args_index++], "--history"))
    {
        if(args_index < args)
        {
            size_t request_history = atoi(argv[args_index++]);
            if(MAX_HISTORY >= request_history)
            {
                history = request_history;
            }
        }
        else
        {
            print_help();
        }
    }

    // Init session and streams
    mr_init_session(&session, comm, key);
    mr_set_topic_callback(&session, on_topic, NULL);
    mr_set_status_callback(&session, on_status, NULL);

    uint8_t input_reliable_stream_buffer[MAX_BUFFER_SIZE];
    uint8_t output_best_effort_stream_buffer[MAX_BUFFER_SIZE];
    uint8_t output_reliable_stream_buffer[MAX_BUFFER_SIZE];

    (void) mr_create_input_best_effort_stream(&session);
    (void) mr_create_input_reliable_stream(&session, input_reliable_stream_buffer, comm->mtu * history, history);
    (void) mr_create_output_best_effort_stream(&session, output_best_effort_stream_buffer, comm->mtu);
    mrStreamId reliable_output = mr_create_output_reliable_stream(&session, output_reliable_stream_buffer, comm->mtu * history, history);

    // Waiting user commands
    char command_stdin_line[256];
    bool running = true;
    while (running)
    {
        if (!check_input())
        {
            (void) mr_run_session_until_timeout(&session, 100);
        }
        else if (fgets(command_stdin_line, 256, stdin))
        {
            running = run_command(command_stdin_line, &session, &reliable_output);
        }
    }

    //TODO: add the close transport functions
    return 0;
}

bool run_command(const char* command, mrSession* session, mrStreamId* stream_id)
{
    char name[128];
    uint32_t arg1 = 0;
    uint32_t arg2 = 0;
    uint32_t arg3 = 0;
    uint32_t arg4 = 0;
    uint32_t arg5 = 0;
    char topic_color[128] = "";
    int length = sscanf(command, "%s %u %u %u %u %u %s", name, &arg1, &arg2, &arg3, &arg4, &arg5, topic_color);
    if(length == 7 && topic_color[0] == '\0')
    {
        length = length - 1; //some implementations of sscanfs add 1 to length if color is empty.
    }

    return compute_command(session, stream_id, length, name, arg1, arg2, arg3, arg4, arg5, topic_color);
}

bool compute_command(mrSession* session, mrStreamId* stream_id, int length, const char* name,
                     uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, const char* topic_color)
{
    if(strcmp(name, "create_session") == 0 && length >= 1)
    {
        (void) mr_create_session(session);
        print_status(session->info.last_requested_status);
    }
    else if(strcmp(name, "create_participant") == 0 && length == 2)
    {
        mrObjectId participant_id = mr_object_id(arg1, MR_PARTICIPANT_ID);
        char* participant_ref = "default participant";
        (void) mr_write_create_participant_ref(session, *stream_id, participant_id, participant_ref, 0);
    }
    else if(strcmp(name, "create_topic") == 0 && length == 3)
    {
        mrObjectId topic_id = mr_object_id(arg1, MR_TOPIC_ID);
        mrObjectId participant_id = mr_object_id(arg2, MR_PARTICIPANT_ID);
        char* topic_xml = "<dds><topic><name>Square</name><dataType>ShapeType</dataType></topic></dds>";
        (void) mr_write_configure_topic_xml(session, *stream_id, topic_id, participant_id, topic_xml, 0);
    }
    else if(strcmp(name, "create_publisher") == 0 && length == 3)
    {
        mrObjectId publisher_id = mr_object_id(arg1, MR_PUBLISHER_ID);
        mrObjectId participant_id = mr_object_id(arg2, MR_PARTICIPANT_ID);
        char* publisher_xml = "<publisher name=\"MyPublisher\"";
        (void) mr_write_configure_publisher_xml(session, *stream_id, publisher_id, participant_id, publisher_xml, 0);
    }
    else if(strcmp(name, "create_subscriber") == 0 && length == 3)
    {
        mrObjectId subscriber_id = mr_object_id(arg1, MR_SUBSCRIBER_ID);
        mrObjectId participant_id = mr_object_id(arg2, MR_PARTICIPANT_ID);
        char* subscriber_xml = {"<subscriber name=\"MySubscriber\""};
        (void) mr_write_configure_subscriber_xml(session, *stream_id, subscriber_id, participant_id, subscriber_xml, 0);
    }
    else if(strcmp(name, "create_datawriter") == 0 && length == 3)
    {
        mrObjectId datawriter_id = mr_object_id(arg1, MR_DATAWRITER_ID);
        mrObjectId publisher_id = mr_object_id(arg2, MR_PUBLISHER_ID);
        char* datawriter_xml = "<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>Square</name><dataType>ShapeType</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>";
        (void) mr_write_configure_datawriter_xml(session, *stream_id, datawriter_id, publisher_id, datawriter_xml, 0);
    }
    else if(strcmp(name, "create_datareader") == 0 && length == 3)
    {
        mrObjectId datareader_id = mr_object_id(arg1, MR_DATAREADER_ID);
        mrObjectId subscriber_id = mr_object_id(arg2, MR_SUBSCRIBER_ID);
        char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>Square</name><dataType>ShapeType</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
        (void) mr_write_configure_datareader_xml(session, *stream_id, datareader_id, subscriber_id, datareader_xml, 0);
    }
    else if(strcmp(name, "write_data") == 0 && length >= 3)
    {
        ShapeType topic = {"GREEN", 100 , 100, 50};
        if(length > 3)
        {
            strncpy(topic.color, topic_color, sizeof(topic.color));
            topic.x = arg3;
            topic.y = arg4;
            topic.shapesize = arg5;
        }

        mrObjectId datawriter_id = mr_object_id(arg1, MR_DATAWRITER_ID);
        mrStreamId output_stream_id = mr_stream_id_from_raw(arg2, MR_INPUT_STREAM);
        (void) mr_write_ShapeType_topic(session, output_stream_id, datawriter_id, &topic);

        printf("Sending... ");
        print_ShapeType_topic(&topic);
    }
    else if(strcmp(name, "request_data") == 0 && length == 4)
    {
        mrDeliveryControl delivery_control;
        delivery_control.max_samples = arg3;
        delivery_control.max_elapsed_time = MR_MAX_ELAPSED_TIME_UNLIMITED;
        delivery_control.max_bytes_per_second = MR_MAX_BYTES_PER_SECOND_UNLIMITED;
        delivery_control.min_pace_period = 0;

        mrObjectId datareader_id = mr_object_id(arg1, MR_DATAREADER_ID);
        mrStreamId input_stream_id = mr_stream_id_from_raw(arg2, MR_INPUT_STREAM);
        (void) mr_write_request_data(session, *stream_id, datareader_id, input_stream_id, &delivery_control);
    }
    else if(strcmp(name, "cancel_data") == 0 && length == 2)
    {
        mrObjectId datareader_id = mr_object_id(arg1, MR_DATAREADER_ID);
        (void) mr_write_cancel_data(session, *stream_id, datareader_id);
    }
    else if(strcmp(name, "delete") == 0 && length == 3)
    {
        mrObjectId entity_id = mr_object_id((arg1 & 0xFFF0) >> 4, arg1 & 0x0F);
        (void) mr_write_delete_entity(session, *stream_id, entity_id);
    }
    else if((strcmp(name, "default_output_stream") == 0 || strcmp(name, "stream") == 0) && length == 2)
    {
        *stream_id = mr_stream_id_from_raw(arg1, MR_OUTPUT_STREAM);
    }
    else if(strcmp(name, "delete_session") == 0 && length == 1)
    {
        (void) mr_delete_session(session);
        print_status(session->info.last_requested_status);
    }
    else if(strcmp(name, "exit") == 0 && length == 1)
    {
        (void) compute_print_command(session, stream_id, 1, "delete_session", 0, 0, 0, 0, 0, "");
        return false;
    }
    else if(strcmp(name, "example") == 0 && length == 2)
    {
        (void) compute_print_command(session, stream_id, 2, "create_participant", arg1, 0, 0, 0, 0, "");
        (void) mr_run_session_until_timeout(session, 20);
        (void) compute_print_command(session, stream_id, 3, "create_topic"      , arg1, arg1, 0, 0, 0, "");
        (void) mr_run_session_until_timeout(session, 20);
        (void) compute_print_command(session, stream_id, 3, "create_publisher"  , arg1, arg1, 0, 0, 0, "");
        (void) mr_run_session_until_timeout(session, 20);
        (void) compute_print_command(session, stream_id, 3, "create_subscriber" , arg1, arg1, 0, 0, 0, "");
        (void) mr_run_session_until_timeout(session, 20);
        (void) compute_print_command(session, stream_id, 3, "create_datawriter" , arg1, arg1, 0, 0, 0, "");
        (void) mr_run_session_until_timeout(session, 20);
        (void) compute_print_command(session, stream_id, 3, "create_datareader" , arg1, arg1, 0, 0, 0, "");
        (void) mr_run_session_until_timeout(session, 20);
    }
    else if(0 == strcmp(name, "list") || 0 == strcmp(name, "l"))
    {
        print_commands();
    }
    else
    {
        printf("%sUnknown command error, write 'l' or 'list' to show the command list.%s\n", RED_CONSOLE_COLOR, RESTORE_COLOR);
    }

    return true;
}


static bool compute_print_command(mrSession* session, mrStreamId* stream_id, int length, const char* name,
                            uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, const char* topic_color)
{
    printf("%s\n", name);
    return compute_command(session, stream_id, length, name, arg1, arg2, arg3, arg4, arg5, topic_color);
}

void on_status(mrSession* session, mrObjectId object_id, uint16_t request_id, uint8_t status, void* args)
{
    (void) session; (void) object_id; (void) request_id; (void) args;
    print_status(status);
}

void on_topic(mrSession* session, mrObjectId object_id, uint16_t request_id, mrStreamId stream_id, MicroBuffer* serialization, void* args)
{
    (void) session; (void) object_id; (void) request_id; (void) stream_id; (void) serialization; (void) args;

    ShapeType topic;
    deserialize_ShapeType_topic(serialization, &topic);

    printf("Receiving... ");
    print_ShapeType_topic(&topic);
}

void print_ShapeType_topic(const ShapeType* shape_topic)
{
    printf("%s[SHAPE_TYPE | color: %s | x: %u | y: %u | size: %u]%s\n",
           "\x1B[1;34m",
           shape_topic->color,
           shape_topic->x,
           shape_topic->y,
           shape_topic->shapesize,
           "\x1B[0m");
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

void print_help(void)
{
    printf("Usage: program --help\n");
    printf("       program <transport> [--history <history>]\n");
    printf("List of available transports:\n");
    printf("    --serial <device>\n");
    printf("    --udp <agent-ip> <agent-port>\n");
    printf("    --tcp <agent-ip> <agent-port>\n");
}

void print_commands(void)
{
    printf("usage: <command> [<args>]\n");
    printf("    create_session\n");
    printf("        Creates a mrSession, if exists, reuse it.\n");
    printf("    create_participant <participant id>:\n");
    printf("        Creates a new Participant on the current session\n");
    printf("    create_topic       <topic id> <participant id>:\n");
    printf("        Register new Topic using <participant id> participant\n");
    printf("    create_publisher   <publisher id> <participant id>:\n");
    printf("        Creates a Publisher on <participant id> participant\n");
    printf("    create_subscriber  <subscriber id> <participant id>:\n");
    printf("        Creates a Subscriber on <participant id> participant\n");
    printf("    create_datawriter  <datawriter id> <publisher id>:\n");
    printf("        Creates a DataWriter on the publisher <publisher id>\n");
    printf("    create_datareader  <datareader id> <subscriber id>:\n");
    printf("        Creates a DataReader on the subscriber <subscriber id>\n");
    printf("    write_data <datawriter id> <stream id> [<color> <x> <y> <size>]:\n");
    printf("        Write data into a <stream id> using <data writer id> DataWriter\n");
    printf("    request_data       <datareader id> <stream id> <samples>:\n");
    printf("        Read <sample> topics from a <stream id> using <data reader id> DataReader\n");
    printf("    cancel_data        <datareader id>:\n");
    printf("        Cancel any previous request data of <data reader id> DataReader\n");
    printf("    delete             <id_prefix> <type>:\n");
    printf("        Removes object with <id prefix> and <type>\n");
    printf("    stream, default_output_stream <stream_id>:\n");
    printf("        Change the default output stream for all messages except of write data.\n");
    printf("        <stream_id> can be 1-127 for best effort and 128-255 for reliable.\n");
    printf("        The streams must be initially configured.\n");
    printf("    (macro) exit:\n");
    printf("        Close session and exit\n");
    printf("    (macro) example            <id>:\n");
    printf("        Create the necessary entities for a complete publisher and subscriber.\n");
    printf("        All entities will have the same <id> as id.\n");
    printf("    h, help:\n");
    printf("        Shows this message\n");
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
