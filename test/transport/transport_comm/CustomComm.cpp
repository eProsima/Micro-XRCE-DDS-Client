#include "CustomComm.hpp"

#include <stdint.h>

CustomComm::CustomComm() : buffer(2000)
{
    max_payload = 20;
    uxr_set_custom_transport_callbacks(&master_, true, open, close, write, read);
    uxr_init_custom_transport(&master_, this);

    uxr_set_custom_transport_callbacks(&slave_, true, open, close, write, read);
    uxr_init_custom_transport(&slave_, this);
}

CustomComm::~CustomComm()
{
    uxr_close_custom_transport(&master_);
    uxr_close_custom_transport(&slave_);
}

bool CustomComm::open(uxrCustomTransport* /*transport*/)
{   
    return true;
}   

bool CustomComm::close(uxrCustomTransport* /*transport*/)
{
    return true;
}

size_t CustomComm::write(uxrCustomTransport* transport, const uint8_t* buf, size_t len, uint8_t* /*error*/)
{
    CustomComm* custom_comm = static_cast<CustomComm*>(transport->args);

    size_t written = 0;
    while (written < len && written < custom_comm->max_payload)
    {
        if (0 == custom_comm->buffer.write(buf[written]))
        {
            written++;
        }
        else
        {
            break;
        }
    }
    return written;
}

size_t CustomComm::read(uxrCustomTransport* transport, uint8_t* buf, size_t len, int /*timeout*/, uint8_t* /*error*/)
{
    CustomComm* custom_comm = static_cast<CustomComm*>(transport->args);

    size_t readed = 0;
    while (readed < len && readed < custom_comm->max_payload)
    {
        if (0 == custom_comm->buffer.read(&buf[readed]))
        {
            readed++;
        }
        else
        {
            break;
        }
    }

    return readed;
}