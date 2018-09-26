// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/*!
 * @file ShapeType.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _ShapeType_H_
#define _ShapeType_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

/*!
 * @brief This struct represents the structure ShapeType defined by the user in the IDL file.
 * @ingroup SHAPETYPE
 */
typedef struct ShapeType
{
    char color[255];

    int32_t x;
    int32_t y;
    int32_t shapesize;
} ShapeType;

struct mcMicroBuffer;

bool ShapeType_serialize_topic(struct mcMicroBuffer* writer, const ShapeType* topic);
bool ShapeType_deserialize_topic(struct mcMicroBuffer* reader, ShapeType* topic);
uint32_t ShapeType_size_of_topic(const ShapeType* topic, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif // _ShapeType_H_
