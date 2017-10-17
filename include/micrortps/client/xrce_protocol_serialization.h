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
uint32_t size_of_Time_t(const Time_t* data);

void serialize_String_t(MicroBuffer* buffer, const String_t* input);
void deserialize_String_t(MicroBuffer* buffer, String_t* output, AuxMemory* aux);
uint32_t size_of_String_t(const String_t* data);

void serialize_BinarySequence_t(MicroBuffer* buffer, const BinarySequence_t* input);
void deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output, AuxMemory* aux);
uint32_t size_of_BinarySequence_t(const BinarySequence_t* data);

void serialize_StringSequence_t(MicroBuffer* buffer, const StringSequence_t* input);
void deserialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* output, AuxMemory* aux);
uint32_t size_of_StringSequence_t(const StringSequence_t* data);

void serialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, const OBJK_CLIENT_Representation* input);
void deserialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_CLIENT_Representation(const OBJK_CLIENT_Representation* data);

void serialize_OBJK_Representation3Formats(MicroBuffer* buffer, const OBJK_Representation3Formats* input);
void deserialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* output, AuxMemory* aux);
uint32_t size_of_OBJK_Representation3Formats(const OBJK_Representation3Formats* data);

void serialize_OBJK_Representation2Formats(MicroBuffer* buffer, const OBJK_Representation2Formats* input);
void deserialize_OBJK_Representation2Formats(MicroBuffer* buffer, OBJK_Representation2Formats* output, AuxMemory* aux);
uint32_t size_of_OBJK_Representation2Formats(const OBJK_Representation2Formats* data);

void serialize_OBJK_Representation3_Base(MicroBuffer* buffer, const OBJK_Representation3_Base* input);
void deserialize_OBJK_Representation3_Base(MicroBuffer* buffer, OBJK_Representation3_Base* output, AuxMemory* aux);
uint32_t size_of_OBJK_Representation3_Base(const OBJK_Representation3_Base* data);

void serialize_OBJK_Representation2_Base(MicroBuffer* buffer, const OBJK_Representation2_Base* input);
void deserialize_OBJK_Representation2_Base(MicroBuffer* buffer, OBJK_Representation2_Base* output, AuxMemory* aux);
uint32_t size_of_OBJK_Representation2_Base(const OBJK_Representation2_Base* data);

void serialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, const OBJK_QOSPROFILE_Representation* input);
void deserialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, OBJK_QOSPROFILE_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_QOSPROFILE_Representation(const OBJK_QOSPROFILE_Representation* data);

void serialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, const OBJK_APPLICATION_Representation* input);
void deserialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, OBJK_APPLICATION_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_APPLICATION_Representation(const OBJK_APPLICATION_Representation* data);

void serialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, const OBJK_PARTICIPANT_Representation* input);
void deserialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, OBJK_PARTICIPANT_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_PARTICIPANT_Representation(const OBJK_PARTICIPANT_Representation* data);

void serialize_OBJK_TYPE_Representation(MicroBuffer* buffer, const OBJK_TYPE_Representation* input);
void deserialize_OBJK_TYPE_Representation(MicroBuffer* buffer, OBJK_TYPE_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_TYPE_Representation(const OBJK_TYPE_Representation* data);

void serialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, const OBJK_TOPIC_Representation* input);
void deserialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, OBJK_TOPIC_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_TOPIC_Representation(const OBJK_TOPIC_Representation* data);

void serialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, const OBJK_PUBLISHER_Representation* input);
void deserialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, OBJK_PUBLISHER_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_PUBLISHER_Representation(const OBJK_PUBLISHER_Representation* data);

void serialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Representation* input);
void deserialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, OBJK_SUBSCRIBER_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_SUBSCRIBER_Representation(const OBJK_SUBSCRIBER_Representation* data);

void serialize_OBJK_DATAWRITER_Representation(MicroBuffer* buffer, const OBJK_DATAWRITER_Representation* input);
void deserialize_OBJK_DATAWRITER_Representation(MicroBuffer* buffer, OBJK_DATAWRITER_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_DATAWRITER_Representation(const OBJK_DATAWRITER_Representation* data);

void serialize_OBJK_DATAREADER_Representation(MicroBuffer* buffer, const OBJK_DATAREADER_Representation* input);
void deserialize_OBJK_DATAREADER_Representation(MicroBuffer* buffer, OBJK_DATAREADER_Representation* output, AuxMemory* aux);
uint32_t size_of_OBJK_DATAREADER_Representation(const OBJK_DATAREADER_Representation* data);

void serialize_OBJK_TYPE_Binary(MicroBuffer* buffer, const OBJK_TYPE_Binary* input);
void deserialize_OBJK_TYPE_Binary(MicroBuffer* buffer, OBJK_TYPE_Binary* output, AuxMemory* aux);
uint32_t size_of_OBJK_TYPE_Binary(const OBJK_TYPE_Binary* data);

void serialize_OBJK_TOPIC_Binary(MicroBuffer* buffer, const OBJK_TOPIC_Binary* input);
void deserialize_OBJK_TOPIC_Binary(MicroBuffer* buffer, OBJK_TOPIC_Binary* output, AuxMemory* aux);
uint32_t size_of_OBJK_TOPIC_Binary(const OBJK_TOPIC_Binary* data);

void serialize_OBJK_PUBLISHER_Binary(MicroBuffer* buffer, const OBJK_PUBLISHER_Binary* input);
void deserialize_OBJK_PUBLISHER_Binary(MicroBuffer* buffer, OBJK_PUBLISHER_Binary* output, AuxMemory* aux);
uint32_t size_of_OBJK_PUBLISHER_Binary(const OBJK_PUBLISHER_Binary* data);

void serialize_OBJK_SUBSCRIBER_Binary(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Binary* input);
void deserialize_OBJK_SUBSCRIBER_Binary(MicroBuffer* buffer, OBJK_SUBSCRIBER_Binary* output, AuxMemory* aux);
uint32_t size_of_OBJK_SUBSCRIBER_Binary(const OBJK_SUBSCRIBER_Binary* data);

void serialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, const OBJK_Endpoint_QosBinary* input);
void deserialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, OBJK_Endpoint_QosBinary* output, AuxMemory* aux);
uint32_t size_of_OBJK_Endpoint_QosBinary(const OBJK_Endpoint_QosBinary* data);

void serialize_OBJK_DataReader_Binary(MicroBuffer* buffer, const OBJK_DataReader_Binary* input);
void deserialize_OBJK_DataReader_Binary(MicroBuffer* buffer, OBJK_DataReader_Binary* output, AuxMemory* aux);
uint32_t size_of_OBJK_DataReader_Binary(const OBJK_DataReader_Binary* data);

void serialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, const OBJK_DataWriter_Binary* input);
void deserialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, OBJK_DataWriter_Binary* output, AuxMemory* aux);
uint32_t size_of_OBJK_DataWriter_Binary(const OBJK_DataWriter_Binary* data);

void serialize_ObjectVariant(MicroBuffer* buffer, const ObjectVariant* input);
void deserialize_ObjectVariant(MicroBuffer* buffer, ObjectVariant* output, AuxMemory* aux);
uint32_t size_of_ObjectVariant(const ObjectVariant* data);

void serialize_CreationMode(MicroBuffer* buffer, const CreationMode* input);
void deserialize_CreationMode(MicroBuffer* buffer, CreationMode* output, AuxMemory* aux);
uint32_t size_of_CreationMode(const CreationMode* data);

void serialize_ResultStatus(MicroBuffer* buffer, const ResultStatus* input);
void deserialize_ResultStatus(MicroBuffer* buffer, ResultStatus* output, AuxMemory* aux);
uint32_t size_of_ResultStatus(const ResultStatus* data);

void serialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAREADER_ActivityInfo* input);
void deserialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAREADER_ActivityInfo* output, AuxMemory* aux);
uint32_t size_of_OBJK_DATAREADER_ActivityInfo(const OBJK_DATAREADER_ActivityInfo* data);

void serialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAWRITER_ActivityInfo* input);
void deserialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAWRITER_ActivityInfo* output, AuxMemory* aux);
uint32_t size_of_OBJK_DATAWRITER_ActivityInfo(const OBJK_DATAWRITER_ActivityInfo* data);

void serialize_ActivityInfoVariant(MicroBuffer* buffer, const ActivityInfoVariant* input);
void deserialize_ActivityInfoVariant(MicroBuffer* buffer, ActivityInfoVariant* output, AuxMemory* aux);
uint32_t size_of_ActivityInfoVariant(const ActivityInfoVariant* data);

void serialize_Info(MicroBuffer* buffer, const Info* input);
void deserialize_Info(MicroBuffer* buffer, Info* output, AuxMemory* aux);
uint32_t size_of_Info(const Info* data);

void serialize_BaseRequest(MicroBuffer* buffer, const BaseRequest* input);
void deserialize_BaseRequest(MicroBuffer* buffer, BaseRequest* output, AuxMemory* aux);
uint32_t size_of_BaseRequest(const BaseRequest* data);

void serialize_BaseObjectRequest(MicroBuffer* buffer, const BaseObjectRequest* input);
void deserialize_BaseObjectRequest(MicroBuffer* buffer, BaseObjectRequest* output, AuxMemory* aux);
uint32_t size_of_BaseObjectRequest(const BaseObjectRequest* data);

void serialize_BaseReply(MicroBuffer* buffer, const BaseReply* input);
void deserialize_BaseReply(MicroBuffer* buffer, BaseReply* output, AuxMemory* aux);
uint32_t size_of_BaseReply(const BaseReply* data);

void serialize_BaseObjectReply(MicroBuffer* buffer, const BaseObjectReply* input);
void deserialize_BaseObjectReply(MicroBuffer* buffer, BaseObjectReply* output, AuxMemory* aux);
uint32_t size_of_BaseObjectReply(const BaseObjectReply* data);

void serialize_InfoReply(MicroBuffer* buffer, const InfoReply* input);
void deserialize_InfoReply(MicroBuffer* buffer, InfoReply* output, AuxMemory* aux);
uint32_t size_of_InfoReply(const InfoReply* data);

void serialize_DataDeliveryControl(MicroBuffer* buffer, const DataDeliveryControl* input);
void deserialize_DataDeliveryControl(MicroBuffer* buffer, DataDeliveryControl* output, AuxMemory* aux);
uint32_t size_of_DataDeliveryControl(const DataDeliveryControl* data);

void serialize_ReadSpecification(MicroBuffer* buffer, const ReadSpecification* input);
void deserialize_ReadSpecification(MicroBuffer* buffer, ReadSpecification* output, AuxMemory* aux);
uint32_t size_of_ReadSpecification(const ReadSpecification* data);

void serialize_SampleInfo(MicroBuffer* buffer, const SampleInfo* input);
void deserialize_SampleInfo(MicroBuffer* buffer, SampleInfo* output, AuxMemory* aux);
uint32_t size_of_SampleInfo(const SampleInfo* data);

void serialize_SampleInfoDelta(MicroBuffer* buffer, const SampleInfoDelta* input);
void deserialize_SampleInfoDelta(MicroBuffer* buffer, SampleInfoDelta* output, AuxMemory* aux);
uint32_t size_of_SampleInfoDelta(const SampleInfoDelta* data);

void serialize_SampleData(MicroBuffer* buffer, const SampleData* input);
void deserialize_SampleData(MicroBuffer* buffer, SampleData* output, AuxMemory* aux);
uint32_t size_of_SampleData(const SampleData* data);

void serialize_SampleDataSequence(MicroBuffer* buffer, const SampleDataSequence* input);
void deserialize_SampleDataSequence(MicroBuffer* buffer, SampleDataSequence* output, AuxMemory* aux);
uint32_t size_of_SampleDataSequence(const SampleDataSequence* data);

void serialize_Sample(MicroBuffer* buffer, const Sample* input);
void deserialize_Sample(MicroBuffer* buffer, Sample* output, AuxMemory* aux);
uint32_t size_of_Sample(const Sample* data);

void serialize_SampleSequence(MicroBuffer* buffer, const SampleSequence* input);
void deserialize_SampleSequence(MicroBuffer* buffer, SampleSequence* output, AuxMemory* aux);
uint32_t size_of_SampleSequence(const SampleSequence* data);

void serialize_SampleDelta(MicroBuffer* buffer, const SampleDelta* input);
void deserialize_SampleDelta(MicroBuffer* buffer, SampleDelta* output, AuxMemory* aux);
uint32_t size_of_SampleDelta(const SampleDelta* data);

void serialize_SampleDeltaSequence(MicroBuffer* buffer, const SampleDeltaSequence* input);
void deserialize_SampleDeltaSequence(MicroBuffer* buffer, SampleDeltaSequence* output, AuxMemory* aux);
uint32_t size_of_SampleDeltaSequence(const SampleDeltaSequence* data);

void serialize_PackedSamples(MicroBuffer* buffer, const PackedSamples* input);
void deserialize_PackedSamples(MicroBuffer* buffer, PackedSamples* output, AuxMemory* aux);
uint32_t size_of_PackedSamples(const PackedSamples* data);

void serialize_PackedSamplesSequence(MicroBuffer* buffer, const PackedSamplesSequence* input);
void deserialize_PackedSamplesSequence(MicroBuffer* buffer, PackedSamplesSequence* output, AuxMemory* aux);
uint32_t size_of_PackedSamplesSequence(const PackedSamplesSequence* data);

void serialize_DataRepresentation(MicroBuffer* buffer, const DataRepresentation* input);
void deserialize_DataRepresentation(MicroBuffer* buffer, DataRepresentation* output, AuxMemory* aux);
uint32_t size_of_DataRepresentation(const DataRepresentation* data);

void serialize_MessageHeader(MicroBuffer* buffer, const MessageHeader* input);
void deserialize_MessageHeader(MicroBuffer* buffer, MessageHeader* output, AuxMemory* aux);
uint32_t size_of_MessageHeader(const MessageHeader* data);

void serialize_SubmessageHeader(MicroBuffer* buffer, const SubmessageHeader* input);
void deserialize_SubmessageHeader(MicroBuffer* buffer, SubmessageHeader* output, AuxMemory* aux);
uint32_t size_of_SubmessageHeader(const SubmessageHeader* data);

void serialize_CreateResourcePayload(MicroBuffer* buffer, const CreateResourcePayload* input);
void deserialize_CreateResourcePayload(MicroBuffer* buffer, CreateResourcePayload* output, AuxMemory* aux);
uint32_t size_of_CreateResourcePayload(const CreateResourcePayload* data);

void serialize_DeleteResourcePayload(MicroBuffer* buffer, const DeleteResourcePayload* input);
void deserialize_DeleteResourcePayload(MicroBuffer* buffer, DeleteResourcePayload* output, AuxMemory* aux);
uint32_t size_of_DeleteResourcePayload(const DeleteResourcePayload* data);

void serialize_ResourceStatusPayload(MicroBuffer* buffer, const ResourceStatusPayload* input);
void deserialize_ResourceStatusPayload(MicroBuffer* buffer, ResourceStatusPayload* output, AuxMemory* aux);
uint32_t size_of_ResourceStatusPayload(const ResourceStatusPayload* data);

void serialize_GetInfoPayload(MicroBuffer* buffer, const GetInfoPayload* input);
void deserialize_GetInfoPayload(MicroBuffer* buffer, GetInfoPayload* output, AuxMemory* aux);
uint32_t size_of_GetInfoPayload(const GetInfoPayload* data);

void serialize_InfoPayload(MicroBuffer* buffer, const InfoPayload* input);
void deserialize_InfoPayload(MicroBuffer* buffer, InfoPayload* output, AuxMemory* aux);
uint32_t size_of_InfoPayload(const InfoPayload* data);

void serialize_ReadDataPayload(MicroBuffer* buffer, const ReadDataPayload* input);
void deserialize_ReadDataPayload(MicroBuffer* buffer, ReadDataPayload* output, AuxMemory* aux);
uint32_t size_of_ReadDataPayload(const ReadDataPayload* data);

void serialize_WriteDataPayload(MicroBuffer* buffer, const WriteDataPayload* input);
void deserialize_WriteDataPayload(MicroBuffer* buffer, WriteDataPayload* output, AuxMemory* aux);
uint32_t size_of_WriteDataPayload(const WriteDataPayload* data);


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
