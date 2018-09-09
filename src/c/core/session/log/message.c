// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "message.h"

#include <micrortps/client/core/session/session_info.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/util/time.h>
#include <micrortps/client/core/serialization/xrce_header.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>

#include <string.h>
#include <stdio.h>

#define YELLOW_DARK    "\x1B[0;33m"
#define PURPLE_DARK    "\x1B[0;35m"
#define GREY_LIGHT     "\x1B[0;37m"

#define GREY_DARK      "\x1B[1;30m"
#define RED            "\x1B[1;31m"
#define GREEN          "\x1B[1;32m"
#define YELLOW         "\x1B[1;33m"
#define BLUE           "\x1B[1;34m"
#define PURPLE         "\x1B[1;35m"
#define CYAN           "\x1B[1;36m"
#define WHITE          "\x1B[1;37m"

#define RESTORE_COLOR  "\x1B[0m"

#define SEND_ARROW       "=>> "
#define RECV_ARROW       "<<= "

#define DATA_TO_STRING_BUFFER     4096

static char* print_array_2(const uint8_t* array_2);
static const char* data_to_string(const uint8_t* data, uint32_t size);
static const char* request_to_string(const BaseObjectRequest* request);
static const char* reply_to_string(const BaseObjectReply* reply);
static void print_create_client_submessage(const char* pre, const CREATE_CLIENT_Payload* payload);
static void print_create_submessage(const char* pre, const CREATE_Payload* payload, uint8_t flags);
//static void print_get_info_submessage(const char* pre, const GET_INFO_Payload* payload);
static void print_delete_submessage(const char* pre, const DELETE_Payload* payload);
static void print_status_agent_submessage(const char* pre, const STATUS_AGENT_Payload* payload);
static void print_status_submessage(const char* pre, const STATUS_Payload* payload);
//static void print_info_submessage(const char* pre, const INFO_Payload* payload);
static void print_read_data_submessage(const char* pre, const READ_DATA_Payload* payload);
static void print_write_data_data_submessage(const char* pre, const WRITE_DATA_Payload_Data* payload);
static void print_data_data_submessage(const char* pre, const DATA_Payload_Data* payload);
static void print_acknack_submessage(const char* pre, const ACKNACK_Payload* payload);
static void print_heartbeat_submessage(const char* pre, const HEARTBEAT_Payload* payload);
static void print_header(size_t size, int direction, uint8_t stream_id, uint16_t seq_num, const uint8_t* client_key);
static void print_tail(int64_t initial_log_time);


//==================================================================
//                             PUBLIC
//==================================================================
void print_message(int direction, uint8_t* buffer, size_t size, const uint8_t* client_key)
{
    static int64_t initial_log_time = 0;

    const char* color = (direction == SEND) ? YELLOW : PURPLE;

    MicroBuffer mb;
    init_micro_buffer(&mb, buffer, (uint32_t)size);

    uint8_t session_id; uint8_t stream_id_raw; uint16_t seq_num; uint8_t key[CLIENT_KEY_SIZE];
    (void) deserialize_message_header(&mb, &session_id, &stream_id_raw, &seq_num, key);

    print_header(size, direction, stream_id_raw, seq_num, client_key);

    size_t submessage_counter = 0;
    uint8_t submessage_id; uint16_t length; uint8_t flags; uint8_t* payload_it = NULL;
    while(read_submessage_header(&mb, &submessage_id, &length, &flags, &payload_it))
    {
        if(submessage_counter != 0)
        {
            printf("\n                                        ");
        }

        switch(submessage_id)
        {
            case SUBMESSAGE_ID_CREATE_CLIENT:
            {
                initial_log_time = get_milli_time();
                CREATE_CLIENT_Payload payload;
                deserialize_CREATE_CLIENT_Payload(&mb, &payload);
                print_create_client_submessage(color, &payload);

            } break;

            case SUBMESSAGE_ID_CREATE:
            {
                CREATE_Payload payload;
                deserialize_CREATE_Payload(&mb, &payload);
                print_create_submessage(color, &payload, flags);

            } break;

            case SUBMESSAGE_ID_GET_INFO:
            {
                printf("%s[GET INFO SUBMESSAGE (not supported)]%s", RED, RESTORE_COLOR);

            } break;

            case SUBMESSAGE_ID_DELETE:
            {
                DELETE_Payload payload;
                deserialize_DELETE_Payload(&mb, &payload);
                print_delete_submessage(color, &payload);

            } break;

            case SUBMESSAGE_ID_STATUS_AGENT:
            {
                STATUS_AGENT_Payload payload;
                deserialize_STATUS_AGENT_Payload(&mb, &payload);
                print_status_agent_submessage(color, &payload);
            } break;

            case SUBMESSAGE_ID_STATUS:
            {
                STATUS_Payload payload;
                deserialize_STATUS_Payload(&mb, &payload);
                print_status_submessage(color, &payload);

            } break;

            case SUBMESSAGE_ID_INFO:
            {
                printf("%s[INFO SUBMESSAGE (not supported)]%s", RED, RESTORE_COLOR);
            } break;

            case SUBMESSAGE_ID_WRITE_DATA:
            {
                WRITE_DATA_Payload_Data payload;
                deserialize_WRITE_DATA_Payload_Data(&mb, &payload);
                print_write_data_data_submessage(color, &payload);

            } break;

            case SUBMESSAGE_ID_READ_DATA:
            {
                READ_DATA_Payload payload;
                deserialize_READ_DATA_Payload(&mb, &payload);
                print_read_data_submessage(color, &payload);

            } break;

            case SUBMESSAGE_ID_HEARTBEAT:
            {
                HEARTBEAT_Payload payload;
                deserialize_HEARTBEAT_Payload(&mb, &payload);
                print_heartbeat_submessage(color, &payload);

            } break;

            case SUBMESSAGE_ID_ACKNACK:
            {
                ACKNACK_Payload payload;
                deserialize_ACKNACK_Payload(&mb, &payload);
                print_acknack_submessage(color, &payload);

            } break;

            case SUBMESSAGE_ID_DATA:
            {
                DATA_Payload_Data payload;
                deserialize_DATA_Payload_Data(&mb, &payload);
                print_data_data_submessage(color, &payload);

            } break;

            case SUBMESSAGE_ID_FRAGMENT:
            {
                printf("%s[FRAGMENT SUBMESSAGE (not supported)]%s", RED, RESTORE_COLOR);
            } break;

            default:
            {
                printf("%s[UNKNOWN SUBMESSAGE]%s", RED, RESTORE_COLOR);
            } break;
        }

        //avanzar hasta length.
        submessage_counter++;
    }
    print_tail(initial_log_time);
    printf(" \n");
}

void print_serialization(int direction, const uint8_t* buffer, size_t size)
{
    const char* dir = (direction == SEND) ? SEND_ARROW : RECV_ARROW;

    printf("%s%s<< [%zu]: %s>>%s\n",
            dir,
            GREY_DARK,
            size,
            data_to_string(buffer, (uint32_t)size),
            RESTORE_COLOR);
}

//==================================================================
//                             PRIVATE
//==================================================================
char* print_array_2(const uint8_t* array_2)
{
    static char buffer[256];
    sprintf(buffer, "%02X%02X", array_2[0], array_2[1]);
    return buffer;
}

const char* data_to_string(const uint8_t* data, uint32_t size)
{
    static char buffer[4096];
    for(uint32_t i = 0; i < size; i++)
        sprintf(buffer + 3 * i, "%02X ", data[i]);
    buffer[3 * size] = '\0';
    return buffer;
}

const char* request_to_string(const BaseObjectRequest* request)
{
    static char buffer[256];
    int pos = sprintf(buffer, "req: 0x%s", print_array_2(request->request_id.data));
    sprintf(buffer + pos, " | obj: 0x%s", print_array_2(request->object_id.data));

    return buffer;
}

const char* reply_to_string(const BaseObjectReply* reply)
{
    static char buffer[256];

    char status[64];
    switch(reply->result.status)
    {
        case MR_STATUS_OK:
            sprintf(status, "OK");
            break;
        case MR_STATUS_OK_MATCHED:
            sprintf(status, "OK_MATCHED");
            break;
        case MR_STATUS_ERR_DDS_ERROR:
            sprintf(status, "ERR_DDS_ERROR");
            break;
        case MR_STATUS_ERR_MISMATCH:
            sprintf(status, "ERR_MISMATCH");
            break;
        case MR_STATUS_ERR_ALREADY_EXISTS:
            sprintf(status, "ERR_ALREADY_EXISTS");
            break;
        case MR_STATUS_ERR_DENIED:
            sprintf(status, "ERR_DENIED");
            break;
        case MR_STATUS_ERR_UNKNOWN_REFERENCE:
            sprintf(status, "ERR_UNKNOWN_REFERENCE");
            break;
        case MR_STATUS_ERR_INVALID_DATA:
            sprintf(status, "ERR_INVALID_DATA");
            break;
        case MR_STATUS_ERR_INCOMPATIBLE:
            sprintf(status, "ERR_INCOMPATIBLE");
            break;
        case MR_STATUS_ERR_RESOURCES:
            sprintf(status, "ERR_RESOURCES");
            break;
        default:
            sprintf(status, "UNKNOWN");
    }

    sprintf(buffer, "%s | %s",
            request_to_string(&reply->related_request),
            status);

    return buffer;
}

void print_create_client_submessage(const char* pre, const CREATE_CLIENT_Payload* payload)
{
    printf("%s[CREATE CLIENT | %s | session: 0x%02X | key: %s]%s",
            pre,
            request_to_string(&payload->base),
            payload->client_representation.session_id,
            data_to_string(payload->client_representation.client_key.data, 4),
            RESTORE_COLOR);
}

void print_create_submessage(const char* pre, const CREATE_Payload* payload, uint8_t flags)
{
    char type[4];
    switch(payload->object_representation._.participant.base.representation.format)
    {
        case REPRESENTATION_AS_XML_STRING:
            strcpy(type, "xml");
            break;

        case REPRESENTATION_BY_REFERENCE:
            strcpy(type, "ref");
            break;

        case REPRESENTATION_IN_BINARY:
            strcpy(type, "bin");
            break;

    }

    char content[4096];
    switch(payload->object_representation.kind)
    {
        case OBJK_PARTICIPANT:
            sprintf(content, "PARTICIPANT | %s: %u",
                    type,
                    payload->object_representation._.participant.base.representation._.xml_string_represenatation.size);
            break;
        case OBJK_TOPIC:
            sprintf(content, "TOPIC | obj: 0x%s | %s: %u",
                    print_array_2(payload->object_representation._.data_reader.subscriber_id.data),
                    type,
                    payload->object_representation._.topic.base.representation._.xml_string_represenatation.size);
            break;
        case OBJK_PUBLISHER:
            sprintf(content, "PUBLISHER | obj: 0x%s | %s: %u",
                    print_array_2(payload->object_representation._.publisher.participant_id.data),
                    type,
                    payload->object_representation._.publisher.base.representation._.string_represenatation.size);
            break;
        case OBJK_SUBSCRIBER:
            sprintf(content, "SUBSCRIBER | obj: 0x%s | %s: %u",
                    print_array_2(payload->object_representation._.subscriber.participant_id.data),
                    type,
                    payload->object_representation._.subscriber.base.representation._.string_represenatation.size);
            break;
        case OBJK_DATAWRITER:
            sprintf(content, "DATAWRITER | obj: 0x%s | %s: %u",
                    print_array_2(payload->object_representation._.data_writer.publisher_id.data),
                    type,
                    payload->object_representation._.data_writer.base.representation._.xml_string_represenatation.size);
             break;
        case OBJK_DATAREADER:
            sprintf(content, "DATAREADER | obj: 0x%s | %s: %u",
                    print_array_2(payload->object_representation._.data_reader.subscriber_id.data),
                    type,
                    payload->object_representation._.data_reader.base.representation._.xml_string_represenatation.size);
            break;
        default:
            sprintf(content, "UNKNOWN");
    }

    const char* reuse_flag = (flags & MR_REUSE) ? "REUSE" : "-";
    const char* replace_flag = (flags & MR_REPLACE) ? "REPLACE" : "";
    const char* separator = (flags & MR_REUSE && flags & MR_REPLACE) ? " " : "";

    printf("%s[CREATE | %s%s%s | %s | %s]%s",
            pre,
            reuse_flag,
            separator,
            replace_flag,
            request_to_string(&payload->base),
            content,
            RESTORE_COLOR);
}

/*void print_get_info_submessage(const char* pre, const GET_INFO_Payload* payload)
{
    //TODO
    (void) payload;
}*/

void print_delete_submessage(const char* pre, const DELETE_Payload* payload)
{
    printf("%s[DELETE | %s]%s",
            pre,
            request_to_string(&payload->base),
            RESTORE_COLOR);
}

void print_status_agent_submessage(const char* pre, const STATUS_AGENT_Payload* payload)
{
    printf("%s[STATUS AGENT | %s]%s",
            pre,
            reply_to_string(&payload->base),
            RESTORE_COLOR);
}

void print_status_submessage(const char* pre, const STATUS_Payload* payload)
{
    printf("%s[STATUS | %s]%s",
            pre,
            reply_to_string(&payload->base),
            RESTORE_COLOR);
}

/*void print_info_submessage(const char* pre, const INFO_Payload* payload)
{
    //TODO
    (void) payload;
}*/

void print_read_data_submessage(const char* pre, const READ_DATA_Payload* payload)
{
    char format[128];
    switch(payload->read_specification.data_format)
    {
        case FORMAT_DATA:
            sprintf(format, "DATA");
            break;
        case FORMAT_DATA_SEQ:
            sprintf(format, "DATA_SEQ");
            break;
        case FORMAT_SAMPLE:
            sprintf(format, "SAMPLE");
            break;
        case FORMAT_SAMPLE_SEQ:
            sprintf(format, "SAMPLE_SEQ");
            break;
        case FORMAT_PACKED_SAMPLES:
            sprintf(format, "PACKED_SAMPLES");
            break;
        default:
            sprintf(format, "UNKNOWN");
            break;
    }

    printf("%s[READ DATA | format: %s | istream: 0x%02X | %s | dc: %s]%s",
            pre,
            format,
            payload->read_specification.input_stream_id,
            request_to_string(&payload->base),
            (payload->read_specification.optional_delivery_control) ? "yes" : "no",
            RESTORE_COLOR);
}

void print_write_data_data_submessage(const char* pre, const WRITE_DATA_Payload_Data* payload)
{
    printf("%s[WRITE DATA | format: DATA | %s]%s",
            pre,
            request_to_string(&payload->base),
            RESTORE_COLOR);
}

void print_data_data_submessage(const char* pre, const DATA_Payload_Data* payload)
{
    printf("%s[DATA | format: DATA | %s]%s",
            pre,
            request_to_string(&payload->base),
            RESTORE_COLOR);
}

void print_acknack_submessage(const char* pre, const ACKNACK_Payload* payload)
{
    char bitmask[17] = {0};
    for(int i = 0; i < 8; i++)
    {
        bitmask[15 - i] = (payload->nack_bitmap[1] & (1 << i)) ? '1' : '0';
        bitmask[7 - i] = (payload->nack_bitmap[0] & (1 << i)) ? '1' : '0';
    }

    printf("%s[ACKNACK | seq_num: %hu | bitmap: %s]%s",
            pre,
            payload->first_unacked_seq_num,
            bitmask,
            RESTORE_COLOR);
}

void print_heartbeat_submessage(const char* pre, const HEARTBEAT_Payload* payload)
{
    printf("%s[HEARTBEAT | first: %hu | last: %hu]%s",
            pre,
            payload->first_unacked_seq_nr,
            payload->last_unacked_seq_nr,
            RESTORE_COLOR);
}

void print_header(size_t size, int direction, uint8_t stream_id, uint16_t seq_num, const uint8_t* client_key)
{
    const char* arrow = (direction == SEND) ? SEND_ARROW : RECV_ARROW;
    const char* color = (direction == SEND) ? YELLOW : PURPLE;

    char stream_representation;
    if(0 == stream_id)
    {
        stream_representation = 'n';
    }
    else if(0x80 <= stream_id)
    {
        stream_representation = 'r';
    }
    else // if(1 <= stream_id && 80 > stream_id)
    {
        stream_representation = 'b';
    }

    uint16_t seq_num_to_print = 0;
    if(stream_id != 0)
    {
        seq_num_to_print = seq_num;
    }
    else
    {
        stream_id = (uint8_t)seq_num;
    }

    const char* client_key_str = data_to_string(client_key, 4);
    printf("%s%s%3zu: %s(key: %s| %c:%2X |%3hu) %s", GREY_LIGHT, arrow, size, color, client_key_str, stream_representation, stream_id, seq_num_to_print, RESTORE_COLOR);
}

void print_tail(int64_t initial_log_time)
{
    int64_t ms = get_milli_time() - initial_log_time;
#ifdef WIN32
    printf(" %st: %I64ims%s", BLUE, ms, RESTORE_COLOR);
#else
    printf(" %st: %lims%s", BLUE, ms, RESTORE_COLOR);
#endif
}

