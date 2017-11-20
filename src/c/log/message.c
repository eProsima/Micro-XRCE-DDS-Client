#include "message.h"

#define YELLOW_DARK    "\e[0;33m"
#define PURPLE_DARK    "\e[0;35m"
#define GREY_LIGHT     "\e[0;37m"

#define GREY_DARK      "\e[1;30m"
#define RED            "\e[1;31m"
#define GREEN          "\e[1;32m"
#define YELLOW         "\e[1;33m"
#define BLUE           "\e[1;34m"
#define PURPLE         "\e[1;35m"
#define CYAN           "\e[1;36m"
#define WHITE          "\e[1;37m"

#define RESTORE_COLOR  "\e[0m"

#if defined(SERIALIZATION_LOGS) || defined(MESSAGE_LOGS)
//Only one call into a printf.
const char* data_to_string(const uint8_t* data, uint32_t size);

const char* data_to_string(const uint8_t* data, uint32_t size)
{
    static char buffer[4096];
    for(uint32_t i = 0; i < size; i++)
        sprintf(buffer + 3 * i, "%02X ", data[i]);
    return buffer;
}
#endif

#ifdef MESSAGE_LOGS
const char* request_to_string(const BaseObjectRequest* request);
const char* reply_to_string(const BaseObjectReply* reply);

const char* request_to_string(const BaseObjectRequest* request)
{
    static char buffer[256];
    sprintf(buffer, "#0x%04X | id: 0x%04X",
            get_num_request_id(request->base.request_id),
            get_num_object_id(request->object_id));

    return buffer;
}

const char* reply_to_string(const BaseObjectReply* reply)
{
    static char buffer[256];
    char kind[64];
    switch(reply->base.result.status)
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
    switch(reply->base.result.implementation_status)
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

    sprintf(buffer, "#0x%04X | id: 0x%04X | from #0x%04X | %s | %s",
            get_num_request_id(reply->base.request_id),
            get_num_object_id(reply->object_id),
            get_num_request_id(reply->base.result.request_id),
            kind, implementation);

    return buffer;
}
#endif

#ifdef SERIALIZATION_LOGS
void PRINTL_SERIALIZATION(const char* pre, const uint8_t* buffer, uint32_t size)
{
    printf("%s%s<< [%u]: %s>>%s\n",
            pre,
            GREY_DARK,
            size,
            data_to_string(buffer, size),
            RESTORE_COLOR);
}
#endif

#ifdef MESSAGE_LOGS
void PRINT_SEQUENCE_NUMBER(uint16_t message_sequence_number, uint16_t local_sequence_number)
{
    printf("%s%sSequence number error => received: %u, expected: %u.%s\n",
            RECV,
            RED,
            message_sequence_number,
            local_sequence_number,
            RESTORE_COLOR);
}

void PRINTL_CREATE_CLIENT_SUBMESSAGE(const CreateClientPayload* payload)
{
    printf("%s%s[Create client | %s | session: 0x%02X]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->request),
            payload->representation.session_id,
            RESTORE_COLOR);
}

void PRINTL_CREATE_RESOURCE_SUBMESSAGE(const CreateResourcePayload* payload)
{
    char content[4096];
    switch(payload->representation.kind)
    {
        case OBJK_PARTICIPANT:
            sprintf(content, "PARTICIPANT");
        break;

        case OBJK_TOPIC:
            sprintf(content, "TOPIC | id: 0x%04X | topic: %u",
                    get_num_object_id(payload->representation._.data_reader.participant_id),
                    payload->representation._.data_reader.base3._.xml.size);
        break;

        case OBJK_PUBLISHER:
            sprintf(content, "PUBLISHER | id: 0x%04X",
                    get_num_object_id(payload->representation._.publisher.participant_id));
        break;

        case OBJK_SUBSCRIBER:
            sprintf(content, "SUBSCRIBER | id: 0x%04X",
                    get_num_object_id(payload->representation._.subscriber.participant_id));
        break;

        case OBJK_DATAWRITER:
            sprintf(content, "DATA_WRITER | id: 0x%04X | id: 0x%04X | xml: %u",
                    get_num_object_id(payload->representation._.data_writer.participant_id),
                    get_num_object_id(payload->representation._.data_writer.publisher_id),
                    payload->representation._.data_writer.base3._.xml.size);
        break;

        case OBJK_DATAREADER:
            sprintf(content, "DATA_READER | id: 0x%04X | id: 0x%04X | xml: %u",
                    get_num_object_id(payload->representation._.data_reader.participant_id),
                    get_num_object_id(payload->representation._.data_reader.subscriber_id),
                    payload->representation._.data_reader.base3._.xml.size);
        break;

        default:
            sprintf(content, "UNKNOWN");
    }

    printf("%s%s[Create | %s | %s]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->request),
            content,
            RESTORE_COLOR);
}

void PRINTL_DELETE_RESOURCE_SUBMESSAGE(const DeleteResourcePayload* payload)
{
    printf("%s%s[Delete | %s]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->request),
            RESTORE_COLOR);
}

void PRINTL_STATUS_SUBMESSAGE(const StatusPayload* payload)
{
    printf("%s%s[Status | %s]%s\n",
            RECV,
            PURPLE,
            reply_to_string(&payload->reply),
            RESTORE_COLOR);
}

void PRINTL_WRITE_DATA_SUBMESSAGE(const WriteDataPayload* payload)
{
    char content[1024];
    switch(payload->data_to_write.format)
    {
        case FORMAT_DATA:
            sprintf(content, "DATA | size: %u", payload->data_to_write._.data.size);
        break;
        case FORMAT_DATA_SEQ:
        break;
        case FORMAT_SAMPLE:
        break;
        case FORMAT_SAMPLE_SEQ:
        break;
        case FORMAT_PACKED_SAMPLES:
        break;
    }

    printf("%s%s[Write data | %s | %s]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->request),
            content,
            RESTORE_COLOR);
}

void PRINTL_READ_DATA_SUBMESSAGE(const ReadDataPayload* payload)
{
    char format[128];
    switch(payload->read_specification.optional_delivery_config)
    {
        case FORMAT_DATA:
            sprintf(format, "FORMAT_DATA");
        break;
        case FORMAT_DATA_SEQ:
            sprintf(format, "FORMAT_DATA_SEQ");
        break;
        case FORMAT_SAMPLE:
            sprintf(format, "FORMAT_SAMPLE");
        break;
        case FORMAT_SAMPLE_SEQ:
            sprintf(format, "FORMAT_SAMPLE_SEQ");
        break;
        case FORMAT_PACKED_SAMPLES:
            sprintf(format, "FORMAT_PACKED_SAMPLES");
        break;
        default:
            sprintf(format, "FORMAT_DATA_UNKNOWN");
    }

    printf("%s%s[Read data | %s | %s]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->request),
            format,
            RESTORE_COLOR);
}

void PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA(const BaseObjectReply* reply, const SampleData* payload)
{
    char content[64];
    printf("%s%s[Data | %s | FORMAT_DATA | size: %u]%s\n",
            RECV,
            PURPLE,
            reply_to_string(reply),
            payload->size,
            RESTORE_COLOR);
}

void PRINTL_DATA_SUBMESSAGE_SAMPLE(const BaseObjectReply* reply, const Sample* payload)
{
    //TODO
}

void PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA_SEQUENCE(const BaseObjectReply* reply, const SampleDataSequence* payload)
{
    //TODO
}

void PRINTL_DATA_SUBMESSAGE_SAMPLE_SEQUENCE(const BaseObjectReply* reply, const SampleSequence* payload)
{
    //TODO
}

void PRINTL_DATA_SUBMESSAGE_PACKED_SAMPLES(const BaseObjectReply* reply, const PackedSamples* payload)
{
    //TODO
}
#endif