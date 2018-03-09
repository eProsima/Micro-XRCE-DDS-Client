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

#ifndef _MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_
#define _MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#define STRING_SEQUENCE_MAX 10
#define SAMPLE_SEQUENCE_MAX 10
#define SAMPLE_DATA_SEQUENCE_MAX 10
#define SAMPLE_DELTA_SEQUENCE_MAX 10
#define PACKED_SAMPLES_SEQUENCE_MAX 10
#define TRANSPORT_LOCATOR_SEQUENCE_MAX 10
#define PROPERTY_SEQUENCE_MAX 10


typedef struct Time_t
{
    int32_t seconds;
    uint32_t nanoseconds;

} Time_t;


typedef struct String_t
{
    uint32_t size;
    char* data;

} String_t;


typedef struct BinarySequence_t
{
    uint32_t size;
    uint8_t* data;

} BinarySequence_t;


typedef struct StringSequence_t
{
    uint32_t size;
    String_t data[STRING_SEQUENCE_MAX];

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

typedef uint8_t SessionId;
#define SESSIONID_NONE_WITH_CLIENT_KEY 0x00
#define SESSIONID_NONE_WITHOUT_CLIENT_KEY 0x80

typedef uint8_t StreamId;
#define STREAMID_NONE 0x00
#define STREAMID_BUILTIN_BEST_EFFORTS 0x01
#define STREAMID_BUILTIN_RELIABLE 0x80


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
    String_t value;

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
    String_t name;
    String_t value;

} Property;


typedef struct PropertySeq
{
    uint32_t size;
    Property data[PROPERTY_SEQUENCE_MAX];

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
    String_t object_reference;
    String_t xml_string_represenatation;
    BinarySequence_t binary_representation;

} OBJK_Representation3FormatsU;


typedef struct OBJK_Representation3Formats
{
    uint8_t format;
    OBJK_Representation3FormatsU _;

} OBJK_Representation3Formats;


typedef union OBJK_RepresentationRefAndXMLFormatsU
{
    String_t object_name;
    String_t xml_string_represenatation;

} OBJK_RepresentationRefAndXMLFormatsU;


typedef struct OBJK_RepresentationRefAndXMLFormats
{
    uint8_t format;
    OBJK_RepresentationRefAndXMLFormatsU _;

} OBJK_RepresentationRefAndXMLFormats;


typedef union OBJK_RepresentationBinAndXMLFormatsU
{
    BinarySequence_t binary_representation;
    String_t string_represenatation;

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
    OBJK_RepresentationBinAndXML_Base base;
    ObjectId publisher_id;

} DATAWRITER_Representation;


typedef struct DATAREADER_Representation
{
    OBJK_RepresentationBinAndXML_Base base;
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
    String_t domain_reference;
    bool optional_qos_profile_reference;
    String_t qos_profile_reference;

} OBJK_DomainParticipant_Binary;


typedef struct OBJK_Topic_Binary
{
    String_t topic_name;
    bool optional_type_reference;
    String_t type_reference;
    bool optional_type_name;
    String_t type_name;

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
    String_t publisher_name;
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
    String_t subscriber_name;
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
    String_t contentbased_filter;

} OBJK_DataReader_Binary_Qos;


typedef struct OBJK_DataReader_Binary
{
    String_t topic_name;
    bool optional_qos;
    OBJK_DataReader_Binary_Qos qos;

} OBJK_DataReader_Binary;


typedef struct OBJK_DataWriter_Binary
{
    String_t topic_name;
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


typedef enum StatusValue
{
    STATUS_OK = 0x00,
    STATUS_OK_MATCHED = 0x01,
    STATUS_ERR_DDS_ERROR = 0x80,
    STATUS_ERR_MISMATCH = 0x81,
    STATUS_ERR_ALREADY_EXISTS = 0x82,
    STATUS_ERR_DENIED = 0x83,
    STATUS_ERR_UNKNOWN_REFERENCE = 0x84,
    STATUS_ERR_INVALID_DATA = 0x85,
    STATUS_ERR_INCOMPATIBLE = 0x86,
    STATUS_ERR_RESOURCES = 0x87

} StatusValue;


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
    FORMAT_MASK = 0x0F

} DataFormat;


typedef struct DataDeliveryControl
{
    uint16_t max_samples;
    uint32_t max_elapsed_time;
    uint32_t max_bytes_per_seconds;
    uint32_t min_pace_period;

} DataDeliveryControl;


typedef struct ReadSpecification
{
    uint8_t data_format;
    bool optional_content_filter_expression;
    String_t content_filter_expression;
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
    uint8_t* data;

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
    SampleData data;

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
    int16_t first_unacked_seq_num;
    uint8_t nack_bitmap[2];

} ACKNACK_Payload;


typedef struct HEARTBEAT_Payload
{
    int16_t first_unacked_seq_nr;
    int16_t last_unacked_seq_nr;

} HEARTBEAT_Payload;


typedef struct MessageHeader
{
    uint8_t session_id;
    uint8_t stream_id;
    uint16_t sequence_nr;

} MessageHeader;


typedef enum SubmessageId
{
    SUBMESSAGE_ID_CREATE_CLIENT = 0,
    SUBMESSAGE_ID_CREATE = 1,
    SUBMESSAGE_ID_GET_INFO = 2,
    SUBMESSAGE_ID_DELETE = 3,
    SUBMESSAGE_ID_STATUS = 4,
    SUBMESSAGE_ID_INFO = 5,
    SUBMESSAGE_ID_WRITE_DATA = 6,
    SUBMESSAGE_ID_READ_DATA = 7,
    SUBMESSAGE_ID_DATA = 8,
    SUBMESSAGE_ID_ACKNACK = 9,
    SUBMESSAGE_ID_HEARTBEAT = 10,
    SUBMESSAGE_ID_FRAGMENT = 12,
    SUBMESSAGE_ID_FRAGMENT_END = 13

} SubmessageId;


typedef enum SubmessageHeaderFlags
{
    FLAG_ENDIANNESS = 0x01 << 0,
    FLAG_REUSE = 0x01 << 1,
    FLAG_REPLACE = 0x01 << 2,
    FLAG_LAST_FRAGMENT = 0x01 << 1,
    FORMAT_DATA_F = 0x00,
    FORMAT_SAMPLE_F = 0x02,
    FORMAT_DATA_SEQ_F = 0x08,
    FORMAT_SAMPLE_SEQ_F = 0x0A,
    FORMAT_PACKED_SAMPLES_F = 0x0E

} SubmessageHeaderFlags;


typedef struct SubmessageHeader
{
    uint8_t id;
    uint8_t flags;
    uint16_t length;

} SubmessageHeader;


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_
