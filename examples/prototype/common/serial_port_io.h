#ifndef _TEST_PROTOTYPE_SERIAL_PORT_IO_H_
#define _TEST_PROTOTYPE_SERIAL_PORT_IO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <transport/ddsxrce_transport.h>

typedef struct SerialPort
{
    locator_t locator;
    channel_id_t channel_id;

} SerialPort;

void init_serial_port_io(SerialPort* serial_port, const char* device);
void send_serial_port_io(uint8_t* buffer, uint32_t length, void* data);
uint32_t received_serial_port_io(uint8_t* buffer, uint32_t size, void* data);

#ifdef __cplusplus
}
#endif

#endif //_TEST_PROTOTYPE_SERIAL_PORT_IO_H_