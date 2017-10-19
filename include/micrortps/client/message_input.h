#ifndef _MICRORTPS_CLIENT_MESSAGE_INPUT_H_
#define _MICRORTPS_CLIENT_MESSAGE_INPUT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "micrortps/client/xrce_protocol_spec.h"
#include "micrortps/client/aux_memory.h"

#include <microcdr/microcdr.h>


typedef struct MessageInputCallback
{
    // Headers
    int  (*on_message_header)   (const MessageHeader* header, void* callback_object);
    void (*on_submessage_header)(const SubmessageHeader* header, void* callback_object);

    // Submessages
    void (*on_status_submessage)(const StatusPayload* payload, void* callback_object);
    void (*on_info_submessage)  (const InfoPayload* payload, void* callback_object);
    DataFormat (*on_data_submessage)  (const BaseObjectReply* data_reply, void* callback_object);

    // Data payloads
    int (*on_data_payload)           (const SampleData* data, void* callback_object);
    int (*on_sample_payload)         (const Sample* sample, void* callback_object);
    int (*on_data_sequence_payload)  (const SampleDataSequence* data_sequence, void* callback_object);
    int (*on_sample_sequence_payload)(const SampleSequence* sample_sequence, void* callback_object);
    int (*on_packed_samples_payload) (const PackedSamples* packed_samples, void* callback_object);

    void* object;

} MessageInputCallback;

void parse_data_payload(MicroBuffer* reader, AuxMemory* aux_memory, MessageInputCallback* callback, DataFormat format);
int parse_submessage(MicroBuffer* reader, AuxMemory* aux_memory, MessageInputCallback* callback);

int parse_message(uint8_t* message_data, uint32_t length, AuxMemory* aux_memory, MessageInputCallback* callback);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_MESSAGE_INPUT_H_