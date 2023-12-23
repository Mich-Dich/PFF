
#include "IOCore.h"

#include <stdio.h>

// 20 MiB, can probably change this to a higher value without issue.
// Check your target platform.
#define IO_READ_CHUNK_SIZE 2097152
#define IO_READ_ERROR_GENERAL(var)      "Error reading filie: %s. errno: %d\n", var, ERROR_STR
#define IO_READ_ERROR_MEMORY(var)       "Not enough free memory to read file: %s\n", var

// Adapted from https://stackoverflow.com/a/44894946 (not the chosen answer) by Nominal Animal
File read_from_file(const char* path) {

    File file = { .is_valid = false };
    FILE* fp = fopen(path, "rb");
    CL_VALIDATE(fp, return file, "", IO_READ_ERROR_GENERAL(path));

    char* data = NULL;
    char* tmp;
    size_t used = 0;
    size_t size = 0;
    size_t n;

    while (true) {

        if (used + IO_READ_CHUNK_SIZE + 1 > size) {
            size = used + IO_READ_CHUNK_SIZE + 1;

            CL_VALIDATE(size > used, return file, "", "Input file too large: %s\n", path);

            tmp = realloc(data, size);
            CL_VALIDATE(tmp, free(data);  return file, "", IO_READ_ERROR_MEMORY(path));

            data = tmp;
        }

        n = fread(data + used, 1, IO_READ_CHUNK_SIZE, fp);
        if (n == 0)
            break;

        used += n;
    }

    CL_VALIDATE(!ferror(fp), free(data);  return file, "", IO_READ_ERROR_GENERAL(path));

    tmp = realloc(data, used + 1);
    CL_VALIDATE(tmp, free(data);  return file, "", IO_READ_ERROR_MEMORY(path));

    data = tmp;
    data[used] = 0;

    file.data = data;
    file.len = used;
    file.is_valid = true;

    return file;
}

int write_to_file(void* buffer, size_t size, const char* path) {

    return -1;
}
