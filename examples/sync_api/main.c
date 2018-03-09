#include <micrortps/client/sync_client.h>

#include <stdio.h>
#include <string.h>


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

// User callback for receiving status messages from the Agent.
void on_status(XRCEInfo info, uint8_t operation, uint8_t status, void* args)
{
    (void)info;
    (void)operation;
    (void)status;
    (void)args;
    // Process status message.
}

int main()
{
    ResultStatus status;

    /* Init session. */
    ClientState* state = new_udp_client_state(4096, 4000, 2020, 2019, "127.0.0.1");
    create_client(state, on_status, NULL);
    send_to_agent(state);

    /* Create participant. */
    ObjectId my_participant = {{0x00, 0x01}};
    status = create_participant_by_ref(state, my_participant, "default_participant", false, false);

    /* Create topic. */
    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId my_topic = {{0x00, 0x02}};
    status = create_topic_by_xml(state, my_topic, topic_xml, my_participant, false, false);

    /* Create publisher. */
    const char* publisher_xml = {"<publisher name=\"Myblisher\""};
    ObjectId my_publisher = {{0x00, 0x03}};
    status = create_publisher_by_xml(state, my_publisher, publisher_xml, my_participant, false, false);

    /* Create data writer. */
    const char* datawriter_xml = {"<data_writer name=\"MySquareWriter\" topic_ref=\"HelloWorldTopic\""};
    ObjectId my_datawriter = {{0x00, 0x05}};
    status = create_datawriter_by_xml(state, my_datawriter, datawriter_xml, my_publisher, false, false);
}
