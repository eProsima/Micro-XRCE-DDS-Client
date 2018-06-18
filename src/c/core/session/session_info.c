#include <micrortps/client/core/session/session_info.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/core/serialization/xrce_header.h>
#include <microcdr/microcdr.h>

#include <string.h>

// Remove when Microcdr supports size_of functions
#define CREATE_CLIENT_PAYLOAD_SIZE 30
#define DELETE_CLIENT_PAYLOAD_SIZE 4

#define VENDOR_ID_EPROSIMA (XrceVendorId){{0x01, 0x0F}}

#define NO_REQUEST            0
#define STATE_LOGOUT          1
#define STATE_LOGIN           2

void process_status_agent(SessionInfo* info, uint8_t status, AGENT_Representation* agent);

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
    info->state = STATE_LOGOUT;
    info->request = NO_REQUEST;
}

void write_create_session(SessionInfo* info, MicroBuffer* mb, uint32_t nanoseconds)
{
    CREATE_CLIENT_Payload payload;
    payload.base.request_id = (RequestId){{0x00, STATE_LOGIN}};
    payload.base.object_id = OBJECTID_CLIENT;
    payload.client_representation.xrce_cookie = XRCE_COOKIE;
    payload.client_representation.xrce_version = XRCE_VERSION;
    payload.client_representation.xrce_vendor_id = VENDOR_ID_EPROSIMA;
    payload.client_representation.client_timestamp.seconds = (int32_t)(nanoseconds / 1000000000);
    payload.client_representation.client_timestamp.nanoseconds = nanoseconds % 1000000000;
    payload.client_representation.client_key = (ClientKey){{info->key[0], info->key[1], info->key[2], info->key[3]}};
    payload.client_representation.session_id = info->id;
    payload.client_representation.optional_properties = false;

    (void) write_submessage_header(mb, SUBMESSAGE_ID_CREATE_CLIENT, CREATE_CLIENT_PAYLOAD_SIZE, 0);
    (void) serialize_CREATE_CLIENT_Payload(mb, &payload);

    info->request = STATE_LOGIN;
}

void write_delete_session(SessionInfo* info, MicroBuffer* mb)
{
    DELETE_Payload payload;
    payload.base.request_id = (RequestId){{0x00, STATE_LOGOUT}};
    payload.base.object_id = OBJECTID_CLIENT;

    (void) write_submessage_header(mb, SUBMESSAGE_ID_DELETE, DELETE_CLIENT_PAYLOAD_SIZE, 0);
    (void) serialize_DELETE_Payload(mb, &payload);

    info->request = STATE_LOGOUT;
}

void read_status_agent(SessionInfo* info, MicroBuffer* buffer)
{
    STATUS_AGENT_Payload payload;
    if(deserialize_STATUS_AGENT_Payload(buffer, &payload))
    {
        if(payload.base.related_request.request_id.data[0] == info->request)
        {
            process_status_agent(info, payload.base.result.status, &payload.agent_info);
        }
    }
}

void stamp_first_session_header(SessionInfo* info, uint8_t* buffer)
{
    MicroBuffer mb;
    init_micro_buffer(&mb, buffer, MAX_HEADER_SIZE);

    uint8_t session_id = SESSION_ID_WITHOUT_CLIENT_KEY & info->id;
    (void) serialize_message_header(&mb, session_id, 0, 0, info->key);
}

void stamp_session_header(SessionInfo* info, uint8_t stream_id_raw, uint16_t seq_num, uint8_t* buffer)
{
    MicroBuffer mb;
    init_micro_buffer(&mb, buffer, MAX_HEADER_SIZE);

    (void) serialize_message_header(&mb, info->id, stream_id_raw, seq_num, info->key);
}

bool read_session_header(SessionInfo* info, MicroBuffer* mb, uint8_t* stream_id_raw, uint16_t* seq_num)
{
    bool must_be_read = micro_buffer_remaining(mb) > MAX_HEADER_SIZE;
    if(must_be_read)
    {
        uint8_t session_id; uint8_t key[CLIENT_KEY_SIZE];
        (void) deserialize_message_header(mb, &session_id, stream_id_raw, seq_num, key);

        must_be_read = session_id == info->id;
        if(must_be_read)
        {
            if (SESSION_ID_WITHOUT_CLIENT_KEY > info->id)
            {
                must_be_read &= (0 == memcmp(key, info->key, CLIENT_KEY_SIZE));
            }
        }
    }

    return must_be_read;
}

uint8_t session_header_offset(SessionInfo* info)
{
    return (SESSION_ID_WITHOUT_CLIENT_KEY > info->id) ? MAX_HEADER_SIZE : MAX_HEADER_SIZE;
}

bool check_session_info_pending_request(SessionInfo* info)
{
    return NO_REQUEST != info->request;
}

void restore_session_info_request(SessionInfo* info)
{
    info->request = NO_REQUEST;
}

//==================================================================
//                            PRIVATE
//==================================================================
void process_status_agent(SessionInfo* info, uint8_t status, AGENT_Representation* agent)
{
    (void) agent;
    if(status == STATUS_OK)
    {
        info->state = info->request;
    }

    info->request = NO_REQUEST;
}

