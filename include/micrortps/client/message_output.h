#ifndef _MICRORTPS_CLIENT_MESSAGE_OUTPUT_H_
#define _MICRORTPS_CLIENT_MESSAGE_OUTPUT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "micrortps/client/xrce_protocol_spec.h"

#include <microcdr/microcdr.h>

typedef struct MessageOutputCallback
{
    //serialize callbacks
    int (*on_initialize_message)(MessageHeader* header, void* callback_object);

    void* object;

} MessageOutputCallback;

typedef struct MessageOutput
{
    uint8_t* out_buffer; //64 bytes minimun

    MicroBuffer writer;

    MessageOutputCallback callback;

} MessageOutput;

void init_message_output(MessageOutput* message, MessageOutputCallback callback,
                          uint8_t* out_buffer, uint32_t out_buffer_size);

bool add_create_client_submessage  (MessageOutput* message, const CreateClientPayload* payload);
bool add_create_resource_submessage(MessageOutput* message, const CreateResourcePayload* payload, uint8_t creation_mode);
bool add_delete_resource_submessage(MessageOutput* message, const DeleteResourcePayload* payload);
bool add_get_info_submessage       (MessageOutput* message, const GetInfoPayload* payload);
bool add_read_data_submessage      (MessageOutput* message, const ReadDataPayload* payload);
bool add_write_data_submessage     (MessageOutput* message, const WriteDataPayload* payload);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_MESSAGE_OUTPUT_H_