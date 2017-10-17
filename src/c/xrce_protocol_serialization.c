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

uint32_t size_of_Time_t(const Time_t* data)
{
    return sizeof(data->seconds)
         + sizeof(data->nanoseconds);
}

void serialize_String_t(MicroBuffer* buffer, const String_t* input)
{
    serialize_sequence_char(buffer, input->data, input->size);
}

void deserialize_String_t(MicroBuffer* buffer, String_t* output, AuxMemory* aux)
{
    deserialize_sequence_char(buffer, output->data, &output->size);
}

uint32_t size_of_String_t(const String_t* data)
{
    return sizeof(data->size) + data->size;
}

void serialize_BinarySequence_t(MicroBuffer* buffer, const BinarySequence_t* input)
{
    serialize_sequence_uint8_t(buffer, input->data, input->size);
}

void deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output, AuxMemory* aux)
{
    deserialize_sequence_uint8_t(buffer, output->data, &output->size);
}

uint32_t size_of_BinarySequence_t(const BinarySequence_t* data)
{
    return sizeof(data->size) + data->size;
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

uint32_t size_of_StringSequence_t(const StringSequence_t* data)
{
    return sizeof(data->size) + data->size;
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

uint32_t size_of_OBJK_CLIENT_Representation(const OBJK_CLIENT_Representation* data)
{
    return sizeof(data->xrce_cookie)
         + sizeof(data->xrce_version)
         + sizeof(data->xrce_vendor_id)
         + size_of_Time_t(&data->client_timestamp)
         + sizeof(data->client_key)
         + sizeof(data->session_id);
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

uint32_t size_of_OBJK_Representation3Formats(const OBJK_Representation3Formats* data)
{
    uint32_t size = sizeof(data->format);
    switch(data->format)
    {
        case REPRESENTATION_BY_REFERENCE:
            size += size_of_String_t(&data->_.object_name);
        break;
        case REPRESENTATION_AS_XML_STRING:
            size += size_of_String_t(&data->_.string_representation);
        break;
        case REPRESENTATION_IN_BINARY:
            size += size_of_BinarySequence_t(&data->_.binary_representation);
        break;
    }

    return size;
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

uint32_t size_of_OBJK_Representation2Formats(const OBJK_Representation2Formats* data)
{
    uint32_t size = sizeof(data->format);
    switch(data->format)
    {
        case REPRESENTATION_BY_REFERENCE:
            size += size_of_String_t(&data->_.object_name);
        break;
        case REPRESENTATION_AS_XML_STRING:
            size += size_of_String_t(&data->_.string_representation);
        break;
    }

    return size;
}

void serialize_OBJK_Representation3_Base(MicroBuffer* buffer, const OBJK_Representation3_Base* input)
{
    serialize_OBJK_Representation3Formats(buffer, &input->representation);
}

void deserialize_OBJK_Representation3_Base(MicroBuffer* buffer, OBJK_Representation3_Base* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3Formats(buffer, &output->representation, aux);
}

uint32_t size_of_OBJK_Representation3_Base(const OBJK_Representation3_Base* data)
{
    return size_of_OBJK_Representation3Formats(&data->representation);
}

void serialize_OBJK_Representation2_Base(MicroBuffer* buffer, const OBJK_Representation2_Base* input)
{
    serialize_OBJK_Representation2Formats(buffer, &input->representation);
}

void deserialize_OBJK_Representation2_Base(MicroBuffer* buffer, OBJK_Representation2_Base* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation2Formats(buffer, &output->representation, aux);
}

uint32_t size_of_OBJK_Representation2_Base(const OBJK_Representation2_Base* data)
{
    return size_of_OBJK_Representation2Formats(&data->representation);
}

void serialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, const OBJK_QOSPROFILE_Representation* input)
{
    serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

void deserialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, OBJK_QOSPROFILE_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

uint32_t size_of_OBJK_QOSPROFILE_Representation(const OBJK_QOSPROFILE_Representation* data)
{
    return size_of_OBJK_Representation2_Base(&data->base2);
}

void serialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, const OBJK_APPLICATION_Representation* input)
{
    serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

void deserialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, OBJK_APPLICATION_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

uint32_t size_of_OBJK_APPLICATION_Representation(const OBJK_APPLICATION_Representation* data)
{
    return size_of_OBJK_Representation2_Base(&data->base2);
}

void serialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, const OBJK_PARTICIPANT_Representation* input)
{
    serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

void deserialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, OBJK_PARTICIPANT_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

uint32_t size_of_OBJK_PARTICIPANT_Representation(const OBJK_PARTICIPANT_Representation* data)
{
    return size_of_OBJK_Representation2_Base(&data->base2);
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

uint32_t size_of_OBJK_TYPE_Representation(const OBJK_TYPE_Representation* data)
{
    return size_of_OBJK_Representation3_Base(&data->base3)
         + sizeof(data->participant_id)
         + size_of_String_t(&data->registered_type_name);
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

uint32_t size_of_OBJK_TOPIC_Representation(const OBJK_TOPIC_Representation* data)
{
    return size_of_OBJK_Representation3_Base(&data->base3)
         + sizeof(data->participant_id);
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

uint32_t size_of_OBJK_PUBLISHER_Representation(const OBJK_PUBLISHER_Representation* data)
{
    return size_of_OBJK_Representation3_Base(&data->base3)
         + sizeof(data->participant_id);
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

uint32_t size_of_OBJK_SUBSCRIBER_Representation(const OBJK_SUBSCRIBER_Representation* data)
{
    return size_of_OBJK_Representation3_Base(&data->base3)
         + sizeof(data->participant_id);
}

void serialize_OBJK_DATAWRITER_Representation(MicroBuffer* buffer, const OBJK_DATAWRITER_Representation* input)
{
    serialize_OBJK_Representation3_Base(buffer, &input->base3);
    serialize_array_uint8_t(buffer, input->participant_id, 2);
    serialize_array_uint8_t(buffer, input->publisher_id, 2);
}

void deserialize_OBJK_DATAWRITER_Representation(MicroBuffer* buffer, OBJK_DATAWRITER_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux);
    deserialize_array_uint8_t(buffer, output->participant_id, 2);
    deserialize_array_uint8_t(buffer, output->publisher_id, 2);
}

uint32_t size_of_OBJK_DATAWRITER_Representation(const OBJK_DATAWRITER_Representation* data)
{
    return size_of_OBJK_Representation3_Base(&data->base3)
         + sizeof(data->participant_id)
         + sizeof(data->publisher_id);
}

void serialize_OBJK_DATAREADER_Representation(MicroBuffer* buffer, const OBJK_DATAREADER_Representation* input)
{
    serialize_OBJK_Representation3_Base(buffer, &input->base3);
    serialize_array_uint8_t(buffer, input->participant_id, 2);
    serialize_array_uint8_t(buffer, input->subscriber_id, 2);
}

void deserialize_OBJK_DATAREADER_Representation(MicroBuffer* buffer, OBJK_DATAREADER_Representation* output, AuxMemory* aux)
{
    deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux);
    deserialize_array_uint8_t(buffer, output->participant_id, 2);
    deserialize_array_uint8_t(buffer, output->subscriber_id, 2);
}

uint32_t size_of_OBJK_DATAREADER_Representation(const OBJK_DATAREADER_Representation* data)
{
    return size_of_OBJK_Representation3_Base(&data->base3)
         + sizeof(data->participant_id)
         + sizeof(data->subscriber_id);
}

void serialize_OBJK_TYPE_Binary(MicroBuffer* buffer, const OBJK_TYPE_Binary* input)
{
    serialize_String_t(buffer, &input->type_name);
}

void deserialize_OBJK_TYPE_Binary(MicroBuffer* buffer, OBJK_TYPE_Binary* output, AuxMemory* aux)
{
    deserialize_String_t(buffer, &output->type_name, aux);
}

uint32_t size_of_OBJK_TYPE_Binary(const OBJK_TYPE_Binary* data)
{
    return size_of_String_t(&data->type_name);
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

uint32_t size_of_OBJK_TOPIC_Binary(const OBJK_TOPIC_Binary* data)
{
    return size_of_String_t(&data->topic_name)
         + size_of_String_t(&data->type_name);
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

uint32_t size_of_OBJK_PUBLISHER_Binary(const OBJK_PUBLISHER_Binary* data)
{
    return size_of_StringSequence_t(&data->partitions)
         + size_of_BinarySequence_t(&data->group_data);
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

uint32_t size_of_OBJK_SUBSCRIBER_Binary(const OBJK_SUBSCRIBER_Binary* data)
{
    return size_of_StringSequence_t(&data->partitions)
         + size_of_BinarySequence_t(&data->group_data);
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

uint32_t size_of_OBJK_Endpoint_QosBinary(const OBJK_Endpoint_QosBinary* data)
{
    return sizeof(data->qos_flags)
         + sizeof(data->history_depth)
         + sizeof(data->deadline_msec)
         + sizeof(data->lifespan_msec)
         + size_of_BinarySequence_t(&data->user_data);
}

void serialize_OBJK_DataReader_Binary(MicroBuffer* buffer, const OBJK_DataReader_Binary* input)
{
    serialize_String_t(buffer, &input->topic_name);
    serialize_OBJK_Endpoint_QosBinary(buffer, &input->endpoint_qos);
    serialize_uint32_t(buffer, input->timebasedfilter_msec);
    serialize_String_t(buffer, &input->contentbased_filter);
}

void deserialize_OBJK_DataReader_Binary(MicroBuffer* buffer, OBJK_DataReader_Binary* output, AuxMemory* aux)
{
    deserialize_String_t(buffer, &output->topic_name, aux);
    deserialize_OBJK_Endpoint_QosBinary(buffer, &output->endpoint_qos, aux);
    deserialize_uint32_t(buffer, &output->timebasedfilter_msec);
    deserialize_String_t(buffer, &output->contentbased_filter, aux);
}

uint32_t size_of_OBJK_DataReader_Binary(const OBJK_DataReader_Binary* data)
{
    return size_of_String_t(&data->topic_name)
         + size_of_OBJK_Endpoint_QosBinary(&data->endpoint_qos)
         + sizeof(data->timebasedfilter_msec)
         + size_of_String_t(&data->contentbased_filter);
}

void serialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, const OBJK_DataWriter_Binary* input)
{
    serialize_String_t(buffer, &input->topic_name);
    serialize_OBJK_Endpoint_QosBinary(buffer, &input->endpoint_qos);
    serialize_uint32_t(buffer, input->ownership_strength);
}

void deserialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, OBJK_DataWriter_Binary* output, AuxMemory* aux)
{
    deserialize_String_t(buffer, &output->topic_name, aux);
    deserialize_OBJK_Endpoint_QosBinary(buffer, &output->endpoint_qos, aux);
    deserialize_uint32_t(buffer, &output->ownership_strength);
}

uint32_t size_of_OBJK_DataWriter_Binary(const OBJK_DataWriter_Binary* data)
{
    return size_of_String_t(&data->topic_name)
         + size_of_OBJK_Endpoint_QosBinary(&data->endpoint_qos)
         + sizeof(data->ownership_strength);
}

void serialize_ObjectVariant(MicroBuffer* buffer, const ObjectVariant* input)
{
    serialize_uint8_t(buffer, input->kind);
    switch(input->kind)
    {
        case OBJK_CLIENT:
            serialize_OBJK_CLIENT_Representation(buffer, &input->_.client);
        break;
        case OBJK_APPLICATION:
            serialize_OBJK_APPLICATION_Representation(buffer, &input->_.application);
        break;
        case OBJK_PARTICIPANT:
            serialize_OBJK_PARTICIPANT_Representation(buffer, &input->_.participant);
        break;
        case OBJK_QOSPROFILE:
            serialize_OBJK_QOSPROFILE_Representation(buffer, &input->_.qos_profile);
        break;
        case OBJK_TYPE:
            serialize_OBJK_TYPE_Representation(buffer, &input->_.type);
        break;
        case OBJK_TOPIC:
            serialize_OBJK_TOPIC_Representation(buffer, &input->_.topic);
        break;
        case OBJK_PUBLISHER:
            serialize_OBJK_PUBLISHER_Representation(buffer, &input->_.publisher);
        break;
        case OBJK_SUBSCRIBER:
            serialize_OBJK_SUBSCRIBER_Representation(buffer, &input->_.subscriber);
        break;
        case OBJK_DATAWRITER:
            serialize_OBJK_DATAWRITER_Representation(buffer, &input->_.data_writer);
        break;
        case OBJK_DATAREADER:
            serialize_OBJK_DATAREADER_Representation(buffer, &input->_.data_reader);
        break;
    }
}

void deserialize_ObjectVariant(MicroBuffer* buffer, ObjectVariant* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->kind);
    switch(output->kind)
    {
        case OBJK_CLIENT:
            deserialize_OBJK_CLIENT_Representation(buffer, &output->_.client, aux);
        break;
        case OBJK_APPLICATION:
            deserialize_OBJK_APPLICATION_Representation(buffer, &output->_.application, aux);
        break;
        case OBJK_PARTICIPANT:
            deserialize_OBJK_PARTICIPANT_Representation(buffer, &output->_.participant, aux);
        break;
        case OBJK_QOSPROFILE:
            deserialize_OBJK_QOSPROFILE_Representation(buffer, &output->_.qos_profile, aux);
        break;
        case OBJK_TYPE:
            deserialize_OBJK_TYPE_Representation(buffer, &output->_.type, aux);
        break;
        case OBJK_TOPIC:
            deserialize_OBJK_TOPIC_Representation(buffer, &output->_.topic, aux);
        break;
        case OBJK_PUBLISHER:
            deserialize_OBJK_PUBLISHER_Representation(buffer, &output->_.publisher, aux);
        break;
        case OBJK_SUBSCRIBER:
            deserialize_OBJK_SUBSCRIBER_Representation(buffer, &output->_.subscriber, aux);
        break;
        case OBJK_DATAWRITER:
            deserialize_OBJK_DATAWRITER_Representation(buffer, &output->_.data_writer, aux);
        break;
        case OBJK_DATAREADER:
            deserialize_OBJK_DATAREADER_Representation(buffer, &output->_.data_reader, aux);
        break;
    }
}

uint32_t size_of_ObjectVariant(const ObjectVariant* data)
{
    uint32_t size = sizeof(data->kind);
    switch(data->kind)
    {
        case OBJK_CLIENT:
            size += size_of_OBJK_CLIENT_Representation(&data->_.client);
        break;
        case OBJK_APPLICATION:
            size += size_of_OBJK_APPLICATION_Representation(&data->_.application);
        break;
        case OBJK_PARTICIPANT:
            size += size_of_OBJK_PARTICIPANT_Representation(&data->_.participant);
        break;
        case OBJK_QOSPROFILE:
            size += size_of_OBJK_QOSPROFILE_Representation(&data->_.qos_profile);
        break;
        case OBJK_TYPE:
            size += size_of_OBJK_TYPE_Representation(&data->_.type);
        break;
        case OBJK_TOPIC:
            size += size_of_OBJK_TOPIC_Representation(&data->_.topic);
        break;
        case OBJK_PUBLISHER:
            size += size_of_OBJK_PUBLISHER_Representation(&data->_.publisher);
        break;
        case OBJK_SUBSCRIBER:
            size += size_of_OBJK_SUBSCRIBER_Representation(&data->_.subscriber);
        break;
        case OBJK_DATAWRITER:
            size += size_of_OBJK_DATAWRITER_Representation(&data->_.data_writer);
        break;
        case OBJK_DATAREADER:
            size += size_of_OBJK_DATAREADER_Representation(&data->_.data_reader);
        break;
    }

    return size;
}

void serialize_CreationMode(MicroBuffer* buffer, const CreationMode* input)
{
    serialize_bool(buffer, input->reuse);
    serialize_bool(buffer, input->replace);
}

void deserialize_CreationMode(MicroBuffer* buffer, CreationMode* output, AuxMemory* aux)
{
    deserialize_bool(buffer, &output->reuse);
    deserialize_bool(buffer, &output->replace);
}

uint32_t size_of_CreationMode(const CreationMode* data)
{
    return sizeof(data->reuse)
         + sizeof(data->replace);
}

void serialize_ResultStatus(MicroBuffer* buffer, const ResultStatus* input)
{
    serialize_array_uint8_t(buffer, input->request_id, 2);
    serialize_uint8_t(buffer, input->status);
    serialize_uint8_t(buffer, input->implementation_status);
}

void deserialize_ResultStatus(MicroBuffer* buffer, ResultStatus* output, AuxMemory* aux)
{
    deserialize_array_uint8_t(buffer, output->request_id, 2);
    deserialize_uint8_t(buffer, &output->status);
    deserialize_uint8_t(buffer, &output->implementation_status);
}

uint32_t size_of_ResultStatus(const ResultStatus* data)
{
    return sizeof(data->request_id)
         + sizeof(data->status)
         + sizeof(data->implementation_status);
}

void serialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAREADER_ActivityInfo* input)
{
    serialize_int16_t(buffer, input->highest_acked_num);
}

void deserialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAREADER_ActivityInfo* output, AuxMemory* aux)
{
    deserialize_int16_t(buffer, &output->highest_acked_num);
}

uint32_t size_of_OBJK_DATAREADER_ActivityInfo(const OBJK_DATAREADER_ActivityInfo* data)
{
    return sizeof(data->highest_acked_num);
}

void serialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAWRITER_ActivityInfo* input)
{
    serialize_int16_t(buffer, input->streaseq_num);
    serialize_uint64_t(buffer, input->sample_seq_num);
}

void deserialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAWRITER_ActivityInfo* output, AuxMemory* aux)
{
    deserialize_int16_t(buffer, &output->streaseq_num);
    deserialize_uint64_t(buffer, &output->sample_seq_num);
}

uint32_t size_of_OBJK_DATAWRITER_ActivityInfo(const OBJK_DATAWRITER_ActivityInfo* data)
{
    return sizeof(data->streaseq_num)
         + sizeof(data->sample_seq_num);
}

void serialize_ActivityInfoVariant(MicroBuffer* buffer, const ActivityInfoVariant* input)
{
    serialize_uint8_t(buffer, input->kind);
    switch(input->kind)
    {
        case OBJK_DATAWRITER:
            serialize_OBJK_DATAWRITER_ActivityInfo(buffer, &input->_.data_writer);
        break;
        case OBJK_DATAREADER:
            serialize_OBJK_DATAREADER_ActivityInfo(buffer, &input->_.data_reader);
        break;
    }
}

void deserialize_ActivityInfoVariant(MicroBuffer* buffer, ActivityInfoVariant* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->kind);
    switch(output->kind)
    {
        case OBJK_DATAWRITER:
            deserialize_OBJK_DATAWRITER_ActivityInfo(buffer, &output->_.data_writer, aux);
        break;
        case OBJK_DATAREADER:
            deserialize_OBJK_DATAREADER_ActivityInfo(buffer, &output->_.data_reader, aux);
        break;
    }
}

uint32_t size_of_ActivityInfoVariant(const ActivityInfoVariant* data)
{
    uint32_t size = sizeof(data->kind);
    switch(data->kind)
    {
        case OBJK_DATAWRITER:
            size += size_of_OBJK_DATAWRITER_ActivityInfo(&data->_.data_writer);
        break;
        case OBJK_DATAREADER:
            size += size_of_OBJK_DATAREADER_ActivityInfo(&data->_.data_reader);
        break;
    }

    return size;
}

void serialize_Info(MicroBuffer* buffer, const Info* input)
{
    serialize_ObjectVariant(buffer, &input->config);
    serialize_ActivityInfoVariant(buffer, &input->activity);
}

void deserialize_Info(MicroBuffer* buffer, Info* output, AuxMemory* aux)
{
    deserialize_ObjectVariant(buffer, &output->config, aux);
    deserialize_ActivityInfoVariant(buffer, &output->activity, aux);
}

uint32_t size_of_Info(const Info* data)
{
    return size_of_ObjectVariant(&data->config)
         + size_of_ActivityInfoVariant(&data->activity);
}

void serialize_BaseRequest(MicroBuffer* buffer, const BaseRequest* input)
{
    serialize_array_uint8_t(buffer, input->request_id, 2);
    serialize_array_uint8_t(buffer, input->object_id, 2);
}

void deserialize_BaseRequest(MicroBuffer* buffer, BaseRequest* output, AuxMemory* aux)
{
    deserialize_array_uint8_t(buffer, output->request_id, 2);
    deserialize_array_uint8_t(buffer, output->object_id, 2);
}

uint32_t size_of_BaseRequest(const BaseRequest* data)
{
    return sizeof(data->request_id)
         + sizeof(data->object_id);
}

void serialize_BaseObjectRequest(MicroBuffer* buffer, const BaseObjectRequest* input)
{
    serialize_array_uint8_t(buffer, input->base_request, 2);
    serialize_array_uint8_t(buffer, input->object_id, 2);
}

void deserialize_BaseObjectRequest(MicroBuffer* buffer, BaseObjectRequest* output, AuxMemory* aux)
{
    deserialize_array_uint8_t(buffer, output->base_request, 2);
    deserialize_array_uint8_t(buffer, output->object_id, 2);
}

uint32_t size_of_BaseObjectRequest(const BaseObjectRequest* data)
{
    return sizeof(data->base_request)
         + sizeof(data->object_id);
}

void serialize_BaseReply(MicroBuffer* buffer, const BaseReply* input)
{
    serialize_ResultStatus(buffer, &input->result);
    serialize_array_uint8_t(buffer, input->request_id, 2);
}

void deserialize_BaseReply(MicroBuffer* buffer, BaseReply* output, AuxMemory* aux)
{
    deserialize_ResultStatus(buffer, &output->result, aux);
    deserialize_array_uint8_t(buffer, output->request_id, 2);
}

uint32_t size_of_BaseReply(const BaseReply* data)
{
    return size_of_ResultStatus(&data->result)
         + sizeof(data->request_id);
}

void serialize_BaseObjectReply(MicroBuffer* buffer, const BaseObjectReply* input)
{
    serialize_BaseReply(buffer, &input->base_reply);
    serialize_array_uint8_t(buffer, input->object_id, 2);
}

void deserialize_BaseObjectReply(MicroBuffer* buffer, BaseObjectReply* output, AuxMemory* aux)
{
    deserialize_BaseReply(buffer, &output->base_reply, aux);
    deserialize_array_uint8_t(buffer, output->object_id, 2);
}

uint32_t size_of_BaseObjectReply(const BaseObjectReply* data)
{
    return size_of_BaseReply(&data->base_reply)
         + sizeof(data->object_id);
}

void serialize_InfoReply(MicroBuffer* buffer, const InfoReply* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
    serialize_ObjectVariant(buffer, &input->info);
}

void deserialize_InfoReply(MicroBuffer* buffer, InfoReply* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
    deserialize_ObjectVariant(buffer, &output->info, aux);
}

uint32_t size_of_InfoReply(const InfoReply* data)
{
    return size_of_BaseObjectReply(&data->reply)
         + size_of_ObjectVariant(&data->info);
}

void serialize_DataDeliveryControl(MicroBuffer* buffer, const DataDeliveryControl* input)
{
    serialize_uint16_t(buffer, input->max_samples);
    serialize_uint32_t(buffer, input->max_elapsed_time);
    serialize_uint32_t(buffer, input->max_rate);
}

void deserialize_DataDeliveryControl(MicroBuffer* buffer, DataDeliveryControl* output, AuxMemory* aux)
{
    deserialize_uint16_t(buffer, &output->max_samples);
    deserialize_uint32_t(buffer, &output->max_elapsed_time);
    deserialize_uint32_t(buffer, &output->max_rate);
}

uint32_t size_of_DataDeliveryControl(const DataDeliveryControl* data)
{
    return sizeof(data->max_samples)
         + sizeof(data->max_elapsed_time)
         + sizeof(data->max_rate);
}

void serialize_ReadSpecification(MicroBuffer* buffer, const ReadSpecification* input)
{
    serialize_uint8_t(buffer, input->optional_content_filter_expression);
    if(input->optional_content_filter_expression == true)
        serialize_String_t(buffer, &input->content_filter_expression);

    serialize_uint8_t(buffer, input->optional_delivery_config);
    if(input->optional_delivery_config == FORMAT_DATA_SEQ
    || input->optional_delivery_config == FORMAT_SAMPLE_SEQ
    || input->optional_delivery_config == FORMAT_PACKED_SAMPLES)
        serialize_DataDeliveryControl(buffer, &input->delivery_config);

}

void deserialize_ReadSpecification(MicroBuffer* buffer, ReadSpecification* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->optional_content_filter_expression);
    if(output->optional_content_filter_expression == true)
        deserialize_String_t(buffer, &output->content_filter_expression, aux);

    deserialize_uint8_t(buffer, &output->optional_delivery_config);
    if(output->optional_delivery_config == FORMAT_DATA_SEQ
    || output->optional_delivery_config == FORMAT_SAMPLE_SEQ
    || output->optional_delivery_config == FORMAT_PACKED_SAMPLES)
        deserialize_DataDeliveryControl(buffer, &output->delivery_config, aux);

}

uint32_t size_of_ReadSpecification(const ReadSpecification* data)
{
    return ((data->optional_content_filter_expression == true) ? size_of_String_t(&data->content_filter_expression) : 0 )
         + ((data->optional_delivery_config == FORMAT_DATA_SEQ || data->optional_delivery_config == FORMAT_SAMPLE_SEQ || data->optional_delivery_config == FORMAT_PACKED_SAMPLES) ? size_of_DataDeliveryControl(&data->delivery_config) : 0 );
}

void serialize_SampleInfo(MicroBuffer* buffer, const SampleInfo* input)
{
    serialize_uint32_t(buffer, input->state);
    serialize_uint32_t(buffer, input->sequence_number);
    serialize_uint32_t(buffer, input->session_time_offset);
}

void deserialize_SampleInfo(MicroBuffer* buffer, SampleInfo* output, AuxMemory* aux)
{
    deserialize_uint32_t(buffer, &output->state);
    deserialize_uint32_t(buffer, &output->sequence_number);
    deserialize_uint32_t(buffer, &output->session_time_offset);
}

uint32_t size_of_SampleInfo(const SampleInfo* data)
{
    return sizeof(data->state)
         + sizeof(data->sequence_number)
         + sizeof(data->session_time_offset);
}

void serialize_SampleInfoDelta(MicroBuffer* buffer, const SampleInfoDelta* input)
{
    serialize_uint8_t(buffer, input->state);
    serialize_uint8_t(buffer, input->seq_number_delta);
    serialize_uint16_t(buffer, input->timestamp_delta);
}

void deserialize_SampleInfoDelta(MicroBuffer* buffer, SampleInfoDelta* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->state);
    deserialize_uint8_t(buffer, &output->seq_number_delta);
    deserialize_uint16_t(buffer, &output->timestamp_delta);
}

uint32_t size_of_SampleInfoDelta(const SampleInfoDelta* data)
{
    return sizeof(data->state)
         + sizeof(data->seq_number_delta)
         + sizeof(data->timestamp_delta);
}

void serialize_SampleData(MicroBuffer* buffer, const SampleData* input)
{
    serialize_sequence_uint8_t(buffer, input->data, input->size);
}

void deserialize_SampleData(MicroBuffer* buffer, SampleData* output, AuxMemory* aux)
{
    deserialize_sequence_uint8_t(buffer, output->data, &output->size);
}

uint32_t size_of_SampleData(const SampleData* data)
{
    return sizeof(data->size) + data->size;
}

void serialize_SampleDataSequence(MicroBuffer* buffer, const SampleDataSequence* input)
{
    serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size; i++)
        serialize_SampleData(buffer, &input->data[i]);
}

void deserialize_SampleDataSequence(MicroBuffer* buffer, SampleDataSequence* output, AuxMemory* aux)
{
    deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size; i++)
        deserialize_SampleData(buffer, &output->data[i], aux);
}

uint32_t size_of_SampleDataSequence(const SampleDataSequence* data)
{
    return sizeof(data->size) + data->size;
}

void serialize_Sample(MicroBuffer* buffer, const Sample* input)
{
    serialize_SampleInfo(buffer, &input->info);
    serialize_SampleData(buffer, &input->data);
}

void deserialize_Sample(MicroBuffer* buffer, Sample* output, AuxMemory* aux)
{
    deserialize_SampleInfo(buffer, &output->info, aux);
    deserialize_SampleData(buffer, &output->data, aux);
}

uint32_t size_of_Sample(const Sample* data)
{
    return size_of_SampleInfo(&data->info)
         + size_of_SampleData(&data->data);
}

void serialize_SampleSequence(MicroBuffer* buffer, const SampleSequence* input)
{
    serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size; i++)
        serialize_Sample(buffer, &input->data[i]);
}

void deserialize_SampleSequence(MicroBuffer* buffer, SampleSequence* output, AuxMemory* aux)
{
    deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size; i++)
        deserialize_Sample(buffer, &output->data[i], aux);
}

uint32_t size_of_SampleSequence(const SampleSequence* data)
{
    return sizeof(data->size) + data->size;
}

void serialize_SampleDelta(MicroBuffer* buffer, const SampleDelta* input)
{
    serialize_SampleInfoDelta(buffer, &input->info_delta);
    serialize_SampleData(buffer, &input->data);
}

void deserialize_SampleDelta(MicroBuffer* buffer, SampleDelta* output, AuxMemory* aux)
{
    deserialize_SampleInfoDelta(buffer, &output->info_delta, aux);
    deserialize_SampleData(buffer, &output->data, aux);
}

uint32_t size_of_SampleDelta(const SampleDelta* data)
{
    return size_of_SampleInfoDelta(&data->info_delta)
         + size_of_SampleData(&data->data);
}

void serialize_SampleDeltaSequence(MicroBuffer* buffer, const SampleDeltaSequence* input)
{
    serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size; i++)
        serialize_SampleDelta(buffer, &input->data[i]);
}

void deserialize_SampleDeltaSequence(MicroBuffer* buffer, SampleDeltaSequence* output, AuxMemory* aux)
{
    deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size; i++)
        deserialize_SampleDelta(buffer, &output->data[i], aux);
}

uint32_t size_of_SampleDeltaSequence(const SampleDeltaSequence* data)
{
    return sizeof(data->size) + data->size;
}

void serialize_PackedSamples(MicroBuffer* buffer, const PackedSamples* input)
{
    serialize_SampleInfo(buffer, &input->info_base);
    serialize_SampleDeltaSequence(buffer, &input->sample_delta_seq);
}

void deserialize_PackedSamples(MicroBuffer* buffer, PackedSamples* output, AuxMemory* aux)
{
    deserialize_SampleInfo(buffer, &output->info_base, aux);
    deserialize_SampleDeltaSequence(buffer, &output->sample_delta_seq, aux);
}

uint32_t size_of_PackedSamples(const PackedSamples* data)
{
    return size_of_SampleInfo(&data->info_base)
         + size_of_SampleDeltaSequence(&data->sample_delta_seq);
}

void serialize_PackedSamplesSequence(MicroBuffer* buffer, const PackedSamplesSequence* input)
{
    serialize_uint32_t(buffer, input->size);
    for(uint32_t i = 0; i < input->size; i++)
        serialize_PackedSamples(buffer, &input->data[i]);
}

void deserialize_PackedSamplesSequence(MicroBuffer* buffer, PackedSamplesSequence* output, AuxMemory* aux)
{
    deserialize_uint32_t(buffer, &output->size);
    for(uint32_t i = 0; i < output->size; i++)
        deserialize_PackedSamples(buffer, &output->data[i], aux);
}

uint32_t size_of_PackedSamplesSequence(const PackedSamplesSequence* data)
{
    return sizeof(data->size) + data->size;
}

void serialize_DataRepresentation(MicroBuffer* buffer, const DataRepresentation* input)
{
    serialize_uint8_t(buffer, input->format);
    switch(input->format)
    {
        case FORMAT_DATA:
            serialize_SampleData(buffer, &input->_.data);
        break;
        case FORMAT_DATA_SEQ:
            serialize_SampleDataSequence(buffer, &input->_.data_seq);
        break;
        case FORMAT_SAMPLE:
            serialize_Sample(buffer, &input->_.sample);
        break;
        case FORMAT_SAMPLE_SEQ:
            serialize_SampleSequence(buffer, &input->_.sample_seq);
        break;
        case FORMAT_PACKED_SAMPLES:
            serialize_PackedSamples(buffer, &input->_.packed_samples);
        break;
    }
}

void deserialize_DataRepresentation(MicroBuffer* buffer, DataRepresentation* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->format);
    switch(output->format)
    {
        case FORMAT_DATA:
            deserialize_SampleData(buffer, &output->_.data, aux);
        break;
        case FORMAT_DATA_SEQ:
            deserialize_SampleDataSequence(buffer, &output->_.data_seq, aux);
        break;
        case FORMAT_SAMPLE:
            deserialize_Sample(buffer, &output->_.sample, aux);
        break;
        case FORMAT_SAMPLE_SEQ:
            deserialize_SampleSequence(buffer, &output->_.sample_seq, aux);
        break;
        case FORMAT_PACKED_SAMPLES:
            deserialize_PackedSamples(buffer, &output->_.packed_samples, aux);
        break;
    }
}

uint32_t size_of_DataRepresentation(const DataRepresentation* data)
{
    uint32_t size = sizeof(data->format);
    switch(data->format)
    {
        case FORMAT_DATA:
            size += size_of_SampleData(&data->_.data);
        break;
        case FORMAT_DATA_SEQ:
            size += size_of_SampleDataSequence(&data->_.data_seq);
        break;
        case FORMAT_SAMPLE:
            size += size_of_Sample(&data->_.sample);
        break;
        case FORMAT_SAMPLE_SEQ:
            size += size_of_SampleSequence(&data->_.sample_seq);
        break;
        case FORMAT_PACKED_SAMPLES:
            size += size_of_PackedSamples(&data->_.packed_samples);
        break;
    }

    return size;
}

void serialize_MessageHeader(MicroBuffer* buffer, const MessageHeader* input)
{
    serialize_uint8_t(buffer, input->session_id);
    serialize_uint8_t(buffer, input->stream_id);
    serialize_uint16_t(buffer, input->sequence_nr);
    serialize_array_uint8_t(buffer, input->client_key, 4);
}

void deserialize_MessageHeader(MicroBuffer* buffer, MessageHeader* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->session_id);
    deserialize_uint8_t(buffer, &output->stream_id);
    deserialize_uint16_t(buffer, &output->sequence_nr);
    deserialize_array_uint8_t(buffer, output->client_key, 4);
}

uint32_t size_of_MessageHeader(const MessageHeader* data)
{
    return sizeof(data->session_id)
         + sizeof(data->stream_id)
         + sizeof(data->sequence_nr)
         + sizeof(data->client_key);
}

void serialize_SubmessageHeader(MicroBuffer* buffer, const SubmessageHeader* input)
{
    serialize_uint8_t(buffer, input->submessage_id);
    serialize_uint8_t(buffer, input->flags);
    serialize_uint16_t(buffer, input->submessage_length);
}

void deserialize_SubmessageHeader(MicroBuffer* buffer, SubmessageHeader* output, AuxMemory* aux)
{
    deserialize_uint8_t(buffer, &output->submessage_id);
    deserialize_uint8_t(buffer, &output->flags);
    deserialize_uint16_t(buffer, &output->submessage_length);
}

uint32_t size_of_SubmessageHeader(const SubmessageHeader* data)
{
    return sizeof(data->submessage_id)
         + sizeof(data->flags)
         + sizeof(data->submessage_length);
}

void serialize_CreateResourcePayload(MicroBuffer* buffer, const CreateResourcePayload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
    serialize_ObjectVariant(buffer, &input->object_representation);
}

void deserialize_CreateResourcePayload(MicroBuffer* buffer, CreateResourcePayload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
    deserialize_ObjectVariant(buffer, &output->object_representation, aux);
}

uint32_t size_of_CreateResourcePayload(const CreateResourcePayload* data)
{
    return size_of_BaseObjectRequest(&data->request)
         + size_of_ObjectVariant(&data->object_representation);
}

void serialize_DeleteResourcePayload(MicroBuffer* buffer, const DeleteResourcePayload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
}

void deserialize_DeleteResourcePayload(MicroBuffer* buffer, DeleteResourcePayload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
}

uint32_t size_of_DeleteResourcePayload(const DeleteResourcePayload* data)
{
    return size_of_BaseObjectRequest(&data->request);
}

void serialize_ResourceStatusPayload(MicroBuffer* buffer, const ResourceStatusPayload* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
}

void deserialize_ResourceStatusPayload(MicroBuffer* buffer, ResourceStatusPayload* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
}

uint32_t size_of_ResourceStatusPayload(const ResourceStatusPayload* data)
{
    return size_of_BaseObjectReply(&data->reply);
}

void serialize_GetInfoPayload(MicroBuffer* buffer, const GetInfoPayload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
    serialize_uint32_t(buffer, input->info_mask);
}

void deserialize_GetInfoPayload(MicroBuffer* buffer, GetInfoPayload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
    deserialize_uint32_t(buffer, &output->info_mask);
}

uint32_t size_of_GetInfoPayload(const GetInfoPayload* data)
{
    return size_of_BaseObjectRequest(&data->request)
         + sizeof(data->info_mask);
}

void serialize_InfoPayload(MicroBuffer* buffer, const InfoPayload* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
    serialize_Info(buffer, &input->info);
}

void deserialize_InfoPayload(MicroBuffer* buffer, InfoPayload* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
    deserialize_Info(buffer, &output->info, aux);
}

uint32_t size_of_InfoPayload(const InfoPayload* data)
{
    return size_of_BaseObjectReply(&data->reply)
         + size_of_Info(&data->info);
}

void serialize_ReadDataPayload(MicroBuffer* buffer, const ReadDataPayload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
    serialize_ReadSpecification(buffer, &input->read_specification);
}

void deserialize_ReadDataPayload(MicroBuffer* buffer, ReadDataPayload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
    deserialize_ReadSpecification(buffer, &output->read_specification, aux);
}

uint32_t size_of_ReadDataPayload(const ReadDataPayload* data)
{
    return size_of_BaseObjectRequest(&data->request)
         + size_of_ReadSpecification(&data->read_specification);
}

void serialize_WriteDataPayload(MicroBuffer* buffer, const WriteDataPayload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
    serialize_DataRepresentation(buffer, &input->data_to_write);
}

void deserialize_WriteDataPayload(MicroBuffer* buffer, WriteDataPayload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
    deserialize_DataRepresentation(buffer, &output->data_to_write, aux);
}

uint32_t size_of_WriteDataPayload(const WriteDataPayload* data)
{
    return size_of_BaseObjectRequest(&data->request)
         + size_of_DataRepresentation(&data->data_to_write);
}


