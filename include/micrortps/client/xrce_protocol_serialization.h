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

#include <microcdr/microcdr.h>


bool serialize_Time_t(MicroBuffer* buffer, const Time_t* input);
bool deserialize_Time_t(MicroBuffer* buffer, Time_t* output);

bool serialize_String_t(MicroBuffer* buffer, const String_t* input);
bool deserialize_String_t(MicroBuffer* buffer, String_t* output);

bool serialize_BinarySequence_t(MicroBuffer* buffer, const BinarySequence_t* input);
bool deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output);

bool serialize_StringSequence_t(MicroBuffer* buffer, const StringSequence_t* input);
bool deserialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* output);

bool serialize_ClientKey(MicroBuffer* buffer, const ClientKey* input);
bool deserialize_ClientKey(MicroBuffer* buffer, ClientKey* output);

bool serialize_ObjectId(MicroBuffer* buffer, const ObjectId* input);
bool deserialize_ObjectId(MicroBuffer* buffer, ObjectId* output);

bool serialize_ObjectPrefix(MicroBuffer* buffer, const ObjectPrefix* input);
bool deserialize_ObjectPrefix(MicroBuffer* buffer, ObjectPrefix* output);

bool serialize_XrceCookie(MicroBuffer* buffer, const XrceCookie* input);
bool deserialize_XrceCookie(MicroBuffer* buffer, XrceCookie* output);

bool serialize_XrceVersion(MicroBuffer* buffer, const XrceVersion* input);
bool deserialize_XrceVersion(MicroBuffer* buffer, XrceVersion* output);

bool serialize_XrceVendorId(MicroBuffer* buffer, const XrceVendorId* input);
bool deserialize_XrceVendorId(MicroBuffer* buffer, XrceVendorId* output);

bool serialize_TransportLocatorSmall(MicroBuffer* buffer, const TransportLocatorSmall* input);
bool deserialize_TransportLocatorSmall(MicroBuffer* buffer, TransportLocatorSmall* output);

bool serialize_TransportLocatorMedium(MicroBuffer* buffer, const TransportLocatorMedium* input);
bool deserialize_TransportLocatorMedium(MicroBuffer* buffer, TransportLocatorMedium* output);

bool serialize_TransportLocatorLarge(MicroBuffer* buffer, const TransportLocatorLarge* input);
bool deserialize_TransportLocatorLarge(MicroBuffer* buffer, TransportLocatorLarge* output);

bool serialize_TransportLocatorString(MicroBuffer* buffer, const TransportLocatorString* input);
bool deserialize_TransportLocatorString(MicroBuffer* buffer, TransportLocatorString* output);

bool serialize_TransportLocator(MicroBuffer* buffer, const TransportLocator* input);
bool deserialize_TransportLocator(MicroBuffer* buffer, TransportLocator* output);

bool serialize_TransportLocatorSeq(MicroBuffer* buffer, const TransportLocatorSeq* input);
bool deserialize_TransportLocatorSeq(MicroBuffer* buffer, TransportLocatorSeq* output);

bool serialize_Property(MicroBuffer* buffer, const Property* input);
bool deserialize_Property(MicroBuffer* buffer, Property* output);

bool serialize_PropertySeq(MicroBuffer* buffer, const PropertySeq* input);
bool deserialize_PropertySeq(MicroBuffer* buffer, PropertySeq* output);

bool serialize_CLIENT_Representation(MicroBuffer* buffer, const CLIENT_Representation* input);
bool deserialize_CLIENT_Representation(MicroBuffer* buffer, CLIENT_Representation* output);

bool serialize_AGENT_Representation(MicroBuffer* buffer, const AGENT_Representation* input);
bool deserialize_AGENT_Representation(MicroBuffer* buffer, AGENT_Representation* output);

bool serialize_OBJK_Representation3Formats(MicroBuffer* buffer, const OBJK_Representation3Formats* input);
bool deserialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* output);

bool serialize_OBJK_RepresentationRefAndXMLFormats(MicroBuffer* buffer, const OBJK_RepresentationRefAndXMLFormats* input);
bool deserialize_OBJK_RepresentationRefAndXMLFormats(MicroBuffer* buffer, OBJK_RepresentationRefAndXMLFormats* output);

bool serialize_OBJK_RepresentationBinAndXMLFormats(MicroBuffer* buffer, const OBJK_RepresentationBinAndXMLFormats* input);
bool deserialize_OBJK_RepresentationBinAndXMLFormats(MicroBuffer* buffer, OBJK_RepresentationBinAndXMLFormats* output);

bool serialize_OBJK_RepresentationRefAndXML_Base(MicroBuffer* buffer, const OBJK_RepresentationRefAndXML_Base* input);
bool deserialize_OBJK_RepresentationRefAndXML_Base(MicroBuffer* buffer, OBJK_RepresentationRefAndXML_Base* output);

bool serialize_OBJK_RepresentationBinAndXML_Base(MicroBuffer* buffer, const OBJK_RepresentationBinAndXML_Base* input);
bool deserialize_OBJK_RepresentationBinAndXML_Base(MicroBuffer* buffer, OBJK_RepresentationBinAndXML_Base* output);

bool serialize_OBJK_Representation3_Base(MicroBuffer* buffer, const OBJK_Representation3_Base* input);
bool deserialize_OBJK_Representation3_Base(MicroBuffer* buffer, OBJK_Representation3_Base* output);

bool serialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, const OBJK_QOSPROFILE_Representation* input);
bool deserialize_OBJK_QOSPROFILE_Representation(MicroBuffer* buffer, OBJK_QOSPROFILE_Representation* output);

bool serialize_OBJK_TYPE_Representation(MicroBuffer* buffer, const OBJK_TYPE_Representation* input);
bool deserialize_OBJK_TYPE_Representation(MicroBuffer* buffer, OBJK_TYPE_Representation* output);

bool serialize_OBJK_DOMAIN_Representation(MicroBuffer* buffer, const OBJK_DOMAIN_Representation* input);
bool deserialize_OBJK_DOMAIN_Representation(MicroBuffer* buffer, OBJK_DOMAIN_Representation* output);

bool serialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, const OBJK_APPLICATION_Representation* input);
bool deserialize_OBJK_APPLICATION_Representation(MicroBuffer* buffer, OBJK_APPLICATION_Representation* output);

bool serialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, const OBJK_PUBLISHER_Representation* input);
bool deserialize_OBJK_PUBLISHER_Representation(MicroBuffer* buffer, OBJK_PUBLISHER_Representation* output);

bool serialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, const OBJK_SUBSCRIBER_Representation* input);
bool deserialize_OBJK_SUBSCRIBER_Representation(MicroBuffer* buffer, OBJK_SUBSCRIBER_Representation* output);

bool serialize_DATAWRITER_Representation(MicroBuffer* buffer, const DATAWRITER_Representation* input);
bool deserialize_DATAWRITER_Representation(MicroBuffer* buffer, DATAWRITER_Representation* output);

bool serialize_DATAREADER_Representation(MicroBuffer* buffer, const DATAREADER_Representation* input);
bool deserialize_DATAREADER_Representation(MicroBuffer* buffer, DATAREADER_Representation* output);

bool serialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, const OBJK_PARTICIPANT_Representation* input);
bool deserialize_OBJK_PARTICIPANT_Representation(MicroBuffer* buffer, OBJK_PARTICIPANT_Representation* output);

bool serialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, const OBJK_TOPIC_Representation* input);
bool deserialize_OBJK_TOPIC_Representation(MicroBuffer* buffer, OBJK_TOPIC_Representation* output);

bool serialize_OBJK_DomainParticipant_Binary(MicroBuffer* buffer, const OBJK_DomainParticipant_Binary* input);
bool deserialize_OBJK_DomainParticipant_Binary(MicroBuffer* buffer, OBJK_DomainParticipant_Binary* output);

bool serialize_OBJK_Topic_Binary(MicroBuffer* buffer, const OBJK_Topic_Binary* input);
bool deserialize_OBJK_Topic_Binary(MicroBuffer* buffer, OBJK_Topic_Binary* output);

bool serialize_OBJK_Publisher_Binary_Qos(MicroBuffer* buffer, const OBJK_Publisher_Binary_Qos* input);
bool deserialize_OBJK_Publisher_Binary_Qos(MicroBuffer* buffer, OBJK_Publisher_Binary_Qos* output);

bool serialize_OBJK_Publisher_Binary(MicroBuffer* buffer, const OBJK_Publisher_Binary* input);
bool deserialize_OBJK_Publisher_Binary(MicroBuffer* buffer, OBJK_Publisher_Binary* output);

bool serialize_OBJK_Subscriber_Binary_Qos(MicroBuffer* buffer, const OBJK_Subscriber_Binary_Qos* input);
bool deserialize_OBJK_Subscriber_Binary_Qos(MicroBuffer* buffer, OBJK_Subscriber_Binary_Qos* output);

bool serialize_OBJK_Subscriber_Binary(MicroBuffer* buffer, const OBJK_Subscriber_Binary* input);
bool deserialize_OBJK_Subscriber_Binary(MicroBuffer* buffer, OBJK_Subscriber_Binary* output);

bool serialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, const OBJK_Endpoint_QosBinary* input);
bool deserialize_OBJK_Endpoint_QosBinary(MicroBuffer* buffer, OBJK_Endpoint_QosBinary* output);

bool serialize_OBJK_DataWriter_Binary_Qos(MicroBuffer* buffer, const OBJK_DataWriter_Binary_Qos* input);
bool deserialize_OBJK_DataWriter_Binary_Qos(MicroBuffer* buffer, OBJK_DataWriter_Binary_Qos* output);

bool serialize_OBJK_DataReader_Binary_Qos(MicroBuffer* buffer, const OBJK_DataReader_Binary_Qos* input);
bool deserialize_OBJK_DataReader_Binary_Qos(MicroBuffer* buffer, OBJK_DataReader_Binary_Qos* output);

bool serialize_OBJK_DataReader_Binary(MicroBuffer* buffer, const OBJK_DataReader_Binary* input);
bool deserialize_OBJK_DataReader_Binary(MicroBuffer* buffer, OBJK_DataReader_Binary* output);

bool serialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, const OBJK_DataWriter_Binary* input);
bool deserialize_OBJK_DataWriter_Binary(MicroBuffer* buffer, OBJK_DataWriter_Binary* output);

bool serialize_ObjectVariant(MicroBuffer* buffer, const ObjectVariant* input);
bool deserialize_ObjectVariant(MicroBuffer* buffer, ObjectVariant* output);

bool serialize_CreationMode(MicroBuffer* buffer, const CreationMode* input);
bool deserialize_CreationMode(MicroBuffer* buffer, CreationMode* output);

bool serialize_RequestId(MicroBuffer* buffer, const RequestId* input);
bool deserialize_RequestId(MicroBuffer* buffer, RequestId* output);

bool serialize_ResultStatus(MicroBuffer* buffer, const ResultStatus* input);
bool deserialize_ResultStatus(MicroBuffer* buffer, ResultStatus* output);

bool serialize_BaseObjectRequest(MicroBuffer* buffer, const BaseObjectRequest* input);
bool deserialize_BaseObjectRequest(MicroBuffer* buffer, BaseObjectRequest* output);

bool serialize_AGENT_ActivityInfo(MicroBuffer* buffer, const AGENT_ActivityInfo* input);
bool deserialize_AGENT_ActivityInfo(MicroBuffer* buffer, AGENT_ActivityInfo* output);

bool serialize_DATAREADER_ActivityInfo(MicroBuffer* buffer, const DATAREADER_ActivityInfo* input);
bool deserialize_DATAREADER_ActivityInfo(MicroBuffer* buffer, DATAREADER_ActivityInfo* output);

bool serialize_DATAWRITER_ActivityInfo(MicroBuffer* buffer, const DATAWRITER_ActivityInfo* input);
bool deserialize_DATAWRITER_ActivityInfo(MicroBuffer* buffer, DATAWRITER_ActivityInfo* output);

bool serialize_ActivityInfoVariant(MicroBuffer* buffer, const ActivityInfoVariant* input);
bool deserialize_ActivityInfoVariant(MicroBuffer* buffer, ActivityInfoVariant* output);

bool serialize_ObjectInfo(MicroBuffer* buffer, const ObjectInfo* input);
bool deserialize_ObjectInfo(MicroBuffer* buffer, ObjectInfo* output);

bool serialize_BaseObjectReply(MicroBuffer* buffer, const BaseObjectReply* input);
bool deserialize_BaseObjectReply(MicroBuffer* buffer, BaseObjectReply* output);

bool serialize_DataDeliveryControl(MicroBuffer* buffer, const DataDeliveryControl* input);
bool deserialize_DataDeliveryControl(MicroBuffer* buffer, DataDeliveryControl* output);

bool serialize_ReadSpecification(MicroBuffer* buffer, const ReadSpecification* input);
bool deserialize_ReadSpecification(MicroBuffer* buffer, ReadSpecification* output);

bool serialize_SeqNumberAndTimestamp(MicroBuffer* buffer, const SeqNumberAndTimestamp* input);
bool deserialize_SeqNumberAndTimestamp(MicroBuffer* buffer, SeqNumberAndTimestamp* output);

bool serialize_SampleInfoDetail(MicroBuffer* buffer, const SampleInfoDetail* input);
bool deserialize_SampleInfoDetail(MicroBuffer* buffer, SampleInfoDetail* output);

bool serialize_SampleInfo(MicroBuffer* buffer, const SampleInfo* input);
bool deserialize_SampleInfo(MicroBuffer* buffer, SampleInfo* output);

bool serialize_SampleInfoDelta(MicroBuffer* buffer, const SampleInfoDelta* input);
bool deserialize_SampleInfoDelta(MicroBuffer* buffer, SampleInfoDelta* output);

bool serialize_SampleData(MicroBuffer* buffer, const SampleData* input);
bool deserialize_SampleData(MicroBuffer* buffer, SampleData* output);

bool serialize_SampleDataSeq(MicroBuffer* buffer, const SampleDataSeq* input);
bool deserialize_SampleDataSeq(MicroBuffer* buffer, SampleDataSeq* output);

bool serialize_Sample(MicroBuffer* buffer, const Sample* input);
bool deserialize_Sample(MicroBuffer* buffer, Sample* output);

bool serialize_SampleSeq(MicroBuffer* buffer, const SampleSeq* input);
bool deserialize_SampleSeq(MicroBuffer* buffer, SampleSeq* output);

bool serialize_SampleDelta(MicroBuffer* buffer, const SampleDelta* input);
bool deserialize_SampleDelta(MicroBuffer* buffer, SampleDelta* output);

bool serialize_SampleDeltaSequence(MicroBuffer* buffer, const SampleDeltaSequence* input);
bool deserialize_SampleDeltaSequence(MicroBuffer* buffer, SampleDeltaSequence* output);

bool serialize_PackedSamples(MicroBuffer* buffer, const PackedSamples* input);
bool deserialize_PackedSamples(MicroBuffer* buffer, PackedSamples* output);

bool serialize_SamplePackedSeq(MicroBuffer* buffer, const SamplePackedSeq* input);
bool deserialize_SamplePackedSeq(MicroBuffer* buffer, SamplePackedSeq* output);

bool serialize_DataRepresentation(MicroBuffer* buffer, const DataRepresentation* input);
bool deserialize_DataRepresentation(MicroBuffer* buffer, DataRepresentation* output);

bool serialize_CREATE_CLIENT_Payload(MicroBuffer* buffer, const CREATE_CLIENT_Payload* input);
bool deserialize_CREATE_CLIENT_Payload(MicroBuffer* buffer, CREATE_CLIENT_Payload* output);

bool serialize_CREATE_Payload(MicroBuffer* buffer, const CREATE_Payload* input);
bool deserialize_CREATE_Payload(MicroBuffer* buffer, CREATE_Payload* output);

bool serialize_GET_INFO_Payload(MicroBuffer* buffer, const GET_INFO_Payload* input);
bool deserialize_GET_INFO_Payload(MicroBuffer* buffer, GET_INFO_Payload* output);

bool serialize_DELETE_Payload(MicroBuffer* buffer, const DELETE_Payload* input);
bool deserialize_DELETE_Payload(MicroBuffer* buffer, DELETE_Payload* output);

bool serialize_STATUS_AGENT_Payload(MicroBuffer* buffer, const STATUS_AGENT_Payload* input);
bool deserialize_STATUS_AGENT_Payload(MicroBuffer* buffer, STATUS_AGENT_Payload* output);

bool serialize_STATUS_Payload(MicroBuffer* buffer, const STATUS_Payload* input);
bool deserialize_STATUS_Payload(MicroBuffer* buffer, STATUS_Payload* output);

bool serialize_INFO_Payload(MicroBuffer* buffer, const INFO_Payload* input);
bool deserialize_INFO_Payload(MicroBuffer* buffer, INFO_Payload* output);

bool serialize_READ_DATA_Payload(MicroBuffer* buffer, const READ_DATA_Payload* input);
bool deserialize_READ_DATA_Payload(MicroBuffer* buffer, READ_DATA_Payload* output);

bool serialize_WRITE_DATA_Payload_Data(MicroBuffer* buffer, const WRITE_DATA_Payload_Data* input);
bool deserialize_WRITE_DATA_Payload_Data(MicroBuffer* buffer, WRITE_DATA_Payload_Data* output);

bool serialize_WRITE_DATA_Payload_Sample(MicroBuffer* buffer, const WRITE_DATA_Payload_Sample* input);
bool deserialize_WRITE_DATA_Payload_Sample(MicroBuffer* buffer, WRITE_DATA_Payload_Sample* output);

bool serialize_WRITE_DATA_Payload_DataSeq(MicroBuffer* buffer, const WRITE_DATA_Payload_DataSeq* input);
bool deserialize_WRITE_DATA_Payload_DataSeq(MicroBuffer* buffer, WRITE_DATA_Payload_DataSeq* output);

bool serialize_WRITE_DATA_Payload_SampleSeq(MicroBuffer* buffer, const WRITE_DATA_Payload_SampleSeq* input);
bool deserialize_WRITE_DATA_Payload_SampleSeq(MicroBuffer* buffer, WRITE_DATA_Payload_SampleSeq* output);

bool serialize_WRITE_DATA_Payload_PackedSamples(MicroBuffer* buffer, const WRITE_DATA_Payload_PackedSamples* input);
bool deserialize_WRITE_DATA_Payload_PackedSamples(MicroBuffer* buffer, WRITE_DATA_Payload_PackedSamples* output);

bool serialize_DATA_Payload_Data(MicroBuffer* buffer, const DATA_Payload_Data* input);
bool deserialize_DATA_Payload_Data(MicroBuffer* buffer, DATA_Payload_Data* output);

bool serialize_DATA_Payload_Sample(MicroBuffer* buffer, const DATA_Payload_Sample* input);
bool deserialize_DATA_Payload_Sample(MicroBuffer* buffer, DATA_Payload_Sample* output);

bool serialize_DATA_Payload_DataSeq(MicroBuffer* buffer, const DATA_Payload_DataSeq* input);
bool deserialize_DATA_Payload_DataSeq(MicroBuffer* buffer, DATA_Payload_DataSeq* output);

bool serialize_DATA_Payload_SampleSeq(MicroBuffer* buffer, const DATA_Payload_SampleSeq* input);
bool deserialize_DATA_Payload_SampleSeq(MicroBuffer* buffer, DATA_Payload_SampleSeq* output);

bool serialize_DATA_Payload_PackedSamples(MicroBuffer* buffer, const DATA_Payload_PackedSamples* input);
bool deserialize_DATA_Payload_PackedSamples(MicroBuffer* buffer, DATA_Payload_PackedSamples* output);

bool serialize_ACKNACK_Payload(MicroBuffer* buffer, const ACKNACK_Payload* input);
bool deserialize_ACKNACK_Payload(MicroBuffer* buffer, ACKNACK_Payload* output);

bool serialize_HEARTBEAT_Payload(MicroBuffer* buffer, const HEARTBEAT_Payload* input);
bool deserialize_HEARTBEAT_Payload(MicroBuffer* buffer, HEARTBEAT_Payload* output);

bool serialize_MessageHeader(MicroBuffer* buffer, const MessageHeader* input);
bool deserialize_MessageHeader(MicroBuffer* buffer, MessageHeader* output);

bool serialize_SubmessageHeader(MicroBuffer* buffer, const SubmessageHeader* input);
bool deserialize_SubmessageHeader(MicroBuffer* buffer, SubmessageHeader* output);


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
