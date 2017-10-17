#include "micrortps/client/aux_memory.h"

#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
    #include <stdio.h>
#endif

void init_aux_memory(AuxMemory* memory, uint32_t memory_size)
{
    memory->buffer = malloc(memory_size);
    memory->buffer_alloc = memory_size;
    memory->size = 0;
}

void* request_aux_memory(AuxMemory* memory, uint32_t size)
{
    #ifndef NDEBUG
    if(memory->size + size > memory->buffer_alloc)
    {
        printf("ERROR: AuxMemory oveflow.\n");
        return NULL;
    }
    #endif

    uint32_t init = memory->size;
    memory->size += size;
    return ((uint8_t*)memory->buffer) + init;
}

void free_aux_memory(AuxMemory* memory)
{
    free(memory->buffer);
}