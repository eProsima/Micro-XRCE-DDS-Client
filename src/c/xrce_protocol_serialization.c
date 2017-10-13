#include "xrce_protocol_serialization.h"

void serialize_Time_t(MicroBuffer* buffer, const Time_t* input)
{
    serialize_int32_t(buffer, input->seconds);
    serialize_uint32_t(buffer, input->nanoseconds);
}

void deserialize_Time_t(MicroBuffer* buffer, Time_t* output, AuxMemory* aux)
{
    deserialize_int32_t(buffer, &output->seconds);
    deserialize_uint32_t(buffer, &output->nanoseconds);
}

void serialize_String_t(MicroBuffer* buffer, const String_t* input)
{
    serialize_sequence_char(buffer, input->data, input->size);
}

void deserialize_String_t(MicroBuffer* buffer, String_t* output, AuxMemory* aux)
{
    deserialize_sequence_char(buffer, output->data, &output->size);
}

void serialize_BinarySequence_t(MicroBuffer* buffer, const BinarySequence_t* input)
{
    serialize_sequence_uint8_t(buffer, input->data, input->size);
}

void deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output, AuxMemory* aux)
{
    deserialize_sequence_uint8_t(buffer, output->data, &output->size);
}

void serialize_StringSequence_t(MicroBuffer* buffer, const StringSequence_t* input)
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

void serialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, const OBJK_CLIENT_Representation* input)
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

void serialize_OBJK_Representation3Formats(MicroBuffer* buffer, const OBJK_Representation3Formats* input)
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

void serialize_OBJK_Representation2Formats(MicroBuffer* buffer, const OBJK_Representation2Formats* input)
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
    }
}

void deserialize_OBJK_Representation2Formats(MicroBuffer* buffer, OBJK_Representation2Formats* output, AuxMemory* aux)
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
    }
}

void serialize_OBJK_Representation3_Base(MicroBuffer* buffer, const OBJK_Representation3_Base* input)
{
    serialize_OBJK_Representation3Formats(buffer, &input->representation);
}

void deserialize_OBJK_Representation3_Base(MicroBuffer* buffer, OBJK_Representation3_Base* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3Formats(buffer, &output->representation, aux);
}

void serialize_OBJK_Representation2_Base(MicroBuffer* buffer, const OBJK_Representation2_Base* input)
{
    serialize_OBJK_Representation2Formats(buffer, &input->representation);
}

void deserialize_OBJK_Representation2_Base(MicroBuffer* buffer, OBJK_Representation2_Base* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation2Formats(buffer, &output->representation, aux);
}

void serialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, const OBJK_QOSPROFILE_Representation* input)
{
    serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

void deserialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, OBJK_QOSPROFILE_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

void serialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, const OBJK_APPLICATION_Representation* input)
{
    serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

void deserialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, OBJK_APPLICATION_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

void serialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, const OBJK_PARTICIPANT_Representation* input)
{
    serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

void deserialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, OBJK_PARTICIPANT_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

void serialize_OBJK_TYPE_Representation(MicroBuffer* buffer, const OBJK_TYPE_Representation* input)
{
    serialize_OBJK_Representation3_Base(buffer, &input->base3);
    serialize_array_uint8_t(buffer, input->participant_id, 2);
    serialize_String_t(buffer, &input->registered_type_name);
}

void deserialize_OBJK_TYPE_Representation(MicroBuffer* buffer, OBJK_TYPE_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux);
    deserialize_array_uint8_t(buffer, output->participant_id, 2);
    deserialize_String_t(buffer, &output->registered_type_name, aux);
}

void serialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, const OBJK_TOPIC_Representation* input)
{
    serialize_OBJK_Representation3_Base(buffer, &input->base3);
    serialize_array_uint8_t(buffer, input->participant_id, 2);
}

void deserialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, OBJK_TOPIC_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux);
    deserialize_array_uint8_t(buffer, output->participant_id, 2);
}

void serialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, const OBJK_PUBLISHER_Representation* input)
{
    serialize_OBJK_Representation3_Base(buffer, &input->base3);
    serialize_array_uint8_t(buffer, input->participant_id, 2);
}

void deserialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, OBJK_PUBLISHER_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux);
    deserialize_array_uint8_t(buffer, output->participant_id, 2);
}

void serialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Representation* input)
{
    serialize_OBJK_Representation3_Base(buffer, &input->base3);
    serialize_array_uint8_t(buffer, input->participant_id, 2);
}

void deserialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, OBJK_SUBSCRIBER_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux);
    deserialize_array_uint8_t(buffer, output->participant_id, 2);
}

void serialize_OBJK_DATA_WRITER_Representation(MicroBuffer* buffer, const OBJK_DATA_WRITER_Representation* input)
{
    serialize_OBJK_Representation3_Base(buffer, &input->base3);
    serialize_array_uint8_t(buffer, input->participant_id, 2);
    serialize_array_uint8_t(buffer, input->publisher_id, 2);
}

void deserialize_OBJK_DATA_WRITER_Representation(MicroBuffer* buffer, OBJK_DATA_WRITER_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux);
    deserialize_array_uint8_t(buffer, output->participant_id, 2);
    deserialize_array_uint8_t(buffer, output->publisher_id, 2);
}

void serialize_OBJK_DATA_READER_Representation(MicroBuffer* buffer, const OBJK_DATA_READER_Representation* input)
{
    serialize_OBJK_Representation3_Base(buffer, &input->base3);
    serialize_array_uint8_t(buffer, input->participant_id, 2);
    serialize_array_uint8_t(buffer, input->subscriber_id, 2);
}

void deserialize_OBJK_DATA_READER_Representation(MicroBuffer* buffer, OBJK_DATA_READER_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux);
    deserialize_array_uint8_t(buffer, output->participant_id, 2);
    deserialize_array_uint8_t(buffer, output->subscriber_id, 2);
}

void serialize_OBJK_TYPE_Binary(MicroBuffer* buffer, const OBJK_TYPE_Binary* input)
{
    serialize_String_t(buffer, &input->type_name);
}

void deserialize_OBJK_TYPE_Binary(MicroBuffer* buffer, OBJK_TYPE_Binary* output, AuxMemory* aux)
{
    deserialize_String_t(buffer, &output->type_name, aux);
}

void serialize_OBJK_TOPIC_Binary(MicroBuffer* buffer, const OBJK_TOPIC_Binary* input)
{
    serialize_String_t(buffer, &input->topic_name);
    serialize_String_t(buffer, &input->type_name);
}

void deserialize_OBJK_TOPIC_Binary(MicroBuffer* buffer, OBJK_TOPIC_Binary* output, AuxMemory* aux)
{
    deserialize_String_t(buffer, &output->topic_name, aux);
    deserialize_String_t(buffer, &output->type_name, aux);
}

void serialize_OBJK_PUBLISHER_Binary(MicroBuffer* buffer, const OBJK_PUBLISHER_Binary* input)
{
    serialize_StringSequence_t(buffer, &input->partitions);
    serialize_BinarySequence_t(buffer, &input->group_data);
}

void deserialize_OBJK_PUBLISHER_Binary(MicroBuffer* buffer, OBJK_PUBLISHER_Binary* output, AuxMemory* aux)
{
    deserialize_StringSequence_t(buffer, &output->partitions, aux);
    deserialize_BinarySequence_t(buffer, &output->group_data, aux);
}

void serialize_OBJK_SUBSCRIBER_Binary(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Binary* input)
{
    serialize_StringSequence_t(buffer, &input->partitions);
    serialize_BinarySequence_t(buffer, &input->group_data);
}

void deserialize_OBJK_SUBSCRIBER_Binary(MicroBuffer* buffer, OBJK_SUBSCRIBER_Binary* output, AuxMemory* aux)
{
    deserialize_StringSequence_t(buffer, &output->partitions, aux);
    deserialize_BinarySequence_t(buffer, &output->group_data, aux);
}

void serialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, const OBJK_Endpoint_QosBinary* input)
{
    serialize_uint16_t(buffer, input->qos_flags);
    serialize_uint16_t(buffer, input->history_depth);
    serialize_uint32_t(buffer, input->deadline_msec);
    serialize_uint32_t(buffer, input->lifespan_msec);
    serialize_BinarySequence_t(buffer, &input->user_data);
}

void deserialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, OBJK_Endpoint_QosBinary* output, AuxMemory* aux)
{
    deserialize_uint16_t(buffer, &output->qos_flags);
    deserialize_uint16_t(buffer, &output->history_depth);
    deserialize_uint32_t(buffer, &output->deadline_msec);
    deserialize_uint32_t(buffer, &output->lifespan_msec);
    deserialize_BinarySequence_t(buffer, &output->user_data, aux);
}


