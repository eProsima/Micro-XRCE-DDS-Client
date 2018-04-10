#include "HelloWorld.h"
#include <micrortps/client/xrce_client.h>

#include <stdio.h>
#include <unistd.h>

#define HELLO_WORLD_TOPIC 1

void on_HelloWorld_topic(ObjectId id, MicroBuffer* serialized_topic, void* args)
{
    switch(id.data[0])
    {
        case HELLO_WORLD_TOPIC:
        {
            HelloWorld topic;
            deserialize_HelloWorld_topic(serialized_topic, &topic);
            printf("Received topic: %s\n, count: %i", topic.m_message, topic.m_index);
            break;
        }

        default:
            break;
    }
}

int main()
{
    /* Init session. */
    Session my_session;
    ClientKey key = {{0xAA, 0xBB, 0xCC, 0xDD}};
    uint8_t ip[] = {127, 0, 0, 1};
    if (!new_udp_session(&my_session, 0x01, key, ip, 2019, on_HelloWorld_topic, NULL))
    {
        return 1;
    }

    init_session_syn(&my_session);

    /* Create participant. */
    ObjectId participant_id = {{0x00, OBJK_PARTICIPANT}};
    create_participant_sync_by_ref(&my_session, participant_id, "default_participant", false, false);

    /* Create topic. */
    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId topic_id = {{0x00, OBJK_TOPIC}};
    create_topic_sync_by_xml(&my_session, topic_id, topic_xml, participant_id, false, false);

    /* Create publisher. */
    const char* publisher_xml = {"<publisher name=\"MyPublisher\""};
    ObjectId publisher_id = {{HELLO_WORLD_TOPIC, OBJK_PUBLISHER}};
    create_publisher_sync_by_xml(&my_session, publisher_id, publisher_xml, participant_id, false, false);

    /* Create data writer. */
    const char* datawriter_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datawriter_id = {{HELLO_WORLD_TOPIC, OBJK_DATAWRITER}};
    create_datawriter_sync_by_xml(&my_session, datawriter_id, datawriter_xml, publisher_id, false, false);

    /* Create subscriber. */
    const char* subscriber_xml = {"<publisher name=\"MySubscriber\""};
    ObjectId subscriber_id = {{HELLO_WORLD_TOPIC, OBJK_SUBSCRIBER}};
    create_subscriber_sync_by_xml(&my_session, subscriber_id, subscriber_xml, participant_id, false, false);

    /* Create data writer. */
    const char* datareader_xml = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};
    ObjectId datareader_id = {{HELLO_WORLD_TOPIC, OBJK_DATAREADER}};
    create_datareader_sync_by_xml(&my_session, datareader_id, datareader_xml, subscriber_id, false, false);


    while(true)
    {
        OutputBestEffortStream* best_effort = &my_session.output_best_effort_stream;

        HelloWorld topic1 = {1, "Hello MicroRTPS!"};
        uint32_t topic_size_1 = size_of_HelloWorld_topic(&topic1);
        MicroBuffer* topic_buffer_1 = prepare_best_effort_stream_for_topic(best_effort, datawriter_id, topic_size_1);
        if(topic_buffer_1 != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer_1, &topic1);
        }

        HelloWorld topic2 = {2, "Hello MicroRTPS!"};
        uint32_t topic_size_2 = size_of_HelloWorld_topic(&topic2);
        MicroBuffer* topic_buffer_2 = prepare_best_effort_stream_for_topic(best_effort, datawriter_id, topic_size_2);
        if(topic_buffer_2 != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer_2, &topic2);
        }


        OutputReliableStream* reliable = &my_session.output_reliable_stream;

        HelloWorld topic3 = {3, "Hello MicroRTPS!"};
        uint32_t topic_size_3 = size_of_HelloWorld_topic(&topic3);
        MicroBuffer* topic_buffer_3 = prepare_reliable_stream_for_topic(reliable, datawriter_id, topic_size_3);
        if(topic_buffer_3 != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer_3, &topic3);
        }

        HelloWorld topic4 = {4, "Hello MicroRTPS!"};
        uint32_t topic_size_4 = size_of_HelloWorld_topic(&topic4);
        MicroBuffer* topic_buffer_4 = prepare_reliable_stream_for_topic(reliable, datawriter_id, topic_size_4);
        if(topic_buffer_4 != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer_4, &topic4);
        }

        read_data_sync(&my_session, datareader_id);

        run_communication(&my_session);
        sleep(5);
    }

    close_session(&my_session);

    return 0;
}

