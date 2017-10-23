#ifndef _C_LOG_MESSAGE_PRIVATE_H_
#define _C_LOG_MESSAGE_PRIVATE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"
#include <stdint.h>

#if defined(SERIALIZATION_LOGS) || defined(MESSAGE_LOGS)
#include <stdio.h>
const char* data_to_string(const uint8_t* data, uint32_t size);
#endif

#ifdef MESSAGE_LOGS
#define SEND "==> "
#define RECV "<== "
#endif

void PRINT_SERIALIZATION(const char* pre, const uint8_t* buffer, uint32_t size)
#ifndef SERIALIZATION_LOGS
{}
#else
;
#endif

void PRINTL_CREATE_CLIENT_SUBMESSAGE(const char* pre, const CreateClientPayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINTL_CREATE_RESOURCE_SUBMESSAGE(const char* pre, const CreateResourcePayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_DELETE_RESOURCE_SUBMESSAGE(const char* pre, const DeleteResourcePayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_STATUS_SUBMESSAGE(const char* pre, const StatusPayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_WRITE_DATA_SUBMESSAGE(const char* pre, const WriteDataPayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_READ_DATA_SUBMESSAGE(const char* pre, const ReadDataPayload* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_DATA_SUBMESSAGE_SAMPLE_DATA(const char* pre, const BaseObjectReply* reply, const SampleData* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_DATA_SUBMESSAGE_SAMPLE(const char* pre, const BaseObjectReply* reply, const Sample* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_DATA_SUBMESSAGE_SAMPLE_DATA_SEQUENCE(const char* pre, const BaseObjectReply* reply, const SampleDataSequence* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_DATA_SUBMESSAGE_SAMPLE_SEQUENCE(const char* pre, const BaseObjectReply* reply, const SampleSequence* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif

void PRINT_DATA_SUBMESSAGE_PACKED_SAMPLES(const char* pre, const BaseObjectReply* reply, const PackedSamples* payload)
#ifndef MESSAGE_LOGS
{}
#else
;
#endif


#ifdef __cplusplus
}
#endif

#endif //_C_LOG_MESSAGE_PRIVATE_H_
