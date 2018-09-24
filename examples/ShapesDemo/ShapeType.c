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
 * @file ShapeType.c
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#include "ShapeType.h"

#include <microcdr/microcdr.h>
#include <string.h>

bool ShapeType_serialize_topic(MicroBuffer* writer, const ShapeType* topic)
{
    (void) serialize_string(writer, topic->color);

    (void) serialize_int32_t(writer, topic->x);

    (void) serialize_int32_t(writer, topic->y);

    (void) serialize_int32_t(writer, topic->shapesize);

    return writer->error;
}

bool ShapeType_deserialize_topic(MicroBuffer* reader, ShapeType* topic)
{
    (void) deserialize_string(reader, topic->color, 255);

    (void) deserialize_int32_t(reader, &topic->x);

    (void) deserialize_int32_t(reader, &topic->y);

    (void) deserialize_int32_t(reader, &topic->shapesize);

    return reader->error;
}

uint32_t ShapeType_size_of_topic(const ShapeType* topic, uint32_t size)
{
    uint32_t previousSize = size;
    size += get_alignment(size, 4) + 4 + (uint32_t)strlen(topic->color) + 1;

    size += get_alignment(size, 4) + 4;

    size += get_alignment(size, 4) + 4;

    size += get_alignment(size, 4) + 4;

    return size - previousSize;
}
