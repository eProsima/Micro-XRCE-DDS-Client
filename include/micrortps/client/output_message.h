#ifndef _MICRORTPS_CLIENT_OUTPUT_MESSAGE_H_
#define _MICRORTPS_CLIENT_OUTPUT_MESSAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"

#include <microcdr/microcdr.h>

typedef struct OutputMessageCallback
{
    void (*on_initialize_message)(MessageHeader* header, ClientKey* key, void* args);
    void (*on_submessage_header)(const SubmessageHeader* header, void* args);
    void* args;

} OutputMessageCallback;

typedef struct OutputMessage
{
    MicroBuffer writer;
    OutputMessageCallback callback;

} OutputMessage;

typedef enum CreationMode
{
    CREATION_MODE_REPLACE = FLAG_REPLACE,
    CREATION_MODE_REUSE = FLAG_REUSE

} CreationMode;

void init_output_message(OutputMessage* message, OutputMessageCallback callback,
                          uint8_t* out_buffer, uint32_t out_buffer_size);

uint32_t get_message_length(OutputMessage* message);

bool add_create_client_submessage  (OutputMessage* message, const CreateClientPayload* payload);
bool add_create_resource_submessage(OutputMessage* message, const CreateResourcePayload* payload, CreationMode creation_mode);
bool add_delete_resource_submessage(OutputMessage* message, const DeleteResourcePayload* payload);
bool add_get_info_submessage       (OutputMessage* message, const GetInfoPayload* payload);
bool add_read_data_submessage      (OutputMessage* message, const ReadDataPayload* payload);
bool add_write_data_submessage     (OutputMessage* message, const WriteDataPayload* payload);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_OUTPUT_MESSAGE_H_