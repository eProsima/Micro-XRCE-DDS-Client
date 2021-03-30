#include <gtest/gtest.h>
#include <uxr/client/client.h>

#include <functional>

#define BUFFER_SIZE     1024
#define HISTORY_SIZE    4

inline bool operator==(const uxrObjectId& lhs, const uxrObjectId& rhs)
{  
    return lhs.id == rhs.id && lhs.type == rhs.type;
}


class SharedMemoryTest : public testing::Test
{
public:
    SharedMemoryTest()
    {
        comm.send_msg =send_msg_mock;
        comm.recv_msg =recv_msg_mock;

        uxr_init_session(&session, &comm, 0xAAAABBBB);
        uxr_set_topic_callback(&session, on_topic_dispatcher, this);

        output_besteffort = uxr_create_output_best_effort_stream(&session, output_besteffort_buffer, BUFFER_SIZE);
        input_besteffort = uxr_create_input_best_effort_stream(&session);

        output_reliable = uxr_create_output_reliable_stream(&session, output_reliable_buffer, sizeof(output_reliable_buffer), HISTORY_SIZE);
        input_reliable = uxr_create_input_reliable_stream(&session, input_reliable_buffer, sizeof(input_reliable_buffer), HISTORY_SIZE);
    }

    void TearDown() override
    {
        ASSERT_NO_FATAL_FAILURE(uxr_delete_session(&session));
    }

protected:
    uxrObjectId create_datawriter(std::string topic, std::string type){
        uxrObjectId publisher_id = uxr_object_id(0x01, UXR_PUBLISHER_ID);
        uxrObjectId datawriter_id = uxr_object_id(0x01, UXR_DATAWRITER_ID);

        std::ostringstream stringStream;
        stringStream << "<dds>"
                        "<data_writer>"
                        "<topic>"
                        "<kind>NO_KEY</kind>"
                        "<name>" << topic << "</name>"
                        "<dataType>" << type << "</dataType>"
                        "</topic>"
                        "</data_writer>"
                        "</dds>";
        uxr_buffer_create_datawriter_xml(&session, output_besteffort, datawriter_id, publisher_id, stringStream.str().c_str(), UXR_REPLACE);
        return datawriter_id;
    }

    uxrObjectId create_datareaded(std::string topic, std::string type){

        uxrObjectId subscriber_id = uxr_object_id(0x01, UXR_SUBSCRIBER_ID);
        uxrObjectId datareader_id = uxr_object_id(0x01, UXR_DATAREADER_ID);

        std::ostringstream stringStream;
        stringStream << "<dds>"
                        "<data_reader>"
                        "<topic>"
                        "<kind>NO_KEY</kind>"
                        "<name>" << topic << "</name>"
                        "<dataType>" << type << "</dataType>"
                        "</topic>"
                        "</data_reader>"
                        "</dds>";

        uxr_buffer_create_datareader_xml(&session, output_besteffort, datareader_id, subscriber_id, stringStream.str().c_str(), UXR_REPLACE);   

        return datareader_id;
    }

    static void on_topic_dispatcher(uxrSession* session_, uxrObjectId object_id, uint16_t request_id, uxrStreamId stream_id, struct ucdrBuffer* serialization, uint16_t length, void* args)
    {
        static_cast<SharedMemoryTest*>(args)->on_topic(session_, object_id, request_id, stream_id, serialization, length);
    }

    static bool send_msg_mock(void* /*instance*/, const uint8_t* /*buf*/, size_t /*len*/){
        return true;
    }

    static bool recv_msg_mock(void* /*instance*/, uint8_t** /*buf*/, size_t* /*len*/, int /*timeout*/){
        return false;
    }

protected:
    uxrSession session;
    uxrCommunication comm;
    uint8_t output_besteffort_buffer[BUFFER_SIZE];
    uint8_t output_reliable_buffer[BUFFER_SIZE * HISTORY_SIZE];
    uint8_t input_reliable_buffer[BUFFER_SIZE * HISTORY_SIZE];

    uxrStreamId output_besteffort;
    uxrStreamId input_besteffort;
    uxrStreamId output_reliable;
    uxrStreamId input_reliable;

    std::function<void(uxrSession*, uxrObjectId, uint16_t, uxrStreamId, struct ucdrBuffer*, uint16_t)> on_topic;
};

TEST_F(SharedMemoryTest, SharedMemoryPubSub)
{
    uxrObjectId datawriter_id = create_datawriter("shared_memory_topic", "shared_memory_type");
    uxrObjectId datareader_id = create_datareaded("shared_memory_topic", "shared_memory_type");

    const size_t data_lenght = sizeof(uint32_t);
    const uint32_t data = 42;

    int received_topics = 0;

    on_topic = [&](
        uxrSession* /*session*/, 
        uxrObjectId object_id, 
        uint16_t /*request_id*/, 
        uxrStreamId stream_id, 
        struct ucdrBuffer* serialization, 
        uint16_t length) -> void 
    {
        ASSERT_EQ(stream_id.type, UXR_INTERPROCESS_STREAM);
        ASSERT_EQ(length, data_lenght);
        ASSERT_EQ(object_id, datareader_id);

        uint32_t out;
        ucdr_deserialize_uint32_t(serialization, &out);
        ASSERT_EQ(out, data);

        received_topics++;
    };

    ucdrBuffer ub;

    // Test serializing and running session: no limit
    for (size_t i = 0; i < UXR_CONFIG_INTERPROCESS_STATIC_MEM_SIZE * 2; i++)
    {
        uxr_prepare_output_stream(&session, output_besteffort, datawriter_id, &ub, data_lenght);
        ucdr_serialize_uint32_t(&ub, data);

        uxr_run_session_time(&session, 1000);
    }
    ASSERT_EQ(received_topics, UXR_CONFIG_INTERPROCESS_STATIC_MEM_SIZE * 2);

    received_topics = 0;
    // Test serializing and then running session: mempool limit
    for (size_t i = 0; i < UXR_CONFIG_INTERPROCESS_STATIC_MEM_SIZE; i++)
    {
        uxr_prepare_output_stream(&session, output_besteffort, datawriter_id, &ub, data_lenght);
        ucdr_serialize_uint32_t(&ub, data);
    }
    uxr_run_session_time(&session, 1000);

    ASSERT_EQ(received_topics, UXR_CONFIG_INTERPROCESS_STATIC_MEM_SIZE);
}

TEST_F(SharedMemoryTest, SharedMemory1Pub3Sub)
{
    uxrObjectId datawriter_id = create_datawriter("shared_memory_topic", "shared_memory_type");
    create_datareaded("shared_memory_topic", "shared_memory_type");
    create_datareaded("shared_memory_topic", "shared_memory_type");
    create_datareaded("shared_memory_topic", "shared_memory_type");

    const size_t data_lenght = sizeof(uint32_t);
    const uint32_t data = 42;

    int received_topics = 0;

    on_topic = [&](
        uxrSession* /*session*/, 
        uxrObjectId /*object_id*/, 
        uint16_t /*request_id*/, 
        uxrStreamId /*stream_id*/, 
        struct ucdrBuffer* /*serialization*/, 
        uint16_t /*length*/) -> void 
    {
        received_topics++;
    };

    ucdrBuffer ub;
    uxr_prepare_output_stream(&session, output_besteffort, datawriter_id, &ub, data_lenght);
    ucdr_serialize_uint32_t(&ub, data);
    uxr_run_session_time(&session, 1000);

    ASSERT_EQ(received_topics, 3);
}

TEST_F(SharedMemoryTest, SharedMemory3Pub1Sub)
{
    uxrObjectId datawriter_id_1 = create_datawriter("shared_memory_topic", "shared_memory_type");
    uxrObjectId datawriter_id_2 = create_datawriter("shared_memory_topic", "shared_memory_type");
    uxrObjectId datawriter_id_3 = create_datawriter("shared_memory_topic", "shared_memory_type");
    create_datareaded("shared_memory_topic", "shared_memory_type");

    const size_t data_lenght = sizeof(uint32_t);
    const uint32_t data = 42;

    int received_topics = 0;

    on_topic = [&](
        uxrSession* /*session*/, 
        uxrObjectId /*object_id*/, 
        uint16_t /*request_id*/, 
        uxrStreamId /*stream_id*/, 
        struct ucdrBuffer* /*serialization*/, 
        uint16_t /*length*/) -> void 
    {
        received_topics++;
    };

    ucdrBuffer ub;
    uxr_prepare_output_stream(&session, output_besteffort, datawriter_id_1, &ub, data_lenght);
    ucdr_serialize_uint32_t(&ub, data);

    uxr_prepare_output_stream(&session, output_besteffort, datawriter_id_2, &ub, data_lenght);
    ucdr_serialize_uint32_t(&ub, data);

    uxr_prepare_output_stream(&session, output_besteffort, datawriter_id_3, &ub, data_lenght);
    ucdr_serialize_uint32_t(&ub, data);
    
    uxr_run_session_time(&session, 1000);

    ASSERT_EQ(received_topics, 3);
}

TEST_F(SharedMemoryTest, SharedMemoryNoMatch)
{
    uxrObjectId datawriter_id = create_datawriter("shared_memory_topic", "shared_memory_type");
    create_datareaded("shared_memory_topic_123", "shared_memory_type_123");

    const size_t data_lenght = sizeof(uint32_t);
    const uint32_t data = 42;

    int received_topics = 0;

    on_topic = [&](
        uxrSession* /*session*/, 
        uxrObjectId /*object_id*/, 
        uint16_t /*request_id*/, 
        uxrStreamId /*stream_id*/, 
        struct ucdrBuffer* /*serialization*/, 
        uint16_t /*length*/) -> void 
    {
        received_topics++;
    };

    ucdrBuffer ub;
    uxr_prepare_output_stream(&session, output_besteffort, datawriter_id, &ub, data_lenght);
    ucdr_serialize_uint32_t(&ub, data);
    
    uxr_run_session_time(&session, 1000);

    ASSERT_EQ(received_topics, 0);
}

TEST_F(SharedMemoryTest, SharedMemoryFragmentation)
{
    uxrObjectId datawriter_id = create_datawriter("shared_memory_topic", "shared_memory_type");
    create_datareaded("shared_memory_topic", "shared_memory_type");

    const size_t data_lenght = 2*BUFFER_SIZE;
    char * data = static_cast<char *>(malloc(data_lenght));
    memset(data, 'a', data_lenght);
    
    int received_topics = 0;

    on_topic = [&](
        uxrSession* /*session*/, 
        uxrObjectId /*object_id*/, 
        uint16_t /*request_id*/, 
        uxrStreamId /*stream_id*/, 
        struct ucdrBuffer* serialization, 
        uint16_t length) -> void 
    {
        ASSERT_EQ(length, data_lenght);

        char * data_out = static_cast<char *>(malloc(data_lenght));
        memset(data_out, 'z', data_lenght);
        ucdr_deserialize_array_char(serialization, data_out, data_lenght);

        bool array_check = true;
        for (size_t i = 0; i < data_lenght; i++)
        {
            if (data_out[i] != 'a')
            {
                array_check = false;
                break;
            }   
        }
        
        ASSERT_TRUE(array_check);

        received_topics++;
    };

    ucdrBuffer ub;
    uxr_prepare_output_stream(&session, output_reliable, datawriter_id, &ub, data_lenght);
    ucdr_serialize_array_char(&ub, data, data_lenght);
    
    uxr_run_session_time(&session, 1000);

    ASSERT_EQ(received_topics, 1);

    free(data);
}
