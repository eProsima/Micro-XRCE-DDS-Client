#include <gtest/gtest.h>
#include <string>

#include <micrortps/client/xrce_protocol_serialization.h>

#define BUFFER_LENGTH 1024
#define DATA_LENGTH 256

// ====================================================================================
//                                   GTEST FUNCTIONS
// ====================================================================================

class XRCEProtocolSerialization : public testing::Test
{
    protected:
        XRCEProtocolSerialization()
        {
            init_aux_memory(&aux_memory, DATA_LENGTH);

            init_external_buffer(&writer, buffer, BUFFER_LENGTH);
            init_external_buffer(&reader, buffer, BUFFER_LENGTH);
        }

        ~XRCEProtocolSerialization()
        {
            free_aux_memory(&aux_memory);
        }

        AuxMemory aux_memory;

        MicroBuffer writer;
        MicroBuffer reader;

        uint8_t buffer[BUFFER_LENGTH];
};


TEST_F(XRCEProtocolSerialization, Time_t)
{
    uint8_t* input = {};
    Time_t* output = {};

    init(&input);

    serialize_Time_t(&writer, &input);
    deserialize_Time_t(&reader, &output, &aux_memory);

    cmp(&input, &output);
}