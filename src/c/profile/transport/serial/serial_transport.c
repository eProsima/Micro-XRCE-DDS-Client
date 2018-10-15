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

size_t uxr_write_serial_msg(uxrSerialOutputBuffer* output,
                            uxr_write_cb write_cb,
                            void* cb_arg,
                            const uint8_t* buf,
                            size_t len,
                            uint8_t src_addr,
                            uint8_t dst_addr)
{
    /* Check output buffer size. */
    if (UXR_SERIAL_MTU < len)
    {
        return 0;
    }

    /* Buffer being flag. */
    output->wb[0] = UXR_FRAMING_BEGIN_FLAG;
    output->wb_pos = 1;

    /* Buffer header. */
    add_next_octet(output, src_addr);
    add_next_octet(output, dst_addr);
    add_next_octet(output, (uint8_t)(len & 0xFF));
    add_next_octet(output, (uint8_t)(len >> 8));

    /* Write payload. */
    uint8_t octet = 0;
    uint16_t written_len = 0;
    uint16_t crc = 0;
    bool cond = true;
    while (written_len < len && cond)
    {
        octet = *(buf + written_len);
        if (add_next_octet(output, octet))
        {
            update_crc(&crc, octet);
            ++written_len;
        }
        else
        {
            size_t bytes_written = write_cb(cb_arg, output->wb, output->wb_pos);
            if (0 < bytes_written)
            {
                cond = true;
                output->wb_pos = (uint8_t)(output->wb_pos - bytes_written);
            }
            else
            {
                cond = false;
            }
        }
    }

    /* Write CRC. */
    uint8_t tmp_crc[2];
    tmp_crc[0] = (uint8_t)(crc & 0xFF);
    tmp_crc[1] = (uint8_t)(crc >> 8);
    written_len = 0;
    while (written_len < sizeof(tmp_crc) && cond)
    {
        octet = *(tmp_crc + written_len);
        if (add_next_octet(output, octet))
        {
            update_crc(&crc, octet);
            ++written_len;
        }
        else
        {
            size_t bytes_written = write_cb(cb_arg, output->wb, output->wb_pos);
            if (0 < bytes_written)
            {
                cond = true;
                output->wb_pos = (uint8_t)(output->wb_pos - bytes_written);
            }
            else
            {
                cond = false;
            }
        }
    }

    /* Flus write buffer. */
    if (cond && (0 < output->wb_pos))
    {
            size_t bytes_written = write_cb(cb_arg, output->wb, output->wb_pos);
            if (0 < bytes_written)
            {
                cond = true;
                output->wb_pos = (uint8_t)(output->wb_pos - bytes_written);
            }
            else
            {
                cond = false;
            }
    }

    return cond ? (uint16_t)(len) : 0;
}

size_t uxr_read_serial_msg(uxrSerialInputBuffer* input, uxr_read_cb read_cb, void* cb_arg, int timeout)
{
    size_t rv = 0;

    /* Compute read-buffer available size. */
    uint8_t av_len[2] = {0, 0};
    if (input->rb_head == input->rb_tail)
    {
        input->rb_head = 0;
        input->rb_tail = 0;
        av_len[0] = sizeof(input->rb) - 1;
    }
    else if (input->rb_head > input->rb_tail)
    {
        if (0 < input->rb_tail)
        {
            av_len[0] = (uint8_t)(sizeof(input->rb) - input->rb_head);
            av_len[1] = (uint8_t)(input->rb_tail - 1);
        }
        else
        {
            av_len[0] = (uint8_t)(sizeof(input->rb) - input->rb_head - 1);
        }
    }
    else
    {
        av_len[0] = (uint8_t)(input->rb_tail - input->rb_head - 1);
    }

    /* Read from serial. */
    size_t bytes_read[2] = {0};
    if (0 < av_len[0])
    {
        bytes_read[0] = read_cb(cb_arg, &input->rb[input->rb_head], av_len[0], timeout);
        input->rb_head = (uint8_t)((size_t)(input->rb_head + bytes_read[0]) % sizeof(input->rb));
        if (0 < bytes_read[0])
        {
            if ((bytes_read[0] == av_len[0]) && (0 < av_len[1]))
            {
                bytes_read[1] = read_cb(cb_arg, &input->rb[input->rb_head], av_len[1], 0);
                input->rb_head = (uint8_t)((size_t)(input->rb_head + bytes_read[1]) % sizeof(input->rb));
            }
        }
    }

    if (0 < (bytes_read[0] + bytes_read[1]))
    {
        /* State Machine. */
        bool exit_cond = false;
        while (!exit_cond)
        {
            uint8_t octet = 0;
            switch (input->state)
            {
                case UXR_SERIAL_UNINITIALIZED:
                {
                    octet = 0;
                    while ((UXR_FRAMING_BEGIN_FLAG != octet) && (input->rb_head != input->rb_tail))
                    {
                        octet = input->rb[input->rb_tail];
                        input->rb_tail = (uint8_t)((size_t)(input->rb_tail + 1) % sizeof(input->rb));
                    }

                    if (UXR_FRAMING_BEGIN_FLAG == octet)
                    {
                        input->state = UXR_SERIAL_READING_SRC_ADDR;
                    }
                    else
                    {
                        exit_cond = true;
                    }
                    break;
                }
                case UXR_SERIAL_READING_SRC_ADDR:
                {
                    if (get_next_octet(input, &input->src_addr))
                    {
                        input->state = UXR_SERIAL_READING_DST_ADDR;
                    }
                    else
                    {
                        if (UXR_FRAMING_BEGIN_FLAG != input->src_addr)
                        {
                            exit_cond = true;
                        }
                    }
                    break;
                }
                case UXR_SERIAL_READING_DST_ADDR:
                    if (get_next_octet(input, &input->dst_addr))
                    {
                        input->state = UXR_SERIAL_READING_LEN_LSB;
                    }
                    else
                    {
                        if (UXR_FRAMING_BEGIN_FLAG == input->dst_addr)
                        {
                            input->state = UXR_SERIAL_READING_SRC_ADDR;
                        }
                        else
                        {
                            exit_cond = true;
                        }
                    }
                    break;
                case UXR_SERIAL_READING_LEN_LSB:
                    if (get_next_octet(input, &octet))
                    {
                        input->msg_len = octet;
                        input->state = UXR_SERIAL_READING_LEN_MSB;
                    }
                    else
                    {
                        if (UXR_FRAMING_BEGIN_FLAG == octet)
                        {
                            input->state = UXR_SERIAL_READING_SRC_ADDR;
                        }
                        else
                        {
                            exit_cond = true;
                        }
                    }
                    break;
                case UXR_SERIAL_READING_LEN_MSB:
                    if (get_next_octet(input, &octet))
                    {
                        input->msg_len = (uint16_t)(input->msg_len + (octet << 8));
                        input->msg_pos = 0;
                        input->msg_crc = 0;
                        input->state = UXR_SERIAL_READING_PAYLOAD;
                    }
                    else
                    {
                        if (UXR_FRAMING_BEGIN_FLAG == octet)
                        {
                            input->state = UXR_SERIAL_READING_SRC_ADDR;
                        }
                        else
                        {
                            exit_cond = true;
                        }
                    }
                    break;
                case UXR_SERIAL_READING_PAYLOAD:
                {
                    while ((input->msg_pos < input->msg_len) && get_next_octet(input, &octet))
                    {
                        input->buffer[(size_t)input->msg_pos] = octet;
                        input->msg_pos = (uint16_t)(input->msg_pos + 1);
                        update_crc(&input->msg_crc, octet);
                    }

                    if (input->msg_pos == input->msg_len)
                    {
                        input->state = UXR_SERIAL_READING_CRC_LSB;
                    }
                    else
                    {
                        if (UXR_FRAMING_BEGIN_FLAG == octet)
                        {
                            input->state = UXR_SERIAL_READING_SRC_ADDR;
                        }
                        else
                        {
                            exit_cond = true;
                        }
                    }
                    break;
                }
                case UXR_SERIAL_READING_CRC_LSB:
                    if (get_next_octet(input, &octet))
                    {
                        input->crc = octet;
                        input->state = UXR_SERIAL_READING_CRC_MSB;
                    }
                    else
                    {
                        if (UXR_FRAMING_BEGIN_FLAG == octet)
                        {
                            input->state = UXR_SERIAL_READING_SRC_ADDR;
                        }
                        else
                        {
                            exit_cond = true;
                        }
                    }
                    break;
                case UXR_SERIAL_READING_CRC_MSB:
                    if (get_next_octet(input, &octet))
                    {
                        input->crc = (uint16_t)(input->crc + (octet << 8));
                        input->state = UXR_SERIAL_UNINITIALIZED;
                        if (input->crc == input->msg_crc)
                        {
                            rv = input->msg_len;
                        }
                        exit_cond = true;
                    }
                    else
                    {
                        if (UXR_FRAMING_BEGIN_FLAG == octet)
                        {
                            input->state = UXR_SERIAL_READING_SRC_ADDR;
                        }
                        else
                        {
                            exit_cond = true;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    return rv;
}
