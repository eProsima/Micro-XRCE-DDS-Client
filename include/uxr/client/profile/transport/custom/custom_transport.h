// Copyright 2020 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef UXR_CLIENT_CUSTOM_TRANSPORT_H_
#define UXR_CLIENT_CUSTOM_TRANSPORT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <uxr/client/core/communication/communication.h>
#include <uxr/client/config.h>
#ifdef UCLIENT_CUSTOM_TRANSPORT_STREAM_FRAMING
#include <uxr/client/profile/transport/stream_framing/stream_framing_protocol.h>
#endif
#include <uxr/client/visibility.h>

typedef bool (*open_custom_func)(void * args);
typedef bool (*close_custom_func)(uxrCustomTransport* transport);
typedef size_t (*write_custom_func)(uxrCustomTransport* transport, uint8_t* buf, size_t len, uint8_t* errcode);
typedef size_t (*read_custom_func)(uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* errcode);

typedef struct uxrCustomTransport
{
    uint8_t buffer[UCLIENT_CUSTOM_TRANSPORT_MTU];
#ifdef UCLIENT_CUSTOM_TRANSPORT_STREAM_FRAMING
    uxrFramingIO framing_io;
#endif
    open_custom_func open;
    close_custom_func close;
    write_custom_func write;
    read_custom_func read;

    uxrCommunication comm;
    void * args;
} uxrCustomTransport;


/**
 * @brief Initializes a Custom transport.
 * @param transport     The uninitialized transport structure used for managing the transport.
 *                      This structure must be accesible during the connection.
 * @return `true` in case of successful initialization. `false` in other case.
 */
UXRDLLAPI bool uxr_init_custom_transport(uxrCustomTransport* transport,
                                         void * args);

/**
 * @brief Closes a Custom transport.
 * @param transport The transport structure.
 * @return `true` in case of successful closing. `false` in other case.
 */
UXRDLLAPI bool uxr_close_serial_transport(uxrCustomTransport* transport);

#ifdef __cplusplus
}
#endif

#endif // UXR_CLIENT_CUSTOM_TRANSPORT_H_
