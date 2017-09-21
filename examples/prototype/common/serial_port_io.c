#include "serial_port_io.h"

#define RED "\e[1;31m"
#define RESTORE_COLOR "\e[0m"

#include <string.h>
#include <stdio.h>

void init_serial_port_io(SerialPort* serial_port, const char* device)
{
    serial_port->locator.kind = LOC_SERIAL;
    strcpy(serial_port->locator.data, device);

    serial_port->channel_id = add_locator(&serial_port->locator);
}

void send_serial_port_io(uint8_t* buffer, uint32_t length, void* data)
{
    SerialPort* serial_port = (SerialPort*)data;
    int bytes = send(buffer, length, serial_port->locator.kind, serial_port->channel_id);
    if(bytes <= 0)
        printf("    %s[SEND ERROR]%s\n", RED, RESTORE_COLOR);
}

uint32_t received_serial_port_io(uint8_t* buffer, uint32_t size, void* data)
{
    SerialPort* serial_port = (SerialPort*)data;
    int bytes = receive(buffer, size, serial_port->locator.kind, serial_port->channel_id);
    if(bytes <= 0)
    {
        printf("    %s[RECV ERROR]%s\n", RED, RESTORE_COLOR);
        return 0;
    }

    return bytes;
}