#include <uxr/client/profile/transport/custom/custom_transport.h>
#ifdef UCLIENT_CUSTOM_TRANSPORT_STREAM_FRAMING
#include "../stream_framing/stream_framing_protocol.h"
#endif
#include <uxr/client/util/time.h>

/*******************************************************************************
 * Static members.
 *******************************************************************************/
static uint8_t error_code;

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
static bool send_custom_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = false;
    uxrCustomTransport* transport = (uxrCustomTransport*)instance;

    uint8_t errcode;
    size_t bytes_written = 0;
#ifdef UCLIENT_CUSTOM_TRANSPORT_STREAM_FRAMING
    bytes_written = uxr_write_framed_msg(&transport->serial_io,
                                         transport->write,
                                         transport,
                                         buf,
                                         len,
                                         0x00,
                                         &errcode);
#else
    bytes_written = transport->write(transport, buf, len, &errcode);                                       
#endif

    if ((0 < bytes_written) && (bytes_written == len))
    {
        rv = true;
    }
    else
    {
        error_code = errcode;
    }

    return rv;
}

static bool recv_custom_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = false;
    uxrCustomTransport* transport = (uxrCustomTransport*)instance;

    size_t bytes_read = 0;
    do
    {
        int64_t time_init = uxr_millis();
        uint8_t remote_addr = 0x00;
        uint8_t errcode;

#ifdef UCLIENT_CUSTOM_TRANSPORT_STREAM_FRAMING
        bytes_read = uxr_read_framed_msg(&transport->serial_io,
                                         transport->read,
                                         transport,
                                         transport->buffer,
                                         sizeof(transport->buffer),
                                         &remote_addr,
                                         timeout,
                                         &errcode);
#else
        bytes_read = transport->read(transport, 
                                     transport->buffer, 
                                     sizeof(transport->buffer), 
                                     timeout, 
                                     &errcode);                                  
#endif

        if ((0 < bytes_read) && (remote_addr == 0x00))
        {
            *len = bytes_read;
            *buf = transport->buffer;
            rv = true;
        }
        else
        {
            error_code = errcode;
        }
        timeout -= (int)(uxr_millis() - time_init);
    }
    while ((0 == bytes_read) && (0 < timeout));

    return rv;
}

static uint8_t get_custom_error(void)
{
    return error_code;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
bool uxr_init_custom_transport(uxrCustomTransport* transport,
                               void * args)
{
    bool rv = false;
    if (transport->open(args))
    {
        /* Setup platform. */
        // transport->platform = platform;

#ifdef UCLIENT_CUSTOM_TRANSPORT_STREAM_FRAMING
        /* Init SerialIO. */
        uxr_init_serial_io(&transport->serial_io, 0x00);
#endif

        /* Setup interface. */
        transport->comm.instance = (void*)transport;
        transport->comm.send_msg = send_custom_msg;
        transport->comm.recv_msg = recv_custom_msg;
        transport->comm.comm_error = get_custom_error;
        transport->comm.mtu = UCLIENT_CUSTOM_TRANSPORT_MTU;

        rv = true;
    }
    return rv;
}

bool uxr_close_serial_transport(uxrCustomTransport* transport)
{
    return transport->close(transport);
}
