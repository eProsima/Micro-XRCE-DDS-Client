#include "serial_protocol_internal.h"
#include <string.h>

/*******************************************************************************
 * Static members.
 *******************************************************************************/
// CRC-16 table for POLY 0x8005 (x^16 + x^15 + x^2 + 1).
static const uint16_t crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static void update_crc(uint16_t* crc, const uint8_t data);
static inline bool get_next_octet(uxrSerialInputBuffer* input, uint8_t* octet);
static inline bool add_next_octet(uxrSerialOutputBuffer* output, uint8_t octet);

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
void update_crc(uint16_t* crc, const uint8_t data)
{
    *crc = (*crc >> 8) ^ crc16_table[(*crc ^ data) & 0xFF];
}

bool get_next_octet(uxrSerialInputBuffer* input, uint8_t* octet)
{
    bool rv = false;
    *octet = 0;
    if (input->rb_head != input->rb_tail)
    {
        if (UXR_FRAMING_ESC_FLAG != input->rb[input->rb_tail])
        {
            *octet = input->rb[input->rb_tail];
            input->rb_tail = (uint8_t)((size_t)(input->rb_tail + 1) % sizeof(input->rb));
            rv = (UXR_FRAMING_BEGIN_FLAG != *octet);
        }
        else
        {
            uint8_t temp_tail = (uint8_t)((size_t)(input->rb_tail + 1) % sizeof(input->rb));
            if (temp_tail != input->rb_head)
            {
                *octet = input->rb[temp_tail];
                input->rb_tail = (uint8_t)((size_t)(input->rb_tail + 2) % sizeof(input->rb));
                if (UXR_FRAMING_BEGIN_FLAG != *octet)
                {
                    *octet ^= UXR_FRAMING_XOR_FLAG;
                    rv = true;
                }
            }
        }
    }
    return rv;
}

bool add_next_octet(uxrSerialOutputBuffer* output, uint8_t octet)
{
    bool rv = false;

    if (UXR_FRAMING_BEGIN_FLAG == octet || UXR_FRAMING_ESC_FLAG == octet)
    {
        if ((uint8_t)(output->wb_pos + 1) < sizeof(output->wb))
        {
            output->wb[output->wb_pos] = UXR_FRAMING_ESC_FLAG;
            output->wb[output->wb_pos + 1] = octet ^ UXR_FRAMING_XOR_FLAG;
            output->wb_pos = (uint8_t)(output->wb_pos + 2);
            rv = true;
        }
    }
    else
    {
        if (output->wb_pos < sizeof(output->wb))
        {
            output->wb[output->wb_pos] = octet;
            output->wb_pos = (uint8_t)(output->wb_pos + 1);
            rv = true;
        }
    }

    return rv;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
void uxr_init_serial_io(uxrSerialIO* serial_io)
{
    serial_io->input.state = UXR_SERIAL_UNINITIALIZED;
    serial_io->input.rb_head = 0;
    serial_io->input.rb_tail = 0;
}

uint16_t uxr_write_serial_msg(uxrSerialOutputBuffer* output,
                              uxr_write_cb write_cb,
                              void* cb_arg,
                              const uint8_t* buf,
                              uint16_t len,
                              uint8_t src_addr,
                              uint8_t rmt_addr)
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
    add_next_octet(output, rmt_addr);
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
            uint16_t bytes_written = write_cb(cb_arg, output->wb, output->wb_pos);
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
    uint8_t tmp_crc[2] = {(uint8_t)(crc & 0xFF), (uint8_t)(crc >> 8)};
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
            uint16_t bytes_written = write_cb(cb_arg, output->wb, output->wb_pos);
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
            uint16_t bytes_written = write_cb(cb_arg, output->wb, output->wb_pos);
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

uint16_t uxr_read_serial_msg(uxrSerialInputBuffer* input, uxr_read_cb read_cb, void* cb_arg, int timeout)
{
    uint16_t rv = 0;

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
    uint16_t bytes_read[2] = {0};
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
