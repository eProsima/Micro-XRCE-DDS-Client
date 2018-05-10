#include "HelloWorld.h"
#include <micrortps/client/xrce_client.h>

#include <stdio.h>

#define HELLO_WORLD_TOPIC 1

void check_and_print_error(Session* session, const char* where)
{
    if(session->last_status_received)
    {
        if(session->last_status.status == STATUS_OK)
        {
            return; //All things go well
        }
        else
        {
            printf("%sStatus error (%i)%s", "\x1B[1;31m", session->last_status.status, "\x1B[0m");
        }
    }
    else
    {
        printf("%sConnection error", "\x1B[1;31m");
    }

    printf(" at %s%s\n", where, "\x1B[0m");
    exit(1);
}

void on_topic(ObjectId id, MicroBuffer* serialized_topic, void* args)
{
    (void) args;
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

int main(int args, char** argv)
{
    Session my_session;
    ClientKey key = {{0xAA, 0xBB, 0xCC, 0xDD}};
    if(args == 3 && strcmp(argv[1], "serial") == 0)
    {
        const char* device = argv[2];
        if(!new_serial_session(&my_session, 0x01, key, device, on_topic, NULL))
        {
            printf("%sCan not create serial connection%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        printf("<< Serial mode => dev: %s >>\n", device);
    }
    else if(args == 4 && strcmp(argv[1], "udp") == 0)
    {
        uint8_t ip[4];
        int length = sscanf(argv[2], "%d.%d.%d.%d", (int*)&ip[0], (int*)&ip[1], (int*)&ip[2], (int*)&ip[3]);
        if(length != 4)
        {
            printf("%sIP must have th format a.b.c.d%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }

        uint16_t port = (uint16_t)atoi(argv[3]);
        if(!new_udp_session(&my_session, 0x01, key, ip, port, on_topic, NULL))
        {
            printf("%sCan not create a socket%s\n", "\x1B[1;31m", "\x1B[0m");
            return 1;
        }
        printf("<< UDP mode => ip: %s - port: %hu >>\n", argv[2], port);
    }
    else
    {
        printf("Usage: program <command>\n");
        printf("List of commands:\n");
        printf("    serial device\n");
        printf("    udp agent_ip agent_port\n");
        printf("    help\n");
        return 1;
    }

    /* Init session. */
    init_session_sync(&my_session);
    check_and_print_error(&my_session, "init session");

    /* Create participant. */
    ObjectId participant_id = {{0x00, OBJK_PARTICIPANT}};
    create_participant_sync_by_ref(&my_session, participant_id, "default_participant", false, false);
    check_and_print_error(&my_session, "create participant");

    /* Create topic. */
    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId topic_id = {{0x00, OBJK_TOPIC}};
    create_topic_sync_by_xml(&my_session, topic_id, topic_xml, participant_id, false, false);
    check_and_print_error(&my_session, "create topic");

    /* Create publisher. */
    const char* publisher_xml = {"<publisher name=\"MyPublisher\""};
    ObjectId publisher_id = {{HELLO_WORLD_TOPIC, OBJK_PUBLISHER}};
    create_publisher_sync_by_xml(&my_session, publisher_id, publisher_xml, participant_id, false, false);
    check_and_print_error(&my_session, "create publisher");

    /* Create data writer. */
    const char* datawriter_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datawriter_id = {{HELLO_WORLD_TOPIC, OBJK_DATAWRITER}};
    create_datawriter_sync_by_xml(&my_session, datawriter_id, datawriter_xml, publisher_id, false, false);
    check_and_print_error(&my_session, "create datawriter");

    /* Create subscriber. */
    const char* subscriber_xml = {"<publisher name=\"MySubscriber\""};
    ObjectId subscriber_id = {{HELLO_WORLD_TOPIC, OBJK_SUBSCRIBER}};
    create_subscriber_sync_by_xml(&my_session, subscriber_id, subscriber_xml, participant_id, false, false);
    check_and_print_error(&my_session, "create subscriber");

    /* Create data writer. */
    const char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
    ObjectId datareader_id = {{HELLO_WORLD_TOPIC, OBJK_DATAREADER}};
    create_datareader_sync_by_xml(&my_session, datareader_id, datareader_xml, subscriber_id, false, false);
    check_and_print_error(&my_session, "create datareader");

    uint32_t counter = 0;
    while(true)
    {
        HelloWorld topic;
	topic.index = counter;
	topic.message = "Hello DDS World!";
        if(write_HelloWorld(&my_session, datawriter_id, STREAMID_BUILTIN_RELIABLE, &topic))
        {
            counter++;
            printf("Write topic: %s, count: %i\n", topic.message, topic.index);
        }

        read_data_sync(&my_session, datareader_id, STREAMID_BUILTIN_RELIABLE);
        check_and_print_error(&my_session, "read data");

        run_communication(&my_session);
        ms_sleep(2000);
    }

    close_session_sync(&my_session);
    check_and_print_error(&my_session, "close session");

    free_session(&my_session);

    return 0;
}

