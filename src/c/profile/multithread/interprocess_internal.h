// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef UXR_CLIENT_PROFILE_INTERPROCESS_INTERNAL_H_
#define UXR_CLIENT_PROFILE_INTERPROCESS_INTERNAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <uxr/client/config.h>

#include <uxr/client/visibility.h>
#include <uxr/client/core/session/session.h>
#include <uxr/client/profile/multithread/multithread.h>

#ifdef UCLIENT_PROFILE_INTERPROCESS

#define UXR_PREPARE_INTERPROCESS(a,b,c,d) uxr_prepare_interprocess(a,b,c,d)
#define UXR_HANDLE_INTERPROCESS() uxr_handle_interprocess()
#define UXR_ADD_INTERPROCESS_ENTITY_XML(a,b,c) uxr_add_interprocess_entity_xml(a,b,c)
#define UXR_CLEAN_INTERPROCESS() uxr_deinit_static_list()

#else // UCLIENT_PROFILE_INTERPROCESS

#define UXR_PREPARE_INTERPROCESS(a,b,c,d)
#define UXR_HANDLE_INTERPROCESS()
#define UXR_ADD_INTERPROCESS_ENTITY_XML(a,b,c)
#define UXR_CLEAN_INTERPROCESS()

#endif // UCLIENT_PROFILE_INTERPROCESS

/**
 * @brief
 * TODO
 */
UXRDLLAPI void uxr_prepare_interprocess(uxrSession* session, uxrObjectId entity_id, ucdrBuffer* ub, uint16_t data_size);

/**
 * @brief
 * TODO
 */
UXRDLLAPI void uxr_handle_interprocess();

/**
 * @brief
 * TODO
 */
UXRDLLAPI void uxr_add_interprocess_entity_xml(uxrSession* session, uxrObjectId entity_id, const char* xml);

/**
 * @brief
 * TODO
 */
UXRDLLAPI void uxr_deinit_static_list();

#ifdef __cplusplus
}
#endif

#endif // UXR_CLIENT_PROFILE_INTERPROCESS_INTERNAL_H_
