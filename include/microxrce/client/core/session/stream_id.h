// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef _MICROXRCE_CLIENT_CORE_SESSION_STREAM_ID_H_
#define _MICROXRCE_CLIENT_CORE_SESSION_STREAM_ID_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microxrce/client/dll.h>
#include <stdint.h>

typedef enum mrStreamType
{
    MR_NONE_STREAM,
    MR_BEST_EFFORT_STREAM,
    MR_RELIABLE_STREAM

} mrStreamType;

typedef enum mrStreamDirection
{
    MR_INPUT_STREAM,
    MR_OUTPUT_STREAM

} mrStreamDirection;

typedef struct mrStreamId
{
    uint8_t raw;
    uint8_t index;
    mrStreamType type;
    mrStreamDirection direction;

} mrStreamId;

MRDLLAPI mrStreamId mr_stream_id(uint8_t index, mrStreamType type, mrStreamDirection direction);
MRDLLAPI mrStreamId mr_stream_id_from_raw(uint8_t stream_id_raw, mrStreamDirection direction);

#ifdef __cplusplus
}
#endif

#endif // _MICROXRCE_CLIENT_CORE_SESSION_STREAM_ID_H
