#ifndef _MICRORTPS_CLIENT_INPUT_MESSAGE_H_
#define _MICRORTPS_CLIENT_INPUT_MESSAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "micrortps/client/xrce_protocol_spec.h"
#include "micrortps/client/aux_memory.h"

#include <microcdr/microcdr.h>


typedef struct InputMessageCallback
{
    // Headers
    int  (*on_message_header)   (const MessageHeader* header, const ClientKey* key, void* callback_object);
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

} InputMessageCallback;

typedef struct InputMessage
{
    MicroBuffer reader;
    AuxMemory aux_memory;
    InputMessageCallback callback;

} InputMessage;

void init_input_message(InputMessage* message, InputMessageCallback callback, uint8_t* in_buffer, uint32_t in_buffer_size);
void free_input_message(InputMessage* message);

void parse_data_payload(InputMessage* message, DataFormat format);
int parse_submessage(InputMessage* message);

int parse_message(InputMessage* message, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_INPUT_MESSAGE_H_