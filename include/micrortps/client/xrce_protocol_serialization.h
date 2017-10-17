#ifndef _MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
#define _MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"
#include "aux_memory.h"

#include <microcdr/microcdr.h>


void serialize_Time_t(MicroBuffer* buffer, const Time_t* input);
void deserialize_Time_t(MicroBuffer* buffer, Time_t* output, AuxMemory* aux);

void serialize_String_t(MicroBuffer* buffer, const String_t* input);
void deserialize_String_t(MicroBuffer* buffer, String_t* output, AuxMemory* aux);

void serialize_BinarySequence_t(MicroBuffer* buffer, const BinarySequence_t* input);
void deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output, AuxMemory* aux);

void serialize_StringSequence_t(MicroBuffer* buffer, const StringSequence_t* input);
void deserialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* output, AuxMemory* aux);

void serialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, const OBJK_CLIENT_Representation* input);
void deserialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* output, AuxMemory* aux);

void serialize_OBJK_Representation3Formats(MicroBuffer* buffer, const OBJK_Representation3Formats* input);
void deserialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* output, AuxMemory* aux);

void serialize_OBJK_Representation2Formats(MicroBuffer* buffer, const OBJK_Representation2Formats* input);
void deserialize_OBJK_Representation2Formats(MicroBuffer* buffer, OBJK_Representation2Formats* output, AuxMemory* aux);

void serialize_OBJK_Representation3_Base(MicroBuffer* buffer, const OBJK_Representation3_Base* input);
void deserialize_OBJK_Representation3_Base(MicroBuffer* buffer, OBJK_Representation3_Base* output, AuxMemory* aux);

void serialize_OBJK_Representation2_Base(MicroBuffer* buffer, const OBJK_Representation2_Base* input);
void deserialize_OBJK_Representation2_Base(MicroBuffer* buffer, OBJK_Representation2_Base* output, AuxMemory* aux);

void serialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, const OBJK_QOSPROFILE_Representation* input);
void deserialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, OBJK_QOSPROFILE_Representation* output, AuxMemory* aux);

void serialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, const OBJK_APPLICATION_Representation* input);
void deserialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, OBJK_APPLICATION_Representation* output, AuxMemory* aux);

void serialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, const OBJK_PARTICIPANT_Representation* input);
void deserialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, OBJK_PARTICIPANT_Representation* output, AuxMemory* aux);

void serialize_OBJK_TYPE_Representation(MicroBuffer* buffer, const OBJK_TYPE_Representation* input);
void deserialize_OBJK_TYPE_Representation(MicroBuffer* buffer, OBJK_TYPE_Representation* output, AuxMemory* aux);

void serialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, const OBJK_TOPIC_Representation* input);
void deserialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, OBJK_TOPIC_Representation* output, AuxMemory* aux);

void serialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, const OBJK_PUBLISHER_Representation* input);
void deserialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, OBJK_PUBLISHER_Representation* output, AuxMemory* aux);

void serialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Representation* input);
void deserialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, OBJK_SUBSCRIBER_Representation* output, AuxMemory* aux);

void serialize_OBJK_DATAWRITER_Representation(MicroBuffer* buffer, const OBJK_DATAWRITER_Representation* input);
void deserialize_OBJK_DATAWRITER_Representation(MicroBuffer* buffer, OBJK_DATAWRITER_Representation* output, AuxMemory* aux);

void serialize_OBJK_DATAREADER_Representation(MicroBuffer* buffer, const OBJK_DATAREADER_Representation* input);
void deserialize_OBJK_DATAREADER_Representation(MicroBuffer* buffer, OBJK_DATAREADER_Representation* output, AuxMemory* aux);

void serialize_OBJK_TYPE_Binary(MicroBuffer* buffer, const OBJK_TYPE_Binary* input);
void deserialize_OBJK_TYPE_Binary(MicroBuffer* buffer, OBJK_TYPE_Binary* output, AuxMemory* aux);

void serialize_OBJK_TOPIC_Binary(MicroBuffer* buffer, const OBJK_TOPIC_Binary* input);
void deserialize_OBJK_TOPIC_Binary(MicroBuffer* buffer, OBJK_TOPIC_Binary* output, AuxMemory* aux);

void serialize_OBJK_PUBLISHER_Binary(MicroBuffer* buffer, const OBJK_PUBLISHER_Binary* input);
void deserialize_OBJK_PUBLISHER_Binary(MicroBuffer* buffer, OBJK_PUBLISHER_Binary* output, AuxMemory* aux);

void serialize_OBJK_SUBSCRIBER_Binary(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Binary* input);
void deserialize_OBJK_SUBSCRIBER_Binary(MicroBuffer* buffer, OBJK_SUBSCRIBER_Binary* output, AuxMemory* aux);

void serialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, const OBJK_Endpoint_QosBinary* input);
void deserialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, OBJK_Endpoint_QosBinary* output, AuxMemory* aux);

void serialize_OBJK_DataReader_Binary(MicroBuffer* buffer, const OBJK_DataReader_Binary* input);
void deserialize_OBJK_DataReader_Binary(MicroBuffer* buffer, OBJK_DataReader_Binary* output, AuxMemory* aux);

void serialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, const OBJK_DataWriter_Binary* input);
void deserialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, OBJK_DataWriter_Binary* output, AuxMemory* aux);

void serialize_ObjectVariant(MicroBuffer* buffer, const ObjectVariant* input);
void deserialize_ObjectVariant(MicroBuffer* buffer, ObjectVariant* output, AuxMemory* aux);

void serialize_CreationMode(MicroBuffer* buffer, const CreationMode* input);
void deserialize_CreationMode(MicroBuffer* buffer, CreationMode* output, AuxMemory* aux);

void serialize_ResultStatus(MicroBuffer* buffer, const ResultStatus* input);
void deserialize_ResultStatus(MicroBuffer* buffer, ResultStatus* output, AuxMemory* aux);

void serialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAREADER_ActivityInfo* input);
void deserialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAREADER_ActivityInfo* output, AuxMemory* aux);

void serialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAWRITER_ActivityInfo* input);
void deserialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAWRITER_ActivityInfo* output, AuxMemory* aux);

void serialize_ActivityInfoVariant(MicroBuffer* buffer, const ActivityInfoVariant* input);
void deserialize_ActivityInfoVariant(MicroBuffer* buffer, ActivityInfoVariant* output, AuxMemory* aux);

void serialize_Info(MicroBuffer* buffer, const Info* input);
void deserialize_Info(MicroBuffer* buffer, Info* output, AuxMemory* aux);

void serialize_BaseRequest(MicroBuffer* buffer, const BaseRequest* input);
void deserialize_BaseRequest(MicroBuffer* buffer, BaseRequest* output, AuxMemory* aux);

void serialize_BaseObjectRequest(MicroBuffer* buffer, const BaseObjectRequest* input);
void deserialize_BaseObjectRequest(MicroBuffer* buffer, BaseObjectRequest* output, AuxMemory* aux);

void serialize_BaseReply(MicroBuffer* buffer, const BaseReply* input);
void deserialize_BaseReply(MicroBuffer* buffer, BaseReply* output, AuxMemory* aux);

void serialize_BaseObjectReply(MicroBuffer* buffer, const BaseObjectReply* input);
void deserialize_BaseObjectReply(MicroBuffer* buffer, BaseObjectReply* output, AuxMemory* aux);

void serialize_InfoReply(MicroBuffer* buffer, const InfoReply* input);
void deserialize_InfoReply(MicroBuffer* buffer, InfoReply* output, AuxMemory* aux);

void serialize_DataDeliveryControl(MicroBuffer* buffer, const DataDeliveryControl* input);
void deserialize_DataDeliveryControl(MicroBuffer* buffer, DataDeliveryControl* output, AuxMemory* aux);

void serialize_ReadSpecification(MicroBuffer* buffer, const ReadSpecification* input);
void deserialize_ReadSpecification(MicroBuffer* buffer, ReadSpecification* output, AuxMemory* aux);

void serialize_SampleInfo(MicroBuffer* buffer, const SampleInfo* input);
void deserialize_SampleInfo(MicroBuffer* buffer, SampleInfo* output, AuxMemory* aux);

void serialize_SampleInfoDelta(MicroBuffer* buffer, const SampleInfoDelta* input);
void deserialize_SampleInfoDelta(MicroBuffer* buffer, SampleInfoDelta* output, AuxMemory* aux);

void serialize_SampleData(MicroBuffer* buffer, const SampleData* input);
void deserialize_SampleData(MicroBuffer* buffer, SampleData* output, AuxMemory* aux);

void serialize_SampleDataSequence(MicroBuffer* buffer, const SampleDataSequence* input);
void deserialize_SampleDataSequence(MicroBuffer* buffer, SampleDataSequence* output, AuxMemory* aux);

void serialize_Sample(MicroBuffer* buffer, const Sample* input);
void deserialize_Sample(MicroBuffer* buffer, Sample* output, AuxMemory* aux);

void serialize_SampleSequence(MicroBuffer* buffer, const SampleSequence* input);
void deserialize_SampleSequence(MicroBuffer* buffer, SampleSequence* output, AuxMemory* aux);

void serialize_SampleDelta(MicroBuffer* buffer, const SampleDelta* input);
void deserialize_SampleDelta(MicroBuffer* buffer, SampleDelta* output, AuxMemory* aux);

void serialize_SampleDeltaSequence(MicroBuffer* buffer, const SampleDeltaSequence* input);
void deserialize_SampleDeltaSequence(MicroBuffer* buffer, SampleDeltaSequence* output, AuxMemory* aux);

void serialize_PackedSamples(MicroBuffer* buffer, const PackedSamples* input);
void deserialize_PackedSamples(MicroBuffer* buffer, PackedSamples* output, AuxMemory* aux);

void serialize_PackedSamplesSequence(MicroBuffer* buffer, const PackedSamplesSequence* input);
void deserialize_PackedSamplesSequence(MicroBuffer* buffer, PackedSamplesSequence* output, AuxMemory* aux);

void serialize_DataRepresentation(MicroBuffer* buffer, const DataRepresentation* input);
void deserialize_DataRepresentation(MicroBuffer* buffer, DataRepresentation* output, AuxMemory* aux);

void serialize_MessageHeader(MicroBuffer* buffer, const MessageHeader* input);
void deserialize_MessageHeader(MicroBuffer* buffer, MessageHeader* output, AuxMemory* aux);

void serialize_SubmessageHeader(MicroBuffer* buffer, const SubmessageHeader* input);
void deserialize_SubmessageHeader(MicroBuffer* buffer, SubmessageHeader* output, AuxMemory* aux);

void serialize_CREATE_Payload(MicroBuffer* buffer, const CREATE_Payload* input);
void deserialize_CREATE_Payload(MicroBuffer* buffer, CREATE_Payload* output, AuxMemory* aux);

void serialize_DELETE_RESOURCE_Payload(MicroBuffer* buffer, const DELETE_RESOURCE_Payload* input);
void deserialize_DELETE_RESOURCE_Payload(MicroBuffer* buffer, DELETE_RESOURCE_Payload* output, AuxMemory* aux);

void serialize_RESOURCE_STATUS_Payload(MicroBuffer* buffer, const RESOURCE_STATUS_Payload* input);
void deserialize_RESOURCE_STATUS_Payload(MicroBuffer* buffer, RESOURCE_STATUS_Payload* output, AuxMemory* aux);

void serialize_GET_INFO_Payload(MicroBuffer* buffer, const GET_INFO_Payload* input);
void deserialize_GET_INFO_Payload(MicroBuffer* buffer, GET_INFO_Payload* output, AuxMemory* aux);

void serialize_INFO_Payload(MicroBuffer* buffer, const INFO_Payload* input);
void deserialize_INFO_Payload(MicroBuffer* buffer, INFO_Payload* output, AuxMemory* aux);

void serialize_READ_DATA_Payload(MicroBuffer* buffer, const READ_DATA_Payload* input);
void deserialize_READ_DATA_Payload(MicroBuffer* buffer, READ_DATA_Payload* output, AuxMemory* aux);

void serialize_DATA_Payload_Data(MicroBuffer* buffer, const DATA_Payload_Data* input);
void deserialize_DATA_Payload_Data(MicroBuffer* buffer, DATA_Payload_Data* output, AuxMemory* aux);

void serialize_DATA_Payload_Sample(MicroBuffer* buffer, const DATA_Payload_Sample* input);
void deserialize_DATA_Payload_Sample(MicroBuffer* buffer, DATA_Payload_Sample* output, AuxMemory* aux);

void serialize_DATA_Payload_DataSequence(MicroBuffer* buffer, const DATA_Payload_DataSequence* input);
void deserialize_DATA_Payload_DataSequence(MicroBuffer* buffer, DATA_Payload_DataSequence* output, AuxMemory* aux);

void serialize_DATA_Payload_SampleSequence(MicroBuffer* buffer, const DATA_Payload_SampleSequence* input);
void deserialize_DATA_Payload_SampleSequence(MicroBuffer* buffer, DATA_Payload_SampleSequence* output, AuxMemory* aux);

void serialize_DATA_Payload_PackageSamples(MicroBuffer* buffer, const DATA_Payload_PackageSamples* input);
void deserialize_DATA_Payload_PackageSamples(MicroBuffer* buffer, DATA_Payload_PackageSamples* output, AuxMemory* aux);

void serialize_WRITE_DATA_Payload(MicroBuffer* buffer, const WRITE_DATA_Payload* input);
void deserialize_WRITE_DATA_Payload(MicroBuffer* buffer, WRITE_DATA_Payload* output, AuxMemory* aux);


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
