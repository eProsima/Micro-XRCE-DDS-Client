#include "micrortps/client/message.h"
#include "micrortps/client/xrce_protocol_serialization.h"

#ifndef NDEBUG
#include <string.h>
#include <stdio.h>
#endif

// PRIVATE DEFINITIONS
int check_and_set_headers(MessageManager* message_manager, const SubmessageHeaderSpec* header);


void init_message_manager(MessageManager* message_manager, MessageCallback callback,
                          uint8_t* out_buffer, uint32_t out_buffer_size,
                          uint8_t* in_buffer, uint32_t in_buffer_size)
{
    message_manager->out_buffer = out_buffer;
    message_manager->in_buffer = in_buffer;

    init_serialized_buffer(&message_manager->writer, message_manager->out_buffer, out_buffer_size);
    init_serialized_buffer(&message_manager->reader, message_manager->in_buffer, in_buffer_size);

    message_manager->callback = callback;
}

int check_and_set_headers(MessageManager* message_manager, const SubmessageHeaderSpec* header)
{
    SerializedBufferHandle* writer = &message_manager->writer;

    if(writer->iterator == writer->data)
    {
        MessageHeaderSpec message_header;
        if(!message_manager->callback.on_initialize_message(&message_header, message_manager->callback.data))
            return 0;

        serialize_message_header(writer, &message_header);
    }
    else //because the message header is already alignment.
    {
        //align_to(writer, 4);
    }

    if(writer->iterator + sizeof(SubmessageHeaderSpec) + header->length > writer->final)
    {
        #ifndef NDEBUG
        printf("******* Serialization problem *******\n");
        #endif
        return 0;
    }

    serialize_submessage_header(writer, header);
    return 1;
}



uint32_t message_create_resource(MessageManager* message, const CreateResourcePayload* payload)
{
    begin_submessage(message);
    serialize_CreateResourcePayload(&message->writer, payload);
    return end_submessage(SubmessageId::CREATE, 0x07);
}

int parse_message(MessageManager* message_manager, uint32_t message_length)
{
    MessageCallback* callback = &message_manager->callback;
    MemoryCache* cache = &message_manager->cache;
    SerializedBufferHandle* reader = &message_manager->reader;
    reset_buffer_iterator(reader);

    if(reader->data + message_length > reader->final || message_length < sizeof(MessageHeaderSpec))
        return 0;

    MessageHeaderSpec message_header;
    deserialize_message_header(reader, cache, &message_header);
    if(callback->on_message_header)
        if(!callback->on_message_header(&message_header, callback->data))
            return 0;

    do
    {
        SubmessageHeaderSpec submessage_header;
        deserialize_submessage_header(reader, cache, &submessage_header);
        if(callback->on_submessage_header)
            callback->on_submessage_header(&submessage_header, callback->data);

        if(reader->iterator + submessage_header.length > reader->data + message_length)
            return 0;

        //buffer->endian_mode = //set with flags

        reset_memory_cache(cache, message_length);

        PayloadSpec payload;
        #ifndef NDEBUG
        memset(&payload, 0xFF, sizeof(PayloadSpec));

        #endif

        switch(submessage_header.id)
        {
            case SUBMESSAGE_CREATE:
                deserialize_create_payload(reader, cache, &payload.create_resource);
                if(callback->on_create_resource)
                    callback->on_create_resource(&payload.create_resource, callback->data);
            break;

            case SUBMESSAGE_DELETE:
                deserialize_delete_payload(reader, cache, &payload.delete_resource);
                if(callback->on_delete_resource)
                    callback->on_delete_resource(&payload.delete_resource, callback->data);
            break;

            case SUBMESSAGE_STATUS:
                deserialize_status_payload(reader, cache, &payload.status);
                if(callback->on_status)
                    callback->on_status(&payload.status, callback->data);
            break;

            case SUBMESSAGE_WRITE_DATA:
                deserialize_write_data_payload(reader, cache, &payload.write_data);
                if(callback->on_write_data)
                    callback->on_write_data(&payload.write_data, callback->data);
            break;

            case SUBMESSAGE_READ_DATA:
                deserialize_read_data_payload(reader, cache, &payload.read_data);
                if(callback->on_read_data)
                    callback->on_read_data(&payload.read_data, callback->data);
            break;

            case SUBMESSAGE_DATA:
                deserialize_data_payload(reader, cache, &payload.data);
                if(callback->on_data)
                    callback->on_data(&payload.data, callback->data);
            break;

            default:
                return 0;
        }

        //align_to(reader, 4);
    }
    while(reader->iterator + sizeof(SubmessageHeaderSpec) <= reader->data + message_length);

    return 1;
}