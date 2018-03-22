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
#include <unistd.h>

#define MAXBUFLEN 4096

// ----------------------------------------------------
//    App client
// ----------------------------------------------------
void on_hello_topic(uint16_t id, MicroBuffer *topic, void* args);
void printl_hello_topic(const HelloWorld *hello_topic);

void *listen_agent(void *args);

bool compute_command(const char *command, Session *session);

void list_commands();
void help();

size_t read_file(const char *file_name, char* data_file, size_t buf_size);

int check_input()
{
    struct timeval tv = {0, 0};
    fd_set fds = {{0, 0}};
    FD_ZERO(&fds);
    FD_SET(0, &fds); //STDIN 0
    select(1, &fds, NULL, NULL, &tv);
    return FD_ISSET(0, &fds);
}

int main(int args, char **argv)
{
    printf("<< HELLO WORLD XRCE CLIENT >>\n");

    uint8_t result = 0xFF;
    uint8_t my_buffer[MAXBUFLEN];
    Session my_session;

    if (args > 3)
    {
        if (strcmp(argv[1], "serial") == 0)
        {
            printf("<< Serial mode => dev: %s >>\n", argv[2]);
        }
        else if (strcmp(argv[1], "udp") == 0 && args == 5)
        {
            uint16_t received_port = atoi(argv[3]);
            uint16_t remote_port = atoi(argv[4]);
            result = new_udp_session_sync(&my_session, my_buffer, sizeof(my_buffer),
                                     4000, received_port, remote_port, argv[2]);
            printf("<< UDP mode => recv port: %u, send port: %u >>\n", received_port, remote_port);
        }
    }
    if (result != SESSION_CREATED)
    {
        printf("Help: program [serial | udp dest_ip recv_port send_port]\n");
        return 1;
    }

    // Waiting user commands
    printf(":>");
    char command_stdin_line[256];
    bool running = true;
    while (running)
    {
        if (!check_input())
        {
            receive_from_agent(&my_session);
        }
        else if (fgets(command_stdin_line, 256, stdin))
        {
            if (!compute_command(command_stdin_line, &my_session))
            {
                running = false;
                break;
            }
        }
        usleep(1000); // usleep takes sleep time in us (1 millionth of a second)
    }
}

bool compute_command(const char* command, Session* session)
{
    char name[128];
    char data_file[4096];
    static unsigned int hello_world_id = 0;
    int id = 0;
    int extra = 0;
    int length = sscanf(command, "%s %i %i", name, &id, &extra);

    if (strcmp(name, "create_client") == 0)
    {
        init_session(session, NULL, NULL);
    }
    else if (strcmp(name, "create_participant") == 0)
    {
        create_participant(session);
    }
    else if (strcmp(name, "create_topic") == 0 && length == 2)
    {
        size_t length = read_file("hello_topic.xml", data_file, MAXBUFLEN);
        if (length > 0)
        {
            create_topic(session, id, data_file);
        }
    }
    else if (strcmp(name, "create_publisher") == 0 && length == 2)
    {
        create_publisher(session, id);
    }
    else if (strcmp(name, "create_subscriber") == 0 && length == 2)
    {
        create_subscriber(session, id);
    }
    else if (strcmp(name, "create_data_writer") == 0 && length == 3)
    {
        size_t length = read_file("hello_data_writer_profile.xml", data_file, MAXBUFLEN);
        if (length > 0)
        {
            create_data_writer(session, id, extra, data_file);
        }
    }
    else if (strcmp(name, "create_data_reader") == 0 && length == 3)
    {
        size_t length = read_file("hello_data_reader_profile.xml", data_file, MAXBUFLEN);
        if (length > 0)
        {
            create_data_reader(session, id, extra, data_file);
        }
    }
    else if (strcmp(name, "write_data") == 0 && length == 2)
    {
        char message[] = "Hello from client";
        HelloWorld hello_topic = (HelloWorld){hello_world_id++, message};
        write_data(session, id, serialize_HelloWorld_topic, &hello_topic);
        printl_hello_topic(&hello_topic);
    }
    else if (strcmp(name, "read_data") == 0 && length == 2)
    {
        read_data(session, id, on_hello_topic, NULL);
    }
    else if (strcmp(name, "delete") == 0 && length == 2)
    {
        delete_resource(session, id);
    }
    else if (strcmp(name, "h") == 0 || strcmp(name, "help") == 0)
    {
        list_commands();
    }
    else if (strcmp(name, "exit") == 0)
    {
        return false;
    }
    else
    {
        help();
    }

    // only send data if there is.
    send_to_agent(session);

    return true;
}

void on_hello_topic(uint16_t id, MicroBuffer* message, void* args)
{
    (void)args;
    HelloWorld* topic = deserialize_HelloWorld_message(message);
    printl_hello_topic(topic);
}

void printl_hello_topic(const HelloWorld *hello_topic)
{
    printf("        %s[%s | index: %u]%s\n",
            "\x1B[1;34m",
            hello_topic->message,
            hello_topic->index,
            "\x1B[0m");
}

size_t read_file(const char *file_name, char* data_file, size_t buf_size)
{
    printf("READ FILE\n");
    FILE *fp = fopen(file_name, "r");
    size_t length = 0;
    if (fp != NULL)
    {
        length = fread(data_file, sizeof(char), buf_size, fp);
        if (length == 0)
        {
            printf("Error reading %s\n", file_name);
        }
        fclose(fp);
    }
    else
    {
        printf("Error opening %s\n", file_name);
    }

    return length;
}

void help()
{
    printf("usage: <command> [<args>]\n");
    printf("    h, help: for command list\n");
}

void list_commands()
{
    printf("usage: <command> [<args>]\n");
    printf("    create_client:                                       Creates a Client\n");
    printf("    create_participant:                                  Creates a new Participant on the current Client\n");
    printf("    create_topic <participant id>:                       Register new Topic using <participant id> participant\n");
    printf("    create_publisher <participant id>:                   Creates a Publisher on <participant id> participant\n");
    printf("    create_subscriber <participant id>:                  Creates a Subscriber on <participant id> participant\n");
    printf("    create_data_writer <participant id> <publisher id>:  Creates a DataWriter on the publisher <publisher id> of the <participant id> participant\n");
    printf("    create_data_reader <participant id> <subscriber id>: Creates a DataReader on the subscriber <subscriber id> of the <participant id> participant\n");
    printf("    write_data <data writer id>:                         Write data using <data writer id> DataWriter\n");
    printf("    read_data <data reader id>:                          Read data using <data reader id> DataReader\n");
    printf("    delete <id>:                                         Removes object with <id> identifier\n");
    printf("    exit:                                                Close program\n");
    printf("    h, help:                                             Shows this message\n");
}
