#include <micrortps/client/session/session_info.h>
#include <micrortps/client/serialization/xrce_protocol.h>
#include <micrortps/client/serialization/xrce_header.h>
#include <micrortps/client/serialization/xrce_subheader.h>
#include <microcdr/microcdr.h>

#include <string.h>

// Remove when Microcdr support size_of functions
#define CREATE_CLIENT_PAYLOAD_SIZE 26
#define DELETE_CLIENT_PAYLOAD_SIZE 4

#define REQUEST_ID_CREATE_CLIENT (RequestId){{0x00, 0x00}}
#define REQUEST_ID_DELETE_CLIENT (RequestId){{0x00, 0x01}}
#define VENDOR_ID_EPROSIMA (XrceVendorId){{0x01, 0x0F}}

int process_status_agent(SessionInfo* info);

//==================================================================
//                             PUBLIC
//==================================================================

void init_session_info(SessionInfo* info, uint8_t id, uint32_t key)
{
    info->id = id;
    info->key[0] = key >> 24;
    info->key[1] = (key << 8) >> 24;
    info->key[2] = (key << 16) >> 24;
    info->key[3] = (key << 24) >> 24;
}

void write_create_session(SessionInfo* info, MicroBuffer* mb, uint32_t nanoseconds)
{
    SubmessageHeader subheader;
    subheader.id = SUBMESSAGE_ID_CREATE_CLIENT;
    subheader.flags = FLAG_ENDIANNESS * mb->endianness;
    subheader.length = CREATE_CLIENT_PAYLOAD_SIZE;

    CREATE_CLIENT_Payload payload;
    payload.base.request_id = REQUEST_ID_CREATE_CLIENT;
    payload.base.object_id = OBJECTID_CLIENT;
    payload.client_representation.xrce_cookie = XRCE_COOKIE;
    payload.client_representation.xrce_version = XRCE_VERSION;
    payload.client_representation.xrce_vendor_id = VENDOR_ID_EPROSIMA;
    payload.client_representation.client_timestamp.seconds = (int32_t)(nanoseconds / 1000000000);
    payload.client_representation.client_timestamp.nanoseconds = nanoseconds % 1000000000;
    payload.client_representation.client_key = (ClientKey){{info->key[0], info->key[1], info->key[2], info->key[3]}};
    payload.client_representation.session_id = info->id;
    payload.client_representation.optional_properties = false;

    (void) serialize_submessage_header(mb, &subheader);
    (void) serialize_CREATE_CLIENT_Payload(mb, &payload);
}

void write_delete_session(SessionInfo* info, MicroBuffer* mb)
{
    (void) info; // not used

    SubmessageHeader subheader;
    subheader.id = SUBMESSAGE_ID_DELETE;
    subheader.flags = FLAG_ENDIANNESS * mb->endianness;
    subheader.length = DELETE_CLIENT_PAYLOAD_SIZE;

    DELETE_Payload payload;
    payload.base.request_id = REQUEST_ID_DELETE_CLIENT;
    payload.base.object_id = OBJECTID_CLIENT;

    (void) serialize_submessage_header(mb, &subheader);
    (void) serialize_DELETE_Payload(mb, &payload);
}

bool read_status_agent_message(SessionInfo* info, MicroBuffer* buffer, int* status_agent)
{
    (void) info; (void) buffer; (void) status_agent;
    //TODO
}

void stamp_first_session_header(SessionInfo* info, uint8_t* buffer)
{
    MessageHeader header;
    header.session_id = info->id;
    header.stream_id = 0;
    header.sequence_nr = 0;
    memcpy(header.key, info->key, CLIENT_KEY_SIZE);

    MicroBuffer mb;
    init_micro_buffer(&mb, buffer, MAX_HEADER_SIZE);

    (void) serialize_message_header(&mb, &header);
}

void stamp_session_header(SessionInfo* info, uint8_t stream_id_raw, uint16_t seq_num, uint8_t* buffer)
{
    MessageHeader header;
    header.session_id = info->id;
    header.stream_id = stream_id_raw;
    header.sequence_nr = seq_num;
    memcpy(header.key, info->key, CLIENT_KEY_SIZE);

    MicroBuffer mb;
    init_micro_buffer(&mb, buffer, MAX_HEADER_SIZE);

    (void) serialize_message_header(&mb, &header);
}

bool read_session_header(SessionInfo* info, MicroBuffer* mb, uint8_t* stream_id_raw, uint16_t* seq_num)
{
    bool must_be_read = micro_buffer_remaining(mb) > MAX_HEADER_SIZE;
    if(must_be_read)
    {
        MessageHeader header;
        deserialize_message_header(mb, &header);

        *stream_id_raw = header.stream_id;
        *seq_num = header.sequence_nr;

        must_be_read = header.session_id == info->id;
        if(must_be_read)
        {
            if (SESSION_ID_WITHOUT_CLIENT_KEY > info->id)
            {
                must_be_read &= (0 == memcmp(header.key, info->key, sizeof(info->key)));
            }
        }
    }

    return must_be_read;
}

uint8_t session_header_offset(SessionInfo* info)
{
    return (SESSION_ID_WITHOUT_CLIENT_KEY > info->id) ? MAX_HEADER_SIZE : MAX_HEADER_SIZE;
}

//==================================================================
//                            PRIVATE
//==================================================================
int process_status_agent(SessionInfo* info)
{
    (void) info;
    //TODO
}

