// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef UXR_CLIENT_TRANSPORT_H_
#define UXR_CLIENT_TRANSPORT_H_

#include <uxr/client/config.h>

#ifdef PROFILE_UDP_TRANSPORT
#include <uxr/client/profile/transport/ip/udp/udp_transport.h>
#if defined(UCLIENT_PLATFORM_POSIX)
#include <uxr/client/profile/transport/ip/udp/udp_transport_posix.h>
#elif defined(UCLIENT_PLATFORM_WINDOWS)
#include <uxr/client/profile/transport/ip/udp/udp_transport_windows.h>
#endif
#endif //PROFILE_UDP_TRANSPORT

#ifdef PROFILE_TCP_TRANSPORT
#include <uxr/client/profile/transport/ip/tcp/tcp_transport.h>
#if defined(UCLIENT_PLATFORM_POSIX)
#include <uxr/client/profile/transport/ip/tcp/tcp_transport_posix.h>
#elif defined(UCLIENT_PLATFORM_WINDOWS)
#include <uxr/client/profile/transport/ip/tcp/tcp_transport_windows.h>
#endif
#endif //PROFILE_TCP_TRANSPORT

#ifdef PROFILE_SERIAL_TRANSPORT
#include <uxr/client/profile/transport/serial/serial_transport.h>
#if defined(UCLIENT_PLATFORM_POSIX)
#include <uxr/client/profile/transport/serial/serial_transport_posix.h>
#elif defined(UCLIENT_PLATFORM_WINDOWS)
#elif defined(_external_transport)
#include EXTERNAL_TRANSPORT_HEADER_FILENAME
#endif
#endif //PROFILE_SERIAL_TRANSPORT

#endif // UXR_CLIENT_TRANSPORT_H_
