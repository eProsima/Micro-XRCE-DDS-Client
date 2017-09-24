#include "micrortps/client/debug/message_debugger.h"

#include <stdio.h>

#define YELLOW "\e[1;33m"
#define RESTORE_COLOR "\e[0m"

int print_message_header(const MessageHeaderSpec* header, void* data)
{
    printf("<Header> \n");
    printf("  - client_key: 0x%08X\n", header->client_key);
    printf("  - session_id: 0x%02X\n", header->session_id);
    printf("  - stream_id: 0x%02X\n", header->stream_id);
    printf("  - sequence_number: %u\n", header->sequence_number);
    printf("\n\n");

    return 1;
}

void print_submessage_header(const SubmessageHeaderSpec* header, void* data)
{
    switch(header->id)
    {
        case SUBMESSAGE_CREATE:
            printf("<Submessage> [CREATE] \n");
        break;
        case SUBMESSAGE_DELETE:
            printf("<Submessage> [DELETE]\n");
        break;
        case SUBMESSAGE_STATUS:
            printf("<Submessage> [STATUS]\n");
        break;
        case SUBMESSAGE_WRITE_DATA:
            printf("<Submessage> [WRITE_DATA]\n");
        break;
        case SUBMESSAGE_READ_DATA:
            printf("<Submessage> [READ_DATA]\n");
        break;
        case SUBMESSAGE_DATA:
            printf("<Submessage> [DATA]\n");
        break;
        default:
            printf("<Submessage> [*UNKNOWN*]\n");
    }

    printf("  <Submessage header> \n");
    printf("  - id: 0x%02X\n", header->id);
    printf("  - flags: 0x%02X\n", header->flags);
    printf("  - length: %u\n", header->length);
    printf("\n");
}

void print_create_submessage(const CreatePayloadSpec* payload, void* data)
{
    printf("  <Payload>\n");
    printf("  - request_id: 0x%08X\n", payload->request_id);
    printf("  - object_id: 0x%06X\n", payload->object_id);
    printf("  - kind: 0x%02X\n", payload->object.kind);

    const ObjectVariantSpec* variant = &payload->object.variant;
    switch(payload->object.kind)
    {
        case OBJECT_KIND_PARTICIPANT:
            printf("    <Data publisher>\n");
            printf("    - string_size: 0x%08X\n", payload->object.string_size);
            printf("    - string: %s\n", payload->object.string);
        break;

        case OBJECT_KIND_DATA_WRITER:
            printf("    <Data writer>\n");
            printf("    - string_size: 0x%08X\n", payload->object.string_size);
            printf("    - string: %s\n", payload->object.string);
            printf("    - participan_id: 0x%06X\n", variant->data_writer.participant_id);
            printf("    - publisher_id: 0x%06X\n", variant->data_writer.publisher_id);
        break;

        case OBJECT_KIND_DATA_READER:
            printf("    <Data reader>\n");
            printf("    - string_size: 0x%08X\n", payload->object.string_size);
            printf("    - string: %s\n", payload->object.string);
            printf("    - participan_id: 0x%06X\n", variant->data_reader.participant_id);
            printf("    - subscriber_id: 0x%06X\n", variant->data_reader.subscriber_id);
        break;

        case OBJECT_KIND_SUBSCRIBER:
            printf("    <Data subscriber>\n");
            printf("    - string_size: 0x%08X\n", payload->object.string_size);
            printf("    - string: %s\n", payload->object.string);
            printf("    - participan_id: 0x%06X\n", variant->subscriber.participant_id);
        break;

        case OBJECT_KIND_PUBLISHER:
            printf("    <Data publisher>\n");
            printf("    - string_size: 0x%08X\n", payload->object.string_size);
            printf("    - string: %s\n", payload->object.string);
            printf("    - participan_id: 0x%06X\n", variant->publisher.participant_id);
        break;
    }
    printf("\n\n");
}

void print_delete_submessage(const DeletePayloadSpec* payload, void* data)
{
    printf("  <Payload>\n");
    printf("  - request_id: 0x%08X\n", payload->request_id);
    printf("  - object_id: 0x%06X\n", payload->object_id);
    printf("\n\n");
}

void print_status_submessage(const StatusPayloadSpec* payload, void* data)
{
    printf("  <Payload>\n");
    printf("  - request_id: 0x%08X\n", payload->result.request_id);
    printf("  - kind: 0x%02X\n", payload->result.kind);
    printf("  - implementation: 0x%02X\n", payload->result.implementation);
    printf("  - object_id: 0x%06X\n", payload->object_id);

    printf("\n\n");
}

void print_write_data_submessage(const WriteDataPayloadSpec* payload, void* data)
{
    printf("  <Payload>\n");
    printf("  - request_id: 0x%08X\n", payload->request_id);
    printf("  - object_id: 0x%06X\n", payload->object_id);
    printf("  - read_mode: 0x%02X\n", payload->data_writer.read_mode);

    const SampleKindSpec* kind = &payload->data_writer.sample_kind;
    switch(payload->data_writer.read_mode)
    {
        case READ_MODE_DATA:
            printf("    <Data>\n");
            printf("    - serialized_data_size: 0x%08X\n", kind->data.serialized_data_size);
            printf("    - serialized_data: %s\n", (char*)kind->data.serialized_data);
        break;

        case READ_MODE_SAMPLE:
            printf("    - state: %02X\n", kind->sample.info.state);
            printf("    - sequence_number: 0x%08X\n", kind->sample.info.sequence_number);
            printf("    - session_time_offset: 0x%08X\n", kind->sample.info.session_time_offset);
            printf("    - serialized_data_size: 0x%08X\n", kind->sample.data.serialized_data_size);
            printf("    - serialized_data: %s\n", data_to_string(kind->sample.data.serialized_data,
                kind->sample.data.serialized_data_size));
        break;
    }
    printf("\n\n");
}

void print_read_data_submessage(const ReadDataPayloadSpec* payload, void* data)
{
    printf("  <Payload>\n");
    printf("  - request_id: 0x%08X\n", payload->request_id);
    printf("  - object_id: 0x%06X\n", payload->object_id);
    printf("  - max_messages: %hu\n", payload->max_messages);
    printf("  - read_mode: 0x%02X\n", payload->read_mode);
    printf("  - max_elapsed_time: %u\n", payload->max_elapsed_time);
    printf("  - max_rate: %u\n", payload->max_rate);
    //printf("  - expression_size: %u\n", payload->expression_size);
    //printf("  - content_filter_expression: %s\n", payload->content_filter_expression);
    printf("  - max_samples: %hu\n", payload->max_samples);
    printf("  - include_sample_info: 0x%02X\n", payload->include_sample_info);
    printf("\n\n");
}

void print_data_submessage(const DataPayloadSpec* payload, void* data)
{
    printf("  <Payload>\n");
    printf("  - request_id: 0x%08X\n", payload->request_id);
    printf("  - object_id: 0x%06X\n", payload->object_id);
    printf("  - read_mode: 0x%02X\n", payload->data_reader.read_mode);

    const SampleKindSpec* kind = &payload->data_reader.sample_kind;
    switch(payload->data_reader.read_mode)
    {
        case READ_MODE_DATA:
            printf("    <Data>\n");
            printf("    - serialized_data_size: 0x%08X\n", kind->data.serialized_data_size);
            printf("    - serialized_data: %s\n", data_to_string(kind->data.serialized_data,
                kind->data.serialized_data_size));
        break;

        case READ_MODE_SAMPLE:
            printf("    - state: 0x%02X\n", kind->sample.info.state);
            printf("    - sequence_number: 0x%08X\n", kind->sample.info.sequence_number);
            printf("    - session_time_offset: 0x%08X\n", kind->sample.info.session_time_offset);
            printf("    - serialized_data_size: 0x%08X\n", kind->sample.data.serialized_data_size);
            printf("    - serialized_data: %s\n", data_to_string(kind->sample.data.serialized_data,
                kind->sample.data.serialized_data_size));

        break;
    }
    printf("\n\n");
}

void printl_create_submessage(const CreatePayloadSpec* payload, void* data)
{
    char content[128];
    switch(payload->object.kind)
    {
        case OBJECT_KIND_PARTICIPANT:
            sprintf(content, "PARTICIPANT");
        break;
        case OBJECT_KIND_PUBLISHER:
            sprintf(content, "PUBLISHER | id: %u | topic: %s",
                    payload->object.variant.publisher.participant_id,
                    payload->object.string);
        break;
        case OBJECT_KIND_SUBSCRIBER:
            sprintf(content, "SUBSCRIBER | id: %u | topic: %s",
                    payload->object.variant.subscriber.participant_id,
                    payload->object.string);
        break;
        default:
            sprintf(content, "UNKNOWN");
    }

    printf("%s[Create | #%08X | id: %u | %s]%s\n", YELLOW,
            payload->request_id,
            payload->object_id,
            content,
            RESTORE_COLOR);
}

void printl_delete_submessage(const DeletePayloadSpec* payload, void* data)
{
    printf("%s[Delete | #%08X | id: %u]%s\n", YELLOW,
            payload->request_id,
            payload->object_id,
            RESTORE_COLOR);
}

void printl_status_submessage(const StatusPayloadSpec* payload, void* data)
{
    char kind[64];
    switch(payload->result.kind)
    {
        case STATUS_LAST_OP_NONE:
            sprintf(kind, "NONE");
        break;
        case STATUS_LAST_OP_CREATE:
            sprintf(kind, "CREATE");
        break;
        case STATUS_LAST_OP_UPDATE:
            sprintf(kind, "UPDATE");
        break;
        case STATUS_LAST_OP_DELETE:
            sprintf(kind, "DELETE");
        break;
        case STATUS_LAST_OP_LOOKUP:
            sprintf(kind, "LOOKUP");
        break;
        case STATUS_LAST_OP_READ:
            sprintf(kind, "READ");
        break;
        case STATUS_LAST_OP_WRITE:
            sprintf(kind, "WRITE");
        break;
        default:
            sprintf(kind, "UNKNOWN");
    }

    char implementation[64];
    switch(payload->result.implementation)
    {
        case STATUS_OK:
            sprintf(implementation, "OK");
        break;
        case STATUS_OK_MATCHED:
            sprintf(implementation, "OK_MATCHED");
        break;
        case STATUS_ERR_DDS_ERROR:
            sprintf(implementation, "ERR_DDS_ERROR");
        break;
        case STATUS_ERR_MISMATCH:
            sprintf(implementation, "ERR_MISMATCH");
        break;
        case STATUS_ERR_ALREADY_EXISTS:
            sprintf(implementation, "ERR_ALREADY_EXISTS");
        break;
        case STATUS_ERR_DENIED:
            sprintf(implementation, "ERR_DENIED");
        break;
        case STATUS_ERR_UNKNOWN_REFERENCE:
            sprintf(implementation, "ERR_UNKNOWN_REFERENCE");
        break;
        case STATUS_ERR_INVALID_DATA:
            sprintf(implementation, "ERR_INVALID_DATA");
        break;
        case STATUS_ERR_INCOMPATIBLE:
            sprintf(implementation, "ERR_INCOMPATIBLE");
        break;
        case STATUS_ERR_RESOURCES:
            sprintf(implementation, "ERR_RESOURCES");
        break;
        default:
            sprintf(implementation, "UNKNOWN");
    }



    printf("%s[Status | #%08X | id: %u | %s | %s]%s\n", YELLOW,
            payload->result.request_id,
            payload->object_id,
            kind,
            implementation,
            RESTORE_COLOR);
}

void printl_write_data_submessage(const WriteDataPayloadSpec* payload, void* data)
{
    char content[64];
    const SampleKindSpec* kind = &payload->data_writer.sample_kind;
    switch(payload->data_writer.read_mode)
    {
        case READ_MODE_DATA:
            sprintf(content, "DATA | data size: %u",
                    kind->data.serialized_data_size);
        break;

        case READ_MODE_SAMPLE:
            sprintf(content, "DATA | sq_nr: %u | session time: %u | data size: %u",
                    kind->sample.info.sequence_number,
                    kind->sample.info.session_time_offset,
                    kind->sample.data.serialized_data_size);
        break;
    }

    printf("%s[Write data | #%08X | id: %u | %s]%s\n", YELLOW,
            payload->request_id,
            payload->object_id,
            content,
            RESTORE_COLOR);
}

void printl_read_data_submessage(const ReadDataPayloadSpec* payload, void* data)
{
    printf("%s[Read data | #%08X | id: %u | max messages: %u]%s\n", YELLOW,
            payload->request_id,
            payload->object_id,
            payload->max_messages,
            RESTORE_COLOR);
}

void printl_data_submessage(const DataPayloadSpec* payload, void* data)
{
    char content[64];
    const SampleKindSpec* kind = &payload->data_reader.sample_kind;
    switch(payload->data_reader.read_mode)
    {
        case READ_MODE_DATA:
            sprintf(content, "DATA | data size: %u",
                    kind->data.serialized_data_size);
        break;

        case READ_MODE_SAMPLE:
            sprintf(content, "DATA | sq_nr: %u | session time: %u | data size: %u",
                    kind->sample.info.sequence_number,
                    kind->sample.info.session_time_offset,
                    kind->sample.data.serialized_data_size);
        break;
    }

    printf("%s[Data | #%08X | id: %u | %s]%s\n", YELLOW,
            payload->request_id,
            payload->object_id,
            content,
            RESTORE_COLOR);
}

const char* data_to_string(const uint8_t* data, uint32_t size)
{
    static char buffer[1024];
    for(uint32_t i = 0; i < size; i++)
        sprintf(buffer + 3 * i, "%02X ", data[i]);
    sprintf(buffer + 5 * size, "\n");
    return buffer;
}