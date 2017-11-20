#ifndef _MICRORTPS_C_LOG_MESSAGE_PRIVATE_H_
#define _MICRORTPS_C_LOG_MESSAGE_PRIVATE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"
#include "../client_private.h"
#include <stdint.h>

//#if defined(SERIALIZATION_LOGS) || defined(MESSAGE_LOGS)
#include <stdio.h>
#define SEND "==> "
#define RECV "<== "
//#endif

#ifndef SERIALIZATION_LOGS
#define INLINE_SER inline
#else
#define INLINE_SER
#endif

#ifndef MESSAGE_LOGS
#define INLINE_MES inline
#else
#define INLINE_MES
#endif

INLINE_SER static void PRINTL_SERIALIZATION(const char* pre, const uint8_t* buffer, uint32_t size)
#ifndef SERIALIZATION_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINT_SEQUENCE_NUMBER(uint16_t message_sequence_number, uint16_t local_sequence_number)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_CREATE_CLIENT_SUBMESSAGE(const CreateClientPayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_CREATE_RESOURCE_SUBMESSAGE(const CreateResourcePayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_DELETE_RESOURCE_SUBMESSAGE(const DeleteResourcePayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_STATUS_SUBMESSAGE(const StatusPayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_WRITE_DATA_SUBMESSAGE(const WriteDataPayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_READ_DATA_SUBMESSAGE(const ReadDataPayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA(const BaseObjectReply* reply, const SampleData* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_DATA_SUBMESSAGE_SAMPLE(const BaseObjectReply* reply, const Sample* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA_SEQUENCE(const BaseObjectReply* reply, const SampleDataSequence* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_DATA_SUBMESSAGE_SAMPLE_SEQUENCE(const BaseObjectReply* reply, const SampleSequence* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

INLINE_MES static void PRINTL_DATA_SUBMESSAGE_PACKED_SAMPLES(const BaseObjectReply* reply, const PackedSamples* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_C_LOG_MESSAGE_PRIVATE_H_
