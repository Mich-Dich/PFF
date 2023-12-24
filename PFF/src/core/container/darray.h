#pragma once

#include "core/pch.h"

#define DARRAY_DEFAULT_CAPACITY 2
#define DARRAY_RESIZE_FACTOR 2


typedef struct {
    u64 capacity;
    u64 size;
    u64 stride;
    void* array;
} DArray;

DArray* __darray_create(const u64 capacity, const u64 stride);
void darray_destroy(DArray* array);
void darray_clear(DArray* array);
void darray_push(DArray* array, const void* value_ptr);
void __darray_push_at(DArray* array, u64 index, const void* value_ptr);
void __darray_pop(DArray* array, void* dest);
void __darray_pop_at(DArray* array, u64 index, void* dest);

#define darray_create(type)                                 __darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))
#define darray_create_size(type, size)                      __darray_create(size, sizeof(type))


