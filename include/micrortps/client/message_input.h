#ifndef _MICRORTPS_CLIENT_MESSAGE_INPUT_H_
#define _MICRORTPS_CLIENT_MESSAGE_INPUT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "micrortps/client/xrce_protocol_spec.h"
#include "micrortps/client/aux_memory.h"

#include <microcdr/microcdr.h>

typedef union DataPayloadUnion
{
    SampleData data;
    Sample sample;
    SampleDataSequence data_sequence;
    SampleSequence sample_sequence;
    PackedSamples packed_samples;

} DataPayloadUnion;


typedef struct DataPayload
{
    BaseObjectReply reply;
    DataPayloadUnion payload;

} DataPayload;


typedef struct MessageInputCallback
{
    //serialize callbacks
    int (*on_initialize_message)(MessageHeader* header, void* callback_object);

    //deserialize callbacks
    int  (*on_message_header)   (const MessageHeader* header, void* callback_object);
    void (*on_submessage_header)(const SubmessageHeader* header, void* callback_object);

    void (*on_status_submessage)(const StatusPayload* payload, void* callback_object);
    void (*on_info_submessage)  (const InfoPayload* payload, void* callback_object);
    void (*on_data_submessage)  (const DataPayload* payload, void* callback_object);

    void* object;

} MessageInputCallback;

int parse_submessage(MicroBuffer* reader, AuxMemory* aux_memory, MessageInputCallback* callback);
int parse_message(uint8_t* message_data, uint32_t length, MessageInputCallback* callback);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_MESSAGE_INPUT_H_