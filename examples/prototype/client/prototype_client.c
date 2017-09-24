#include <micrortps/client/client.h>

#include "../common/file_io.h"
#include "../common/serial_port_io.h"
#include "../common/shape_topic.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define RED "\e[1;31m"
#define RESTORE_COLOR "\e[0m"

#define BUFFER_SIZE 1024

typedef struct UserData
{
    uint32_t recieved_topics;
    char command_file_name[256];

} UserData;

static char command_line[256];

int app(int args, char** argv);
void* read_from_stdin(void* args);
void compute_command(UserData* user, Topic* topic);

//callbacks for listening topics
void on_listener_shape_topic(const void* topic_data, void* callback_object);

int app(int args, char** argv)
{
    UserData user = {0, "command.io"};
    fclose(fopen(user.command_file_name, "w"));

    if(args ==  2)
    {
        if(strcmp(argv[1], "-f") == 0)
        {
            SharedFile* shared_file = malloc(sizeof(SharedFile));
            init_file_io(shared_file, "client_to_agent.bin", "agent_to_client.bin");
            fclose(fopen(user.command_file_name, "w"));

            init_client(BUFFER_SIZE, send_file_io, received_file_io, shared_file, &user);
        }
        else if(strcmp(argv[1], "-s") == 0)
        {
            SerialPort* serial_port = malloc(sizeof(SerialPort));
            init_serial_port_io(serial_port, "/dev/ttyACM0");

            init_client(BUFFER_SIZE, send_serial_port_io, received_serial_port_io, serial_port, &user);
        }
        else
        {
            printf(">> Write option: [ -f (file) | -s (serial_port)]\n");
            return 0;
        }
    }
    else
    {
        printf(">> Write option: [ -f (file) | -s (serial_port)]\n");
        return 0;
    }

    Topic topic =
    {
        "SQUARE",
        serialize_shape_topic,
        deserialize_shape_topic,
        size_of_shape_topic
    };

    uint32_t count = 0;
    uint32_t dt = 2;
    Publisher* pub = NULL;
    Subscriber* sub = NULL;
    char time_string[80];
    while(user.recieved_topics < 10)
    {
        time_t t = time(NULL);
        struct tm* timeinfo = localtime(&t);
        strftime(time_string, 80, "%T", timeinfo);

        // user code here
        compute_command(&user, &topic);

        // this function does all comunnications
        update_communication();

        usleep(1000000 * dt);

        count++;
    }

    return 0;
}

void on_listener_shape_topic(const void* topic_data, void* callback_object)
{
    const ShapeTopic* shape_topic = (const ShapeTopic*)topic_data;

    print_shape_topic(shape_topic);

    UserData* user = (UserData*)callback_object;
    user->recieved_topics++;
}

/*
COMMANDS:
    > create_pub
    > create_sub
    > config_read sub_id
    > send pub_id color x y size
    > delete_pub pub_id
    > delete_sub sub_id
*/

void compute_command(UserData* user, Topic* topic)
{
    char command[256];
    uint32_t id;
    char color[256];
    uint32_t x;
    uint32_t y;
    uint32_t size;
    int valid_command = 0;
    int command_size = -1;

    command_size = sscanf(command_line, "%s %u %s %u %u %u", command, &id, color, &x, &y, &size);
    command_line[0] = '\0';

    if(command_size == -1)
    {
        valid_command = 1;
    }
    else if(command_size == 1)
    {
        if(strcmp(command, "create_pub") == 0)
        {
            create_publisher(topic);
            valid_command = 1;
        }
        else if(strcmp(command, "create_sub") == 0)
        {
            Subscriber* subscriber = create_subscriber(topic);
            add_listener_topic(subscriber, on_listener_shape_topic);
            valid_command = 1;
        }
    }
    else if(command_size == 2)
    {
        if(strcmp(command, "delete") == 0)
        {
            void* object;
            int kind = get_xrce_object(id, &object);
            if(kind == OBJECT_PUBLISHER)
            {
                delete_publisher(object);
                if(((Publisher*)object)->object.status != OBJECT_STATUS_AVAILABLE)
                {
                    printf("        %sError%s\n", RED, RESTORE_COLOR);
                }
                valid_command = 1;
            }
            else if(kind == OBJECT_SUBSCRIBER)
            {
                delete_subscriber(object);
                if(((Subscriber*)object)->object.status != OBJECT_STATUS_AVAILABLE)
                {
                    printf("        %sError%s\n", RED, RESTORE_COLOR);
                }
                valid_command = 1;
            }
        }
    }
    else if(command_size == 3)
    {
        if(strcmp(command, "read_data") == 0)
        {
            void* object;
            if(get_xrce_object(id, &object) == OBJECT_SUBSCRIBER)
            {
                read_data(object, atoi(color));
                if(((Subscriber*)object)->object.status != OBJECT_STATUS_AVAILABLE)
                {
                    printf("        %sError%s\n", RED, RESTORE_COLOR);
                }
                valid_command = 1;
            }
        }
    }
    else if(command_size == 6)
    {
        if(strcmp(command, "send") == 0)
        {
            void* object;
            if(get_xrce_object(id, &object) == OBJECT_PUBLISHER)
            {
                ShapeTopic shape_topic = {strlen(color) + 1, color, x, y, size};
                if(send_topic(object, &shape_topic))
                    print_shape_topic(&shape_topic);
                else
                    printf("        %sError%s\n", RED, RESTORE_COLOR);
                valid_command = 1;
            }
        }
    }

    if(!valid_command)
        printf("Unknown command or incorrect input \n");
}

void* read_from_stdin(void* args)
{
    while(fgets(command_line, 256, stdin)){}
    return NULL;
}

int protoclient_main(int argc, char *argv[])
{
    pthread_t th;
    if(pthread_create(&th, NULL, read_from_stdin, NULL))
    {
        printf("ERROR: Error creating thread\n");
        return 1;
    }

    return app(argc, argv);
}

int main(int argc, char *argv[])
{
    return protoclient_main(argc, argv);
}