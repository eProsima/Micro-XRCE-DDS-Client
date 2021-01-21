#include <uxr/client/profile/transport/custom/custom_transport.h>
#include "../stream_framing/stream_framing_protocol.h"
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
    if (transport->framing)
    {    
        bytes_written = uxr_write_framed_msg(&transport->framing_io,
                                            (uxr_write_cb) transport->write,
                                            transport,
                                            buf,
                                            len,
                                            0x00,
                                            &errcode);
    }
    else
    {
        bytes_written = transport->write(transport, buf, len, &errcode); 
    }                                      

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

        if (transport->framing)
        {    
            bytes_read = uxr_read_framed_msg(&transport->framing_io,
                                            (uxr_read_cb) transport->read,
                                            transport,
                                            transport->buffer,
                                            sizeof(transport->buffer),
                                            &remote_addr,
                                            timeout,
                                            &errcode);
        } 
        else 
        {
            bytes_read = transport->read(transport, 
                                        transport->buffer, 
                                        sizeof(transport->buffer), 
                                        timeout, 
                                        &errcode);                                  
        }

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
void uxr_set_custom_transport_callbacks(uxrCustomTransport* transport,
                                        bool framing,
                                        open_custom_func open,
                                        close_custom_func close,
                                        write_custom_func write,
                                        read_custom_func read)
{
    transport->framing = framing;
    transport->open = open;
    transport->close = close;
    transport->write = write;
    transport->read = read;
}

bool uxr_init_custom_transport(uxrCustomTransport* transport,
                               void * args)
{
    bool rv = false;
    if (transport->open == NULL  ||
        transport->close == NULL ||
        transport->write == NULL ||
        transport->read == NULL)
    {
        return rv;
    }
    
    transport->args = args;

    if (transport->open(transport))
    {
        if (transport->framing)
        {
            /* Init FramingIO. */
            uxr_init_framing_io(&transport->framing_io, 0x00);
        }
        
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

bool uxr_close_custom_transport(uxrCustomTransport* transport)
{
    return transport->close(transport);
}
