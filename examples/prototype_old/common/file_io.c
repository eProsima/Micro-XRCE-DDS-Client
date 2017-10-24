#include "file_io.h"

#define RED "\e[1;31m"
#define RESTORE_COLOR "\e[0m"

#include <stdio.h>
#include <string.h>

void init_file_io(SharedFile* shared_file, const char* out_file_name, const char* in_file_name)
{
    strcpy(shared_file->out_file_name, out_file_name);
    strcpy(shared_file->in_file_name, in_file_name);

    fclose(fopen(shared_file->in_file_name, "wb"));
}

void send_file_io(uint8_t* buffer, uint32_t length, void* args)
{
    SharedFile* shared_file = (SharedFile*)args;

    FILE* out_file = fopen(shared_file->out_file_name, "wb");
    if(out_file == NULL)
        printf("    %s[OUT FILE ERROR]%s\n", RED, RESTORE_COLOR);

    fwrite(buffer, 1, length, out_file);

    fclose(out_file);
}

uint32_t received_file_io(uint8_t* buffer, uint32_t size, void* args)
{
    SharedFile* shared_file = (SharedFile*)args;

    FILE* in_file = fopen(shared_file->in_file_name, "rb");
     if(in_file == NULL)
        printf("    %s[IN FILE ERROR]%s\n", RED, RESTORE_COLOR);

    fseek(in_file, 0L, SEEK_END);
    uint32_t in_file_size = ftell(in_file);
    fseek(in_file, 0L, SEEK_SET);

    if(in_file_size > size)
        in_file_size = size;

    in_file_size = fread(buffer, 1, in_file_size, in_file);

    fclose(in_file);
    fclose(fopen(shared_file->in_file_name,"wb"));

    return in_file_size;
}