#include "message.h"

#define BROWN          "\e[0;33m"
#define PURPLE         "\e[0;35m"

#define GREEN          "\e[1;32m"
#define YELLOW         "\e[1;33m"
#define LIGHT_PURPLE   "\e[1;35m"

#define RESTORE_COLOR  "\e[0m"

#ifdef MESSAGE_LOGS
#define SEND_STR "==>"
#define RECV_STR "<=="
#endif

#if defined(SERIALIZATION_LOGS) || defined(MESSAGE_LOGS)
const char* data_to_string(const uint8_t* data, uint32_t size)
{
    static char buffer[1024];
    for(uint32_t i = 0; i < size; i++)
        sprintf(buffer + 3 * i, "%02X ", data[i]);
    sprintf(buffer + 5 * size, "\n");
    return buffer;
}
#endif

#ifdef SERIALIZATION_LOGS
void PRINT_SERIALIZATION(const char* pre, const uint8_t* buffer, uint32_t size)
{
    printf("%s%s<< %s>>%s\n",
            pre,
            BROWN,
            data_to_string(buffer, size),
            RESTORE_COLOR);
}
#endif

#ifdef MESSAGE_LOGS
void PRINTL_CREATE_CLIENT_SUBMESSAGE(const char* pre, const CreateClientPayload* payload)
{
    printf("%s%s[Create | session: 0x%02X]%s\n",
            pre,
            YELLOW,
            payload->representation.session_id,
            RESTORE_COLOR);
}

void PRINTL_CREATE_RESOURCE_SUBMESSAGE(const char* pre, const CreateResourcePayload* payload)
{
    char content[128];
    switch(payload->representation.kind)
    {
        case OBJK_PARTICIPANT:
            sprintf(content, "PARTICIPANT");
            break;

        case OBJK_PUBLISHER:
            sprintf(content, "PUBLISHER | id: %s",
                    data_to_string(payload->representation._.publisher.participant_id.data, sizeof(ObjectId)));
            break;

        case OBJK_SUBSCRIBER:
            sprintf(content, "SUBSCRIBER | id: %s",
                    data_to_string(payload->representation._.subscriber.participant_id.data, sizeof(ObjectId)));
            break;

        case OBJK_DATAWRITER:
            sprintf(content, "DATA_WRITER | id: %s | id: %s | topic: %s",
                    data_to_string(payload->representation._.data_writer.participant_id.data, sizeof(ObjectId)),
                    data_to_string(payload->representation._.data_writer.publisher_id.data, sizeof(ObjectId)),
                    payload->representation._.data_writer.base3._.object_name.data);
            break;

        case OBJK_DATAREADER:
            sprintf(content, "DATA_READER | id: %s | id: %s | topic: %s",
                    data_to_string(payload->representation._.data_reader.participant_id.data, sizeof(ObjectId)),
                    data_to_string(payload->representation._.data_reader.subscriber_id.data, sizeof(ObjectId)),
                    payload->representation._.data_reader.base3._.object_name.data);
            break;

        default:
            sprintf(content, "UNKNOWN");
    }

    printf("%s%s[Create | #%s | id: %s | %s]%s\n",
            pre,
            YELLOW,
            data_to_string(payload->request.base.request_id.data, sizeof(RequestId)),
            data_to_string(payload->request.object_id.data, sizeof(ObjectId)),
            content,
            RESTORE_COLOR);
}

void PRINTL_DELETE_RESOURCE_SUBMESSAGE(const char* pre, const DeleteResourcePayload* payload)
{

}

void PRINTL_STATUS_SUBMESSAGE(const char* pre, const StatusPayload* payload)
{
    char kind[64];
    switch(payload->reply.base.result.status)
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
    switch(payload->reply.base.result.implementation_status)
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

    printf("%s%s[Status | #%s | id: %s | from #%s | %s | %s]%s\n",
            pre,
            YELLOW,
            data_to_string(payload->reply.base.request_id.data, sizeof(RequestId)),
            data_to_string(payload->reply.object_id.data, sizeof(ObjectId)),
            data_to_string(payload->reply.base.result.request_id.data, sizeof(RequestId)),
            kind, implementation,
            RESTORE_COLOR);
}

void PRINTL_WRITE_DATA_SUBMESSAGE(const char* pre, const WriteDataPayload* payload)
{

}

void PRINTL_READ_DATA_SUBMESSAGE(const char* pre, const ReadDataPayload* payload)
{

}

void PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA(const char* pre, const BaseObjectReply* reply, const SampleData* payload)
{

}

void PRINTL_DATA_SUBMESSAGE_SAMPLE(const char* pre, const BaseObjectReply* reply, const Sample* payload)
{

}

void PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA_SEQUENCE(const char* pre, const BaseObjectReply* reply, const SampleDataSequence* payload)
{

}

void PRINTL_DATA_SUBMESSAGE_SAMPLE_SEQUENCE(const char* pre, const BaseObjectReply* reply, const SampleSequence* payload)
{

}

void PRINTL_DATA_SUBMESSAGE_PACKED_SAMPLES(const char* pre, const BaseObjectReply* reply, const PackedSamples* payload)
{

}
#endif