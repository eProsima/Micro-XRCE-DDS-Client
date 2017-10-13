#include <gtest/gtest.h>
#include <string>

#include <micrortps/client/xrce_protocol_serialization.h>

#define BUFFER_LENGTH 1024

class XRCEProtocolSerialization : public testing::Test
{
    protected:
        XRCEProtocolSerialization()
        {
            init_aux_memory(&aux_memory, BUFFER_LENGTH);

            init_external_buffer(&writer, buffer, BUFFER_LENGTH);
            init_external_buffer(&reader, buffer, BUFFER_LENGTH);

            memset(input, '\0', BUFFER_LENGTH);
            memset(output, '\0', BUFFER_LENGTH);
        }

        ~XRCEProtocolSerialization()
        {
            EXPECT_STREQ((char*)input, (char*)output);
            free_aux_memory(&aux_memory);
        }

        AuxMemory aux_memory;

        MicroBuffer writer;
        MicroBuffer reader;

        uint8_t input[BUFFER_LENGTH];
        uint8_t output[BUFFER_LENGTH];
        uint8_t buffer[BUFFER_LENGTH];
};

// ====================================================================================
//                                   GTEST FUNCTIONS
// ====================================================================================

TEST_F(XRCEProtocolSerialization, Time_t)
{
    memset(output, 0xFF, sizeof(Time_t));
    serialize_Time_t(&writer, (Time_t*)input);
    deserialize_Time_t(&reader, (Time_t*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, String_t)
{
    memset(output, 0xFF, sizeof(String_t));
    serialize_String_t(&writer, (String_t*)input);
    deserialize_String_t(&reader, (String_t*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, BinarySequence_t)
{
    memset(output, 0xFF, sizeof(BinarySequence_t));
    serialize_BinarySequence_t(&writer, (BinarySequence_t*)input);
    deserialize_BinarySequence_t(&reader, (BinarySequence_t*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, StringSequence_t)
{
    memset(output, 0xFF, sizeof(StringSequence_t));
    serialize_StringSequence_t(&writer, (StringSequence_t*)input);
    deserialize_StringSequence_t(&reader, (StringSequence_t*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_CLIENT_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_CLIENT_Representation));
    serialize_OBJK_CLIENT_Representation(&writer, (OBJK_CLIENT_Representation*)input);
    deserialize_OBJK_CLIENT_Representation(&reader, (OBJK_CLIENT_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_Representation3_Base)
{
    memset(output, 0xFF, sizeof(OBJK_Representation3_Base));
    serialize_OBJK_Representation3_Base(&writer, (OBJK_Representation3_Base*)input);
    deserialize_OBJK_Representation3_Base(&reader, (OBJK_Representation3_Base*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_Representation2_Base)
{
    memset(output, 0xFF, sizeof(OBJK_Representation2_Base));
    serialize_OBJK_Representation2_Base(&writer, (OBJK_Representation2_Base*)input);
    deserialize_OBJK_Representation2_Base(&reader, (OBJK_Representation2_Base*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_QOSPROFILE_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_QOSPROFILE_Representation));
    serialize_OBJK_QOSPROFILE_Representation(&writer, (OBJK_QOSPROFILE_Representation*)input);
    deserialize_OBJK_QOSPROFILE_Representation(&reader, (OBJK_QOSPROFILE_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_APPLICATION_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_APPLICATION_Representation));
    serialize_OBJK_APPLICATION_Representation(&writer, (OBJK_APPLICATION_Representation*)input);
    deserialize_OBJK_APPLICATION_Representation(&reader, (OBJK_APPLICATION_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_PARTICIPANT_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_PARTICIPANT_Representation));
    serialize_OBJK_PARTICIPANT_Representation(&writer, (OBJK_PARTICIPANT_Representation*)input);
    deserialize_OBJK_PARTICIPANT_Representation(&reader, (OBJK_PARTICIPANT_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_TYPE_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_TYPE_Representation));
    serialize_OBJK_TYPE_Representation(&writer, (OBJK_TYPE_Representation*)input);
    deserialize_OBJK_TYPE_Representation(&reader, (OBJK_TYPE_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_TOPIC_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_TOPIC_Representation));
    serialize_OBJK_TOPIC_Representation(&writer, (OBJK_TOPIC_Representation*)input);
    deserialize_OBJK_TOPIC_Representation(&reader, (OBJK_TOPIC_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_PUBLISHER_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_PUBLISHER_Representation));
    serialize_OBJK_PUBLISHER_Representation(&writer, (OBJK_PUBLISHER_Representation*)input);
    deserialize_OBJK_PUBLISHER_Representation(&reader, (OBJK_PUBLISHER_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_SUBSCRIBER_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_SUBSCRIBER_Representation));
    serialize_OBJK_SUBSCRIBER_Representation(&writer, (OBJK_SUBSCRIBER_Representation*)input);
    deserialize_OBJK_SUBSCRIBER_Representation(&reader, (OBJK_SUBSCRIBER_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_DATA_WRITER_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_DATA_WRITER_Representation));
    serialize_OBJK_DATA_WRITER_Representation(&writer, (OBJK_DATA_WRITER_Representation*)input);
    deserialize_OBJK_DATA_WRITER_Representation(&reader, (OBJK_DATA_WRITER_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_DATA_READER_Representation)
{
    memset(output, 0xFF, sizeof(OBJK_DATA_READER_Representation));
    serialize_OBJK_DATA_READER_Representation(&writer, (OBJK_DATA_READER_Representation*)input);
    deserialize_OBJK_DATA_READER_Representation(&reader, (OBJK_DATA_READER_Representation*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_TYPE_Binary)
{
    memset(output, 0xFF, sizeof(OBJK_TYPE_Binary));
    serialize_OBJK_TYPE_Binary(&writer, (OBJK_TYPE_Binary*)input);
    deserialize_OBJK_TYPE_Binary(&reader, (OBJK_TYPE_Binary*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_TOPIC_Binary)
{
    memset(output, 0xFF, sizeof(OBJK_TOPIC_Binary));
    serialize_OBJK_TOPIC_Binary(&writer, (OBJK_TOPIC_Binary*)input);
    deserialize_OBJK_TOPIC_Binary(&reader, (OBJK_TOPIC_Binary*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_PUBLISHER_Binary)
{
    memset(output, 0xFF, sizeof(OBJK_PUBLISHER_Binary));
    serialize_OBJK_PUBLISHER_Binary(&writer, (OBJK_PUBLISHER_Binary*)input);
    deserialize_OBJK_PUBLISHER_Binary(&reader, (OBJK_PUBLISHER_Binary*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_SUBSCRIBER_Binary)
{
    memset(output, 0xFF, sizeof(OBJK_SUBSCRIBER_Binary));
    serialize_OBJK_SUBSCRIBER_Binary(&writer, (OBJK_SUBSCRIBER_Binary*)input);
    deserialize_OBJK_SUBSCRIBER_Binary(&reader, (OBJK_SUBSCRIBER_Binary*)output, &aux_memory);
}


TEST_F(XRCEProtocolSerialization, OBJK_Endpoint_QosBinary)
{
    memset(output, 0xFF, sizeof(OBJK_Endpoint_QosBinary));
    serialize_OBJK_Endpoint_QosBinary(&writer, (OBJK_Endpoint_QosBinary*)input);
    deserialize_OBJK_Endpoint_QosBinary(&reader, (OBJK_Endpoint_QosBinary*)output, &aux_memory);
}



