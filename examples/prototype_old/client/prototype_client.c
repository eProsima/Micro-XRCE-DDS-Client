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

#define WHITE "\e[1;37m"
#define RED "\e[1;31m"
#define RESTORE_COLOR "\e[0m"

#define BUFFER_SIZE 1024

typedef struct UserData
{
    uint32_t recieved_topics;
    char* command_file_name;

} UserData;

// Global resources
int shared__compute_stdin_command = 0;
char command_stdin_line[256];

// Pointer to callbacks for io
void (*send_io)(uint8_t* buffer, uint32_t length, void* data);
uint32_t (*received_io)(uint8_t* buffer, uint32_t size, void* data);
void* resource_io = NULL;

// Functions prototypes
void print_help(void);
int app_example(int args, char** argv);
void* read_from_stdin(void* args);
void compute_command(UserData* user, Topic* topic);
#ifdef STM32F427
int protoclient_main(int argc, char** argv);
#endif

//callbacks for listening topics
void on_listener_shape_topic(const void* topic_data, void* args);



void print_help(void)
{
    printf("HELP: Write option: [ -f (file) | -s (serial_port)] [-c command_file]\n");
}

int app_example(int args, char** argv)
{
    UserData user;
    user.recieved_topics = 0;
    user.command_file_name = NULL;

    if(args >=  2)
    {
        if(strcmp(argv[1], "-f") == 0)
        {
            SharedFile* shared_file = malloc(sizeof(SharedFile));
            init_file_io(shared_file, "client_to_agent.bin", "agent_to_client.bin");
            send_io = send_file_io;
            received_io = received_file_io;
            resource_io = shared_file;
        }
        else if(strcmp(argv[1], "-s") == 0)
        {
            SerialPort* serial_port = malloc(sizeof(SerialPort));
            init_serial_port_io(serial_port, "/dev/ttyACM0");
            send_io = send_serial_port_io;
            received_io = received_serial_port_io;
            resource_io = serial_port;
        }
    }
    if(args == 4)
    {
        if(strcmp(argv[2], "-c") == 0)
        {
            user.command_file_name = argv[3];
            fclose(fopen(user.command_file_name, "w"));
        }
    }

    if(!resource_io)
    {
        print_help();
        return 0;
    }

    printf("Running DDS-XRCE Client...\n");

    Topic topic =
    {
        "Square",
        serialize_shape_topic,
        deserialize_shape_topic,
        size_of_shape_topic
    };


    while(user.recieved_topics < 1000)
    {
        // User code goes here
        // ...

        compute_command(&user, &topic);

        // this function does all comunnications
        update_communication();

		//fflush(stdin);
        usleep(1000000);
    }

    return 0;
}

void on_listener_shape_topic(const void* topic_data, void* args)
{
    const ShapeTopic* shape_topic = (const ShapeTopic*)topic_data;
    print_shape_topic(shape_topic);

    UserData* user = (UserData*)args;
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
    char color[256] = {'\0'};
    uint32_t x;
    uint32_t y;
    uint32_t size;
    int valid_command = 0;

    int command_size = -1;


    //Using the stdin for read the command
    if(shared__compute_stdin_command)
    {
        printf("%s%s%s", WHITE, command_stdin_line, RESTORE_COLOR);
        command_size = sscanf(command_stdin_line, "%s %u %s %u %u %u", command, &id, color, &x, &y, &size);
        command_stdin_line[0] = '\0';
        shared__compute_stdin_command = 0;
    }


    //Using the command file for read the command
    if(user->command_file_name != NULL)
    {
        FILE* file = fopen(user->command_file_name, "r");
        char command_line[256];
        if(file != NULL && fgets(command_line, 256, file))
        {
            printf("%s%s%s", WHITE, command_line, RESTORE_COLOR);
            command_size = sscanf(command_line, "%s %u %s %u %u %u", command, &id, color, &x, &y, &size);
            fclose(file);
        }
        fclose(fopen(user->command_file_name, "w"));
    }

    // because some scanf implementations interpret no string as an empty string.
    if(command_size == 3 && color[0] == '\0')
        command_size = 2;

    if(command_size == -1)
    {
        //no commands is a valid commnad;
        valid_command = 1;
    }
    else if(command_size == 1)
    {
        if(strcmp(command, "create_client") == 0)
        {
            init_client(BUFFER_SIZE, send_io, received_io, resource_io, user);
            valid_command = 1;
        }
        else if(strcmp(command, "create_participant") == 0)
        {
            create_participant();
            valid_command = 1;
        }
        else if(strcmp(command, "delete_client") == 0)
        {
            destroy_client(); //make as another delete xrce object.
            valid_command = 1;
        }
    }
    else if(command_size == 2)
    {
        if(strcmp(command, "create_publisher") == 0)
        {
            void* participant;
            if(get_xrce_object(id, &participant) == OBJECT_PARTICIPANT)
            {
                Publisher* publisher = create_publisher(participant, topic);
                create_data_writer(publisher);
                valid_command = 1;
            }
        }
        else if(strcmp(command, "create_subscriber") == 0)
        {
            void* participant;
            if(get_xrce_object(id, &participant) == OBJECT_PARTICIPANT)
            {
                Subscriber* subscriber = create_subscriber(participant, topic);
                create_data_reader(subscriber);
                add_listener_topic(subscriber, on_listener_shape_topic);
                valid_command = 1;
            }
        }
        else if(strcmp(command, "delete") == 0)
        {
            void* object;
            int kind = get_xrce_object(id, &object);
            if(kind == OBJECT_PARTICIPANT)
            {
                delete_participant(object);
                if(((Participant*)object)->object.status != OBJECT_STATUS_AVAILABLE)
                {
                    printf("        %sError%s\n", RED, RESTORE_COLOR);
                }
                valid_command = 1;
            }
            else if(kind == OBJECT_PUBLISHER)
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
        if(strcmp(command, "write_data") == 0)
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
    while(fgets(command_stdin_line, 256, stdin))
    {
        #ifdef STM32F427
        getc(stdin);
        #endif //STM32F427
        shared__compute_stdin_command = 1;
    }
    return NULL;
}


#ifdef STM32F427
int protoclient_main(int argc, char** argv)
#else
int main(int argc, char** argv)
#endif //STM32F427
{
    // Read for reading from stdin
    pthread_t th;
    if(pthread_create(&th, NULL, read_from_stdin, NULL))
    {
        printf("%sERROR: Error creating thread%s\n", RED, RESTORE_COLOR);
        return 1;
    }

    return app_example(argc, argv);
}