// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "xrce_protocol_serialization.h"

bool serialize_ClientKey(MicroBuffer *buffer, const ClientKey *input)
{
    return serialize_array_uint8_t(buffer, input->data, 4);
}

bool deserialize_ClientKey(MicroBuffer *buffer, ClientKey *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_array_uint8_t(buffer, output->data, 4);
}

bool serialize_ObjectId(MicroBuffer *buffer, const ObjectId *input)
{
    return serialize_array_uint8_t(buffer, input->data, 2);
}

bool deserialize_ObjectId(MicroBuffer *buffer, ObjectId *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_array_uint8_t(buffer, output->data, 2);
}

bool serialize_XrceCookie(MicroBuffer *buffer, const XrceCookie *input)
{
    return serialize_array_uint8_t(buffer, input->data, 4);
}

bool deserialize_XrceCookie(MicroBuffer *buffer, XrceCookie *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_array_uint8_t(buffer, output->data, 4);
}

bool serialize_XrceVersion(MicroBuffer *buffer, const XrceVersion *input)
{
    return serialize_array_uint8_t(buffer, input->data, 2);
}

bool deserialize_XrceVersion(MicroBuffer *buffer, XrceVersion *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_array_uint8_t(buffer, output->data, 2);
}

bool serialize_XrceVendorId(MicroBuffer *buffer, const XrceVendorId *input)
{
    return serialize_array_uint8_t(buffer, input->data, 2);
}

bool deserialize_XrceVendorId(MicroBuffer *buffer, XrceVendorId *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_array_uint8_t(buffer, output->data, 2);
}

bool serialize_RequestId(MicroBuffer *buffer, const RequestId *input)
{
    return serialize_array_uint8_t(buffer, input->data, 2);
}

bool deserialize_RequestId(MicroBuffer *buffer, RequestId *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_array_uint8_t(buffer, output->data, 2);
}

bool serialize_Time_t(MicroBuffer *buffer, const Time_t *input)
{
    return serialize_int32_t(buffer, input->seconds) && serialize_uint32_t(buffer, input->nanoseconds);
}

bool deserialize_Time_t(MicroBuffer *buffer, Time_t *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_int32_t(buffer, &output->seconds) &&
           deserialize_uint32_t(buffer, &output->nanoseconds);
}

bool serialize_String_t(MicroBuffer *buffer, const String_t *input)
{
    return serialize_uint32_t(buffer, input->size) && serialize_array_char(buffer, input->data, input->size);
}

bool deserialize_String_t(MicroBuffer *buffer, String_t *output, AuxMemory *aux)
{
    if (deserialize_uint32_t(buffer, &output->size))
    {
        output->data = request_aux_memory(aux, output->size * sizeof(char));
        return deserialize_array_char(buffer, output->data, output->size);
    }
    return false;
}

bool serialize_BinarySequence_t(MicroBuffer *buffer, const BinarySequence_t *input)
{
    return serialize_uint32_t(buffer, input->size) &&
           serialize_array_uint8_t(buffer, input->data, input->size);
}

bool deserialize_BinarySequence_t(MicroBuffer *buffer, BinarySequence_t *output, AuxMemory *aux)
{
    if (deserialize_uint32_t(buffer, &output->size))
    {
        output->data = request_aux_memory(aux, output->size * sizeof(uint8_t));
        return deserialize_array_uint8_t(buffer, output->data, output->size);
    }
    return false;
}

bool serialize_StringSequence_t(MicroBuffer *buffer, const StringSequence_t *input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for (uint32_t i = 0; i < input->size; i++)
        ret &= serialize_String_t(buffer, &input->data[i]);
    return ret;
}

bool deserialize_StringSequence_t(MicroBuffer *buffer, StringSequence_t *output, AuxMemory *aux)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    output->data = request_aux_memory(aux, output->size * sizeof(String_t));
    for (uint32_t i = 0; i < output->size; i++)
        ret &= deserialize_String_t(buffer, &output->data[i], aux);
    return ret;
}

bool serialize_OBJK_CLIENT_Representation(MicroBuffer *buffer, const OBJK_CLIENT_Representation *input)
{
    return serialize_XrceCookie(buffer, &input->xrce_cookie) &&
           serialize_XrceVersion(buffer, &input->xrce_version) &&
           serialize_XrceVendorId(buffer, &input->xrce_vendor_id) &&
           serialize_Time_t(buffer, &input->client_timestamp) &&
           serialize_ClientKey(buffer, &input->client_key) &&
           serialize_uint8_t(buffer, input->session_id);
}

bool deserialize_OBJK_CLIENT_Representation(MicroBuffer *buffer, OBJK_CLIENT_Representation *output, AuxMemory *aux)
{
    return deserialize_XrceCookie(buffer, &output->xrce_cookie, aux) &&
           deserialize_XrceVersion(buffer, &output->xrce_version, aux) &&
           deserialize_XrceVendorId(buffer, &output->xrce_vendor_id, aux) &&
           deserialize_Time_t(buffer, &output->client_timestamp, aux) &&
           deserialize_ClientKey(buffer, &output->client_key, aux) &&
           deserialize_uint8_t(buffer, &output->session_id);
}

bool serialize_OBJK_Representation3_Base(MicroBuffer *buffer, const OBJK_Representation3_Base *input)
{
    if (serialize_uint8_t(buffer, input->format))
    {
        switch (input->format)
        {
        case REPRESENTATION_BY_REFERENCE:
            return serialize_String_t(buffer, &input->_.object_name);
            break;
        case REPRESENTATION_AS_XML_STRING:
            return serialize_String_t(buffer, &input->_.xml);
            break;
        case REPRESENTATION_IN_BINARY:
            return serialize_BinarySequence_t(buffer, &input->_.binary_representation);
            break;
        }
    }
    return false;
}

bool deserialize_OBJK_Representation3_Base(MicroBuffer *buffer, OBJK_Representation3_Base *output, AuxMemory *aux)
{
    if (deserialize_uint8_t(buffer, &output->format))
    {
        switch (output->format)
        {
        case REPRESENTATION_BY_REFERENCE:
            return deserialize_String_t(buffer, &output->_.object_name, aux);
            break;
        case REPRESENTATION_AS_XML_STRING:
            return deserialize_String_t(buffer, &output->_.xml, aux);
            break;
        case REPRESENTATION_IN_BINARY:
            return deserialize_BinarySequence_t(buffer, &output->_.binary_representation, aux);
            break;
        }
    }
    return false;
}

bool serialize_OBJK_Representation2_Base(MicroBuffer *buffer, const OBJK_Representation2_Base *input)
{
    if (serialize_uint8_t(buffer, input->format))
    {
        switch (input->format)
        {
        case REPRESENTATION_BY_REFERENCE:
            return serialize_String_t(buffer, &input->_.object_name);
            break;
        case REPRESENTATION_AS_XML_STRING:
            return serialize_String_t(buffer, &input->_.xml);
            break;
        }
    }
    return false;
}

bool deserialize_OBJK_Representation2_Base(MicroBuffer *buffer, OBJK_Representation2_Base *output, AuxMemory *aux)
{
    if (deserialize_uint8_t(buffer, &output->format))
    {
        switch (output->format)
        {
        case REPRESENTATION_BY_REFERENCE:
            return deserialize_String_t(buffer, &output->_.object_name, aux);
            break;
        case REPRESENTATION_AS_XML_STRING:
            return deserialize_String_t(buffer, &output->_.xml, aux);
            break;
        }
    }
    return false;
}

bool serialize_OBJK_QOSPROFILE_Representation(MicroBuffer *buffer, const OBJK_QOSPROFILE_Representation *input)
{
    return serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

bool deserialize_OBJK_QOSPROFILE_Representation(MicroBuffer *buffer, OBJK_QOSPROFILE_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

bool serialize_OBJK_APPLICATION_Representation(MicroBuffer *buffer, const OBJK_APPLICATION_Representation *input)
{
    return serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

bool deserialize_OBJK_APPLICATION_Representation(MicroBuffer *buffer, OBJK_APPLICATION_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

bool serialize_OBJK_PARTICIPANT_Representation(MicroBuffer *buffer, const OBJK_PARTICIPANT_Representation *input)
{
    return serialize_OBJK_Representation2_Base(buffer, &input->base2);
}

bool deserialize_OBJK_PARTICIPANT_Representation(MicroBuffer *buffer, OBJK_PARTICIPANT_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation2_Base(buffer, &output->base2, aux);
}

bool serialize_OBJK_TYPE_Representation(MicroBuffer *buffer, const OBJK_TYPE_Representation *input)
{
    return serialize_OBJK_Representation3_Base(buffer, &input->base3) &&
           serialize_ObjectId(buffer, &input->participant_id) &&
           serialize_String_t(buffer, &input->registered_type_name);
}

bool deserialize_OBJK_TYPE_Representation(MicroBuffer *buffer, OBJK_TYPE_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux) &&
           deserialize_ObjectId(buffer, &output->participant_id, aux) &&
           deserialize_String_t(buffer, &output->registered_type_name, aux);
}

bool serialize_OBJK_TOPIC_Representation(MicroBuffer *buffer, const OBJK_TOPIC_Representation *input)
{
    return serialize_OBJK_Representation3_Base(buffer, &input->base3) &&
           serialize_ObjectId(buffer, &input->participant_id);
}

bool deserialize_OBJK_TOPIC_Representation(MicroBuffer *buffer, OBJK_TOPIC_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux) &&
           deserialize_ObjectId(buffer, &output->participant_id, aux);
}

bool serialize_OBJK_PUBLISHER_Representation(MicroBuffer *buffer, const OBJK_PUBLISHER_Representation *input)
{
    return serialize_OBJK_Representation3_Base(buffer, &input->base3) &&
           serialize_ObjectId(buffer, &input->participant_id);
}

bool deserialize_OBJK_PUBLISHER_Representation(MicroBuffer *buffer, OBJK_PUBLISHER_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux) &&
           deserialize_ObjectId(buffer, &output->participant_id, aux);
}

bool serialize_OBJK_SUBSCRIBER_Representation(MicroBuffer *buffer, const OBJK_SUBSCRIBER_Representation *input)
{
    return serialize_OBJK_Representation3_Base(buffer, &input->base3) &&
           serialize_ObjectId(buffer, &input->participant_id);
}

bool deserialize_OBJK_SUBSCRIBER_Representation(MicroBuffer *buffer, OBJK_SUBSCRIBER_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux) &&
           deserialize_ObjectId(buffer, &output->participant_id, aux);
}

bool serialize_OBJK_DATAWRITER_Representation(MicroBuffer *buffer, const OBJK_DATAWRITER_Representation *input)
{
    return serialize_OBJK_Representation3_Base(buffer, &input->base3) &&
           serialize_ObjectId(buffer, &input->participant_id) &&
           serialize_ObjectId(buffer, &input->publisher_id);
}

bool deserialize_OBJK_DATAWRITER_Representation(MicroBuffer *buffer, OBJK_DATAWRITER_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux) &&
           deserialize_ObjectId(buffer, &output->participant_id, aux) &&
           deserialize_ObjectId(buffer, &output->publisher_id, aux);
}

bool serialize_OBJK_DATAREADER_Representation(MicroBuffer *buffer, const OBJK_DATAREADER_Representation *input)
{
    return serialize_OBJK_Representation3_Base(buffer, &input->base3) &&
           serialize_ObjectId(buffer, &input->participant_id) &&
           serialize_ObjectId(buffer, &input->subscriber_id);
}

bool deserialize_OBJK_DATAREADER_Representation(MicroBuffer *buffer, OBJK_DATAREADER_Representation *output, AuxMemory *aux)
{
    return deserialize_OBJK_Representation3_Base(buffer, &output->base3, aux) &&
           deserialize_ObjectId(buffer, &output->participant_id, aux) &&
           deserialize_ObjectId(buffer, &output->subscriber_id, aux);
}

bool serialize_OBJK_TYPE_Binary(MicroBuffer *buffer, const OBJK_TYPE_Binary *input)
{
    return serialize_String_t(buffer, &input->type_name);
}

bool deserialize_OBJK_TYPE_Binary(MicroBuffer *buffer, OBJK_TYPE_Binary *output, AuxMemory *aux)
{
    return deserialize_String_t(buffer, &output->type_name, aux);
}

bool serialize_OBJK_TOPIC_Binary(MicroBuffer *buffer, const OBJK_TOPIC_Binary *input)
{
    return serialize_String_t(buffer, &input->topic_name) &&
           serialize_String_t(buffer, &input->type_name);
}

bool deserialize_OBJK_TOPIC_Binary(MicroBuffer *buffer, OBJK_TOPIC_Binary *output, AuxMemory *aux)
{
    return deserialize_String_t(buffer, &output->topic_name, aux) &&
           deserialize_String_t(buffer, &output->type_name, aux);
}

bool serialize_OBJK_PUBLISHER_Binary(MicroBuffer *buffer, const OBJK_PUBLISHER_Binary *input)
{
    return serialize_StringSequence_t(buffer, &input->partitions) &&
           serialize_BinarySequence_t(buffer, &input->group_data);
}

bool deserialize_OBJK_PUBLISHER_Binary(MicroBuffer *buffer, OBJK_PUBLISHER_Binary *output, AuxMemory *aux)
{
    return deserialize_StringSequence_t(buffer, &output->partitions, aux) &&
           deserialize_BinarySequence_t(buffer, &output->group_data, aux);
}

bool serialize_OBJK_SUBSCRIBER_Binary(MicroBuffer *buffer, const OBJK_SUBSCRIBER_Binary *input)
{
    return serialize_StringSequence_t(buffer, &input->partitions) &&
           serialize_BinarySequence_t(buffer, &input->group_data);
}

bool deserialize_OBJK_SUBSCRIBER_Binary(MicroBuffer *buffer, OBJK_SUBSCRIBER_Binary *output, AuxMemory *aux)
{
    return deserialize_StringSequence_t(buffer, &output->partitions, aux) &&
           deserialize_BinarySequence_t(buffer, &output->group_data, aux);
}

bool serialize_OBJK_Endpoint_QosBinary(MicroBuffer *buffer, const OBJK_Endpoint_QosBinary *input)
{
    return serialize_uint16_t(buffer, input->qos_flags) &&
           serialize_uint16_t(buffer, input->history_depth) &&
           serialize_uint32_t(buffer, input->deadline_msec) &&
           serialize_uint32_t(buffer, input->lifespan_msec) &&
           serialize_BinarySequence_t(buffer, &input->user_data);
}

bool deserialize_OBJK_Endpoint_QosBinary(MicroBuffer *buffer, OBJK_Endpoint_QosBinary *output, AuxMemory *aux)
{
    return deserialize_uint16_t(buffer, &output->qos_flags) &&
           deserialize_uint16_t(buffer, &output->history_depth) &&
           deserialize_uint32_t(buffer, &output->deadline_msec) &&
           deserialize_uint32_t(buffer, &output->lifespan_msec) &&
           deserialize_BinarySequence_t(buffer, &output->user_data, aux);
}

bool serialize_OBJK_DataReader_Binary(MicroBuffer *buffer, const OBJK_DataReader_Binary *input)
{
    return serialize_String_t(buffer, &input->topic_name) &&
           serialize_OBJK_Endpoint_QosBinary(buffer, &input->endpoint_qos) &&
           serialize_uint32_t(buffer, input->timebasedfilter_msec) &&
           serialize_String_t(buffer, &input->contentbased_filter);
}

bool deserialize_OBJK_DataReader_Binary(MicroBuffer *buffer, OBJK_DataReader_Binary *output, AuxMemory *aux)
{
    return deserialize_String_t(buffer, &output->topic_name, aux) &&
           deserialize_OBJK_Endpoint_QosBinary(buffer, &output->endpoint_qos, aux) &&
           deserialize_uint32_t(buffer, &output->timebasedfilter_msec) &&
           deserialize_String_t(buffer, &output->contentbased_filter, aux);
}

bool serialize_OBJK_DataWriter_Binary(MicroBuffer *buffer, const OBJK_DataWriter_Binary *input)
{
    return serialize_String_t(buffer, &input->topic_name) &&
           serialize_OBJK_Endpoint_QosBinary(buffer, &input->endpoint_qos) &&
           serialize_uint32_t(buffer, input->ownership_strength);
}

bool deserialize_OBJK_DataWriter_Binary(MicroBuffer *buffer, OBJK_DataWriter_Binary *output, AuxMemory *aux)
{
    return deserialize_String_t(buffer, &output->topic_name, aux) &&
           deserialize_OBJK_Endpoint_QosBinary(buffer, &output->endpoint_qos, aux) &&
           deserialize_uint32_t(buffer, &output->ownership_strength);
}

bool serialize_ObjectVariant(MicroBuffer *buffer, const ObjectVariant *input)
{
    if (serialize_uint8_t(buffer, input->kind))
    {
        switch (input->kind)
        {
        case OBJK_APPLICATION:
            return serialize_OBJK_APPLICATION_Representation(buffer, &input->_.application);
            break;
        case OBJK_PARTICIPANT:
            return serialize_OBJK_PARTICIPANT_Representation(buffer, &input->_.participant);
            break;
        case OBJK_QOSPROFILE:
            return serialize_OBJK_QOSPROFILE_Representation(buffer, &input->_.qos_profile);
            break;
        case OBJK_TYPE:
            return serialize_OBJK_TYPE_Representation(buffer, &input->_.type);
            break;
        case OBJK_TOPIC:
            return serialize_OBJK_TOPIC_Representation(buffer, &input->_.topic);
            break;
        case OBJK_PUBLISHER:
            return serialize_OBJK_PUBLISHER_Representation(buffer, &input->_.publisher);
            break;
        case OBJK_SUBSCRIBER:
            return serialize_OBJK_SUBSCRIBER_Representation(buffer, &input->_.subscriber);
            break;
        case OBJK_DATAWRITER:
            return serialize_OBJK_DATAWRITER_Representation(buffer, &input->_.data_writer);
            break;
        case OBJK_DATAREADER:
            return serialize_OBJK_DATAREADER_Representation(buffer, &input->_.data_reader);
            break;
        }
    }
    return false;
}

bool deserialize_ObjectVariant(MicroBuffer *buffer, ObjectVariant *output, AuxMemory *aux)
{
    if (deserialize_uint8_t(buffer, &output->kind))
    {
        switch (output->kind)
        {
        case OBJK_APPLICATION:
            return deserialize_OBJK_APPLICATION_Representation(buffer, &output->_.application, aux);
            break;
        case OBJK_PARTICIPANT:
            return deserialize_OBJK_PARTICIPANT_Representation(buffer, &output->_.participant, aux);
            break;
        case OBJK_QOSPROFILE:
            return deserialize_OBJK_QOSPROFILE_Representation(buffer, &output->_.qos_profile, aux);
            break;
        case OBJK_TYPE:
            return deserialize_OBJK_TYPE_Representation(buffer, &output->_.type, aux);
            break;
        case OBJK_TOPIC:
            return deserialize_OBJK_TOPIC_Representation(buffer, &output->_.topic, aux);
            break;
        case OBJK_PUBLISHER:
            return deserialize_OBJK_PUBLISHER_Representation(buffer, &output->_.publisher, aux);
            break;
        case OBJK_SUBSCRIBER:
            return deserialize_OBJK_SUBSCRIBER_Representation(buffer, &output->_.subscriber, aux);
            break;
        case OBJK_DATAWRITER:
            return deserialize_OBJK_DATAWRITER_Representation(buffer, &output->_.data_writer, aux);
            break;
        case OBJK_DATAREADER:
            return deserialize_OBJK_DATAREADER_Representation(buffer, &output->_.data_reader, aux);
            break;
        }
    }
    return false;
}

bool serialize_ResultStatus(MicroBuffer *buffer, const ResultStatus *input)
{
    return serialize_RequestId(buffer, &input->request_id) &&
           serialize_uint8_t(buffer, input->status) &&
           serialize_uint8_t(buffer, input->implementation_status);
}

bool deserialize_ResultStatus(MicroBuffer *buffer, ResultStatus *output, AuxMemory *aux)
{
    return deserialize_RequestId(buffer, &output->request_id, aux) &&
           deserialize_uint8_t(buffer, &output->status) &&
           deserialize_uint8_t(buffer, &output->implementation_status);
}

bool serialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer *buffer, const OBJK_DATAREADER_ActivityInfo *input)
{
    return serialize_int16_t(buffer, input->highest_acked_num);
}

bool deserialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer *buffer, OBJK_DATAREADER_ActivityInfo *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_int16_t(buffer, &output->highest_acked_num);
}

bool serialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer *buffer, const OBJK_DATAWRITER_ActivityInfo *input)
{
    return serialize_int16_t(buffer, input->streaseq_num) &&
           serialize_uint64_t(buffer, input->sample_seq_num);
}

bool deserialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer *buffer, OBJK_DATAWRITER_ActivityInfo *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_int16_t(buffer, &output->streaseq_num) &&
           deserialize_uint64_t(buffer, &output->sample_seq_num);
}

bool serialize_ActivityInfoVariant(MicroBuffer *buffer, const ActivityInfoVariant *input)
{
    if (serialize_uint8_t(buffer, input->kind))
    {
        switch (input->kind)
        {
        case OBJK_DATAWRITER:
            return serialize_OBJK_DATAWRITER_ActivityInfo(buffer, &input->_.data_writer);
            break;
        case OBJK_DATAREADER:
            return serialize_OBJK_DATAREADER_ActivityInfo(buffer, &input->_.data_reader);
            break;
        }
    }
    return false;
}

bool deserialize_ActivityInfoVariant(MicroBuffer *buffer, ActivityInfoVariant *output, AuxMemory *aux)
{
    if (deserialize_uint8_t(buffer, &output->kind))
    {
        switch (output->kind)
        {
        case OBJK_DATAWRITER:
            return deserialize_OBJK_DATAWRITER_ActivityInfo(buffer, &output->_.data_writer, aux);
            break;
        case OBJK_DATAREADER:
            return deserialize_OBJK_DATAREADER_ActivityInfo(buffer, &output->_.data_reader, aux);
            break;
        }
    }
    return false;
}

bool serialize_Info(MicroBuffer *buffer, const Info *input)
{
    return serialize_ObjectVariant(buffer, &input->config) &&
           serialize_ActivityInfoVariant(buffer, &input->activity);
}

bool deserialize_Info(MicroBuffer *buffer, Info *output, AuxMemory *aux)
{
    return deserialize_ObjectVariant(buffer, &output->config, aux) &&
           deserialize_ActivityInfoVariant(buffer, &output->activity, aux);
}

bool serialize_BaseRequest(MicroBuffer *buffer, const BaseRequest *input)
{
    return serialize_RequestId(buffer, &input->request_id);
}

bool deserialize_BaseRequest(MicroBuffer *buffer, BaseRequest *output, AuxMemory *aux)
{
    return deserialize_RequestId(buffer, &output->request_id, aux);
}

bool serialize_BaseObjectRequest(MicroBuffer *buffer, const BaseObjectRequest *input)
{
    return serialize_BaseRequest(buffer, &input->base) &&
           serialize_ObjectId(buffer, &input->object_id);
}

bool deserialize_BaseObjectRequest(MicroBuffer *buffer, BaseObjectRequest *output, AuxMemory *aux)
{
    return deserialize_BaseRequest(buffer, &output->base, aux) &&
           deserialize_ObjectId(buffer, &output->object_id, aux);
}

bool serialize_BaseReply(MicroBuffer *buffer, const BaseReply *input)
{
    return serialize_ResultStatus(buffer, &input->result) &&
           serialize_RequestId(buffer, &input->request_id);
}

bool deserialize_BaseReply(MicroBuffer *buffer, BaseReply *output, AuxMemory *aux)
{
    return deserialize_ResultStatus(buffer, &output->result, aux) &&
           deserialize_RequestId(buffer, &output->request_id, aux);
}

bool serialize_BaseObjectReply(MicroBuffer *buffer, const BaseObjectReply *input)
{
    return serialize_BaseReply(buffer, &input->base) &&
           serialize_ObjectId(buffer, &input->object_id);
}

bool deserialize_BaseObjectReply(MicroBuffer *buffer, BaseObjectReply *output, AuxMemory *aux)
{
    return deserialize_BaseReply(buffer, &output->base, aux) &&
           deserialize_ObjectId(buffer, &output->object_id, aux);
}

bool serialize_InfoReply(MicroBuffer *buffer, const InfoReply *input)
{
    return serialize_BaseObjectReply(buffer, &input->reply) &&
           serialize_ObjectVariant(buffer, &input->info);
}

bool deserialize_InfoReply(MicroBuffer *buffer, InfoReply *output, AuxMemory *aux)
{
    return deserialize_BaseObjectReply(buffer, &output->reply, aux) &&
           deserialize_ObjectVariant(buffer, &output->info, aux);
}

bool serialize_DataDeliveryControl(MicroBuffer *buffer, const DataDeliveryControl *input)
{
    return serialize_uint16_t(buffer, input->max_samples) &&
           serialize_uint32_t(buffer, input->max_elapsed_time) &&
           serialize_uint32_t(buffer, input->max_rate);
}

bool deserialize_DataDeliveryControl(MicroBuffer *buffer, DataDeliveryControl *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_uint16_t(buffer, &output->max_samples) &&
           deserialize_uint32_t(buffer, &output->max_elapsed_time) &&
           deserialize_uint32_t(buffer, &output->max_rate);
}

bool serialize_ReadSpecification(MicroBuffer *buffer, const ReadSpecification *input)
{
    bool ret = serialize_bool(buffer, input->optional_content_filter_expression);
    if (ret && input->optional_content_filter_expression == true)
        ret &= serialize_String_t(buffer, &input->content_filter_expression);

    ret &= serialize_uint8_t(buffer, input->optional_delivery_config);
    if (ret && (input->optional_delivery_config == FORMAT_DATA_SEQ || input->optional_delivery_config == FORMAT_SAMPLE_SEQ || input->optional_delivery_config == FORMAT_PACKED_SAMPLES))
        ret &= serialize_DataDeliveryControl(buffer, &input->delivery_config);
    return ret;
}

bool deserialize_ReadSpecification(MicroBuffer *buffer, ReadSpecification *output, AuxMemory *aux)
{
    (void)aux;
    bool ret = deserialize_bool(buffer, &output->optional_content_filter_expression);
    if (ret && output->optional_content_filter_expression == true)
        ret &= deserialize_String_t(buffer, &output->content_filter_expression, aux);

    ret &= deserialize_uint8_t(buffer, &output->optional_delivery_config);
    if (ret && (output->optional_delivery_config == FORMAT_DATA_SEQ || output->optional_delivery_config == FORMAT_SAMPLE_SEQ || output->optional_delivery_config == FORMAT_PACKED_SAMPLES))
        ret &= deserialize_DataDeliveryControl(buffer, &output->delivery_config, aux);
    return ret;
}

bool serialize_SampleInfo(MicroBuffer *buffer, const SampleInfo *input)
{
    return serialize_uint32_t(buffer, input->state) &&
           serialize_uint32_t(buffer, input->sequence_number) &&
           serialize_uint32_t(buffer, input->session_time_offset);
}

bool deserialize_SampleInfo(MicroBuffer *buffer, SampleInfo *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_uint32_t(buffer, &output->state) &&
           deserialize_uint32_t(buffer, &output->sequence_number) &&
           deserialize_uint32_t(buffer, &output->session_time_offset);
}

bool serialize_SampleInfoDelta(MicroBuffer *buffer, const SampleInfoDelta *input)
{
    return serialize_uint8_t(buffer, input->state) &&
           serialize_uint8_t(buffer, input->seq_number_delta) &&
           serialize_uint16_t(buffer, input->timestamp_delta);
}

bool deserialize_SampleInfoDelta(MicroBuffer *buffer, SampleInfoDelta *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_uint8_t(buffer, &output->state) &&
           deserialize_uint8_t(buffer, &output->seq_number_delta) &&
           deserialize_uint16_t(buffer, &output->timestamp_delta);
}

bool serialize_SampleData(MicroBuffer *buffer, const SampleData *input)
{
    return serialize_uint32_t(buffer, input->size) &&
           serialize_array_uint8_t(buffer, input->data, input->size);
}

bool deserialize_SampleData(MicroBuffer *buffer, SampleData *output, AuxMemory *aux)
{
    (void)aux;
    if (deserialize_uint32_t(buffer, &output->size))
    {
        output->data = request_aux_memory(aux, output->size * sizeof(uint8_t));
        return deserialize_array_uint8_t(buffer, output->data, output->size);
    }
    return false;
}

bool serialize_SampleDataSequence(MicroBuffer *buffer, const SampleDataSequence *input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for (uint32_t i = 0; i < input->size; i++)
        ret &= serialize_SampleData(buffer, &input->data[i]);
    return ret;
}

bool deserialize_SampleDataSequence(MicroBuffer *buffer, SampleDataSequence *output, AuxMemory *aux)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    output->data = request_aux_memory(aux, output->size * sizeof(SampleData));
    for (uint32_t i = 0; i < output->size; i++)
        ret &= deserialize_SampleData(buffer, &output->data[i], aux);
    return ret;
}

bool serialize_Sample(MicroBuffer *buffer, const Sample *input)
{
    return serialize_SampleInfo(buffer, &input->info) &&
           serialize_SampleData(buffer, &input->data);
}

bool deserialize_Sample(MicroBuffer *buffer, Sample *output, AuxMemory *aux)
{
    return deserialize_SampleInfo(buffer, &output->info, aux) &&
           deserialize_SampleData(buffer, &output->data, aux);
}

bool serialize_SampleSequence(MicroBuffer *buffer, const SampleSequence *input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for (uint32_t i = 0; i < input->size; i++)
        ret &= serialize_Sample(buffer, &input->data[i]);

    return ret;
}

bool deserialize_SampleSequence(MicroBuffer *buffer, SampleSequence *output, AuxMemory *aux)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    output->data = request_aux_memory(aux, output->size * sizeof(Sample));
    for (uint32_t i = 0; i < output->size; i++)
        ret &= deserialize_Sample(buffer, &output->data[i], aux);

    return ret;
}

bool serialize_SampleDelta(MicroBuffer *buffer, const SampleDelta *input)
{
    return serialize_SampleInfoDelta(buffer, &input->info_delta) &&
           serialize_SampleData(buffer, &input->data);
}

bool deserialize_SampleDelta(MicroBuffer *buffer, SampleDelta *output, AuxMemory *aux)
{
    return deserialize_SampleInfoDelta(buffer, &output->info_delta, aux) &&
           deserialize_SampleData(buffer, &output->data, aux);
}

bool serialize_SampleDeltaSequence(MicroBuffer *buffer, const SampleDeltaSequence *input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for (uint32_t i = 0; i < input->size; i++)
        ret &= serialize_SampleDelta(buffer, &input->data[i]);
    return ret;
}

bool deserialize_SampleDeltaSequence(MicroBuffer *buffer, SampleDeltaSequence *output, AuxMemory *aux)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    output->data = request_aux_memory(aux, output->size * sizeof(SampleDelta));
    for (uint32_t i = 0; i < output->size; i++)
        ret &= deserialize_SampleDelta(buffer, &output->data[i], aux);
    return ret;
}

bool serialize_PackedSamples(MicroBuffer *buffer, const PackedSamples *input)
{
    return serialize_SampleInfo(buffer, &input->info_base) &&
           serialize_SampleDeltaSequence(buffer, &input->sample_delta_seq);
}

bool deserialize_PackedSamples(MicroBuffer *buffer, PackedSamples *output, AuxMemory *aux)
{
    return deserialize_SampleInfo(buffer, &output->info_base, aux) &&
           deserialize_SampleDeltaSequence(buffer, &output->sample_delta_seq, aux);
}

bool serialize_PackedSamplesSequence(MicroBuffer *buffer, const PackedSamplesSequence *input)
{
    bool ret = serialize_uint32_t(buffer, input->size);
    for (uint32_t i = 0; i < input->size; i++)
        ret &= serialize_PackedSamples(buffer, &input->data[i]);
    return ret;
}

bool deserialize_PackedSamplesSequence(MicroBuffer *buffer, PackedSamplesSequence *output, AuxMemory *aux)
{
    bool ret = deserialize_uint32_t(buffer, &output->size);
    output->data = request_aux_memory(aux, output->size * sizeof(PackedSamples));
    for (uint32_t i = 0; i < output->size; i++)
        ret &= deserialize_PackedSamples(buffer, &output->data[i], aux);

    return ret;
}

bool serialize_DataRepresentation(MicroBuffer *buffer, const DataRepresentation *input)
{
    if (serialize_uint8_t(buffer, input->format))
    {
        switch (input->format)
        {
        case FORMAT_DATA:
            return serialize_SampleData(buffer, &input->_.data);
            break;
        case FORMAT_DATA_SEQ:
            return serialize_SampleDataSequence(buffer, &input->_.data_seq);
            break;
        case FORMAT_SAMPLE:
            return serialize_Sample(buffer, &input->_.sample);
            break;
        case FORMAT_SAMPLE_SEQ:
            return serialize_SampleSequence(buffer, &input->_.sample_seq);
            break;
        case FORMAT_PACKED_SAMPLES:
            return serialize_PackedSamples(buffer, &input->_.packed_samples);
            break;
        }
    }
    return false;
}

bool deserialize_DataRepresentation(MicroBuffer *buffer, DataRepresentation *output, AuxMemory *aux)
{
    if (deserialize_uint8_t(buffer, &output->format))
    {
        switch (output->format)
        {
        case FORMAT_DATA:
            return deserialize_SampleData(buffer, &output->_.data, aux);
            break;
        case FORMAT_DATA_SEQ:
            return deserialize_SampleDataSequence(buffer, &output->_.data_seq, aux);
            break;
        case FORMAT_SAMPLE:
            return deserialize_Sample(buffer, &output->_.sample, aux);
            break;
        case FORMAT_SAMPLE_SEQ:
            return deserialize_SampleSequence(buffer, &output->_.sample_seq, aux);
            break;
        case FORMAT_PACKED_SAMPLES:
            return deserialize_PackedSamples(buffer, &output->_.packed_samples, aux);
            break;
        }
    }
    return false;
}

bool serialize_MessageHeader(MicroBuffer *buffer, const MessageHeader *input)
{
    return serialize_uint8_t(buffer, input->session_id) &&
           serialize_uint8_t(buffer, input->stream_id) &&
           serialize_endian_uint16_t(buffer, LITTLE_ENDIANNESS, input->sequence_nr);
}

bool deserialize_MessageHeader(MicroBuffer *buffer, MessageHeader *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_uint8_t(buffer, &output->session_id) &&
           deserialize_uint8_t(buffer, &output->stream_id) &&
           deserialize_endian_uint16_t(buffer, LITTLE_ENDIANNESS, &output->sequence_nr);
}

bool serialize_SubmessageHeader(MicroBuffer *buffer, const SubmessageHeader *input)
{
    return serialize_uint8_t(buffer, input->id) &&
           serialize_uint8_t(buffer, input->flags) &&
           serialize_endian_uint16_t(buffer, LITTLE_ENDIANNESS, input->length);
}

bool deserialize_SubmessageHeader(MicroBuffer *buffer, SubmessageHeader *output, AuxMemory *aux)
{
    (void)aux;
    return deserialize_uint8_t(buffer, &output->id) &&
           deserialize_uint8_t(buffer, &output->flags) &&
           deserialize_endian_uint16_t(buffer, LITTLE_ENDIANNESS, &output->length);
}

bool serialize_CreateClientPayload(MicroBuffer *buffer, const CreateClientPayload *input)
{
    return serialize_BaseObjectRequest(buffer, &input->request) &&
           serialize_OBJK_CLIENT_Representation(buffer, &input->representation);
}

bool deserialize_CreateClientPayload(MicroBuffer *buffer, CreateClientPayload *output, AuxMemory *aux)
{
    return deserialize_BaseObjectRequest(buffer, &output->request, aux) &&
           deserialize_OBJK_CLIENT_Representation(buffer, &output->representation, aux);
}

bool serialize_CreateResourcePayload(MicroBuffer *buffer, const CreateResourcePayload *input)
{
    return serialize_BaseObjectRequest(buffer, &input->request) &&
           serialize_ObjectVariant(buffer, &input->representation);
}

bool deserialize_CreateResourcePayload(MicroBuffer *buffer, CreateResourcePayload *output, AuxMemory *aux)
{
    return deserialize_BaseObjectRequest(buffer, &output->request, aux) &&
           deserialize_ObjectVariant(buffer, &output->representation, aux);
}

bool serialize_DeleteResourcePayload(MicroBuffer *buffer, const DeleteResourcePayload *input)
{
    return serialize_BaseObjectRequest(buffer, &input->request);
}

bool deserialize_DeleteResourcePayload(MicroBuffer *buffer, DeleteResourcePayload *output, AuxMemory *aux)
{
    return deserialize_BaseObjectRequest(buffer, &output->request, aux);
}

bool serialize_StatusPayload(MicroBuffer *buffer, const StatusPayload *input)
{
    return serialize_BaseObjectReply(buffer, &input->reply);
}

bool deserialize_StatusPayload(MicroBuffer *buffer, StatusPayload *output, AuxMemory *aux)
{
    return deserialize_BaseObjectReply(buffer, &output->reply, aux);
}

bool serialize_GetInfoPayload(MicroBuffer *buffer, const GetInfoPayload *input)
{
    return serialize_BaseObjectRequest(buffer, &input->request) &&
           serialize_uint32_t(buffer, input->info_mask);
}

bool deserialize_GetInfoPayload(MicroBuffer *buffer, GetInfoPayload *output, AuxMemory *aux)
{
    return deserialize_BaseObjectRequest(buffer, &output->request, aux) &&
           deserialize_uint32_t(buffer, &output->info_mask);
}

bool serialize_InfoPayload(MicroBuffer *buffer, const InfoPayload *input)
{
    return serialize_BaseObjectReply(buffer, &input->reply) &&
           serialize_Info(buffer, &input->info);
}

bool deserialize_InfoPayload(MicroBuffer *buffer, InfoPayload *output, AuxMemory *aux)
{
    return deserialize_BaseObjectReply(buffer, &output->reply, aux) &&
           deserialize_Info(buffer, &output->info, aux);
}

bool serialize_ReadDataPayload(MicroBuffer *buffer, const ReadDataPayload *input)
{
    return serialize_BaseObjectRequest(buffer, &input->request) &&
           serialize_ReadSpecification(buffer, &input->read_specification);
}

bool deserialize_ReadDataPayload(MicroBuffer *buffer, ReadDataPayload *output, AuxMemory *aux)
{
    return deserialize_BaseObjectRequest(buffer, &output->request, aux) &&
           deserialize_ReadSpecification(buffer, &output->read_specification, aux);
}

bool serialize_WriteDataPayload(MicroBuffer *buffer, const WriteDataPayload *input)
{
    return serialize_BaseObjectRequest(buffer, &input->request) &&
           serialize_DataRepresentation(buffer, &input->data_to_write);
}

bool deserialize_WriteDataPayload(MicroBuffer *buffer, WriteDataPayload *output, AuxMemory *aux)
{
    return deserialize_BaseObjectRequest(buffer, &output->request, aux) &&
           deserialize_DataRepresentation(buffer, &output->data_to_write, aux);
}

bool serialize_SampleDataPayloadData(MicroBuffer *buffer, const SampleDataPayloadData *input)
{
    return serialize_BaseObjectReply(buffer, &input->reply) &&
           serialize_SampleData(buffer, &input->data);
}

bool deserialize_SampleDataPayloadData(MicroBuffer *buffer, SampleDataPayloadData *output, AuxMemory *aux)
{
    return deserialize_BaseObjectReply(buffer, &output->reply, aux) &&
           deserialize_SampleData(buffer, &output->data, aux);
}

bool serialize_SamplePayloadData(MicroBuffer *buffer, const SamplePayloadData *input)
{
    return serialize_BaseObjectReply(buffer, &input->reply) &&
           serialize_Sample(buffer, &input->sample);
}

bool deserialize_SamplePayloadData(MicroBuffer *buffer, SamplePayloadData *output, AuxMemory *aux)
{
    return deserialize_BaseObjectReply(buffer, &output->reply, aux) &&
           deserialize_Sample(buffer, &output->sample, aux);
}

bool serialize_SampleDataSequencePayloadData(MicroBuffer *buffer, const SampleDataSequencePayloadData *input)
{
    return serialize_BaseObjectReply(buffer, &input->reply) &&
           serialize_SampleDataSequence(buffer, &input->data_sequence);
}

bool deserialize_SampleDataSequencePayloadData(MicroBuffer *buffer, SampleDataSequencePayloadData *output, AuxMemory *aux)
{
    return deserialize_BaseObjectReply(buffer, &output->reply, aux) &&
           deserialize_SampleDataSequence(buffer, &output->data_sequence, aux);
}

bool serialize_SampleSequencePayloadData(MicroBuffer *buffer, const SampleSequencePayloadData *input)
{
    return serialize_BaseObjectReply(buffer, &input->reply) &&
           serialize_SampleSequence(buffer, &input->sample_sequence);
}

bool deserialize_SampleSequencePayloadData(MicroBuffer *buffer, SampleSequencePayloadData *output, AuxMemory *aux)
{
    return deserialize_BaseObjectReply(buffer, &output->reply, aux) &&
           deserialize_SampleSequence(buffer, &output->sample_sequence, aux);
}

bool serialize_PackedSamplesPayloadData(MicroBuffer *buffer, const PackedSamplesPayloadData *input)
{
    return serialize_BaseObjectReply(buffer, &input->reply) &&
           serialize_PackedSamples(buffer, &input->packed_samples);
}

bool deserialize_PackedSamplesPayloadData(MicroBuffer *buffer, PackedSamplesPayloadData *output, AuxMemory *aux)
{
    return deserialize_BaseObjectReply(buffer, &output->reply, aux) &&
           deserialize_PackedSamples(buffer, &output->packed_samples, aux);
}
