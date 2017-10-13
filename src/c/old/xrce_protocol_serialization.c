#include "xrce_protocol_serialization.h"

void serialize_Time_t(MicroBuffer* buffer, Time_t* input)
{
    serialize_int32(buffer, input->seconds);
    serialize_uint32(buffer, input->nanoseconds);
}

void deserialize_Time_t(MicroBuffer* buffer, Time_t* output, AuxMemory* aux)
{
    deserialize_int32(buffer, &output->seconds);
    deserialize_uint32(buffer, &output ->nanoseconds);
}

void serialize_String_t(MicroBuffer* buffer, String_t* input)
{
    serialize_sequence_char(buffer, input->data, input->size);
}

void deserialize_String_t(MicroBuffer* buffer, String_t* output, AuxMemory* aux)
{
    deserialize_int32(buffer, &output->size);
    output->data = request_aux_memory(aux, output->size);
    deserialize_array_char(buffer, output->data, output->size);
}

void serialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* input)
{
    serialize_sequence_uint8_t(buffer, input->data, input->size);
}

void deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output, AuxMemory* aux)
{
    deserialize_int32(buffer, &output->size);
    output->data = request_aux_memory(aux, output->size);
    deserialize_array_uint8_t(buffer, output->data, output->size);
}

void serialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* input)
{
    serialize_array_uint8_t(buffer, input->xrce_cookie, sizeof(input->xrce_cookie));
    serialize_array_uint8_t(buffer, input->xrce_version, sizeof(input->xrce_version));
    serialize_array_uint8_t(buffer, input->xrce_vendor_id, sizeof(input->xrce_vendor_id));

    serialize_Time_t(buffer, &input->client_timestamp);
    serialize_array_uint8_t(buffer, input->client_key, sizeof(input->client_key));
    serialize_uint8_t(buffer, input->session_id);
}

void deserialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* output, AuxMemory* aux)
{
    deserialize_array_uint8_t(buffer, output->xrce_cookie, sizeof(output->xrce_cookie));
    deserialize_array_uint8_t(buffer, output->xrce_version, sizeof(output->xrce_version));
    deserialize_array_uint8_t(buffer, output->xrce_vendor_id, sizeof(output->xrce_vendor_id));

    deserialize_Time_t(buffer, &output->client_timestamp, aux);
    deserialize_array_uint8_t(buffer, output->client_key, sizeof(output->client_key));
    deserialize_uint8_t(buffer, &output->session_id);
}

void serialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* input)
{
    serialize_uint8_t(buffer, input->format);

    switch(input->format)
    {
        case REPRESENTATION_BY_REFERENCE:
            serialize_String_t(buffer, &input->object_reference);
        break;

        case REPRESENTATION_AS_XML_STRING:
            serialize_String_t(buffer, &input->xml_string_representation);
        break;

        case REPRESENTATION_IN_BINARY:
            serialize_BinarySequence_t(buffer, &input->binary_representation);
        break;
    }
}

void deserialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->format);

    switch(output->format)
    {
        case REPRESENTATION_BY_REFERENCE:
            deserialize_String_t(buffer, &output->object_reference, aux);
        break;

        case REPRESENTATION_AS_XML_STRING:
            deserialize_String_t(buffer, &output->xml_string_representation, aux);
        break;

        case REPRESENTATION_IN_BINARY:
            deserialize_BinarySequence_t(buffer, &output->binary_representation, aux);
        break;
    }
}

void serialize_OBJK_Representation2Formats(MicroBuffer* buffer, OBJK_Representation2Formats* input)
{
    serialize_uint8_t(buffer, input->format);

    switch(input->format)
    {
        case REPRESENTATION_BY_REFERENCE:
            serialize_String_t(buffer, &input->object_name);
        break;

        case REPRESENTATION_AS_XML_STRING:
            serialize_String_t(buffer, &input->string_representation);
        break;
    }
}

void deserialize_OBJK_Representation2Formats(MicroBuffer* buffer, OBJK_Representation2Formats* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->format);

    switch(output->format)
    {
        case REPRESENTATION_BY_REFERENCE:
            deserialize_String_t(buffer, &output->object_name, aux);
        break;

        case REPRESENTATION_AS_XML_STRING:
            deserialize_String_t(buffer, &output->string_representation, aux);
        break;
    }
}