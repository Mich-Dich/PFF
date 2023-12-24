#pragma once

#include "core/pch.h"
#include "core/types.h"

#define DARRAY_DEFAULT_CAPACITY 1   // default starting capacity if macro is used
#define MIN_ARRAY_SIZE 1            // [capacity] can not go under this limit
#define GROW_AMOUNT 10              // will be added or subtracted from [capacity]

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
void darray_push_at(DArray* array, const u64 index, const void* value_ptr);
void darray_pop(DArray* array, void* dest);
void darray_pop_at(DArray* array, const u64 index, void* dest);

#define darray_create(type)                                 __darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))
#define darray_create_size(type, size)                      __darray_create(size, sizeof(type))
