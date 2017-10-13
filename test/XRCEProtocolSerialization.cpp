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



