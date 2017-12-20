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

#ifndef _MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
#define _MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"
#include "aux_memory.h"

#include <microcdr/microcdr.h>


bool serialize_ClientKey(MicroBuffer* buffer, const ClientKey* input);
bool deserialize_ClientKey(MicroBuffer* buffer, ClientKey* output, AuxMemory* aux);

bool serialize_ObjectId(MicroBuffer* buffer, const ObjectId* input);
bool deserialize_ObjectId(MicroBuffer* buffer, ObjectId* output, AuxMemory* aux);

bool serialize_XrceCookie(MicroBuffer* buffer, const XrceCookie* input);
bool deserialize_XrceCookie(MicroBuffer* buffer, XrceCookie* output, AuxMemory* aux);

bool serialize_XrceVersion(MicroBuffer* buffer, const XrceVersion* input);
bool deserialize_XrceVersion(MicroBuffer* buffer, XrceVersion* output, AuxMemory* aux);

bool serialize_XrceVendorId(MicroBuffer* buffer, const XrceVendorId* input);
bool deserialize_XrceVendorId(MicroBuffer* buffer, XrceVendorId* output, AuxMemory* aux);

bool serialize_RequestId(MicroBuffer* buffer, const RequestId* input);
bool deserialize_RequestId(MicroBuffer* buffer, RequestId* output, AuxMemory* aux);

bool serialize_Time_t(MicroBuffer* buffer, const Time_t* input);
bool deserialize_Time_t(MicroBuffer* buffer, Time_t* output, AuxMemory* aux);

bool serialize_String_t(MicroBuffer* buffer, const String_t* input);
bool deserialize_String_t(MicroBuffer* buffer, String_t* output, AuxMemory* aux);

bool serialize_BinarySequence_t(MicroBuffer* buffer, const BinarySequence_t* input);
bool deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output, AuxMemory* aux);

bool serialize_StringSequence_t(MicroBuffer* buffer, const StringSequence_t* input);
bool deserialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* output, AuxMemory* aux);

bool serialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, const OBJK_CLIENT_Representation* input);
bool deserialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* output, AuxMemory* aux);

bool serialize_OBJK_Representation3_Base(MicroBuffer* buffer, const OBJK_Representation3_Base* input);
bool deserialize_OBJK_Representation3_Base(MicroBuffer* buffer, OBJK_Representation3_Base* output, AuxMemory* aux);

bool serialize_OBJK_Representation2_Base(MicroBuffer* buffer, const OBJK_Representation2_Base* input);
bool deserialize_OBJK_Representation2_Base(MicroBuffer* buffer, OBJK_Representation2_Base* output, AuxMemory* aux);

bool serialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, const OBJK_QOSPROFILE_Representation* input);
bool deserialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, OBJK_QOSPROFILE_Representation* output, AuxMemory* aux);

bool serialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, const OBJK_APPLICATION_Representation* input);
bool deserialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, OBJK_APPLICATION_Representation* output, AuxMemory* aux);

bool serialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, const OBJK_PARTICIPANT_Representation* input);
bool deserialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, OBJK_PARTICIPANT_Representation* output, AuxMemory* aux);

bool serialize_OBJK_TYPE_Representation(MicroBuffer* buffer, const OBJK_TYPE_Representation* input);
bool deserialize_OBJK_TYPE_Representation(MicroBuffer* buffer, OBJK_TYPE_Representation* output, AuxMemory* aux);

bool serialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, const OBJK_TOPIC_Representation* input);
bool deserialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, OBJK_TOPIC_Representation* output, AuxMemory* aux);

bool serialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, const OBJK_PUBLISHER_Representation* input);
bool deserialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, OBJK_PUBLISHER_Representation* output, AuxMemory* aux);

bool serialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Representation* input);
bool deserialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, OBJK_SUBSCRIBER_Representation* output, AuxMemory* aux);

bool serialize_OBJK_DATAWRITER_Representation(MicroBuffer* buffer, const OBJK_DATAWRITER_Representation* input);
bool deserialize_OBJK_DATAWRITER_Representation(MicroBuffer* buffer, OBJK_DATAWRITER_Representation* output, AuxMemory* aux);

bool serialize_OBJK_DATAREADER_Representation(MicroBuffer* buffer, const OBJK_DATAREADER_Representation* input);
bool deserialize_OBJK_DATAREADER_Representation(MicroBuffer* buffer, OBJK_DATAREADER_Representation* output, AuxMemory* aux);

bool serialize_OBJK_TYPE_Binary(MicroBuffer* buffer, const OBJK_TYPE_Binary* input);
bool deserialize_OBJK_TYPE_Binary(MicroBuffer* buffer, OBJK_TYPE_Binary* output, AuxMemory* aux);

bool serialize_OBJK_TOPIC_Binary(MicroBuffer* buffer, const OBJK_TOPIC_Binary* input);
bool deserialize_OBJK_TOPIC_Binary(MicroBuffer* buffer, OBJK_TOPIC_Binary* output, AuxMemory* aux);

bool serialize_OBJK_PUBLISHER_Binary(MicroBuffer* buffer, const OBJK_PUBLISHER_Binary* input);
bool deserialize_OBJK_PUBLISHER_Binary(MicroBuffer* buffer, OBJK_PUBLISHER_Binary* output, AuxMemory* aux);

bool serialize_OBJK_SUBSCRIBER_Binary(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Binary* input);
bool deserialize_OBJK_SUBSCRIBER_Binary(MicroBuffer* buffer, OBJK_SUBSCRIBER_Binary* output, AuxMemory* aux);

bool serialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, const OBJK_Endpoint_QosBinary* input);
bool deserialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, OBJK_Endpoint_QosBinary* output, AuxMemory* aux);

bool serialize_OBJK_DataReader_Binary(MicroBuffer* buffer, const OBJK_DataReader_Binary* input);
bool deserialize_OBJK_DataReader_Binary(MicroBuffer* buffer, OBJK_DataReader_Binary* output, AuxMemory* aux);

bool serialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, const OBJK_DataWriter_Binary* input);
bool deserialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, OBJK_DataWriter_Binary* output, AuxMemory* aux);

bool serialize_ObjectVariant(MicroBuffer* buffer, const ObjectVariant* input);
bool deserialize_ObjectVariant(MicroBuffer* buffer, ObjectVariant* output, AuxMemory* aux);

bool serialize_ResultStatus(MicroBuffer* buffer, const ResultStatus* input);
bool deserialize_ResultStatus(MicroBuffer* buffer, ResultStatus* output, AuxMemory* aux);

bool serialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAREADER_ActivityInfo* input);
bool deserialize_OBJK_DATAREADER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAREADER_ActivityInfo* output, AuxMemory* aux);

bool serialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer* buffer, const OBJK_DATAWRITER_ActivityInfo* input);
bool deserialize_OBJK_DATAWRITER_ActivityInfo(MicroBuffer* buffer, OBJK_DATAWRITER_ActivityInfo* output, AuxMemory* aux);

bool serialize_ActivityInfoVariant(MicroBuffer* buffer, const ActivityInfoVariant* input);
bool deserialize_ActivityInfoVariant(MicroBuffer* buffer, ActivityInfoVariant* output, AuxMemory* aux);

bool serialize_Info(MicroBuffer* buffer, const Info* input);
bool deserialize_Info(MicroBuffer* buffer, Info* output, AuxMemory* aux);

bool serialize_BaseRequest(MicroBuffer* buffer, const BaseRequest* input);
bool deserialize_BaseRequest(MicroBuffer* buffer, BaseRequest* output, AuxMemory* aux);

bool serialize_BaseObjectRequest(MicroBuffer* buffer, const BaseObjectRequest* input);
bool deserialize_BaseObjectRequest(MicroBuffer* buffer, BaseObjectRequest* output, AuxMemory* aux);

bool serialize_BaseReply(MicroBuffer* buffer, const BaseReply* input);
bool deserialize_BaseReply(MicroBuffer* buffer, BaseReply* output, AuxMemory* aux);

bool serialize_BaseObjectReply(MicroBuffer* buffer, const BaseObjectReply* input);
bool deserialize_BaseObjectReply(MicroBuffer* buffer, BaseObjectReply* output, AuxMemory* aux);

bool serialize_InfoReply(MicroBuffer* buffer, const InfoReply* input);
bool deserialize_InfoReply(MicroBuffer* buffer, InfoReply* output, AuxMemory* aux);

bool serialize_DataDeliveryControl(MicroBuffer* buffer, const DataDeliveryControl* input);
bool deserialize_DataDeliveryControl(MicroBuffer* buffer, DataDeliveryControl* output, AuxMemory* aux);

bool serialize_ReadSpecification(MicroBuffer* buffer, const ReadSpecification* input);
bool deserialize_ReadSpecification(MicroBuffer* buffer, ReadSpecification* output, AuxMemory* aux);

bool serialize_SampleInfo(MicroBuffer* buffer, const SampleInfo* input);
bool deserialize_SampleInfo(MicroBuffer* buffer, SampleInfo* output, AuxMemory* aux);

bool serialize_SampleInfoDelta(MicroBuffer* buffer, const SampleInfoDelta* input);
bool deserialize_SampleInfoDelta(MicroBuffer* buffer, SampleInfoDelta* output, AuxMemory* aux);

bool serialize_SampleData(MicroBuffer* buffer, const SampleData* input);
bool deserialize_SampleData(MicroBuffer* buffer, SampleData* output, AuxMemory* aux);

bool serialize_SampleDataSequence(MicroBuffer* buffer, const SampleDataSequence* input);
bool deserialize_SampleDataSequence(MicroBuffer* buffer, SampleDataSequence* output, AuxMemory* aux);

bool serialize_Sample(MicroBuffer* buffer, const Sample* input);
bool deserialize_Sample(MicroBuffer* buffer, Sample* output, AuxMemory* aux);

bool serialize_SampleSequence(MicroBuffer* buffer, const SampleSequence* input);
bool deserialize_SampleSequence(MicroBuffer* buffer, SampleSequence* output, AuxMemory* aux);

bool serialize_SampleDelta(MicroBuffer* buffer, const SampleDelta* input);
bool deserialize_SampleDelta(MicroBuffer* buffer, SampleDelta* output, AuxMemory* aux);

bool serialize_SampleDeltaSequence(MicroBuffer* buffer, const SampleDeltaSequence* input);
bool deserialize_SampleDeltaSequence(MicroBuffer* buffer, SampleDeltaSequence* output, AuxMemory* aux);

bool serialize_PackedSamples(MicroBuffer* buffer, const PackedSamples* input);
bool deserialize_PackedSamples(MicroBuffer* buffer, PackedSamples* output, AuxMemory* aux);

bool serialize_PackedSamplesSequence(MicroBuffer* buffer, const PackedSamplesSequence* input);
bool deserialize_PackedSamplesSequence(MicroBuffer* buffer, PackedSamplesSequence* output, AuxMemory* aux);

bool serialize_DataRepresentation(MicroBuffer* buffer, const DataRepresentation* input);
bool deserialize_DataRepresentation(MicroBuffer* buffer, DataRepresentation* output, AuxMemory* aux);

bool serialize_MessageHeader(MicroBuffer* buffer, const MessageHeader* input);
bool deserialize_MessageHeader(MicroBuffer* buffer, MessageHeader* output, AuxMemory* aux);

bool serialize_SubmessageHeader(MicroBuffer* buffer, const SubmessageHeader* input);
bool deserialize_SubmessageHeader(MicroBuffer* buffer, SubmessageHeader* output, AuxMemory* aux);

bool serialize_CreateClientPayload(MicroBuffer* buffer, const CreateClientPayload* input);
bool deserialize_CreateClientPayload(MicroBuffer* buffer, CreateClientPayload* output, AuxMemory* aux);

bool serialize_CreateResourcePayload(MicroBuffer* buffer, const CreateResourcePayload* input);
bool deserialize_CreateResourcePayload(MicroBuffer* buffer, CreateResourcePayload* output, AuxMemory* aux);

bool serialize_DeleteResourcePayload(MicroBuffer* buffer, const DeleteResourcePayload* input);
bool deserialize_DeleteResourcePayload(MicroBuffer* buffer, DeleteResourcePayload* output, AuxMemory* aux);

bool serialize_StatusPayload(MicroBuffer* buffer, const StatusPayload* input);
bool deserialize_StatusPayload(MicroBuffer* buffer, StatusPayload* output, AuxMemory* aux);

bool serialize_GetInfoPayload(MicroBuffer* buffer, const GetInfoPayload* input);
bool deserialize_GetInfoPayload(MicroBuffer* buffer, GetInfoPayload* output, AuxMemory* aux);

bool serialize_InfoPayload(MicroBuffer* buffer, const InfoPayload* input);
bool deserialize_InfoPayload(MicroBuffer* buffer, InfoPayload* output, AuxMemory* aux);

bool serialize_ReadDataPayload(MicroBuffer* buffer, const ReadDataPayload* input);
bool deserialize_ReadDataPayload(MicroBuffer* buffer, ReadDataPayload* output, AuxMemory* aux);

bool serialize_WriteDataPayload(MicroBuffer* buffer, const WriteDataPayload* input);
bool deserialize_WriteDataPayload(MicroBuffer* buffer, WriteDataPayload* output, AuxMemory* aux);

bool serialize_SampleDataPayloadData(MicroBuffer* buffer, const SampleDataPayloadData* input);
bool deserialize_SampleDataPayloadData(MicroBuffer* buffer, SampleDataPayloadData* output, AuxMemory* aux);

bool serialize_SamplePayloadData(MicroBuffer* buffer, const SamplePayloadData* input);
bool deserialize_SamplePayloadData(MicroBuffer* buffer, SamplePayloadData* output, AuxMemory* aux);

bool serialize_SampleDataSequencePayloadData(MicroBuffer* buffer, const SampleDataSequencePayloadData* input);
bool deserialize_SampleDataSequencePayloadData(MicroBuffer* buffer, SampleDataSequencePayloadData* output, AuxMemory* aux);

bool serialize_SampleSequencePayloadData(MicroBuffer* buffer, const SampleSequencePayloadData* input);
bool deserialize_SampleSequencePayloadData(MicroBuffer* buffer, SampleSequencePayloadData* output, AuxMemory* aux);

bool serialize_PackedSamplesPayloadData(MicroBuffer* buffer, const PackedSamplesPayloadData* input);
bool deserialize_PackedSamplesPayloadData(MicroBuffer* buffer, PackedSamplesPayloadData* output, AuxMemory* aux);


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
