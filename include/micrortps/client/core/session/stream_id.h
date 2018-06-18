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

#ifndef _MICRORTPS_CLIENT_CORE_SESSION_STREAM_ID_H_
#define _MICRORTPS_CLIENT_CORE_SESSION_STREAM_ID_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <microcdr/microcdr.h>

typedef enum StreamType
{
    NONE_STREAM,
    BEST_EFFORT_STREAM,
    RELIABLE_STREAM

} StreamType;

typedef enum StreamDirection
{
    INPUT_STREAM,
    OUTPUT_STREAM

} StreamDirection;

typedef struct StreamId
{
    uint8_t raw;
    uint8_t index;
    StreamType type;
    StreamDirection direction;

} StreamId;

StreamId create_stream_id(uint8_t index, StreamType type, StreamDirection direction);
StreamId create_stream_id_from_raw(uint8_t stream_id_raw, StreamDirection direction);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CORE_SESSION_STREAM_ID_H
