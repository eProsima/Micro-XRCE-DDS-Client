#ifndef _MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
#define _MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"
#include "aux_memory.h"

#include <microcdr/microcdr.h>


void serialize_ClientKey(MicroBuffer* buffer, const ClientKey* input);
void deserialize_ClientKey(MicroBuffer* buffer, ClientKey* output, AuxMemory* aux);

void serialize_ObjectId(MicroBuffer* buffer, const ObjectId* input);
void deserialize_ObjectId(MicroBuffer* buffer, ObjectId* output, AuxMemory* aux);

void serialize_XrceCookie(MicroBuffer* buffer, const XrceCookie* input);
void deserialize_XrceCookie(MicroBuffer* buffer, XrceCookie* output, AuxMemory* aux);

void serialize_XrceVersion(MicroBuffer* buffer, const XrceVersion* input);
void deserialize_XrceVersion(MicroBuffer* buffer, XrceVersion* output, AuxMemory* aux);

void serialize_XrceVendorId(MicroBuffer* buffer, const XrceVendorId* input);
void deserialize_XrceVendorId(MicroBuffer* buffer, XrceVendorId* output, AuxMemory* aux);

void serialize_RequestId(MicroBuffer* buffer, const RequestId* input);
void deserialize_RequestId(MicroBuffer* buffer, RequestId* output, AuxMemory* aux);

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

void serialize_CreateClientPayload(MicroBuffer* buffer, const CreateClientPayload* input);
void deserialize_CreateClientPayload(MicroBuffer* buffer, CreateClientPayload* output, AuxMemory* aux);

void serialize_CreateResourcePayload(MicroBuffer* buffer, const CreateResourcePayload* input);
void deserialize_CreateResourcePayload(MicroBuffer* buffer, CreateResourcePayload* output, AuxMemory* aux);

void serialize_DeleteResourcePayload(MicroBuffer* buffer, const DeleteResourcePayload* input);
void deserialize_DeleteResourcePayload(MicroBuffer* buffer, DeleteResourcePayload* output, AuxMemory* aux);

void serialize_StatusPayload(MicroBuffer* buffer, const StatusPayload* input);
void deserialize_StatusPayload(MicroBuffer* buffer, StatusPayload* output, AuxMemory* aux);

void serialize_GetInfoPayload(MicroBuffer* buffer, const GetInfoPayload* input);
void deserialize_GetInfoPayload(MicroBuffer* buffer, GetInfoPayload* output, AuxMemory* aux);

void serialize_InfoPayload(MicroBuffer* buffer, const InfoPayload* input);
void deserialize_InfoPayload(MicroBuffer* buffer, InfoPayload* output, AuxMemory* aux);

void serialize_ReadDataPayload(MicroBuffer* buffer, const ReadDataPayload* input);
void deserialize_ReadDataPayload(MicroBuffer* buffer, ReadDataPayload* output, AuxMemory* aux);

void serialize_WriteDataPayload(MicroBuffer* buffer, const WriteDataPayload* input);
void deserialize_WriteDataPayload(MicroBuffer* buffer, WriteDataPayload* output, AuxMemory* aux);

void serialize_SampleDataPayloadData(MicroBuffer* buffer, const SampleDataPayloadData* input);
void deserialize_SampleDataPayloadData(MicroBuffer* buffer, SampleDataPayloadData* output, AuxMemory* aux);

void serialize_SamplePayloadData(MicroBuffer* buffer, const SamplePayloadData* input);
void deserialize_SamplePayloadData(MicroBuffer* buffer, SamplePayloadData* output, AuxMemory* aux);

void serialize_SampleDataSequencePayloadData(MicroBuffer* buffer, const SampleDataSequencePayloadData* input);
void deserialize_SampleDataSequencePayloadData(MicroBuffer* buffer, SampleDataSequencePayloadData* output, AuxMemory* aux);

void serialize_SampleSequencePayloadData(MicroBuffer* buffer, const SampleSequencePayloadData* input);
void deserialize_SampleSequencePayloadData(MicroBuffer* buffer, SampleSequencePayloadData* output, AuxMemory* aux);

void serialize_PackedSamplesPayloadData(MicroBuffer* buffer, const PackedSamplesPayloadData* input);
void deserialize_PackedSamplesPayloadData(MicroBuffer* buffer, PackedSamplesPayloadData* output, AuxMemory* aux);


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
