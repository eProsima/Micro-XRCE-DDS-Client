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

#include "Shape.h"
#include <stdio.h>

// ----------------------------------------------------
//    App client
// ----------------------------------------------------
#define XML_BUFFER_SIZE 1024
#define SHAPE_TOPIC 0x01

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
    printf("    write_data <datawriter id> [<stream id> <color> <x> <y> <size>]: Write data into a <stream id> using <data writer id> DataWriter\n");
    printf("    read_data <datareader id> <stream id>:                           Read data from a <stream id> using <data reader id> DataReader\n");
    printf("    delete <id>:                                                     Removes object with <id> identifier\n");
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

void printl_ShapeType_topic(const ShapeType* shape_topic)
{
    printf("        %s[%s | x: %u | y: %u | size: %u]%s\n",
            "\x1B[1;34m",
            shape_topic->color,
            shape_topic->x,
            shape_topic->y,
            shape_topic->shapesize,
            "\x1B[0m");
}

void on_topic(ObjectId id, MicroBuffer *message, void* args)
{
    if(SHAPE_TOPIC ==  id.data[0])
    {
        ShapeType topic;
        deserialize_ShapeType_topic(message, &topic);
        printl_ShapeType_topic(&topic);
    }
}

void check_and_print_error(Session* session)
{
    if(session->last_status_received)
    {
        if(session->last_status.status != STATUS_OK)
        {
            printf("%sStatus error (%i)%s\n", "\x1B[1;31m", session->last_status.status, "\x1B[0m");
        }
        else
        {
            //All things go well
        }
    }
    else
    {
        printf("%sConnection error%s\n", "\x1B[1;31m", "\x1B[0m");
    }
}

size_t read_file(const char *file_name, char* data_file, size_t buf_size)
{
    FILE *fp = fopen(file_name, "r");
    size_t length = 0;
    if (fp != NULL)
    {
        length = fread(data_file, sizeof(char), buf_size, fp);
        if (length == 0)
        {
            printf("Error reading %s\n", file_name);
        }

        if(length < buf_size)
        {
            data_file[length] = '\0';
        }
        fclose(fp);
    }
    else
    {
        printf("Error opening %s\n", file_name);
    }

    return length;
}

bool compute_command(const char* command, Session* session)
{
    char name[128];
    uint32_t id_pre = 0;
    uint32_t id_related_pre = 0;
    char color[128];
    uint32_t x;
    uint32_t y;
    uint32_t shapesize;
    int length = sscanf(command, "%s %u %u %s %u %u %u", name, &id_pre, &id_related_pre, color, &x, &y, &shapesize);
    if(length == 4 && color[0] == '\0')
    {
        length = 3; //some implementations of sscanfs add 1 to length if color is empty.
    }

    if(strcmp(name, "create_session") == 0)
    {
        init_session_sync(session);
        check_and_print_error(session);
    }
    else if(strcmp(name, "create_participant") == 0 && length == 2)
    {
        ObjectId id = {{id_pre, OBJK_PARTICIPANT}};
        create_participant_sync_by_ref(session, id, "default_participant", false, false);
        check_and_print_error(session);
    }
    else if(strcmp(name, "create_topic") == 0 && length == 3)
    {
        char xml[XML_BUFFER_SIZE];
        size_t file_length = read_file("shape_topic.xml", xml, XML_BUFFER_SIZE);
        if (file_length > 0)
        {
            ObjectId id = {{id_pre, OBJK_TOPIC}};
            ObjectId id_related = {{id_related_pre, OBJK_PARTICIPANT}};
            create_topic_sync_by_xml(session, id, xml, id_related, false, false);
            check_and_print_error(session);
        }
    }
    else if(strcmp(name, "create_publisher") == 0 && length == 3)
    {
        ObjectId id = {{id_pre, OBJK_PUBLISHER}};
        ObjectId id_related = {{id_related_pre, OBJK_PARTICIPANT}};
        create_publisher_sync_by_xml(session, id, "", id_related, false, false);
        check_and_print_error(session);
    }
    else if(strcmp(name, "create_subscriber") == 0 && length == 3)
    {
        ObjectId id = {{id_pre, OBJK_SUBSCRIBER}};
        ObjectId id_related = {{id_related_pre, OBJK_PARTICIPANT}};
        create_subscriber_sync_by_xml(session, id, "", id_related, false, false);
        check_and_print_error(session);
    }
    else if(strcmp(name, "create_datawriter") == 0 && length == 3)
    {
        char xml[XML_BUFFER_SIZE];
        size_t file_length = read_file("data_writer_profile.xml", xml, XML_BUFFER_SIZE - 1);
        if (file_length > 0)
        {
            ObjectId id = {{id_pre, OBJK_DATAWRITER}};
            ObjectId id_related = {{id_related_pre, OBJK_PUBLISHER}};
            create_datawriter_sync_by_xml(session, id, xml, id_related, false, false);
            check_and_print_error(session);
        }
    }
    else if(strcmp(name, "create_datareader") == 0 && length == 3)
    {
        char xml[XML_BUFFER_SIZE];
        size_t file_length = read_file("data_reader_profile.xml", xml, XML_BUFFER_SIZE);
        if (file_length > 0)
        {
            ObjectId id = {{id_pre, OBJK_DATAREADER}};
            ObjectId id_related = {{id_related_pre, OBJK_SUBSCRIBER}};
            create_datareader_sync_by_xml(session, id, xml, id_related, false, false);
            check_and_print_error(session);
        }
    }
    else if(strcmp(name, "write_data") == 0 && length >= 3)
    {
        ShapeType topic = {"GREEN", 100 , 100, 50};
        if (length == 7)
        {
           topic = (ShapeType){color, x , y, shapesize};
        }

        ObjectId id = {{id_pre, OBJK_DATAWRITER}};
        write_ShapeType(session, id, id_related_pre, &topic);
        printl_ShapeType_topic(&topic);
    }
    else if(strcmp(name, "read_data") == 0 && length == 3)
    {
        ObjectId id = {{id_pre, OBJK_DATAREADER}};
        read_data_sync(session, id, id_related_pre);
        check_and_print_error(session);
    }
    else if(strcmp(name, "delete") == 0 && length == 3)
    {
        ObjectId id = {{id_pre, id_related_pre}};
        delete_object_sync(session, id);
        check_and_print_error(session);
    }
    else if(strcmp(name, "exit") == 0)
    {
        close_session_sync(session);
        free_udp_session(session);
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

int main(int args, char** argv)
{
    printf("<< SHAPES DEMO XRCE CLIENT >>\n");

    Session my_session;
    ClientKey key = {{0xAA, 0xBB, 0xCC, 0xDD}};
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

        uint16_t port = atoi(argv[3]);
        if(!new_udp_session(&my_session, 0x01, key, ip, port, on_topic, NULL))
        {
            printf("%sCan not create a socket%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        printf("<< UDP mode => ip: %s - port: %hu >>\n", argv[2], port);
    }
    else
    {
        help();
        return 1;
    }

    // Waiting user commands
    char command_stdin_line[256];
    bool running = true;
    while (running)
    {
        if (!check_input())
        {
            run_communication(&my_session);
        }
        else if (fgets(command_stdin_line, 256, stdin))
        {
            if (!compute_command(command_stdin_line, &my_session))
            {
                running = false;
            }
        }

        ms_sleep(100);
    }

    return 0;
}



