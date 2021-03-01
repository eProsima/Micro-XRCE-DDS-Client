#include "SerialComm.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

SerialComm::SerialComm() : fd_(-1)
{
}

SerialComm::~SerialComm()
{
    (void) unlink("/tmp/serial_fifo");
}

int SerialComm::init()
{
    int rv = 0;

    (void) unlink("/tmp/serial_fifo");
    if (0 < mkfifo("/tmp/serial_fifo", S_IRWXU | S_IRWXG | S_IRWXO))
    {
        rv = -1;
    }
    else
    {
        fd_ = open("/tmp/serial_fifo", O_RDWR | O_NONBLOCK);
        if (0 < fd_)
        {
            fcntl(fd_, F_SETFL, O_NONBLOCK);
            fcntl(fd_, F_SETPIPE_SZ, 4096);
            if (!uxr_init_serial_transport(&master_, fd_, 1, 0) ||
                !uxr_init_serial_transport(&slave_, fd_, 0, 1))
            {
                rv = -1;
            }
        }
        else
        {
            rv = -1;
        }
    }

    return rv;
}
