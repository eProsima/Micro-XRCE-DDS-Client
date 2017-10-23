#include <micrortps/client/client.h>
#include <microcdr/microcdr.h>

#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

typedef struct ShapeTopic
{
    uint32_t color_length;
    char*    color;
    uint32_t x;
    uint32_t y;
    uint32_t size;

} ShapeTopic;

void serialize_shape_topic(const MicroBuffer* buffer, const void* topic_structure);
void deserialize_shape_topic(MicroBuffer* buffer, uint8_t* topic);

void on_shape_topic(const void* topic, void* callback_object);

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

    char topic[] = "SQUARE";
    uint32_t topic_length = strlen(topic) + 1;

    create_client(state);
    send_to_agent(state);

    uint16_t participant_id = create_participant(state);
    send_to_agent(state);

    uint16_t publisher_id = create_publisher(state, participant_id);
    send_to_agent(state);

    uint16_t subscriber_id = create_subscriber(state, participant_id);
    send_to_agent(state);

    uint16_t data_writer_id =
    create_data_writer(state, participant_id, publisher_id, topic, topic_length, serialize_shape_topic);
    send_to_agent(state);

    uint16_t data_reader_id =
    create_data_reader(state, participant_id, subscriber_id, topic, topic_length, deserialize_shape_topic);
    send_to_agent(state);

    ShapeTopic shape_topic = {strlen("PURPLE") + 1, "PURPLE", 100 , 100, 50};
    write_data(state, data_writer_id, &shape_topic);
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

void serialize_shape_topic(const MicroBuffer* buffer, const void* topic_structure)
{
    //TODO
}

void deserialize_shape_topic(MicroBuffer* buffer, uint8_t* topic)
{
    //TODO
}

void on_shape_topic(const void* topic, void* callback_object)
{
    //TODO
}