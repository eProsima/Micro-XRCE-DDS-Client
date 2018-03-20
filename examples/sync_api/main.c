#include "HelloWorld.h"
#include <micrortps/client/sync_client.h>

#include <stdio.h>
#include <string.h>

#define HELLO_WORLD_TOPIC 1

int main()
{
    /* Init session. */
    SyncSession my_session;
    ClientKey key = {{0xAA, 0xBB, 0xCC, 0xDD}};
    if (MICRORTPS_STATUS_OK != new_udp_session_sync(&my_session, 0x80, key, 4000, 2020, 2019, "127.0.0.1"))
    {
        return 1;
    }

    init_session_syn(&my_session);

    /* Create participant. */
    ObjectId participant_id = {{0x00, 0x01}};
    create_participant_by_ref(&my_session, participant_id, "default_participant", false, false);

    /* Create topic. */
    const char* topic_xml = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
    ObjectId topic_id = {{0x00, 0x02}};
    create_topic_by_xml(&my_session, topic_id, topic_xml, participant_id, false, false);

    /* Create publisher. */
    const char* publisher_xml = {"<publisher name=\"MyPublisher\""};
    ObjectId publisher_id = {{0x00, 0x03}};
    create_publisher_by_xml(&my_session, publisher_id, publisher_xml, participant_id, false, false);

    /* Create data writer. */
    const char* datawriter_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datawriter_id = {{0x00, 0x05}};
    create_datawriter_by_xml(&my_session, datawriter_id, datawriter_xml, publisher_id, false, false);

    /* Create publisher. */
    const char* subscriber_xml = {"<publisher name=\"MySubscriber\""};
    ObjectId subscriber_id = {{0x00, 0x04}};
    create_subscriber_by_xml(&my_session, subscriber_id, subscriber_xml, participant_id, false, false);

    /* Create data writer. */
    const char* datareader_xml = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
    ObjectId datareader_id = {{0x00, 0x06}};
    create_datawriter_by_xml(&my_session, datareader_id, datareader_xml, subscriber_id, false, false);

    /* Main loop */
    while(true)
    {
        BestEffortStream* best_effort = &my_session.output_best_effort_stream;

        HelloWorld topic1 = {18, "Hello MicroRTPS 1"};
        MicroBuffer* topic_buffer_1 = prepare_best_effort_stream_for_topic(best_effort, size_of_HelloWorld_topic(&topic1));
        if(topic_buffer_1 != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer_1, &topic1);
        }

        HelloWorld topic2 = {18, "Hello MicroRTPS 2"};
        MicroBuffer* topic_buffer_2 = prepare_best_effort_stream_for_topic(best_effort, size_of_HelloWorld_topic(&topic2));
        if(topic_buffer_2 != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer_2, &topic2);
        }


        ReliableStream* reliable = &my_session.output_reliable_stream;

        HelloWorld topic3 = {18, "Hello MicroRTPS 3"};
        MicroBuffer* topic_buffer_3 = prepare_reliable_stream_for_topic(reliable, size_of_HelloWorld_topic(&topic3));
        if(topic_buffer_3 != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer_3, &topic3);
        }

        HelloWorld topic4 = {18, "Hello MicroRTPS 4"};
        MicroBuffer* topic_buffer_4 = prepare_reliable_stream_for_topic(reliable, size_of_HelloWorld_topic(&topic4));
        if(topic_buffer_4 != NULL)
        {
            serialize_HelloWorld_topic(topic_buffer_4, &topic4);
        }

        read_data(&my_session, datareader_id);

        while(run_communication(&my_session))
        {
            switch(my_session.last_received_topic_id)
            {
                case HELLO_WORLD_TOPIC:
                {
                    HelloWorld topic1;
                    deserialize_HelloWorld_topic(my_session.last_received_topic, &topic1);
                    printf("Received topic: %s", topic1.m_message);
                    break;
                }

                default:
                    break;
            }
        }

        sleep(1000);
    }

    return 0;
}

