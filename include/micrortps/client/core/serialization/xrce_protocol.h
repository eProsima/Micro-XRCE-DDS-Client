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

#ifndef _MICRORTPS_CLIENT_CORE_SERIALIZATION_XRCE_PROTOCOL_H_
#define _MICRORTPS_CLIENT_CORE_SERIALIZATION_XRCE_PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microcdr/microcdr.h>
#include <stdint.h>
#include <stdbool.h>

#define STRING_SIZE_MAX                512
#define SAMPLE_DATA_SIZE_MAX           512
#define STRING_SEQUENCE_MAX            8
#define BINARY_SEQUENCE_MAX            8
#define SAMPLE_SEQUENCE_MAX            8
#define SAMPLE_DATA_SEQUENCE_MAX       8
#define SAMPLE_DELTA_SEQUENCE_MAX      8
#define PACKED_SAMPLES_SEQUENCE_MAX    8
#define TRANSPORT_LOCATOR_SEQUENCE_MAX 8
#define PROPERTY_SEQUENCE_MAX          8

typedef struct Time_t
{
    int32_t seconds;
    uint32_t nanoseconds;

} Time_t;

typedef struct BinarySequence_t
{
    uint32_t size;
    uint8_t data[BINARY_SEQUENCE_MAX];

} BinarySequence_t;


typedef struct StringSequence_t
{
    uint32_t size;
    char* data[STRING_SEQUENCE_MAX];

} StringSequence_t;


typedef struct ClientKey
{
    uint8_t data[4];

} ClientKey;
#define CLIENT_INVALID (ClientKey){{0x00, 0x00, 0x00, 0x00}}

typedef uint8_t ObjectKind;
#define OBJK_INVALID 0x00
#define OBJK_PARTICIPANT 0x01
#define OBJK_TOPIC 0x02
#define OBJK_PUBLISHER 0x03
#define OBJK_SUBSCRIBER 0x04
#define OBJK_DATAWRITER 0x05
#define OBJK_DATAREADER 0x06
#define OBJK_TYPE 0x0A
#define OBJK_QOSPROFILE 0x0B
#define OBJK_APPLICATION 0x0C
#define OBJK_AGENT 0x0D
#define OBJK_CLIENT 0x0E
#define OBJK_OTHER 0x0F


typedef struct ObjectId
{
    uint8_t data[2];

} ObjectId;


typedef struct ObjectPrefix
{
    uint8_t data[2];

} ObjectPrefix;
#define OBJECTID_INVALID (ObjectId){{0x00, 0x00}}
#define OBJECTID_AGENT (ObjectId){{0xFF, 0xFD}}
#define OBJECTID_CLIENT (ObjectId){{0xFF, 0xFE}}
#define OBJECTID_SESSION (ObjectId){{0xFF, 0xFF}}


typedef struct XrceCookie
{
    uint8_t data[4];

} XrceCookie;
#define XRCE_COOKIE (XrceCookie){{0x58, 0x52, 0x43, 0x45}}


typedef struct XrceVersion
{
    uint8_t data[2];

} XrceVersion;
#define XRCE_VERSION_MAJOR 0x01
#define XRCE_VERSION_MINOR 0x00
#define XRCE_VERSION (XrceVersion){{XRCE_VERSION_MAJOR, XRCE_VERSION_MINOR}}


typedef struct XrceVendorId
{
    uint8_t data[2];

} XrceVendorId;
#define XRCE_VENDOR_INVALID {0x00, 0x00}

typedef enum TransportLocatorFormat
{
    ADDRESS_FORMAT_SMALL = 0,
    ADDRESS_FORMAT_MEDIUM = 1,
    ADDRESS_FORMAT_LARGE = 2,
    ADDRESS_FORMAT_STRING = 3

} TransportLocatorFormat;


typedef struct TransportLocatorSmall
{
    uint8_t address[2];
    uint8_t locator_port;

} TransportLocatorSmall;


typedef struct TransportLocatorMedium
{
    uint8_t address[4];
    uint8_t locator_port;

} TransportLocatorMedium;


typedef struct TransportLocatorLarge
{
    uint8_t address[16];
    uint8_t locator_port;

} TransportLocatorLarge;


typedef struct TransportLocatorString
{
    char* value;

} TransportLocatorString;


typedef union TransportLocatorU
{
    TransportLocatorSmall small_locator;
    TransportLocatorMedium medium_locator;
    TransportLocatorLarge large_locator;
    TransportLocatorString string_locator;

} TransportLocatorU;


typedef struct TransportLocator
{
    uint8_t format;
    TransportLocatorU _;

} TransportLocator;


typedef struct TransportLocatorSeq
{
    uint32_t size;
    TransportLocator data[TRANSPORT_LOCATOR_SEQUENCE_MAX];

} TransportLocatorSeq;


typedef struct Property
{
    char* name;
    char* value;

} Property;


typedef struct PropertySeq
{
    uint32_t size;
    Property* data;

} PropertySeq;


typedef struct CLIENT_Representation
{
    XrceCookie xrce_cookie;
    XrceVersion xrce_version;
    XrceVendorId xrce_vendor_id;
    Time_t client_timestamp;
    ClientKey client_key;
    uint8_t session_id;
    bool optional_properties;
    PropertySeq properties;

} CLIENT_Representation;


typedef struct AGENT_Representation
{
    XrceCookie xrce_cookie;
    XrceVersion xrce_version;
    XrceVendorId xrce_vendor_id;
    Time_t agent_timestamp;
    bool optional_properties;
    PropertySeq properties;

} AGENT_Representation;

typedef uint8_t RepresentationFormat;
#define REPRESENTATION_BY_REFERENCE 0x01
#define REPRESENTATION_AS_XML_STRING 0x02
#define REPRESENTATION_IN_BINARY 0x03
#define REFERENCE_MAX_LEN 128


typedef union OBJK_Representation3FormatsU
{
    char* object_reference;
    char* xml_string_represenatation;
    BinarySequence_t binary_representation;

} OBJK_Representation3FormatsU;


typedef struct OBJK_Representation3Formats
{
    uint8_t format;
    OBJK_Representation3FormatsU _;

} OBJK_Representation3Formats;


typedef union OBJK_RepresentationRefAndXMLFormatsU
{
    char* object_name;
    char* xml_string_represenatation;

} OBJK_RepresentationRefAndXMLFormatsU;


typedef struct OBJK_RepresentationRefAndXMLFormats
{
    uint8_t format;
    OBJK_RepresentationRefAndXMLFormatsU _;

} OBJK_RepresentationRefAndXMLFormats;


typedef union OBJK_RepresentationBinAndXMLFormatsU
{
    BinarySequence_t binary_representation;
    char* string_represenatation;

} OBJK_RepresentationBinAndXMLFormatsU;


typedef struct OBJK_RepresentationBinAndXMLFormats
{
    uint8_t format;
    OBJK_RepresentationBinAndXMLFormatsU _;

} OBJK_RepresentationBinAndXMLFormats;


typedef struct OBJK_RepresentationRefAndXML_Base
{
    OBJK_RepresentationRefAndXMLFormats representation;

} OBJK_RepresentationRefAndXML_Base;


typedef struct OBJK_RepresentationBinAndXML_Base
{
    OBJK_RepresentationBinAndXMLFormats representation;

} OBJK_RepresentationBinAndXML_Base;


typedef struct OBJK_Representation3_Base
{
    OBJK_Representation3Formats representation;

} OBJK_Representation3_Base;


typedef struct OBJK_QOSPROFILE_Representation
{
    OBJK_RepresentationRefAndXML_Base base;

} OBJK_QOSPROFILE_Representation;


typedef struct OBJK_TYPE_Representation
{
    OBJK_RepresentationRefAndXML_Base base;

} OBJK_TYPE_Representation;


typedef struct OBJK_DOMAIN_Representation
{
    OBJK_RepresentationRefAndXML_Base base;

} OBJK_DOMAIN_Representation;


typedef struct OBJK_APPLICATION_Representation
{
    OBJK_RepresentationRefAndXML_Base base;

} OBJK_APPLICATION_Representation;


typedef struct OBJK_PUBLISHER_Representation
{
    OBJK_RepresentationBinAndXML_Base base;
    ObjectId participant_id;

} OBJK_PUBLISHER_Representation;


typedef struct OBJK_SUBSCRIBER_Representation
{
    OBJK_RepresentationBinAndXML_Base base;
    ObjectId participant_id;

} OBJK_SUBSCRIBER_Representation;


typedef struct DATAWRITER_Representation
{
    OBJK_Representation3_Base base;
    ObjectId publisher_id;

} DATAWRITER_Representation;


typedef struct DATAREADER_Representation
{
    OBJK_Representation3_Base base;
    ObjectId subscriber_id;

} DATAREADER_Representation;


typedef struct OBJK_PARTICIPANT_Representation
{
    OBJK_Representation3_Base base;
    int16_t domain_id;

} OBJK_PARTICIPANT_Representation;


typedef struct OBJK_TOPIC_Representation
{
    OBJK_Representation3_Base base;
    ObjectId participant_id;

} OBJK_TOPIC_Representation;


typedef struct OBJK_DomainParticipant_Binary
{
    bool optional_domain_reference;
    char* domain_reference;
    bool optional_qos_profile_reference;
    char* qos_profile_reference;

} OBJK_DomainParticipant_Binary;


typedef struct OBJK_Topic_Binary
{
    char* topic_name;
    bool optional_type_reference;
    char* type_reference;
    bool optional_type_name;
    char* type_name;

} OBJK_Topic_Binary;


typedef struct OBJK_Publisher_Binary_Qos
{
    bool optional_partitions;
    StringSequence_t partitions;
    bool optional_group_data;
    BinarySequence_t group_data;

} OBJK_Publisher_Binary_Qos;


typedef struct OBJK_Publisher_Binary
{
    bool optional_publisher_name;
    char* publisher_name;
    bool optional_qos;
    OBJK_Publisher_Binary_Qos qos;

} OBJK_Publisher_Binary;


typedef struct OBJK_Subscriber_Binary_Qos
{
    bool optional_partitions;
    StringSequence_t partitions;
    bool optional_group_data;
    BinarySequence_t group_data;

} OBJK_Subscriber_Binary_Qos;


typedef struct OBJK_Subscriber_Binary
{
    bool optional_subscriber_name;
    char* subscriber_name;
    bool optional_qos;
    OBJK_Subscriber_Binary_Qos qos;

} OBJK_Subscriber_Binary;


typedef enum EndpointQosFlags
{
    is_reliabel = 0x01 << 0,
    is_history_keep_last = 0x01 << 1,
    is_ownership_exclusive = 0x01 << 2,
    is_durability_transient_local = 0x01 << 3,
    is_durability_transient = 0x01 << 4,
    is_durability_persistent = 0x01 << 5

} EndpointQosFlags;


typedef struct OBJK_Endpoint_QosBinary
{
    uint16_t qos_flags;
    bool optional_history_depth;
    uint16_t history_depth;
    bool optional_deadline_msec;
    uint32_t deadline_msec;
    bool optional_lifespan_msec;
    uint32_t lifespan_msec;
    bool optional_user_data;
    BinarySequence_t user_data;

} OBJK_Endpoint_QosBinary;


typedef struct OBJK_DataWriter_Binary_Qos
{
    OBJK_Endpoint_QosBinary base;
    bool optional_ownership_strength;
    uint64_t ownership_strength;

} OBJK_DataWriter_Binary_Qos;


typedef struct OBJK_DataReader_Binary_Qos
{
    OBJK_Endpoint_QosBinary base;
    bool optional_timebasedfilter_msec;
    uint64_t timebasedfilter_msec;
    bool optional_contentbased_filter;
    char* contentbased_filter;

} OBJK_DataReader_Binary_Qos;


typedef struct OBJK_DataReader_Binary
{
    char* topic_name;
    bool optional_qos;
    OBJK_DataReader_Binary_Qos qos;

} OBJK_DataReader_Binary;


typedef struct OBJK_DataWriter_Binary
{
    char* topic_name;
    bool optional_qos;
    OBJK_DataWriter_Binary_Qos qos;

} OBJK_DataWriter_Binary;


typedef union ObjectVariantU
{
    AGENT_Representation agent;
    CLIENT_Representation client;
    OBJK_APPLICATION_Representation application;
    OBJK_PARTICIPANT_Representation participant;
    OBJK_QOSPROFILE_Representation qos_profile;
    OBJK_TYPE_Representation type;
    OBJK_TOPIC_Representation topic;
    OBJK_PUBLISHER_Representation publisher;
    OBJK_SUBSCRIBER_Representation subscriber;
    DATAWRITER_Representation data_writer;
    DATAREADER_Representation data_reader;

} ObjectVariantU;


typedef struct ObjectVariant
{
    uint8_t kind;
    ObjectVariantU _;

} ObjectVariant;


typedef struct CreationMode
{
    bool reuse;
    bool replace;

} CreationMode;


typedef struct RequestId
{
    uint8_t data[2];

} RequestId;


typedef struct ResultStatus
{
    uint8_t status;
    uint8_t implementation_status;

} ResultStatus;


typedef struct BaseObjectRequest
{
    RequestId request_id;
    ObjectId object_id;

} BaseObjectRequest;

typedef BaseObjectRequest RelatedObjectRequest;
#define STATUS_LAST_OP_NONE 0x00
#define STATUS_LAST_OP_CREATE 0x01
#define STATUS_LAST_OP_UPDATE 0x02
#define STATUS_LAST_OP_DELETE 0x03
#define STATUS_LAST_OP_LOOKUP 0x04
#define STATUS_LAST_OP_READ 0x05
#define STATUS_LAST_OP_WRITE 0x06


typedef enum InfoMask
{
    INFO_CONFIGURATION = 0x01 << 0,
    INFO_ACTIVITY = 0x01 << 1

} InfoMask;


typedef struct AGENT_ActivityInfo
{
    int16_t availibility;
    TransportLocatorSeq address_seq;

} AGENT_ActivityInfo;


typedef struct DATAREADER_ActivityInfo
{
    int16_t highest_acked_num;

} DATAREADER_ActivityInfo;


typedef struct DATAWRITER_ActivityInfo
{
    int16_t stream_seq_num;
    uint64_t sample_seq_num;

} DATAWRITER_ActivityInfo;


typedef union ActivityInfoVariantU
{
    AGENT_ActivityInfo agent;
    DATAWRITER_ActivityInfo data_writer;
    DATAREADER_ActivityInfo data_reader;

} ActivityInfoVariantU;


typedef struct ActivityInfoVariant
{
    uint8_t kind;
    ActivityInfoVariantU _;

} ActivityInfoVariant;


typedef struct ObjectInfo
{
    bool optional_config;
    ObjectVariant config;
    bool optional_activity;
    ActivityInfoVariant activity;

} ObjectInfo;


typedef struct BaseObjectReply
{
    BaseObjectRequest related_request;
    ResultStatus result;

} BaseObjectReply;


typedef enum DataFormat
{
    FORMAT_DATA = 0x00,
    FORMAT_SAMPLE = 0x02,
    FORMAT_DATA_SEQ = 0x08,
    FORMAT_SAMPLE_SEQ = 0x0A,
    FORMAT_PACKED_SAMPLES = 0x0E,
    FORMAT_MASK = 0x0E

} DataFormat;


typedef struct DataDeliveryControl
{
    uint16_t max_samples;
    uint16_t max_elapsed_time;
    uint16_t max_bytes_per_seconds;
    uint16_t min_pace_period;

} DataDeliveryControl;


typedef struct ReadSpecification
{
    uint8_t input_stream_id;
    uint8_t data_format;
    bool optional_content_filter_expression;
    char* content_filter_expression;
    bool optional_delivery_control;
    DataDeliveryControl delivery_control;

} ReadSpecification;


typedef enum SampleInfoFlags
{
    INSTANCE_STATE_UNREGISTERD = 0x01 << 0,
    INSTANCE_STATE_DISPOSED = 0x01 << 1,
    VIEW_STATE_NEW = 0x01 << 2,
    SAMPLE_STATE_READ = 0x01 << 3

} SampleInfoFlags;


typedef enum SampleInfoFormat
{
    FORMAT_EMPTY = 0x00,
    FORMAT_SEQNUM = 0x01,
    FORMAT_TIMESTAMP = 0x02,
    FORMAT_SEQN_TIMS = 0x03

} SampleInfoFormat;


typedef struct SeqNumberAndTimestamp
{
    uint32_t sequence_number;
    uint32_t session_time_offset;

} SeqNumberAndTimestamp;


typedef union SampleInfoDetailU
{
    uint32_t sequence_number;
    uint32_t session_time_offset;
    SeqNumberAndTimestamp seqnum_n_timestamp;

} SampleInfoDetailU;


typedef struct SampleInfoDetail
{
    uint32_t format;
    SampleInfoDetailU _;

} SampleInfoDetail;


typedef struct SampleInfo
{
    uint8_t state;
    SampleInfoDetail detail;

} SampleInfo;

typedef uint16_t DeciSecond;


typedef struct SampleInfoDelta
{
    uint8_t state;
    uint8_t seq_number_delta;
    uint16_t timestamp_delta;

} SampleInfoDelta;


typedef struct SampleData
{
    uint32_t size;
    uint8_t data[SAMPLE_DATA_SIZE_MAX];

} SampleData;


typedef struct SampleDataSeq
{
    uint32_t size;
    SampleData data[SAMPLE_DATA_SEQUENCE_MAX];

} SampleDataSeq;


typedef struct Sample
{
    SampleInfo info;
    SampleData data;

} Sample;


typedef struct SampleSeq
{
    uint32_t size;
    Sample data[SAMPLE_SEQUENCE_MAX];

} SampleSeq;


typedef struct SampleDelta
{
    SampleInfoDelta info_delta;
    SampleData data;

} SampleDelta;


typedef struct SampleDeltaSequence
{
    uint32_t size;
    SampleDelta data[SAMPLE_DELTA_SEQUENCE_MAX];

} SampleDeltaSequence;


typedef struct PackedSamples
{
    SampleInfo info_base;
    SampleDeltaSequence sample_delta_seq;

} PackedSamples;


typedef struct SamplePackedSeq
{
    uint32_t size;
    PackedSamples data[PACKED_SAMPLES_SEQUENCE_MAX];

} SamplePackedSeq;


typedef union DataRepresentationU
{
    SampleData data;
    Sample sample;
    SampleDataSeq data_seq;
    SampleSeq sample_seq;
    PackedSamples packed_samples;

} DataRepresentationU;


typedef struct DataRepresentation
{
    uint8_t format;
    DataRepresentationU _;

} DataRepresentation;


typedef struct CREATE_CLIENT_Payload
{
    BaseObjectRequest base;
    CLIENT_Representation client_representation;

} CREATE_CLIENT_Payload;


typedef struct CREATE_Payload
{
    BaseObjectRequest base;
    ObjectVariant object_representation;

} CREATE_Payload;


typedef struct GET_INFO_Payload
{
    BaseObjectRequest base;
    uint32_t info_mask;

} GET_INFO_Payload;


typedef struct DELETE_Payload
{
    BaseObjectRequest base;

} DELETE_Payload;


typedef struct STATUS_AGENT_Payload
{
    BaseObjectReply base;
    AGENT_Representation agent_info;

} STATUS_AGENT_Payload;


typedef struct STATUS_Payload
{
    BaseObjectReply base;

} STATUS_Payload;


typedef struct INFO_Payload
{
    BaseObjectReply base;
    ObjectInfo object_info;

} INFO_Payload;


typedef struct READ_DATA_Payload
{
    BaseObjectRequest base;
    ReadSpecification read_specification;

} READ_DATA_Payload;


typedef struct WRITE_DATA_Payload_Data
{
    BaseObjectRequest base;

} WRITE_DATA_Payload_Data;


typedef struct WRITE_DATA_Payload_Sample
{
    BaseObjectRequest base;
    Sample sample;

} WRITE_DATA_Payload_Sample;


typedef struct WRITE_DATA_Payload_DataSeq
{
    BaseObjectRequest base;
    SampleDataSeq data_seq;

} WRITE_DATA_Payload_DataSeq;


typedef struct WRITE_DATA_Payload_SampleSeq
{
    BaseObjectRequest base;
    SampleSeq sample_seq;

} WRITE_DATA_Payload_SampleSeq;


typedef struct WRITE_DATA_Payload_PackedSamples
{
    BaseObjectRequest base;
    PackedSamples packed_samples;

} WRITE_DATA_Payload_PackedSamples;


typedef struct DATA_Payload_Data
{
    BaseObjectRequest base;
    SampleData data;

} DATA_Payload_Data;


typedef struct DATA_Payload_Sample
{
    BaseObjectRequest base;
    Sample sample;

} DATA_Payload_Sample;


typedef struct DATA_Payload_DataSeq
{
    BaseObjectRequest base;
    SampleDataSeq data_seq;

} DATA_Payload_DataSeq;


typedef struct DATA_Payload_SampleSeq
{
    BaseObjectRequest base;
    SampleSeq sample_seq;

} DATA_Payload_SampleSeq;


typedef struct DATA_Payload_PackedSamples
{
    BaseObjectRequest base;
    PackedSamples packed_samples;

} DATA_Payload_PackedSamples;


typedef struct ACKNACK_Payload
{
    uint16_t first_unacked_seq_num;
    uint8_t nack_bitmap[2];

} ACKNACK_Payload;

typedef struct HEARTBEAT_Payload
{
    uint16_t first_unacked_seq_nr;
    uint16_t last_unacked_seq_nr;

} HEARTBEAT_Payload;

bool serialize_Time_t(mcBuffer* buffer, const Time_t* input);
bool deserialize_Time_t(mcBuffer* buffer, Time_t* output);

bool serialize_BinarySequence_t(mcBuffer* buffer, const BinarySequence_t* input);
bool deserialize_BinarySequence_t(mcBuffer* buffer, BinarySequence_t* output);

bool serialize_StringSequence_t(mcBuffer* buffer, const StringSequence_t* input);
bool deserialize_StringSequence_t(mcBuffer* buffer, StringSequence_t* output);

bool serialize_ClientKey(mcBuffer* buffer, const ClientKey* input);
bool deserialize_ClientKey(mcBuffer* buffer, ClientKey* output);

bool serialize_ObjectId(mcBuffer* buffer, const ObjectId* input);
bool deserialize_ObjectId(mcBuffer* buffer, ObjectId* output);

bool serialize_ObjectPrefix(mcBuffer* buffer, const ObjectPrefix* input);
bool deserialize_ObjectPrefix(mcBuffer* buffer, ObjectPrefix* output);

bool serialize_XrceCookie(mcBuffer* buffer, const XrceCookie* input);
bool deserialize_XrceCookie(mcBuffer* buffer, XrceCookie* output);

bool serialize_XrceVersion(mcBuffer* buffer, const XrceVersion* input);
bool deserialize_XrceVersion(mcBuffer* buffer, XrceVersion* output);

bool serialize_XrceVendorId(mcBuffer* buffer, const XrceVendorId* input);
bool deserialize_XrceVendorId(mcBuffer* buffer, XrceVendorId* output);

bool serialize_TransportLocatorSmall(mcBuffer* buffer, const TransportLocatorSmall* input);
bool deserialize_TransportLocatorSmall(mcBuffer* buffer, TransportLocatorSmall* output);

bool serialize_TransportLocatorMedium(mcBuffer* buffer, const TransportLocatorMedium* input);
bool deserialize_TransportLocatorMedium(mcBuffer* buffer, TransportLocatorMedium* output);

bool serialize_TransportLocatorLarge(mcBuffer* buffer, const TransportLocatorLarge* input);
bool deserialize_TransportLocatorLarge(mcBuffer* buffer, TransportLocatorLarge* output);

bool serialize_TransportLocatorString(mcBuffer* buffer, const TransportLocatorString* input);
bool deserialize_TransportLocatorString(mcBuffer* buffer, TransportLocatorString* output);

bool serialize_TransportLocator(mcBuffer* buffer, const TransportLocator* input);
bool deserialize_TransportLocator(mcBuffer* buffer, TransportLocator* output);

bool serialize_TransportLocatorSeq(mcBuffer* buffer, const TransportLocatorSeq* input);
bool deserialize_TransportLocatorSeq(mcBuffer* buffer, TransportLocatorSeq* output);

bool serialize_Property(mcBuffer* buffer, const Property* input);
bool deserialize_Property(mcBuffer* buffer, Property* output);

bool serialize_PropertySeq(mcBuffer* buffer, const PropertySeq* input);
bool deserialize_PropertySeq(mcBuffer* buffer, PropertySeq* output);

bool serialize_CLIENT_Representation(mcBuffer* buffer, const CLIENT_Representation* input);
bool deserialize_CLIENT_Representation(mcBuffer* buffer, CLIENT_Representation* output);

bool serialize_AGENT_Representation(mcBuffer* buffer, const AGENT_Representation* input);
bool deserialize_AGENT_Representation(mcBuffer* buffer, AGENT_Representation* output);

bool serialize_OBJK_Representation3Formats(mcBuffer* buffer, const OBJK_Representation3Formats* input);
bool deserialize_OBJK_Representation3Formats(mcBuffer* buffer, OBJK_Representation3Formats* output);

bool serialize_OBJK_RepresentationRefAndXMLFormats(mcBuffer* buffer, const OBJK_RepresentationRefAndXMLFormats* input);
bool deserialize_OBJK_RepresentationRefAndXMLFormats(mcBuffer* buffer, OBJK_RepresentationRefAndXMLFormats* output);

bool serialize_OBJK_RepresentationBinAndXMLFormats(mcBuffer* buffer, const OBJK_RepresentationBinAndXMLFormats* input);
bool deserialize_OBJK_RepresentationBinAndXMLFormats(mcBuffer* buffer, OBJK_RepresentationBinAndXMLFormats* output);

bool serialize_OBJK_RepresentationRefAndXML_Base(mcBuffer* buffer, const OBJK_RepresentationRefAndXML_Base* input);
bool deserialize_OBJK_RepresentationRefAndXML_Base(mcBuffer* buffer, OBJK_RepresentationRefAndXML_Base* output);

bool serialize_OBJK_RepresentationBinAndXML_Base(mcBuffer* buffer, const OBJK_RepresentationBinAndXML_Base* input);
bool deserialize_OBJK_RepresentationBinAndXML_Base(mcBuffer* buffer, OBJK_RepresentationBinAndXML_Base* output);

bool serialize_OBJK_Representation3_Base(mcBuffer* buffer, const OBJK_Representation3_Base* input);
bool deserialize_OBJK_Representation3_Base(mcBuffer* buffer, OBJK_Representation3_Base* output);

bool serialize_OBJK_QOSPROFILE_Representation(mcBuffer* buffer, const OBJK_QOSPROFILE_Representation* input);
bool deserialize_OBJK_QOSPROFILE_Representation(mcBuffer* buffer, OBJK_QOSPROFILE_Representation* output);

bool serialize_OBJK_TYPE_Representation(mcBuffer* buffer, const OBJK_TYPE_Representation* input);
bool deserialize_OBJK_TYPE_Representation(mcBuffer* buffer, OBJK_TYPE_Representation* output);

bool serialize_OBJK_DOMAIN_Representation(mcBuffer* buffer, const OBJK_DOMAIN_Representation* input);
bool deserialize_OBJK_DOMAIN_Representation(mcBuffer* buffer, OBJK_DOMAIN_Representation* output);

bool serialize_OBJK_APPLICATION_Representation(mcBuffer* buffer, const OBJK_APPLICATION_Representation* input);
bool deserialize_OBJK_APPLICATION_Representation(mcBuffer* buffer, OBJK_APPLICATION_Representation* output);

bool serialize_OBJK_PUBLISHER_Representation(mcBuffer* buffer, const OBJK_PUBLISHER_Representation* input);
bool deserialize_OBJK_PUBLISHER_Representation(mcBuffer* buffer, OBJK_PUBLISHER_Representation* output);

bool serialize_OBJK_SUBSCRIBER_Representation(mcBuffer* buffer, const OBJK_SUBSCRIBER_Representation* input);
bool deserialize_OBJK_SUBSCRIBER_Representation(mcBuffer* buffer, OBJK_SUBSCRIBER_Representation* output);

bool serialize_DATAWRITER_Representation(mcBuffer* buffer, const DATAWRITER_Representation* input);
bool deserialize_DATAWRITER_Representation(mcBuffer* buffer, DATAWRITER_Representation* output);

bool serialize_DATAREADER_Representation(mcBuffer* buffer, const DATAREADER_Representation* input);
bool deserialize_DATAREADER_Representation(mcBuffer* buffer, DATAREADER_Representation* output);

bool serialize_OBJK_PARTICIPANT_Representation(mcBuffer* buffer, const OBJK_PARTICIPANT_Representation* input);
bool deserialize_OBJK_PARTICIPANT_Representation(mcBuffer* buffer, OBJK_PARTICIPANT_Representation* output);

bool serialize_OBJK_TOPIC_Representation(mcBuffer* buffer, const OBJK_TOPIC_Representation* input);
bool deserialize_OBJK_TOPIC_Representation(mcBuffer* buffer, OBJK_TOPIC_Representation* output);

bool serialize_OBJK_DomainParticipant_Binary(mcBuffer* buffer, const OBJK_DomainParticipant_Binary* input);
bool deserialize_OBJK_DomainParticipant_Binary(mcBuffer* buffer, OBJK_DomainParticipant_Binary* output);

bool serialize_OBJK_Topic_Binary(mcBuffer* buffer, const OBJK_Topic_Binary* input);
bool deserialize_OBJK_Topic_Binary(mcBuffer* buffer, OBJK_Topic_Binary* output);

bool serialize_OBJK_Publisher_Binary_Qos(mcBuffer* buffer, const OBJK_Publisher_Binary_Qos* input);
bool deserialize_OBJK_Publisher_Binary_Qos(mcBuffer* buffer, OBJK_Publisher_Binary_Qos* output);

bool serialize_OBJK_Publisher_Binary(mcBuffer* buffer, const OBJK_Publisher_Binary* input);
bool deserialize_OBJK_Publisher_Binary(mcBuffer* buffer, OBJK_Publisher_Binary* output);

bool serialize_OBJK_Subscriber_Binary_Qos(mcBuffer* buffer, const OBJK_Subscriber_Binary_Qos* input);
bool deserialize_OBJK_Subscriber_Binary_Qos(mcBuffer* buffer, OBJK_Subscriber_Binary_Qos* output);

bool serialize_OBJK_Subscriber_Binary(mcBuffer* buffer, const OBJK_Subscriber_Binary* input);
bool deserialize_OBJK_Subscriber_Binary(mcBuffer* buffer, OBJK_Subscriber_Binary* output);

bool serialize_OBJK_Endpoint_QosBinary(mcBuffer* buffer, const OBJK_Endpoint_QosBinary* input);
bool deserialize_OBJK_Endpoint_QosBinary(mcBuffer* buffer, OBJK_Endpoint_QosBinary* output);

bool serialize_OBJK_DataWriter_Binary_Qos(mcBuffer* buffer, const OBJK_DataWriter_Binary_Qos* input);
bool deserialize_OBJK_DataWriter_Binary_Qos(mcBuffer* buffer, OBJK_DataWriter_Binary_Qos* output);

bool serialize_OBJK_DataReader_Binary_Qos(mcBuffer* buffer, const OBJK_DataReader_Binary_Qos* input);
bool deserialize_OBJK_DataReader_Binary_Qos(mcBuffer* buffer, OBJK_DataReader_Binary_Qos* output);

bool serialize_OBJK_DataReader_Binary(mcBuffer* buffer, const OBJK_DataReader_Binary* input);
bool deserialize_OBJK_DataReader_Binary(mcBuffer* buffer, OBJK_DataReader_Binary* output);

bool serialize_OBJK_DataWriter_Binary(mcBuffer* buffer, const OBJK_DataWriter_Binary* input);
bool deserialize_OBJK_DataWriter_Binary(mcBuffer* buffer, OBJK_DataWriter_Binary* output);

bool serialize_ObjectVariant(mcBuffer* buffer, const ObjectVariant* input);
bool deserialize_ObjectVariant(mcBuffer* buffer, ObjectVariant* output);

bool serialize_CreationMode(mcBuffer* buffer, const CreationMode* input);
bool deserialize_CreationMode(mcBuffer* buffer, CreationMode* output);

bool serialize_RequestId(mcBuffer* buffer, const RequestId* input);
bool deserialize_RequestId(mcBuffer* buffer, RequestId* output);

bool serialize_ResultStatus(mcBuffer* buffer, const ResultStatus* input);
bool deserialize_ResultStatus(mcBuffer* buffer, ResultStatus* output);

bool serialize_BaseObjectRequest(mcBuffer* buffer, const BaseObjectRequest* input);
bool deserialize_BaseObjectRequest(mcBuffer* buffer, BaseObjectRequest* output);

bool serialize_AGENT_ActivityInfo(mcBuffer* buffer, const AGENT_ActivityInfo* input);
bool deserialize_AGENT_ActivityInfo(mcBuffer* buffer, AGENT_ActivityInfo* output);

bool serialize_DATAREADER_ActivityInfo(mcBuffer* buffer, const DATAREADER_ActivityInfo* input);
bool deserialize_DATAREADER_ActivityInfo(mcBuffer* buffer, DATAREADER_ActivityInfo* output);

bool serialize_DATAWRITER_ActivityInfo(mcBuffer* buffer, const DATAWRITER_ActivityInfo* input);
bool deserialize_DATAWRITER_ActivityInfo(mcBuffer* buffer, DATAWRITER_ActivityInfo* output);

bool serialize_ActivityInfoVariant(mcBuffer* buffer, const ActivityInfoVariant* input);
bool deserialize_ActivityInfoVariant(mcBuffer* buffer, ActivityInfoVariant* output);

bool serialize_ObjectInfo(mcBuffer* buffer, const ObjectInfo* input);
bool deserialize_ObjectInfo(mcBuffer* buffer, ObjectInfo* output);

bool serialize_BaseObjectReply(mcBuffer* buffer, const BaseObjectReply* input);
bool deserialize_BaseObjectReply(mcBuffer* buffer, BaseObjectReply* output);

bool serialize_DataDeliveryControl(mcBuffer* buffer, const DataDeliveryControl* input);
bool deserialize_DataDeliveryControl(mcBuffer* buffer, DataDeliveryControl* output);

bool serialize_ReadSpecification(mcBuffer* buffer, const ReadSpecification* input);
bool deserialize_ReadSpecification(mcBuffer* buffer, ReadSpecification* output);

bool serialize_SeqNumberAndTimestamp(mcBuffer* buffer, const SeqNumberAndTimestamp* input);
bool deserialize_SeqNumberAndTimestamp(mcBuffer* buffer, SeqNumberAndTimestamp* output);

bool serialize_SampleInfoDetail(mcBuffer* buffer, const SampleInfoDetail* input);
bool deserialize_SampleInfoDetail(mcBuffer* buffer, SampleInfoDetail* output);

bool serialize_SampleInfo(mcBuffer* buffer, const SampleInfo* input);
bool deserialize_SampleInfo(mcBuffer* buffer, SampleInfo* output);

bool serialize_SampleInfoDelta(mcBuffer* buffer, const SampleInfoDelta* input);
bool deserialize_SampleInfoDelta(mcBuffer* buffer, SampleInfoDelta* output);

bool serialize_SampleData(mcBuffer* buffer, const SampleData* input);
bool deserialize_SampleData(mcBuffer* buffer, SampleData* output);

bool serialize_SampleDataSeq(mcBuffer* buffer, const SampleDataSeq* input);
bool deserialize_SampleDataSeq(mcBuffer* buffer, SampleDataSeq* output);

bool serialize_Sample(mcBuffer* buffer, const Sample* input);
bool deserialize_Sample(mcBuffer* buffer, Sample* output);

bool serialize_SampleSeq(mcBuffer* buffer, const SampleSeq* input);
bool deserialize_SampleSeq(mcBuffer* buffer, SampleSeq* output);

bool serialize_SampleDelta(mcBuffer* buffer, const SampleDelta* input);
bool deserialize_SampleDelta(mcBuffer* buffer, SampleDelta* output);

bool serialize_SampleDeltaSequence(mcBuffer* buffer, const SampleDeltaSequence* input);
bool deserialize_SampleDeltaSequence(mcBuffer* buffer, SampleDeltaSequence* output);

bool serialize_PackedSamples(mcBuffer* buffer, const PackedSamples* input);
bool deserialize_PackedSamples(mcBuffer* buffer, PackedSamples* output);

bool serialize_SamplePackedSeq(mcBuffer* buffer, const SamplePackedSeq* input);
bool deserialize_SamplePackedSeq(mcBuffer* buffer, SamplePackedSeq* output);

bool serialize_DataRepresentation(mcBuffer* buffer, const DataRepresentation* input);
bool deserialize_DataRepresentation(mcBuffer* buffer, DataRepresentation* output);

bool serialize_CREATE_CLIENT_Payload(mcBuffer* buffer, const CREATE_CLIENT_Payload* input);
bool deserialize_CREATE_CLIENT_Payload(mcBuffer* buffer, CREATE_CLIENT_Payload* output);

bool serialize_CREATE_Payload(mcBuffer* buffer, const CREATE_Payload* input);
bool deserialize_CREATE_Payload(mcBuffer* buffer, CREATE_Payload* output);

bool serialize_GET_INFO_Payload(mcBuffer* buffer, const GET_INFO_Payload* input);
bool deserialize_GET_INFO_Payload(mcBuffer* buffer, GET_INFO_Payload* output);

bool serialize_DELETE_Payload(mcBuffer* buffer, const DELETE_Payload* input);
bool deserialize_DELETE_Payload(mcBuffer* buffer, DELETE_Payload* output);

bool serialize_STATUS_AGENT_Payload(mcBuffer* buffer, const STATUS_AGENT_Payload* input);
bool deserialize_STATUS_AGENT_Payload(mcBuffer* buffer, STATUS_AGENT_Payload* output);

bool serialize_STATUS_Payload(mcBuffer* buffer, const STATUS_Payload* input);
bool deserialize_STATUS_Payload(mcBuffer* buffer, STATUS_Payload* output);

bool serialize_INFO_Payload(mcBuffer* buffer, const INFO_Payload* input);
bool deserialize_INFO_Payload(mcBuffer* buffer, INFO_Payload* output);

bool serialize_READ_DATA_Payload(mcBuffer* buffer, const READ_DATA_Payload* input);
bool deserialize_READ_DATA_Payload(mcBuffer* buffer, READ_DATA_Payload* output);

bool serialize_WRITE_DATA_Payload_Data(mcBuffer* buffer, const WRITE_DATA_Payload_Data* input);
bool deserialize_WRITE_DATA_Payload_Data(mcBuffer* buffer, WRITE_DATA_Payload_Data* output);

bool serialize_WRITE_DATA_Payload_Sample(mcBuffer* buffer, const WRITE_DATA_Payload_Sample* input);
bool deserialize_WRITE_DATA_Payload_Sample(mcBuffer* buffer, WRITE_DATA_Payload_Sample* output);

bool serialize_WRITE_DATA_Payload_DataSeq(mcBuffer* buffer, const WRITE_DATA_Payload_DataSeq* input);
bool deserialize_WRITE_DATA_Payload_DataSeq(mcBuffer* buffer, WRITE_DATA_Payload_DataSeq* output);

bool serialize_WRITE_DATA_Payload_SampleSeq(mcBuffer* buffer, const WRITE_DATA_Payload_SampleSeq* input);
bool deserialize_WRITE_DATA_Payload_SampleSeq(mcBuffer* buffer, WRITE_DATA_Payload_SampleSeq* output);

bool serialize_WRITE_DATA_Payload_PackedSamples(mcBuffer* buffer, const WRITE_DATA_Payload_PackedSamples* input);
bool deserialize_WRITE_DATA_Payload_PackedSamples(mcBuffer* buffer, WRITE_DATA_Payload_PackedSamples* output);

bool serialize_DATA_Payload_Data(mcBuffer* buffer, const DATA_Payload_Data* input);
bool deserialize_DATA_Payload_Data(mcBuffer* buffer, DATA_Payload_Data* output);

bool serialize_DATA_Payload_Sample(mcBuffer* buffer, const DATA_Payload_Sample* input);
bool deserialize_DATA_Payload_Sample(mcBuffer* buffer, DATA_Payload_Sample* output);

bool serialize_DATA_Payload_DataSeq(mcBuffer* buffer, const DATA_Payload_DataSeq* input);
bool deserialize_DATA_Payload_DataSeq(mcBuffer* buffer, DATA_Payload_DataSeq* output);

bool serialize_DATA_Payload_SampleSeq(mcBuffer* buffer, const DATA_Payload_SampleSeq* input);
bool deserialize_DATA_Payload_SampleSeq(mcBuffer* buffer, DATA_Payload_SampleSeq* output);

bool serialize_DATA_Payload_PackedSamples(mcBuffer* buffer, const DATA_Payload_PackedSamples* input);
bool deserialize_DATA_Payload_PackedSamples(mcBuffer* buffer, DATA_Payload_PackedSamples* output);

bool serialize_ACKNACK_Payload(mcBuffer* buffer, const ACKNACK_Payload* input);
bool deserialize_ACKNACK_Payload(mcBuffer* buffer, ACKNACK_Payload* output);

bool serialize_HEARTBEAT_Payload(mcBuffer* buffer, const HEARTBEAT_Payload* input);
bool deserialize_HEARTBEAT_Payload(mcBuffer* buffer, HEARTBEAT_Payload* output);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_SERIALIZATION_XRCE_PROTOCOL_H_
