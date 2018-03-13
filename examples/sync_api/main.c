#include "HelloWorld.h"
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
    SyncSession my_session;
    ClientKey key = {{0xAA, 0xBB, 0xCC, 0xDD}};
    new_udp_session_sync(&my_session, 0x80, key, 4000, 2020, 2019, "127.0.0.1");
    status = init_session_syn(&my_session);

    ReliableStream my_reliable_stream;
    add_reliable_stream(&my_session, &my_reliable_stream, 129, true);
    remove_reliable_stream(&my_session, 129, true);

    BestEffortStream my_best_effort_stream;
    add_best_effort_stream(&my_session, &my_best_effort_stream, 2, true);
    remove_best_effort_stream(&my_session, 2, true);

    /* Create participant. */
    ObjectId participant_id = {{0x00, 0x01}};
    status = create_participant_by_ref(&my_session, participant_id, "default_participant", false, false);

    /* Create topic. */
    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId topic_id = {{0x00, 0x02}};
    status = create_topic_by_xml(&my_session, topic_id, topic_xml, participant_id, false, false);

    /* Create publisher. */
    const char* publisher_xml = {"<publisher name=\"Myblisher\""};
    ObjectId publisher_id = {{0x00, 0x03}};
    status = create_publisher_by_xml(&my_session, publisher_id, publisher_xml, participant_id, false, false);

    /* Create data writer. */
    const char* datawriter_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datawriter_id = {{0x00, 0x05}};
    status = create_datawriter_by_xml(&my_session, datawriter_id, datawriter_xml, publisher_id, false, false);

    return 0;
}








