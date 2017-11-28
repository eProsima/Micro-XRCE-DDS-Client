#include <micrortps/client/client.h>
#include <microcdr/microcdr.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

String read_file(char* file_name)
{
    printf("READ FILE\n");
    const size_t MAXBUFLEN = 4096;
    char data[MAXBUFLEN];
    String xml = {data, 0};
    FILE *fp = fopen(file_name, "r");
    if (fp != NULL)
    {
        xml.length = fread(xml.data, sizeof(char), MAXBUFLEN, fp);
        if (xml.length == 0)
        {
            printf("Error reading %s\n", file_name);
        }
        fclose(fp);
    }
    else
    {
        printf("Error opening %s\n", file_name);
    }

     return xml;
}


// User type declaration
typedef struct HelloWorld
{
    uint32_t message_length;
    char* message;
} HelloTopic;

// Serialization implementation provided by the user. Uses Eprosima MicroCDR.
bool serialize_hello_topic(MicroBuffer* writer, const AbstractTopic* topic_structure)
{
    HelloTopic* topic = (HelloTopic*) topic_structure->topic;
    serialize_array_char(writer, topic->message, topic->message_length);
    return true;
}

// User callback for receiving status messages from the Agent.
void on_status(XRCEInfo info, uint8_t operation, uint8_t status, void* args)
{
    // Process status message.
};

int main(int args, char** argv)
{
    // Creates a client state.
    ClientState* state = new_udp_client_state(4096, 2019, 2020);

    // Creates this client on the Agent.
    create_client(state, on_status, NULL);
    // Creates a participant on the Agent.
    XRCEInfo participant_info = create_participant(state);

    // Register a topic on the given participant. Uses a topic configuration read from xml file
    String topic_profile = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>", 86+1};
    create_topic(state, participant_info.object_id, topic_profile);


    // Creates a publisher on the given participant
    XRCEInfo publisher_info = create_publisher(state, participant_info.object_id);


    String data_writer_profile = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>",
    300+1};

    XRCEInfo data_writer_info = create_data_writer(state, participant_info.object_id, publisher_info.object_id, data_writer_profile); //read_file("hello_data_writer_profile.xml"));

    // Prepare and write the user data to be sent.
    char message[] = "Hello DDS world!";
    uint32_t length = strlen(message) + 1;
    HelloTopic hello_topic = {length, message};
    // Write user type data.
    write_data(state, data_writer_info.object_id, serialize_hello_topic, &hello_topic);

    // Send the data through the UDP transport.
    send_to_agent(state);

    // Free all the ClientState resources.
    free_client_state(state);
}