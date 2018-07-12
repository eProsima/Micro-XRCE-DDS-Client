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

#ifndef _MICRORTPS_CLIENT_PROFILE_SESSION_READ_ACCESS_H_
#define _MICRORTPS_CLIENT_PROFILE_SESSION_READ_ACCESS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/core/session/session.h>

typedef struct DeliveryControl
{
    uint16_t max_samples;
    uint16_t max_elapsed_time;
    uint16_t max_bytes_per_second;
    uint16_t min_pace_period;

} DeliveryControl;

uint16_t write_read_data(Session* session, StreamId stream_id, mrObjectId datareader_id,
                         StreamId data_stream_id, DeliveryControl* control);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_PROFILE_SESSION_READ_ACCESS_H_
