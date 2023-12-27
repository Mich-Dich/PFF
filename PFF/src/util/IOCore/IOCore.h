#pragma once

#include "pch.h"
#include "util/dll_interface.h"

typedef struct file {
    char* data;
    size_t len;
    bool is_valid;
} File;

File read_from_file(const char* path);
int write_to_file(void* buffer, size_t size, const char* path);
