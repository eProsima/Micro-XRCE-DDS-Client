#ifndef _MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_
#define _MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>


typedef uint8_t ClientKey[4];

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
#define OBJK_CLIENT 0x21

typedef uint8_t ObjectId[2];
#define OBJECTID_INVALID {0xFF, 0xFF}
#define OBJECTID_CLIENT {0xFF, 0xFF}
#define OBJECTID_SESSION {0xFF, 0xFF}

typedef uint8_t XrceCookie[4];
#define XRCE_COOKIE {0x58, 0x52, 0x43, 0x45}

typedef uint8_t XrceVersion[2];
#define XRCE_VESION_MAJOR 0x01
#define XRCE_VESION_MINOR 0x00
#define XRCE_VERSION {XRCE_VESION_MAJOR, XRCE_VESION_MINOR}

typedef uint8_t XrceVendorId[2];
#define XRCE_VENDOR_INVALID {0x00, 0x00}

typedef uint8_t SessionId;
#define SESSIONID_NONE_WITH_CLIENT_KEY 0x00
#define SESSIONID_NONE_WITHOUT_CLIENT_KEY 0x80

typedef uint8_t StreamId;
#define STREAMID_NONE 0x01
#define STREAMID_BUILTIN_BEST_EFFORTS 0x00
#define STREAMID_BUILTIN_RELIABLE 0x00

typedef uint8_t RepresentationFormat;
#define REPRESENTATION_BY_REFERENCE 0x01
#define REPRESENTATION_AS_XML_STRING 0x02
#define REPRESENTATION_IN_BINARY 0x03


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
    uint8_t xrce_cookie[4];
    uint8_t xrce_version[2];
    uint8_t xrce_vendor_id[2];
    Time_t client_timestamp;
    uint8_t client_key[4];
    uint8_t session_id;

} OBJK_CLIENT_Representation;


typedef union OBJK_Representation3FormatsU
{
    String_t object_name;
    String_t string_representation;
    BinarySequence_t binary_representation;

} OBJK_Representation3FormatsU;


typedef struct OBJK_Representation3Formats
{
    uint8_t format;
    OBJK_Representation3FormatsU _;

} OBJK_Representation3Formats;


typedef enum EndpointQosFlags
{
    IS_RELIABLE = 0x01 << 0,
    IS_HISTORY_KEEP_LAST = 0x01 << 1

} EndpointQosFlags;


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SPEC_H_
