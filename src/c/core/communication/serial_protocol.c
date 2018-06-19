#include <micrortps/client/core/communication/serial_protocol.h>
#include <errno.h>
#include <string.h>

intmax_t process_input_message(SerialInputBuffer* input, uint8_t* buf, size_t len);
inline uint8_t get_next_octet(SerialInputBuffer* input, uint16_t* relative_position);
inline bool add_next_octet(SerialOutputBuffer* output, uint8_t octet, uint16_t* position);

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

uint16_t calculate_crc(const uint8_t *buffer, size_t len)
{
    uint16_t crc = 0;
    while (len--)
    {
        crc = (crc >> 8) ^ crc16_table[(crc ^ *buffer++) & 0xFF];
    }
    return crc;
}

void update_crc(uint16_t* crc, const uint8_t data)
{
    *crc = (*crc >> 8) ^ crc16_table[(*crc ^ data) & 0xFF];
}

void init_serial_io(SerialIO* serial_io, uint8_t addr)
{
    serial_io->input.state = EMPTY;
    serial_io->addr = addr;
}

intmax_t write_serial_msg(SerialIO* serial_io, const uint8_t* buf, size_t len)
{
    intmax_t result = 0;
    uint16_t crc = 0;
    uint16_t position = 0;
    uint16_t writing_len = 0;

    // Check output buffer size.
    if (MICRORTPS_SERIAL_MTU < len || sizeof(serial_io->output.buffer) - MICRORTPS_SERIAL_OVERHEAD < len)
    {
        result = -EMSGSIZE;
    }

    if (0 <= result)
    {
        // Write header.
        (void) add_next_octet(&serial_io->output, serial_io->addr, &position);
        (void) add_next_octet(&serial_io->output, (uint8_t)len, &position);

        // Write payload.
        uint8_t octet = 0;
        bool octet_written = true;
        while (writing_len < len && octet_written)
        {
            octet = *(buf + writing_len);
            octet_written = add_next_octet(&serial_io->output, octet, &position);
            update_crc(&crc, octet);
            ++writing_len;
        }

        // Write CRC and end flag.
        if (writing_len == len)
        {
            octet_written = add_next_octet(&serial_io->output, (uint8_t)(crc >> 8), &position) &&
                            add_next_octet(&serial_io->output, (uint8_t)(crc & 0xFF), &position);
            result = (octet_written) ? position : -EMSGSIZE;
        }
        else
        {
            result = -EMSGSIZE;
        }

        // Write end flag.
        if (0 < result)
        {
            serial_io->output.buffer[position] = MICRORTPS_FRAMING_FLAG;
            result = ++position;
        }
    }

    return result;
}

intmax_t read_serial_msg(SerialIO* serial_io, read_callback read_cb, void* cb_arg, uint8_t* buf, size_t len)
{
    intmax_t result = 0;
    bool exit_flag = false;
    bool read_taken = false;

    /*
     * State Machine.
     */
    while (!exit_flag)
    {
        switch (serial_io->input.state) {
            case EMPTY:
            {
                if (!read_taken)
                {
                    serial_io->input.head = 0;
                    serial_io->input.market = 1;
                    serial_io->input.tail = 0;

                    result = read_cb(cb_arg, serial_io->input.buffer, sizeof(serial_io->input.buffer));
                    read_taken = true;
                    if (0 < result)
                    {
                        serial_io->input.tail = (serial_io->input.tail + result) % sizeof(serial_io->input.buffer);
                        serial_io->input.state = DATA_AVAILABLE;
                    }
                }
                else
                {
                    result = 0;
                    exit_flag = true;
                }
                break;
            }
            case DATA_AVAILABLE:
            {
                while (DATA_AVAILABLE == serial_io->input.state)
                {
                    if (MICRORTPS_FRAMING_FLAG == serial_io->input.buffer[serial_io->input.head])
                    {
                        serial_io->input.market = (serial_io->input.head + 1) % sizeof(serial_io->input.buffer);
                        serial_io->input.state = MESSAGE_INIT;
                    }
                    else
                    {
                        serial_io->input.head = (serial_io->input.head + 1) % sizeof(serial_io->input.buffer);
                        if (serial_io->input.head == serial_io->input.tail)
                        {
                            serial_io->input.state = EMPTY;
                        }
                    }
                }
                break;
            }
            case MESSAGE_INIT:
            {
                while (MESSAGE_INIT == serial_io->input.state)
                {
                    if (serial_io->input.market == serial_io->input.tail)
                    {
                        serial_io->input.state = MESSAGE_INCOMPLETE;
                    }
                    else
                    {
                        if (MICRORTPS_FRAMING_FLAG == serial_io->input.buffer[serial_io->input.market])
                        {
                            serial_io->input.state = MESSAGE_AVAILABLE;
                        }
                        serial_io->input.market = (serial_io->input.market + 1) % sizeof(serial_io->input.buffer);
                    }
                }
                break;
            }
            case MESSAGE_INCOMPLETE:
            {
                if (!read_taken)
                {
                    size_t available_len = 0;
                    if (serial_io->input.head < serial_io->input.tail)
                    {
                        available_len = sizeof(serial_io->input.buffer) - serial_io->input.tail;
                    }
                    else
                    {
                        available_len = serial_io->input.head - serial_io->input.tail;
                    }

                    result = read_cb(cb_arg, &serial_io->input.buffer[serial_io->input.tail], available_len);
                    read_taken = true;
                    if (0 < result)
                    {
                        serial_io->input.tail = (serial_io->input.tail + result) % sizeof(serial_io->input.buffer);
                        serial_io->input.state = DATA_AVAILABLE;
                    }
                }
                else
                {
                    result = 0;
                    exit_flag = true;
                }
                break;
            }
            case MESSAGE_AVAILABLE:
            {
                result = process_input_message(&serial_io->input, buf, len);
                serial_io->input.head = (serial_io->input.market - 1) % sizeof(serial_io->input.buffer);
                serial_io->input.state = (serial_io->input.head != serial_io->input.tail) ? DATA_AVAILABLE : EMPTY;
                if (0 <= result)
                {
                    exit_flag = true;
                }
                break;
            }
            default:
                result = 0;
                exit_flag = true;
                break;
        }
    }

    return result;
}

intmax_t process_input_message(SerialInputBuffer* input, uint8_t* buf, size_t len)
{
    intmax_t result = 0;
    uint16_t raw_len = 0;
    uint16_t raw_position = 1;
    uint8_t addr = 0;
    uint8_t msg_len = 0;
    uint8_t payload_position = 0;
    uint16_t crc = 0;

    // Precondition.
    raw_len = (input->head < input->market) ? (input->market - input->head) :
                                              (MICRORTPS_SERIAL_BUFFER_SIZE - input->market + input->head);
    if (MICRORTPS_SERIAL_OVERHEAD > raw_len)
    {
        // TODO (julian): define message error.
        return -1;
    }

    // Read address.
    addr = get_next_octet(input, &raw_position);

    // Read payload length.
    msg_len = get_next_octet(input, &raw_position);

    // Check condition.
    if (msg_len > len)
    {
        return -EMSGSIZE;
    }

    // Check condition.
    if (msg_len > raw_len - MICRORTPS_SERIAL_OVERHEAD)
    {
        // TODO (julian): define message error.
        return -1;
    }

    // Read payload.
    uint8_t octet;
    while (payload_position < msg_len && raw_position < raw_len - 2)
    {
        octet = get_next_octet(input, &raw_position);
        update_crc(&crc, octet);
        *(buf + payload_position) = octet;
        ++payload_position;
    }

    // Check condition.
    if (payload_position != msg_len || raw_position > raw_len - 2)
    {
        // TODO (julian): define message error.
        return -1;
    }
    else
    {
        result = payload_position;
    }

    // Check CRC.
    uint16_t msg_crc;
    octet = get_next_octet(input, &raw_position);
    msg_crc = (uint16_t)octet << 8;
    octet = get_next_octet(input, &raw_position);
    msg_crc = msg_crc | (uint16_t)octet;

    if (msg_crc != crc)
    {
        // TODO (julian): define message error.
        return -1;
    }

    return result;
}

uint8_t get_next_octet(SerialInputBuffer* input, uint16_t* relative_position)
{
    uint8_t result = input->buffer[(input->head + *relative_position) % sizeof(input->buffer)];

    *relative_position += 1;
    if (MICRORTPS_FRAMING_FLAG == result || MICRORTPS_FRAMING_ESP == result)
    {
        result = input->buffer[(input->head + *relative_position) % sizeof(input->buffer)] ^ MICRORTPS_FRAMING_XOR;
        *relative_position += 1;
    }

    return result;
}

bool add_next_octet(SerialOutputBuffer* output, uint8_t octet, uint16_t* position)
{
    bool result = false;

    if (MICRORTPS_FRAMING_FLAG == octet || MICRORTPS_FRAMING_ESP == octet)
    {
        if (*position < sizeof(output->buffer))
        {
            output->buffer[*position] = MICRORTPS_FRAMING_ESP;
            output->buffer[*position + 1] = octet ^ MICRORTPS_FRAMING_XOR;
            *position += 2;
            result = true;
        }
    }
    else
    {
        if (*position <= sizeof(output->buffer))
        {
            output->buffer[*position] = octet;
            *position += 1;
            result = true;
        }
    }

    return result;
}
