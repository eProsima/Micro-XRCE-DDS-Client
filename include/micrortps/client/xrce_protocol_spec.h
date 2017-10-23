#ifndef _MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_
#define _MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>



typedef struct ClientKey
{
    uint8_t data[4];

} ClientKey;

typedef uint8_t ObjectKind;
#define OBJK_INVALID 0x00
#define OBJK_PARTICIPANT 0x01
#define OBJK_DATAWRITER 0x03
#define OBJK_TOPIC 0x04
#define OBJK_DATAREADER 0x07
#define OBJK_SUBSCRIBER 0x08
#define OBJK_PUBLISHER 0x09
#define OBJK_TYPE 0x10
#define OBJK_QOSPROFILE 0x11
#define OBJK_APPLICATION 0x20


typedef struct ObjectId
{
    uint8_t data[2];

} ObjectId;
#define OBJECTID_INVALID {0xFF, 0xFF}
#define OBJECTID_CLIENT {0xFF, 0xFF}
#define OBJECTID_SESSION {0xFF, 0xFF}


typedef struct XrceCookie
{
    uint8_t data[4];

} XrceCookie;
#define XRCE_COOKIE (XrceCookie){0x58, 0x52, 0x43, 0x45}


typedef struct XrceVersion
{
    uint8_t data[2];

} XrceVersion;
#define XRCE_VESION_MAJOR 0x01
#define XRCE_VESION_MINOR 0x00
#define XRCE_VERSION (XrceVersion){XRCE_VESION_MAJOR, XRCE_VESION_MINOR}


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
#define STREAMID_BUILTIN_RELIABLE 0x8F

typedef uint8_t RepresentationFormat;
#define REPRESENTATION_BY_REFERENCE 0x01
#define REPRESENTATION_AS_XML_STRING 0x02
#define REPRESENTATION_IN_BINARY 0x03


typedef struct RequestId
{
    uint8_t data[2];

} RequestId;
#define STATUS_OK 0x00
#define STATUS_OK_MATCHED 0x01
#define STATUS_ERR_DDS_ERROR 0x80
#define STATUS_ERR_MISMATCH 0x81
#define STATUS_ERR_ALREADY_EXISTS 0x82
#define STATUS_ERR_DENIED 0x83
#define STATUS_ERR_UNKNOWN_REFERENCE 0x84
#define STATUS_ERR_INVALID_DATA 0x85
#define STATUS_ERR_INCOMPATIBLE 0x86
#define STATUS_ERR_RESOURCES 0x87
#define STATUS_LAST_OP_NONE 0x00
#define STATUS_LAST_OP_CREATE 0x01
#define STATUS_LAST_OP_UPDATE 0x02
#define STATUS_LAST_OP_DELETE 0x03
#define STATUS_LAST_OP_LOOKUP 0x04
#define STATUS_LAST_OP_READ 0x05
#define STATUS_LAST_OP_WRITE 0x06


typedef enum DataFormat
{
    FORMAT_DATA = 0x00,
    FORMAT_DATA_SEQ = 0x01,
    FORMAT_SAMPLE = 0x02,
    FORMAT_SAMPLE_SEQ = 0x03,
    FORMAT_PACKED_SAMPLES = 0x04

} DataFormat;


typedef struct Time_t
{
    int32_t seconds;
    uint32_t nanoseconds;

} Time_t;


typedef struct String_t
{
    int32_t size;
    char* data;

} String_t;


typedef struct BinarySequence_t
{
    int32_t size;
    uint8_t* data;

} BinarySequence_t;


typedef struct StringSequence_t
{
    int32_t size;
    String_t* data;

} StringSequence_t;


typedef struct OBJK_CLIENT_Representation
{
    XrceCookie xrce_cookie;
    XrceVersion xrce_version;
    XrceVendorId xrce_vendor_id;
    Time_t client_timestamp;
    ClientKey client_key;
    uint8_t session_id;

} OBJK_CLIENT_Representation;


typedef union OBJK_Representation3_BaseU
{
    String_t object_name;
    String_t string_representation;
    BinarySequence_t binary_representation;

} OBJK_Representation3_BaseU;


typedef struct OBJK_Representation3_Base
{
    uint8_t format;
    OBJK_Representation3_BaseU _;

} OBJK_Representation3_Base;


typedef union OBJK_Representation2_BaseU
{
    String_t object_name;
    String_t string_representation;

} OBJK_Representation2_BaseU;


typedef struct OBJK_Representation2_Base
{
    uint8_t format;
    OBJK_Representation2_BaseU _;

} OBJK_Representation2_Base;


typedef struct OBJK_QOSPROFILE_Representation
{
    OBJK_Representation2_Base base2;

} OBJK_QOSPROFILE_Representation;


typedef struct OBJK_APPLICATION_Representation
{
    OBJK_Representation2_Base base2;

} OBJK_APPLICATION_Representation;


typedef struct OBJK_PARTICIPANT_Representation
{
    OBJK_Representation2_Base base2;

} OBJK_PARTICIPANT_Representation;


typedef struct OBJK_TYPE_Representation
{
    OBJK_Representation3_Base base3;
    ObjectId participant_id;
    String_t registered_type_name;

} OBJK_TYPE_Representation;


typedef struct OBJK_TOPIC_Representation
{
    OBJK_Representation3_Base base3;
    ObjectId participant_id;

} OBJK_TOPIC_Representation;


typedef struct OBJK_PUBLISHER_Representation
{
    OBJK_Representation3_Base base3;
    ObjectId participant_id;

} OBJK_PUBLISHER_Representation;


typedef struct OBJK_SUBSCRIBER_Representation
{
    OBJK_Representation3_Base base3;
    ObjectId participant_id;

} OBJK_SUBSCRIBER_Representation;


typedef struct OBJK_DATAWRITER_Representation
{
    OBJK_Representation3_Base base3;
    ObjectId participant_id;
    ObjectId publisher_id;

} OBJK_DATAWRITER_Representation;


typedef struct OBJK_DATAREADER_Representation
{
    OBJK_Representation3_Base base3;
    ObjectId participant_id;
    ObjectId subscriber_id;

} OBJK_DATAREADER_Representation;


typedef struct OBJK_TYPE_Binary
{
    String_t type_name;

} OBJK_TYPE_Binary;


typedef struct OBJK_TOPIC_Binary
{
    String_t topic_name;
    String_t type_name;

} OBJK_TOPIC_Binary;


typedef struct OBJK_PUBLISHER_Binary
{
    StringSequence_t partitions;
    BinarySequence_t group_data;

} OBJK_PUBLISHER_Binary;


typedef struct OBJK_SUBSCRIBER_Binary
{
    StringSequence_t partitions;
    BinarySequence_t group_data;

} OBJK_SUBSCRIBER_Binary;


typedef enum EndpointQosFlags
{
    IS_RELIABLE = 0x01 << 0,
    IS_HISTORY_KEEP_LAST = 0x01 << 1,
    IS_OWNERSHIP_EXCLUSIVE = 0x01 << 2,
    IS_DURABILITY_TRANSIENT_LOCAL = 0x01 << 3,
    IS_DURABILITY_TRANSIENT = 0x01 << 4,
    IS_DURABILITY_PERSISTANT = 0x01 << 5

} EndpointQosFlags;


typedef struct OBJK_Endpoint_QosBinary
{
    uint16_t qos_flags;
    uint16_t history_depth;
    uint32_t deadline_msec;
    uint32_t lifespan_msec;
    BinarySequence_t user_data;

} OBJK_Endpoint_QosBinary;


typedef struct OBJK_DataReader_Binary
{
    String_t topic_name;
    OBJK_Endpoint_QosBinary endpoint_qos;
    uint32_t timebasedfilter_msec;
    String_t contentbased_filter;

} OBJK_DataReader_Binary;


typedef struct OBJK_DataWriter_Binary
{
    String_t topic_name;
    OBJK_Endpoint_QosBinary endpoint_qos;
    uint32_t ownership_strength;

} OBJK_DataWriter_Binary;


typedef union ObjectVariantU
{
    OBJK_APPLICATION_Representation application;
    OBJK_PARTICIPANT_Representation participant;
    OBJK_QOSPROFILE_Representation qos_profile;
    OBJK_TYPE_Representation type;
    OBJK_TOPIC_Representation topic;
    OBJK_PUBLISHER_Representation publisher;
    OBJK_SUBSCRIBER_Representation subscriber;
    OBJK_DATAWRITER_Representation data_writer;
    OBJK_DATAREADER_Representation data_reader;

} ObjectVariantU;


typedef struct ObjectVariant
{
    uint8_t kind;
    ObjectVariantU _;

} ObjectVariant;


typedef struct ResultStatus
{
    RequestId request_id;
    uint8_t status;
    uint8_t implementation_status;

} ResultStatus;


typedef enum InfoMask
{
    INFO_CONFIGURATION = 0x01 << 0,
    INFO_ACTIVITY = 0x01 << 1

} InfoMask;


typedef struct OBJK_DATAREADER_ActivityInfo
{
    int16_t highest_acked_num;

} OBJK_DATAREADER_ActivityInfo;


typedef struct OBJK_DATAWRITER_ActivityInfo
{
    int16_t streaseq_num;
    uint64_t sample_seq_num;

} OBJK_DATAWRITER_ActivityInfo;


typedef union ActivityInfoVariantU
{
    OBJK_DATAWRITER_ActivityInfo data_writer;
    OBJK_DATAREADER_ActivityInfo data_reader;

} ActivityInfoVariantU;


typedef struct ActivityInfoVariant
{
    uint8_t kind;
    ActivityInfoVariantU _;

} ActivityInfoVariant;


typedef struct Info
{
    ObjectVariant config;
    ActivityInfoVariant activity;

} Info;


typedef struct BaseRequest
{
    RequestId request_id;

} BaseRequest;


typedef struct BaseObjectRequest
{
    BaseRequest base;
    ObjectId object_id;

} BaseObjectRequest;


typedef struct BaseReply
{
    ResultStatus result;
    RequestId request_id;

} BaseReply;


typedef struct BaseObjectReply
{
    BaseReply base;
    ObjectId object_id;

} BaseObjectReply;


typedef struct InfoReply
{
    BaseObjectReply reply;
    ObjectVariant info;

} InfoReply;


typedef struct DataDeliveryControl
{
    uint16_t max_samples;
    uint32_t max_elapsed_time;
    uint32_t max_rate;

} DataDeliveryControl;


typedef struct ReadSpecification
{
    uint8_t optional_content_filter_expression;
    String_t content_filter_expression;
    uint8_t optional_delivery_config;
    DataDeliveryControl delivery_config;

} ReadSpecification;


typedef struct SampleInfo
{
    uint32_t state;
    uint32_t sequence_number;
    uint32_t session_time_offset;

} SampleInfo;


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


typedef struct SampleDataSequence
{
    uint32_t size;
    SampleData* data;

} SampleDataSequence;


typedef struct Sample
{
    SampleInfo info;
    SampleData data;

} Sample;


typedef struct SampleSequence
{
    uint32_t size;
    Sample* data;

} SampleSequence;


typedef struct SampleDelta
{
    SampleInfoDelta info_delta;
    SampleData data;

} SampleDelta;


typedef struct SampleDeltaSequence
{
    uint32_t size;
    SampleDelta* data;

} SampleDeltaSequence;


typedef struct PackedSamples
{
    SampleInfo info_base;
    SampleDeltaSequence sample_delta_seq;

} PackedSamples;


typedef struct PackedSamplesSequence
{
    uint32_t size;
    PackedSamples* data;

} PackedSamplesSequence;


typedef union DataRepresentationU
{
    SampleData data;
    SampleDataSequence data_seq;
    Sample sample;
    SampleSequence sample_seq;
    PackedSamples packed_samples;

} DataRepresentationU;


typedef struct DataRepresentation
{
    uint8_t format;
    DataRepresentationU _;

} DataRepresentation;


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


typedef struct MessageHeader
{
    uint8_t session_id;
    uint8_t stream_id;
    uint16_t sequence_nr;

} MessageHeader;


typedef enum SubmessageHeaderFlags
{
    FLAG_ENDIANNESS = 0x01 << 0,
    FLAG_REPLACE = 0x01 << 1,
    FLAG_LAST_FRAGMENT = 0x01 << 1,
    FLAG_REUSE = 0x01 << 2

} SubmessageHeaderFlags;


typedef struct SubmessageHeader
{
    uint8_t id;
    uint8_t flags;
    uint16_t length;

} SubmessageHeader;


typedef struct CreateClientPayload
{
    OBJK_CLIENT_Representation representation;

} CreateClientPayload;


typedef struct CreateResourcePayload
{
    BaseObjectRequest request;
    ObjectVariant representation;

} CreateResourcePayload;


typedef struct DeleteResourcePayload
{
    BaseObjectRequest request;

} DeleteResourcePayload;


typedef struct StatusPayload
{
    BaseObjectReply reply;

} StatusPayload;


typedef struct GetInfoPayload
{
    BaseObjectRequest request;
    uint32_t info_mask;

} GetInfoPayload;


typedef struct InfoPayload
{
    BaseObjectReply reply;
    Info info;

} InfoPayload;


typedef struct ReadDataPayload
{
    BaseObjectRequest request;
    ReadSpecification read_specification;

} ReadDataPayload;


typedef struct WriteDataPayload
{
    BaseObjectRequest request;
    DataRepresentation data_to_write;

} WriteDataPayload;


typedef struct SampleDataPayloadData
{
    BaseObjectReply reply;
    SampleData data;

} SampleDataPayloadData;


typedef struct SamplePayloadData
{
    BaseObjectReply reply;
    Sample sample;

} SamplePayloadData;


typedef struct SampleDataSequencePayloadData
{
    BaseObjectReply reply;
    SampleDataSequence data_sequence;

} SampleDataSequencePayloadData;


typedef struct SampleSequencePayloadData
{
    BaseObjectReply reply;
    SampleSequence sample_sequence;

} SampleSequencePayloadData;


typedef struct PackedSamplesPayloadData
{
    BaseObjectReply reply;
    PackedSamples packed_samples;

} PackedSamplesPayloadData;


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_
