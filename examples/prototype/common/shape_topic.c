#include "shape_topic.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CYAN "\e[1;36m"
#define RESTORE_COLOR "\e[0m"

// -------------------------------------------------------------------------------
//                               TOPIC SERIALIZATION
// -------------------------------------------------------------------------------
void serialize_shape_topic(SerializedBufferHandle* writer, const void* topic_struct, void* callback_object)
{
    ShapeTopic* topic = (ShapeTopic*)topic_struct;

    serialize_byte_4(writer, topic->color_length);
    serialize_array(writer, (uint8_t*)topic->color, topic->color_length);
    align_to(writer, 4);
    serialize_byte_4(writer, topic->x);
    serialize_byte_4(writer, topic->y);
    serialize_byte_4(writer, topic->size);
}

void* deserialize_shape_topic(SerializedBufferHandle* reader, void* callback_object)
{
    ShapeTopic* topic = malloc(sizeof(ShapeTopic));

    deserialize_byte_4(reader, &topic->color_length);
    topic->color = malloc(topic->color_length);
    deserialize_array(reader, (uint8_t*)topic->color, topic->color_length);
    align_to(reader, 4);
    deserialize_byte_4(reader, &topic->x);
    deserialize_byte_4(reader, &topic->y);
    deserialize_byte_4(reader, &topic->size);

    return topic;
}

uint32_t size_of_shape_topic(const void* topic_struct)
{
    ShapeTopic* topic = (ShapeTopic*)topic_struct;

    int alignment = (topic->color_length % 4) ? (4 - (topic->color_length % 4)) : 0;
    return
         + sizeof(topic->color_length)
         + strlen(topic->color) + 1
         + sizeof(alignment)
         + sizeof(topic->x)
         + sizeof(topic->y)
         + sizeof(topic->size);
}

// -------------------------------------------------------------------------------
//                                       UTIL
// -------------------------------------------------------------------------------
void print_shape_topic(const ShapeTopic* topic)
{
    printf("      %s[TOPIC | color: %s (%u) | x: %u | y: %u | size: %u]%s\n",
            CYAN,
            topic->color,
            topic->color_length,
            topic->x,
            topic->y,
            topic->size,
            RESTORE_COLOR);
}