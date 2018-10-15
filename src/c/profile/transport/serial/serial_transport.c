#include <uxr/client/profile/transport/serial/serial_transport.h>
#include <uxr/client/core/communication/serial_protocol.h>
#include <uxr/client/core/util/time.h>

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static bool send_serial_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_serial_msg(void* instance, uint8_t** buf, size_t* len, int timeout);
static int get_serial_error(void);

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
static bool send_serial_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = false;
    uxrSerialTransport* transport = (uxrSerialTransport*)instance;

    size_t bytes_written = uxr_write_serial_msg(&transport->serial_io.output,
                                                uxr_write_serial_data_platform,
                                                transport->platform,
                                                buf,
                                                len,
                                                transport->local_addr,
                                                transport->remote_addr);
    if ((0 < bytes_written) && (bytes_written == len))
    {
        rv = true;
    }

    return rv;
}

static bool recv_serial_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = false;
    uxrSerialTransport* transport = (uxrSerialTransport*)instance;

    size_t bytes_read = 0;
    do
    {
        int64_t time_init = uxr_millis();
        bytes_read = uxr_read_serial_msg(&transport->serial_io.input,
                                         uxr_read_serial_data_platform,
                                         transport->platform,
                                         timeout);
        if (0 < bytes_read && transport->serial_io.input.src_addr == transport->remote_addr)
        {
            *len = bytes_read;
            *buf = transport->serial_io.input.buffer;
            rv = true;
        }
        timeout -= (int)(uxr_millis() - time_init);
    }
    while ((0 == bytes_read) && (0 < timeout));

    return rv;
}

static int get_serial_error(void)
{
    return 0; // TODO (julian).
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
bool uxr_init_serial_transport(uxrSerialTransport* transport,
                               struct uxrSerialPlatform* platfrom,
                               const int fd,
                               uint8_t remote_addr,
                               uint8_t local_addr)
{
    bool rv = false;
    if (uxr_init_serial_platform(platfrom, fd, remote_addr, local_addr))
    {
        /* Setup platform. */
        transport->platform = platfrom;

        /* Setup address. */
        transport->remote_addr = remote_addr;
        transport->local_addr = local_addr;

        /* Init SerialIO. */
        uxr_init_serial_io(&transport->serial_io);

        /* Setup interface. */
        transport->comm.instance = (void*)transport;
        transport->comm.send_msg = send_serial_msg;
        transport->comm.recv_msg = recv_serial_msg;
        transport->comm.comm_error = get_serial_error;
        transport->comm.mtu = UXR_CONFIG_SERIAL_TRANSPORT_MTU;

        rv = true;
    }
    return rv;
}

bool uxr_close_serial_transport(uxrSerialTransport* transport)
{
    return uxr_close_serial_platform(transport->platform);
}
