#include <micrortps/client/profile/discovery/discovery.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/core/serialization/xrce_header.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/session/object_id.h>
#include <micrortps/client/core/log/log.h>
#include <micrortps/client/transport.h>

#include <microcdr/microcdr.h>

#define GET_INFO_MSG_SIZE   8
#define GET_INFO_REQUEST_ID 9

static void write_get_info(mcBuffer* mb);

//==================================================================
//                             PUBLIC
//==================================================================

bool mr_discovery_agents_multicast(int time, int period, mrOnAgentFound on_agent_func, void* args, AgentAddress* choosen)
{
    (void) time; (void) period; (void) on_agent_func; (void) args; (void) choosen;
    //TODO
    return false;
}

bool mr_discovery_agents_unicast(int time, int period, mrOnAgentFound on_agent_func, void* args, AgentAddress* choosen,
                                 const AgentAddress* agent_list, size_t agent_list_size)
{
    (void) time; (void) period; (void) on_agent_func; (void) args; (void) choosen;

    uint8_t output_buffer[MR_CONFIG_UDP_TRANSPORT_MTU];
    mcBuffer mb;
    mc_init_buffer(&mb, output_buffer, MR_CONFIG_UDP_TRANSPORT_MTU);
    write_get_info(&mb);
    size_t message_length = mc_buffer_length(&mb);

    for(size_t i = 0; i < agent_list_size; ++i)
    {
        mrUDPTransport transport;
        if(mr_init_udp_transport(&transport, agent_list[i].ip, agent_list[i].port))
        {
            (void) transport.comm.send_msg(&transport, output_buffer, message_length);
            DEBUG_PRINT_MESSAGE(SEND, output_buffer, message_length, 0);
        }
    }

    //TODO
    return false;
}

//==================================================================
//                             INTERNAL
//==================================================================

void write_get_info(mcBuffer* mb)
{
    GET_INFO_Payload payload;
    payload.base.request_id = (RequestId){{0x00, GET_INFO_REQUEST_ID}};
    payload.base.object_id = OBJECTID_AGENT;
    payload.info_mask = 0;

    (void) mc_serialize_message_header(mb, 0x80, 0, 0, 0);
    (void) write_submessage_header(mb, SUBMESSAGE_ID_GET_INFO, GET_INFO_MSG_SIZE, 0);
    (void) serialize_GET_INFO_Payload(mb, &payload);
}

