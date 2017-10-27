#include <micrortps/client/client.h>
#include <microcdr/microcdr.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

// ----------------------------------------------------
//    User topic definition
// ----------------------------------------------------
typedef struct ShapeTopic
{
    uint32_t color_length;
    char*    color;
    uint32_t x;
    uint32_t y;
    uint32_t size;

} ShapeTopic;

bool serialize_shape_topic(MicroBuffer* writer, const AbstractTopic* topic_structure);
bool deserialize_shape_topic(MicroBuffer* reader, AbstractTopic* topic_serialization);

// ----------------------------------------------------
//    App client
// ----------------------------------------------------
void on_shape_topic(const void* topic, void* args);
void printl_shape_topic(const ShapeTopic* shape_topic);
void* listen_agent(void* args);
bool compute_command(const char* command, ClientState* state);

bool stop_listening = false;

int main(int args, char** argv)
{
    printf("<< XRCE CLIENT >>\n");

    ClientState* state = NULL;
    if(args > 2)
    {
        if(strcmp(argv[1], "serial") == 0)
        {
            state = new_serial_client_state(BUFFER_SIZE, argv[2]);
            printf("<< Serial mode => dev: %s >>\n", argv[2]);
        }
        else if(strcmp(argv[1], "udp") == 0 && args == 4)
        {
            uint16_t received_port = atoi(argv[2]);
            uint16_t send_port = atoi(argv[3]);
            state = new_udp_client_state(BUFFER_SIZE, received_port, send_port);
            printf("<< UDP mode => recv port: %u, send port: %u >>\n", received_port, send_port);
        }
    }
    if(!state)
        printf("Help: program [serial | udp recv_port send_port]");


    // Listening agent
    pthread_t listening_thread;
    if(pthread_create(&listening_thread, NULL, listen_agent, state))
    {
        printf("ERROR: Error creating thread\n");
        return 1;
    }

    // Waiting user commands
    char command_stdin_line[256];
    while(fgets(command_stdin_line, 256, stdin))
    {
        if(!compute_command(command_stdin_line, state))
        {
            stop_listening = true;
            break;
        }
    }

    //pthread_join(listening_thread, NULL);

    free_client_state(state);

    return 0;
}

void* listen_agent(void* args)
{
    while(!stop_listening)
    {
        // only read data if there is. (NOT WORKS AT THIS POINT)
        receive_from_agent((ClientState*) args);
        //usleep(1000000);
    }

    return NULL;
}

bool compute_command(const char* command, ClientState* state)
{
    char name[128];
    int id = 0;
    int extra = 0;
    int length = sscanf(command, "%s %u %u", name, &id, &extra);


    if(strcmp(name, "create_client") == 0)
    {
        create_client(state, NULL, NULL);
    }
    else if(strcmp(name, "create_participant") == 0)
    {
        create_participant(state);
    }
    else if(strcmp(name, "create_topic") == 0 && length == 2)
    {
        String topic_name = {"SQUARE", strlen("SQUARE") + 1};
        create_topic(state, id, topic_name);
    }
    else if(strcmp(name, "create_publisher") == 0 && length == 2)
    {
        create_publisher(state, id);
    }
    else if(strcmp(name, "create_subscriber") == 0 && length == 2)
    {
        create_subscriber(state, id);
    }
    else if(strcmp(name, "create_data_writer") == 0 && length == 3)
    {
        String xml = {"SQUARE", strlen("SQUARE") + 1};
        create_data_writer(state, id, extra, xml);
    }
    else if(strcmp(name, "create_data_reader") == 0 && length == 3)
    {
        String xml = {"SQUARE", strlen("SQUARE") + 1};
        create_data_reader(state, id, extra, xml);
    }
    else if(strcmp(name, "write_data") == 0 && length == 2)
    {
        ShapeTopic shape_topic = {strlen("PURPLE") + 1, "PURPLE", 100 , 100, 50};
        write_data(state, id, serialize_shape_topic, &shape_topic);
    }
    else if(strcmp(name, "read_data") == 0 && length == 2)
    {
        read_data(state, id, deserialize_shape_topic, on_shape_topic, NULL);
    }
    else if(strcmp(name, "delete") == 0 && length == 2)
    {
        delete_resource(state, id);
    }

    // only send data if there is.
    send_to_agent(state);

    // close client
    if(strcmp(name, "exit") == 0)
        return false;

    return true;
}

bool serialize_shape_topic(MicroBuffer* writer, const AbstractTopic* topic_structure)
{
    ShapeTopic* topic = (ShapeTopic*) topic_structure->topic;

    serialize_uint32_t(writer, topic->color_length);
    serialize_array_char(writer, topic->color, topic->color_length);
    serialize_uint32_t(writer, topic->x);
    serialize_uint32_t(writer, topic->y);
    serialize_uint32_t(writer, topic->size);

    return true;
}

bool deserialize_shape_topic(MicroBuffer* reader, AbstractTopic* topic_structure)
{
    ShapeTopic* topic = malloc(sizeof(ShapeTopic));

    deserialize_uint32_t(reader, &topic->color_length);
    topic->color = malloc(sizeof(topic->color_length));
    deserialize_array_char(reader, topic->color, topic->color_length);
    deserialize_uint32_t(reader, &topic->x);
    deserialize_uint32_t(reader, &topic->y);
    deserialize_uint32_t(reader, &topic->size);

    topic_structure->topic = topic;

    return true;
}

void on_shape_topic(const void* vtopic, void* args)
{
    ShapeTopic* topic = (ShapeTopic*) vtopic;
    printl_shape_topic(topic);

    free(topic->color);
    free(topic);
}

void printl_shape_topic(const ShapeTopic* shape_topic)
{
    printf("        %s[%s | x: %u | y: %u | size: %u]%s\n",
            "\e[1;34m",
            shape_topic->color,
            shape_topic->x,
            shape_topic->y,
            shape_topic->size,
            "\e[0m");
}