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

void serialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAREADER_ActivityInfo* input)
{
    serialize_int16_t(buffer, input->highest_acked_num);
}

void deserialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAREADER_ActivityInfo* output, AuxMemory* aux)
{
    deserialize_int16_t(buffer, &output->highest_acked_num);
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

void serialize_InfoReply(MicroBuffer* buffer, const InfoReply* input)
{
    serialize_BaseObjectReply(buffer, &input->base_object_reply);
    serialize_ObjectVariant(buffer, &input->info);
}

void deserialize_InfoReply(MicroBuffer* buffer, InfoReply* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->base_object_reply, aux);
    deserialize_ObjectVariant(buffer, &output->info, aux);
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

void serialize_SampleData(MicroBuffer* buffer, const SampleData* input)
{
    serialize_sequence_uint8_t(buffer, input->data, input->size);
}

void deserialize_SampleData(MicroBuffer* buffer, SampleData* output, AuxMemory* aux)
{
    deserialize_sequence_uint8_t(buffer, output->data, &output->size);
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

void serialize_CREATE_Payload(MicroBuffer* buffer, const CREATE_Payload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
    serialize_ObjectVariant(buffer, &input->object_representation);
}

void deserialize_CREATE_Payload(MicroBuffer* buffer, CREATE_Payload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
    deserialize_ObjectVariant(buffer, &output->object_representation, aux);
}

void serialize_DELETE_RESOURCE_Payload(MicroBuffer* buffer, const DELETE_RESOURCE_Payload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
}

void deserialize_DELETE_RESOURCE_Payload(MicroBuffer* buffer, DELETE_RESOURCE_Payload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
}

void serialize_RESOURCE_STATUS_Payload(MicroBuffer* buffer, const RESOURCE_STATUS_Payload* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
}

void deserialize_RESOURCE_STATUS_Payload(MicroBuffer* buffer, RESOURCE_STATUS_Payload* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
}

void serialize_GET_INFO_Payload(MicroBuffer* buffer, const GET_INFO_Payload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
    serialize_uint32_t(buffer, input->info_mask);
}

void deserialize_GET_INFO_Payload(MicroBuffer* buffer, GET_INFO_Payload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
    deserialize_uint32_t(buffer, &output->info_mask);
}

void serialize_INFO_Payload(MicroBuffer* buffer, const INFO_Payload* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
    serialize_Info(buffer, &input->info);
}

void deserialize_INFO_Payload(MicroBuffer* buffer, INFO_Payload* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
    deserialize_Info(buffer, &output->info, aux);
}

void serialize_READ_DATA_Payload(MicroBuffer* buffer, const READ_DATA_Payload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
    serialize_ReadSpecification(buffer, &input->read_specification);
}

void deserialize_READ_DATA_Payload(MicroBuffer* buffer, READ_DATA_Payload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
    deserialize_ReadSpecification(buffer, &output->read_specification, aux);
}

void serialize_DATA_Payload_Data(MicroBuffer* buffer, const DATA_Payload_Data* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
    serialize_SampleData(buffer, &input->data);
}

void deserialize_DATA_Payload_Data(MicroBuffer* buffer, DATA_Payload_Data* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
    deserialize_SampleData(buffer, &output->data, aux);
}

void serialize_DATA_Payload_Sample(MicroBuffer* buffer, const DATA_Payload_Sample* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
    serialize_Sample(buffer, &input->sample);
}

void deserialize_DATA_Payload_Sample(MicroBuffer* buffer, DATA_Payload_Sample* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
    deserialize_Sample(buffer, &output->sample, aux);
}

void serialize_DATA_Payload_DataSequence(MicroBuffer* buffer, const DATA_Payload_DataSequence* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
    serialize_SampleDataSequence(buffer, &input->data_seq);
}

void deserialize_DATA_Payload_DataSequence(MicroBuffer* buffer, DATA_Payload_DataSequence* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
    deserialize_SampleDataSequence(buffer, &output->data_seq, aux);
}

void serialize_DATA_Payload_SampleSequence(MicroBuffer* buffer, const DATA_Payload_SampleSequence* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
    serialize_SampleSequence(buffer, &input->sample_seq);
}

void deserialize_DATA_Payload_SampleSequence(MicroBuffer* buffer, DATA_Payload_SampleSequence* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
    deserialize_SampleSequence(buffer, &output->sample_seq, aux);
}

void serialize_DATA_Payload_PackageSamples(MicroBuffer* buffer, const DATA_Payload_PackageSamples* input)
{
    serialize_BaseObjectReply(buffer, &input->reply);
    serialize_PackedSamples(buffer, &input->packed_samples);
}

void deserialize_DATA_Payload_PackageSamples(MicroBuffer* buffer, DATA_Payload_PackageSamples* output, AuxMemory* aux)
{
    deserialize_BaseObjectReply(buffer, &output->reply, aux);
    deserialize_PackedSamples(buffer, &output->packed_samples, aux);
}

void serialize_WRITE_DATA_Payload(MicroBuffer* buffer, const WRITE_DATA_Payload* input)
{
    serialize_BaseObjectRequest(buffer, &input->request);
    serialize_DataRepresentation(buffer, &input->data_to_write);
}

void deserialize_WRITE_DATA_Payload(MicroBuffer* buffer, WRITE_DATA_Payload* output, AuxMemory* aux)
{
    deserialize_BaseObjectRequest(buffer, &output->request, aux);
    deserialize_DataRepresentation(buffer, &output->data_to_write, aux);
}


