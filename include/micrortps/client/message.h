#ifndef _DDS_XRCE_MESSAGE_H_
#define _DDS_XRCE_MESSAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "micrortps/client/microcdr.h"
#include "micrortps/client/xrce_protocol_spec.h"


typedef struct DataPayload
{
    BaseObjectReply reply;
    DataPayloadUnion payload;
}

typedef union DataPayloadUnion
{
    SampleData data;
    Sample sample;
    SampleDataSequence data_sequence;
    SampleSequence sample_sequence;
    PackageSamples packed_samples;
};

typedef struct MessageCallback
{
    //serialize callbacks
    int (*on_initialize_message)(MessageHeaderSpec* header, void* callback_object);

    //deserialize callbacks
    int  (*on_message_header)   (const MessageHeader* header, void* callback_object);
    void (*on_submessage_header)(const SubmessageHeader* header, void* callback_object);

    void (*on_create_resource_submessage)(const CreateResourcePayload* payload, void* callback_object);
    void (*on_delete_resource_submessage)(const DeleteResourcePayload* payload, void* callback_object);
    void (*on_resource_status_submessage)(const ResourceStatusPayload* payload, void* callback_object);
    void (*on_get_info_submessage)       (const GetInfoPayload* payload, void* callback_object);
    void (*on_info_submessage)           (const InfoPayload* payload, void* callback_object);
    void (*on_read_data_submessage)      (const ReadDataPayload* payload, void* callback_object);
    void (*on_data_submessage)           (const DataPayload* payload, void* callback_object);
    void (*on_write_data_submessage)     (const WriteDataPayload* payload, void* callback_object);

    void* callback_object;

} MessageCallback;

typedef struct WriterState
{
    uint8_t* position;
    uint8_t* last_data_size;

} WriterState;

typedef struct MessageManager
{
    uint8_t* out_buffer; //64 bytes minimun
    uint8_t* in_buffer;  //64 bytes minimun

    MicroBuffer writer;
    WriterState state;

    MessageCallback callback;

} MessageManager;

void init_message_manager(MessageManager* message, MessageCallback callback
                          uint8_t* out_buffer, uint32_t out_buffer_size,
                          uint8_t* in_buffer, uint32_t in_buffer_size);

uint32_t message_create_resource(MessageManager* message, const CreateResourcePayload* payload);
uint32_t message_delete_resource(MessageManager* message, const DeleteResourcePayload* payload);
uint32_t message_resource_status(MessageManager* message, const ResourceStatusPayload* payload);
uint32_t message_get_info       (MessageManager* message, const GetInfoPayload* payload);
uint32_t message_info           (MessageManager* message, const InfoPayload* payload);
uint32_t message_read_data      (MessageManager* message, const ReadDataPayload* payload);
uint32_t message_data           (MessageManager* message, const DataPayload* payload);
uint32_t message_write_data     (MessageManager* message, const WriteDataPayload* payload);

int parse_message(MessageManager* message_manager, uint32_t message_length);


#ifdef __cplusplus
}
#endif

#endif //_DDS_XRCE_MESSAGE_H_