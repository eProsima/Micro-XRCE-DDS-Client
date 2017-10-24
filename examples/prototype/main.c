#include <micrortps/client/client.h>
#include <microcdr/microcdr.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

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

void on_shape_topic(const void* topic, void* callback_object);

void printl_shape_topic(const ShapeTopic* shape_topic);

int main(int args, char** argv)
{
    printf("--- XRCE CLIENT ---\n");

    ClientState* state = NULL;
    if(args > 2)
    {
        if(strcmp(argv[1], "serial") == 0)
        {
            state = new_serial_client_state(BUFFER_SIZE, "/dev/ttyACM0");
        }
        else if(strcmp(argv[1], "udp") == 0 && args == 4)
        {
            state = new_udp_client_state(BUFFER_SIZE, 2000, 2001);
        }
    }
    if(!state)
        printf("Help: program [serial | udp recv_port send_port]");


    create_client(state);
    send_to_agent(state);

    uint16_t participant_id = create_participant(state);
    send_to_agent(state);

    String topic_name = {"SQUARE", strlen("SQUARE") + 1};
    uint16_t topic_id = create_topic(state, participant_id, topic_name, serialize_shape_topic, deserialize_shape_topic);
    send_to_agent(state);

    uint16_t publisher_id = create_publisher(state, participant_id);
    send_to_agent(state);

    uint16_t subscriber_id = create_subscriber(state, participant_id);
    send_to_agent(state);

    uint16_t data_writer_id =
    create_data_writer(state, participant_id, publisher_id, topic_name);
    send_to_agent(state);

    uint16_t data_reader_id =
    create_data_reader(state, participant_id, subscriber_id, topic_name);
    send_to_agent(state);

    ShapeTopic shape_topic = {strlen("PURPLE") + 1, "PURPLE", 100 , 100, 50};
    write_data(state, data_writer_id, &shape_topic);
    printl_shape_topic(&shape_topic);
    send_to_agent(state);

    read_data(state, data_reader_id, on_shape_topic, NULL, 5);
    send_to_agent(state);

    delete_resource(state, data_reader_id);
    send_to_agent(state);

    delete_resource(state, data_writer_id);
    send_to_agent(state);

    delete_resource(state, subscriber_id);
    send_to_agent(state);

    delete_resource(state, publisher_id);
    send_to_agent(state);

    delete_resource(state, participant_id);
    send_to_agent(state);

    free_client_state(state);

    return 0;
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

void on_shape_topic(const void* vtopic, void* callback_object)
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