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

#ifndef _MICRORTPS_CLIENT_CLIENT_H_
#define _MICRORTPS_CLIENT_CLIENT_H_

#include <micrortps/client/config.h>

#ifdef PROFILE_CREATE_ENTITIES_REF
#include <micrortps/client/profile/session/create_entities_ref.h>
#endif //PROFILE_CREATE_ENTITIES_REF

#ifdef PROFILE_CREATE_ENTITIES_XML
#include <micrortps/client/profile/session/create_entities_xml.h>
#endif //PROFILE_CREATE_ENTITIES_XML

#ifdef PROFILE_READ_ACCESS
#include <micrortps/client/profile/session/read_access.h>
#endif //PROFILE_READ_ACCESS

#ifdef PROFILE_UDP_TRANSPORT
#if defined(PLATFORM_NAME_LINUX)
#include <micrortps/client/profile/transport/udp_transport_linux.h>
#elif defined(PLATFORM_NAME_WINDOWS)
#include <micrortps/client/profile/transport/udp_transport_windows.h>
#elif defined(PLATFORM_NAME_NUTTX)
#include <micrortps/client/profile/transport/udp_transport_linux.h>
#endif
#endif //PROFILE_UDP_TRANSPORT

#ifdef PROFILE_TCP_TRANSPORT
#if defined(PLATFORM_NAME_LINUX)
#include <micrortps/client/profile/transport/tcp_transport_linux.h>
#elif defined(PLATFORM_NAME_WINDOWS)
#include <micrortps/client/profile/transport/tcp_transport_windows.h>
#elif defined(PLATFORM_NAME_NUTTX)
#include <micrortps/client/profile/transport/tcp_transport_linux.h>
#endif
#endif //PROFILE_TCP_TRANSPORT

#ifdef PROFILE_UART_TRANSPORT
#if defined(PLATFORM_NAME_LINUX)
#include <micrortps/client/profile/transport/uart_transport_linux.h>
#elif defined(PLATFORM_NAME_WINDOWS)
#include <micrortps/client/profile/transport/uart_transport_windows.h>
#elif defined(PLATFORM_NAME_NUTTX)
#include <micrortps/client/profile/transport/uart_transport_linux.h>
#endif
#endif //PROFILE_UART_TRANSPORT

#include <micrortps/client/core/session/session.h>

#endif // _MICRORTPS_CLIENT_CLIENT_H_
