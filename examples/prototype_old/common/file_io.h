#ifndef _TEST_PROTOTYPE_FILE_IO_H_
#define _TEST_PROTOTYPE_FILE_IO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef struct SharedFile
{
    char out_file_name[256];
    char in_file_name[256];

} SharedFile;

void init_file_io(SharedFile* shared_file, const char* out_file_name, const char* in_file_name);
void send_file_io(uint8_t* buffer, uint32_t length, void* data);
uint32_t received_file_io(uint8_t* buffer, uint32_t size, void* data);

#ifdef __cplusplus
}
#endif

#endif //_TEST_PROTOTYPE_FILE_IO_H_