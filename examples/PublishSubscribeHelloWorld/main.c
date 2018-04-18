#include "HelloWorld.h"
#include <micrortps/client/xrce_client.h>

#include <stdio.h>

#define HELLO_WORLD_TOPIC 1

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

void on_HelloWorld_topic(ObjectId id, MicroBuffer* serialized_topic, void* args)
{
    switch(id.data[0])
    {
        case HELLO_WORLD_TOPIC:
        {
            HelloWorld topic;
            deserialize_HelloWorld_topic(serialized_topic, &topic);
            printf("Read topic: %s, count: %i\n", topic.message, topic.index);
            break;
        }

        default:
            break;
    }
}

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("Usage: program agent_ip agent_port\n");
        return 1;
    }

    /* Init session. */
    Session my_session;
    ClientKey key = {{0xBB, 0xBB, 0xCC, 0xDD}};
    uint8_t ip[] = {atoi(strtok(argv[1], ".")), atoi(strtok(NULL, ".")),
                    atoi(strtok(NULL, ".")), atoi(strtok(NULL, "."))};
    uint16_t port = atoi(argv[2]);
    if (!new_udp_session(&my_session, 0x01, key, ip, port, on_HelloWorld_topic, NULL))
    {
        printf("Error: socket is not available.");
        return 1;
    }

    init_session_sync(&my_session);
    check_and_print_error(&my_session);

    /* Create participant. */
    ObjectId participant_id = {{0x00, OBJK_PARTICIPANT}};
    create_participant_sync_by_ref(&my_session, participant_id, "default_participant", false, false);
    check_and_print_error(&my_session);

    /* Create topic. */
    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId topic_id = {{0x00, OBJK_TOPIC}};
    create_topic_sync_by_xml(&my_session, topic_id, topic_xml, participant_id, false, false);
    check_and_print_error(&my_session);

    /* Create publisher. */
    const char* publisher_xml = {"<publisher name=\"MyPublisher\""};
    ObjectId publisher_id = {{HELLO_WORLD_TOPIC, OBJK_PUBLISHER}};
    create_publisher_sync_by_xml(&my_session, publisher_id, publisher_xml, participant_id, false, false);
    check_and_print_error(&my_session);

    /* Create data writer. */
    const char* datawriter_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datawriter_id = {{HELLO_WORLD_TOPIC, OBJK_DATAWRITER}};
    create_datawriter_sync_by_xml(&my_session, datawriter_id, datawriter_xml, publisher_id, false, false);
    check_and_print_error(&my_session);

    /* Create subscriber. */
    const char* subscriber_xml = {"<publisher name=\"MySubscriber\""};
    ObjectId subscriber_id = {{HELLO_WORLD_TOPIC, OBJK_SUBSCRIBER}};
    create_subscriber_sync_by_xml(&my_session, subscriber_id, subscriber_xml, participant_id, false, false);
    check_and_print_error(&my_session);

    /* Create data writer. */
    const char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
    ObjectId datareader_id = {{HELLO_WORLD_TOPIC, OBJK_DATAREADER}};
    create_datareader_sync_by_xml(&my_session, datareader_id, datareader_xml, subscriber_id, false, false);
    check_and_print_error(&my_session);

    uint32_t counter = 0;
    while(true)
    {
        HelloWorld topic = {counter, "Hello DDS World!"};
        if(write_HelloWorld(&my_session, datawriter_id, STREAMID_BUILTIN_RELIABLE, &topic))
        {
            counter++;
            printf("Write topic: %s, count: %i\n", topic.message, topic.index);
        }

        read_data_sync(&my_session, datareader_id, STREAMID_BUILTIN_RELIABLE);
        check_and_print_error(&my_session);

        run_communication(&my_session);
        ms_sleep(2000);
    }

    close_session_sync(&my_session);
    free_udp_session(&my_session);

    return 0;
}

