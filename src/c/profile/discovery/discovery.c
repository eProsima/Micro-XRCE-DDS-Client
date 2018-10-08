#include <uxr/client/profile/discovery/discovery.h>
#include <uxr/client/core/communication/communication.h>
#include <uxr/client/core/session/object_id.h>
#include <uxr/client/core/session/stream/seq_num.h>
#include <uxr/client/transport.h>

#include "../../core/serialization/xrce_protocol_internal.h"
#include "../../core/serialization/xrce_header_internal.h"
#include "../../core/session/submessage_internal.h"
#include "../../core/log/log_internal.h"

#include <string.h>

#define GET_INFO_MSG_SIZE   8
#define GET_INFO_REQUEST_ID 9

typedef struct CallbackData
{
    uxrAgentAddress* choosen;
    uxrOnAgentFound on_agent;
    void* args;

} CallbackData;

static void write_get_info_message(ucdrBuffer* mb);
static bool listen_read_info_message(uxrCommunication* communication, int period, CallbackData* callback);
static bool read_info_headers(ucdrBuffer* mb);
static bool read_info_message(ucdrBuffer* mb, CallbackData* callback);
static bool process_info(CallbackData* callback, Time_t timestamp, TransportLocator* transport);

//==================================================================
//                             PUBLIC
//==================================================================

bool uxr_discovery_agents_multicast(int time, int period, uxrOnAgentFound on_agent_func, void* args, uxrAgentAddress* choosen)
{
    (void) time; (void) period; (void) on_agent_func; (void) args; (void) choosen;
    //TODO
    return false;
}

bool uxr_discovery_agents_unicast(int time, int period, uxrOnAgentFound on_agent_func, void* args, uxrAgentAddress* choosen,
                                 const uxrAgentAddress* agent_list, size_t agent_list_size)
{
    (void) time;

    CallbackData callback = {choosen, on_agent_func, args};

    uint8_t output_buffer[UXR_CONFIG_UDP_TRANSPORT_MTU];
    ucdrBuffer mb;
    ucdr_init_buffer(&mb, output_buffer, UXR_CONFIG_UDP_TRANSPORT_MTU);
    write_get_info_message(&mb);
    size_t message_length = ucdr_buffer_length(&mb);

    bool consumed = false;
    for(size_t i = 0; i < agent_list_size && !consumed; ++i)
    {
        uxrUDPTransport transport;
        if(uxr_init_udp_transport(&transport, agent_list[i].ip, agent_list[i].port))
        {
            bool sent = transport.comm.send_msg(&transport, output_buffer, message_length);
            UXR_DEBUG_PRINT_MESSAGE(UXR_SEND, output_buffer, message_length, 0);

            if(sent)
            {
                consumed = listen_read_info_message(&transport.comm, period, &callback);
            }
        }
    }

    return consumed;
}

//==================================================================
//                             INTERNAL
//==================================================================

void write_get_info_message(ucdrBuffer* mb)
{
    GET_INFO_Payload payload;
    payload.base.request_id = (RequestId){{0x00, GET_INFO_REQUEST_ID}};
    payload.base.object_id = OBJECTID_AGENT;
    payload.info_mask = INFO_CONFIGURATION | INFO_ACTIVITY;

    uxr_serialize_message_header(mb, 0x80, 0, 0, 0);
    (void) uxr_write_submessage_header(mb, SUBMESSAGE_ID_GET_INFO, GET_INFO_MSG_SIZE, 0);
    (void) uxr_serialize_GET_INFO_Payload(mb, &payload);
}

bool listen_read_info_message(uxrCommunication* communication, int period, CallbackData* callback)
{
    bool consumed = false;
    uint8_t* input_buffer; size_t length;

    bool received = communication->recv_msg(communication->instance, &input_buffer, &length, period);
    if(received)
    {
        UXR_DEBUG_PRINT_MESSAGE(UXR_RECV, input_buffer, length, 0);

        ucdrBuffer mb;
        ucdr_init_buffer(&mb, input_buffer, (uint32_t)length);
        if(read_info_headers(&mb))
        {
            consumed = read_info_message(&mb, callback);
        }
    }

    return consumed;
}

bool read_info_headers(ucdrBuffer* mb)
{
    bool valid = false;

    uint8_t session_id; uint8_t stream_id_raw; uxrSeqNum seq_num; uint8_t key[UXR_CLIENT_KEY_SIZE];
    uxr_deserialize_message_header(mb, &session_id, &stream_id_raw, &seq_num, key);

    uint8_t id; uint16_t length; uint8_t flags; uint8_t* payload_it = NULL;
    if(uxr_read_submessage_header(mb, &id, &length, &flags, &payload_it))
    {
        valid = true;
    }

    return valid;
}

bool read_info_message(ucdrBuffer* mb, CallbackData* callback)
{
    bool consumed = false;
    INFO_Payload payload;

    if(uxr_deserialize_INFO_Payload(mb, &payload))
    {
        XrceVersion* version = &payload.object_info.config._.agent.xrce_version;
        TransportLocator* transport = &payload.object_info.activity._.agent.address_seq.data[0];
        Time_t timestamp = payload.object_info.config._.agent.agent_timestamp;

        if(0 == memcmp(version->data, XRCE_VERSION.data, sizeof(XRCE_VERSION.data)))
        {
            consumed = process_info(callback, timestamp, transport);
        }
    }

    return consumed;
}

bool process_info(CallbackData* callback, Time_t timestamp, TransportLocator* transport)
{
    bool consumed = false;
    int64_t nanoseconds = (int64_t)timestamp.seconds + (int64_t)timestamp.nanoseconds * 1000000000L;

    if(transport->format == ADDRESS_FORMAT_MEDIUM)
    {
        memcpy(callback->choosen->ip, transport->_.medium_locator.address, 4); //CHECK
        callback->choosen->port = transport->_.medium_locator.locator_port;

        consumed = callback->on_agent(callback->choosen, nanoseconds, callback->args);
    }

    return consumed;
}

