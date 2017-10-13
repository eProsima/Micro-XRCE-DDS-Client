#ifndef _MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_
#define _MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef uint8_t ClientKey[4];
typedef uint8_t ObjectKind;
#define OBJK_INVALID     0x0
#define OBJK_PARTICIPANT 0x1
#define OBJK_TOPIC       0x4
#define OBJK_DATAWRITER  0x3
#define OBJK_DATAREADER  0x7
#define OBJK_SUBSCRIBER  0x8
#define OBJK_PUBLISHER   0x9
#define OBJK_TYPE        0x10
#define OBJK_QOSPROFILE  0x11
#define OBJK_APPLICATION 0x20
#define OBJK_CLIENT      0x21

typedef uint8_t ObjectId[2];
#define OBJECTID_INVALID {0xFF, 0xFF}
#define OBJECTID_CLIENT  {0xFF, 0xF0}
#define OBJECTID_SESSION {0xFF, 0xF1}

typedef uint8_t XrceCookie[4];
#define XRCE_COOKIE {0x58, 0x52, 0x43, 0x45}

typedef uint8_t XrceVersion[2];
#define XRCE_VESION_MAJOR 0x01
#define XRCE_VESION_MINOR 0x00
#define XRCE_VERSION {XRCE_VESION_MAJOR, XRCE_VESION_MINOR}

typedef uint8_t XrceVendorId[2];
#define XRCE_VENDOR_INVALID {0x00, 0x00}

typedef uint8_t SessionId;
#define SESSIONID_NONE_WITH_CLIENT_KEY    = 0x00;
#define SESSIONID_NONE_WITHOUT_CLIENT_KEY = 0x80;
typedef uint8_t StreamId;
#define STREAMID_NONE                 = 0x00;
#define STREAMID_BUILTIN_BEST_EFFORTS = 0x01;
#define STREAMID_BUILTIN_RELIABLE     = 0x80;

typedef uint8_t RepresentationFormat;
#define REPRESENTATION_BY_REFERENCE  0x01
#define REPRESENTATION_AS_XML_STRING 0x02
#define REPRESENTATION_IN_BINARY     0x03

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
    String_t* data;

} StringSequence_t;

typedef struct OBJK_CLIENT_Representation
{
    XrceCookie xrce_cookie;
    XrceVersion xrce_version;
    XrceVendorId xrce_vendor_id;

    Time_t client_timestamp;
    ClientKey client_key;
    SessionId session_id;

} OBJK_CLIENT_Representation;


typedef struct OBJK_Representation3Formats
{
    RepresentationFormat format;

    String_t object_reference;
    String_t xml_string_representation;
    BinarySequence_t binary_representation;

} OBJK_Representation3Formats;

typedef struct OBJK_Representation2Formats
{
    RepresentationFormat format;

    String_t object_name;
    String_t string_representation;

} OBJK_Representation2Formats;


typedef struct OBJK_Representation2_Base
{
    OBJK_Representation2Formats representation;

} OBJK_Representation2_Base;

typedef struct OBJK_Representation3_Base
{
    OBJK_Representation3Formats representation;

} OBJK_Representation3_Base;

typedef struct OBJK_QOSPROFILE_Representation
{
    OBJK_Representation2_Base base_representation;

} OBJK_QOSPROFILE_Representation;

typedef struct OBJK_APPLICATION_Representation
{
    OBJK_Representation2_Base base_representation;

} OBJK_APPLICATION_Representation;

typedef struct OBJK_PARTICIPANT_Representation
{
    OBJK_Representation2_Base base_representation;

} OBJK_PARTICIPANT_Representation;

typedef struct OBJK_TYPE_Representation
{
    OBJK_Representation3_Base base_representation;
    ObjectId participant_id;
    String_t registered_type_name;

} OBJK_TYPE_Representation;

typedef struct OBJK_TOPIC_Representation
{
    OBJK_Representation3_Base base_representation;
    ObjectId participant_id;

} OBJK_TOPIC_Representation;

typedef struct OBJK_PUB_Representation
{
    OBJK_Representation3_Base base_representation;
    :ObjectId participant_id;

} OBJK_PUB_Representation;

typedef struct OBJK_SUB_Representation
{
    OBJK_Representation3_Base base_representation;
    ObjectId participant_id;

} OBJK_SUB_Representation;

typedef struct OBJK_DW_Representation
{
    OBJK_Representation3_Base base_representation;
    ObjectId participant_id;
    ObjectId publisher_id;

} OBJK_DW_Representation;

typedef struct OBJK_DR_Representation
{
    OBJK_Representation3_Base base_representation;
    ObjectId participant_id;
    ObjectId subscriber_id;

} OBJK_DR_Representation;

typedef struct OBJK_Type_Binary
{
    String_t type_name;
};

typedef struct OBJK_Topic_QosBinary
{
    String_t topic_name;
    String_t type_name;
};

typedef struct OBJK_PUB_QosBinary
{
    std::vector<std::string> partitions;
    std::vector<uint8_t> group_data;
};

typedef struct OBJK_SUB_QosBinary
{
    std::vector<std::string> partitions;
    std::vector<uint8_t> group_data;
};

enum EndpointQosFlags //16
{
    is_reliable                   = 0x01 << 0,
    is_history_keep_last          = 0x01 << 1,
    is_ownership_exclusive        = 0x01 << 2,
    is_durability_transient_local = 0x01 << 3,
    is_durability_transient       = 0x01 << 4,
    is_durability_persistent      = 0x01 << 5
};


typedef struct OBJK_Endpoint_QosBinary
{
    EndpointQosFlags qos_flags;
    uint16_t history_depth;
    uint32_t deadline_msec;
    uint32_t lifespan_msec;
    std::vector<uint8_t> user_data;
};

typedef struct OBJK_DataReader_Binary
{
    std::string topic_name;
    OBJK_Endpoint_QosBinary endpoint_qos;
    uint32_t timebasedfilter_msec;
    std::string contentbased_filter;
};

typedef struct OBJK_DataWriter_Binary
{
    std::string topic_name;
    OBJK_Endpoint_QosBinary endpoint_qos;
    uint32_t ownership_strength;
};

typedef struct ObjectVariant
{
    ObjectKind _d;

    OBJK_CLIENT_Representation client;
    OBJK_APPLICATION_Representation application;
    OBJK_PARTICIPANT_Representation participant;
    OBJK_QOSPROFILE_Representation qos_profile;
    OBJK_TYPE_Representation type;
    OBJK_TOPIC_Representation topic;
    OBJK_PUB_Representation publisher;
    OBJK_SUB_Representation subscriber;
    OBJK_DW_Representation data_writer;
    OBJK_DR_Representation data_reader;
};


typedef struct CreationMode
{
    bool reuse;
    bool replace;
};

typedef uint8_t RequestId[2];

typedef struct ResultStatus
{
    RequestId request_id;
    uint8_t status;
    uint8_t implementation_status;
};

const uint8_t STATUS_OK                    = 0x00;
const uint8_t STATUS_OK_MATCHED            = 0x01;
const uint8_t STATUS_ERR_DDS_ERROR         = 0x80;
const uint8_t STATUS_ERR_MISMATCH          = 0x81;
const uint8_t STATUS_ERR_ALREADY_EXISTS    = 0x82;
const uint8_t STATUS_ERR_DENIED            = 0x83;
const uint8_t STATUS_ERR_UNKNOWN_REFERENCE = 0x84;
const uint8_t STATUS_ERR_INVALID_DATA      = 0x85;
const uint8_t STATUS_ERR_INCOMPATIBLE      = 0x86;
const uint8_t STATUS_ERR_RESOURCES         = 0x87;

const uint8_t STATUS_LAST_OP_NONE   = 0;
const uint8_t STATUS_LAST_OP_CREATE = 1;
const uint8_t STATUS_LAST_OP_UPDATE = 2;
const uint8_t STATUS_LAST_OP_DELETE = 3;
const uint8_t STATUS_LAST_OP_LOOKUP = 4;
const uint8_t STATUS_LAST_OP_READ   = 5;
const uint8_t STATUS_LAST_OP_WRITE  = 6;

enum InfoMask //32bits
{
    INFO_CONFIGURATION = 0x01 << 0,
    INFO_ACTIVITY      = 0x01 << 1
};

typedef struct OBJK_DATAREADER_ActivityInfo
{
    int16_t highest_acked_num;
};

typedef struct OBJK_DATAWRITER_ActivityInfo
{
    int16_t streaseq_num;
    uint64_t sample_seq_num;
};

typedef struct ActivityInfoVariant
{
    ObjectKind _d;

    OBJK_DATAWRITER_ActivityInfo data_writer;
    OBJK_DATAREADER_ActivityInfo data_reader;
};

typedef struct Info
{
    ObjectVariant config;
    ActivityInfoVariant activity;
};

typedef struct BaseRequest
{
    RequestId request_id;
    ObjectId object_id;
};

typedef struct BaseObjectRequest
{
    BaseRequest base_request;
    ObjectId object_id;
};

typedef struct BaseReply
{
    ResultStatus result;
    RequestId request_id;
};

typedef struct BaseObjectReply
{
    BaseReply base_reply;
    ObjectId object_id;
};

typedef struct InfoReply
{
    BaseObjectReply base_object_reply;
    ObjectVariant info;
};

typedef uint8_t DataFormat;
const DataFormat FORMAT_DATA           = 0x00;
const DataFormat FORMAT_DATA_SEQ       = 0x01;
const DataFormat FORMAT_SAMPLE         = 0x02;
const DataFormat FORMAT_SAMPLE_SEQ     = 0x03;
const DataFormat FORMAT_PACKED_SAMPLES = 0x04;

typedef struct DataDeliveryControl
{
    uint16_t max_samples;      // Maximum number of samples
    uint32_t max_elapsed_time; // In milliseconds
    uint32_t max_rate;         // Bytes per second
};

typedef struct DataDeliveryConfig
{
    DataFormat _d;

    DataDeliveryControl delivey_control;
};

typedef struct ReadSpecification
{
    std::string content_filter_expression;
    DataDeliveryConfig delivery_config;
};

typedef struct SampleInfo
{
    uint32_t state;
    uint32_t sequence_number;
    uint32_t session_time_offset;
};

typedef struct SampleInfoDelta
{
    uint8_t state;
    uint8_t seq_number_delta;
    uint16_t timestamp_delta;
};

typedef struct SampleData
{
    std::vector<uint8_t> serialized_data;
};

typedef std::vector<SampleData> SampleDataSeq;

typedef struct Sample
{
    SampleInfo info;
    SampleData data;
};

typedef std::vector<Sample> SampleSeq;

typedef struct SampleDelta
{
    SampleInfoDelta info_delta;
    SampleData data;
};

typedef struct PackedSamples
{
    SampleInfo info_base;
    std::vector<SampleDelta> sample_delta_seq;
};

typedef std::vector<PackedSamples> PackedSampleSeq;

typedef struct DataRepresentation
{
    DataFormat _d;

    SampleData data;
    SampleDataSeq data_seq;
    Sample sample;
    SampleSeq sample_seq;
    PackedSamples packed_samples;
} DataRepresentation;

typedef enum SubmessageId //uint8_t
{
    CREATE_CLIENT = 0,
    CREATE        = 1,
    GET_INFO      = 2,
    DELETE        = 3,
    STATUS        = 4,
    INFO          = 5,
    WRITE_DATA    = 6,
    READ_DATA     = 7,
    DATA          = 8,
    ACKNACK       = 9,
    HEARTBEAT     = 10,
    FRAGMENT      = 12,
    FRAGMENT_END  = 13
} SubmessageId;

typedef struct MessageHeader
{
    uint8_t session_id;
    uint8_t streaid;
    uint16_t sequence_nr;
    ClientKey client_key;
} MessageHeader;

typedef struct SubmessageHeader
{
    uint8_t submessage_id;
    uint8_t flags;
    uint16_t submessage_length;
} SubmessageHeader;

typedef struct CREATE_Payload
{
    BaseObjectRequest base_object_request;
    ObjectVariant object_representation;
} CREATE_Payload;

typedef struct DELETE_RESOURCE_Payload
{
    BaseObjectRequest base_object_request;
} DELETE_RESOURCE_Payload;

typedef struct RESOURCE_STATUS_Payload
{
    BaseObjectReply base_object_reply;
} RESOURCE_STATUS_Payload;

typedef struct GET_INFO_Payload
{
    BaseObjectRequest base_object_request;
    InfoMask info_mask;
} GET_INFO_Payload;

typedef struct INFO_Payload
{
    BaseObjectReply base_object_reply;
    Info info;
} INFO_Payload;

typedef struct READ_DATA_Payload
{
    BaseObjectRequest base_object_request;
    ReadSpecification read_specification;
} READ_DATA_Payload;

typedef struct DATA_Payload_Data
{
    BaseObjectReply base_object_reply;
    SampleData data;
} DATA_Payload_Data;

typedef struct DATA_Payload_Sample
{
    BaseObjectReply base_object_reply;
    Sample sample;
} DATA_Payload_Sample;

typedef struct DATA_Payload_DataSeq
{
    BaseObjectReply base_object_reply;
    std::vector<SampleData> data_seq;
} DATA_Payload_DataSeq;

typedef struct DATA_Payload_SampleSeq
{
    BaseObjectReply base_object_reply;
    std::vector<Sample> sample_seq;
} DATA_Payload_SampleSeq;

typedef struct DATA_Payload_PackedSamples
{
    BaseObjectReply base_object_reply;
    PackedSamples packed_samples;
} DATA_Payload_PackedSamples;

typedef struct WRITE_DATA_Payload
{
    BaseObjectRequest base_object_request;
    DataRepresentation data_to_write;
} WRITE_DATA_Payload;

*/

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_