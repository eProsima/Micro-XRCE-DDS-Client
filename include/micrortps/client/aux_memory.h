#ifndef _MICRORTPS_CLIENT_AUX_MEMORY_H_
#define _MICRORTPS_CLIENT_AUX_MEMORY_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>

typedef struct AuxMemory
{
    void* buffer;
    uint32_t size;
    uint32_t buffer_alloc;

} AuxMemory;

void init_aux_memory(AuxMemory* memory);
void increase_aux_memory(AuxMemory* memory, uint32_t memory_size);
void* request_aux_memory(AuxMemory* memory, uint32_t size);
void free_aux_memory(AuxMemory* memory);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_AUX_MEMORY_H_
