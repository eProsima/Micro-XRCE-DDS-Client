#include "xrce_protocol_serialization.h"

void serialize_Time_t(MicroBuffer* buffer, Time_t* input)
{
    serialize_int32_t(buffer, input->seconds);
    serialize_uint32_t(buffer, input->nanoseconds);
}

void deserialize_Time_t(MicroBuffer* buffer, Time_t* output, AuxMemory* aux)
{
    deserialize_int32_t(buffer, &output->seconds);
    deserialize_uint32_t(buffer, &output->nanoseconds);
}

void serialize_String_t(MicroBuffer* buffer, String_t* input)
{
    serialize_sequence_char(buffer, input->data, input->size);
}

void deserialize_String_t(MicroBuffer* buffer, String_t* output, AuxMemory* aux)
{
    deserialize_sequence_char(buffer, output->data, &output->size);
}

void serialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* input)
{
    serialize_sequence_uint8_t(buffer, input->data, input->size);
}

void deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output, AuxMemory* aux)
{
    deserialize_sequence_uint8_t(buffer, output->data, &output->size);
}

void serialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* input)
{
    serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size; i++)
        serialize_String_t(buffer, &input->data[i]);
}

void deserialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* output, AuxMemory* aux)
{
    deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size; i++)
        deserialize_String_t(buffer, &output->data[i], aux);
}

void serialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* input)
{
    serialize_array_uint8_t(buffer, input->xrce_cookie, 4);
    serialize_array_uint8_t(buffer, input->xrce_version, 2);
    serialize_array_uint8_t(buffer, input->xrce_vendor_id, 2);
    serialize_Time_t(buffer, &input->client_timestamp);
    serialize_array_uint8_t(buffer, input->client_key, 4);
    serialize_uint8_t(buffer, input->session_id);
}

void deserialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* output, AuxMemory* aux)
{
    deserialize_array_uint8_t(buffer, output->xrce_cookie, 4);
    deserialize_array_uint8_t(buffer, output->xrce_version, 2);
    deserialize_array_uint8_t(buffer, output->xrce_vendor_id, 2);
    deserialize_Time_t(buffer, &output->client_timestamp, aux);
    deserialize_array_uint8_t(buffer, output->client_key, 4);
    deserialize_uint8_t(buffer, &output->session_id);
}

void serialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* input)
{
    serialize_uint8_t(buffer, input->format);
    switch(input->format)
    {
        case REPRESENTATION_BY_REFERENCE:
            serialize_String_t(buffer, &input->_.object_name);
        break;
        case REPRESENTATION_AS_XML_STRING:
            serialize_String_t(buffer, &input->_.string_representation);
        break;
        case REPRESENTATION_IN_BINARY:
            serialize_BinarySequence_t(buffer, &input->_.binary_representation);
        break;
    }
}

void deserialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->format);
    switch(output->format)
    {
        case REPRESENTATION_BY_REFERENCE:
            deserialize_String_t(buffer, &output->_.object_name, aux);
        break;
        case REPRESENTATION_AS_XML_STRING:
            deserialize_String_t(buffer, &output->_.string_representation, aux);
        break;
        case REPRESENTATION_IN_BINARY:
            deserialize_BinarySequence_t(buffer, &output->_.binary_representation, aux);
        break;
    }
}


