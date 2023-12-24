#include "core/container/darray.h"

#include "core/logger.h"


// ------------------------------------------------------------------------------------------ private functions ------------------------------------------------------------------------------------------

DArray* __darray_resize(DArray* array);


// ------------------------------------------------------------------------------------------ private functions ------------------------------------------------------------------------------------------

// 
DArray* __darray_create(const u64 capacity, const u64 stride) {

    DArray* loc_array = (DArray*)malloc(sizeof(DArray));
    CL_ASSERT(loc_array != 0, "", "Failed to create DArray using malloc()");
    loc_array->capacity = (u64)capacity;
    loc_array->size = (u64)0;
    loc_array->stride = (u64)stride;

    u64 array_size = capacity * stride;
    loc_array->array = (void*)malloc(array_size);
    CL_ASSERT(loc_array->array != 0, "Allocated memory for array->array [size: %llu]", "Failed to create DArray->array using malloc() [used size: %llu]", array_size);
    memset(loc_array->array, 0, array_size);

    return loc_array;
}

// 
void darray_destroy(DArray* array) {

    free(array->array);
    free(array);
}

//
void darray_clear(DArray* array) {

    array->size = 0;
}

// 
void darray_push(DArray* array, const void* value_ptr) {

    DArray* loc_buffer = array;
    CL_LOG_FUNC_START("[size: %llu]  [capcazity: %llu]  [stride: %llu]", loc_buffer->size, loc_buffer->capacity, loc_buffer->stride);
    if (loc_buffer->size >= loc_buffer->capacity)
        loc_buffer = __darray_resize(loc_buffer);

    u64 addr = (u64)loc_buffer->array;
    addr += (loc_buffer->size * loc_buffer->stride);
    memcpy((void*)addr, value_ptr, loc_buffer->stride);
    loc_buffer->size++;

    array = loc_buffer;

    CL_LOG_FUNC_END("[size: %llu]  [capcazity: %llu]  [stride: %llu]", array->size, array->capacity, array->stride)
}

void __darray_push_at(DArray* array, u64 index, const void* value_ptr) {

    CL_VALIDATE(index < array->size, return, "", "Index outside the bounds of this array! Index/Length: [%llu/%llu], ", index, array->size);

    if (array->size >= array->capacity)
        array = __darray_resize(array);

    u64 addr = (u64)array;

    // If not on the last element, copy the rest outward.
    if (index != array->size - 1)
        memcpy( (void*)(addr + ((index + 1) * array->stride)),
                (void*)(addr + (index * array->stride)),
                array->stride * (array->size - index));
    
    // Set the value at the index
    memcpy((void*)(addr + (index * array->stride)), value_ptr, array->stride);

    array->size++;
}

//
void __darray_pop(DArray* array, void* dest) {

    u64 addr = (u64)array->array;
    addr += ((array->size - 1) * array->stride);
    memcpy(dest, (void*)addr, array->stride);
    array->size--;
}

//
void __darray_pop_at(DArray* array, u64 index, void* dest) {

    CL_VALIDATE(index < array->size, return, "", "Index outside the bounds of this array! Index/Length: [%llu/%llu], ", index, array->size);

    u64 addr = (u64)array->array;
    memcpy(dest, (void*)(addr + (index * array->stride)), array->stride);

    // If not on the last element, snip out the entry and copy the rest inward.
    if (index != array->size - 1)
        memcpy((void*)(addr + (index * array->stride)),
            (void*)(addr + ((index + 1) * array->stride)),
            array->stride * (array->size - index));
    
    array->size--;
}

// ------------------------------------------------------------------------------------------ private functions ------------------------------------------------------------------------------------------

// 
DArray* __darray_resize(DArray* array) {

    CL_LOG_FUNC_START("")
    DArray* loc_array = array;
    DArray* new_array = __darray_create((DARRAY_RESIZE_FACTOR * loc_array->capacity), loc_array->stride);
    new_array->size = array->size;
    memcpy(new_array->array, loc_array->array, loc_array->size * loc_array->stride);

    CL_LOG(Warn, "New array: [size: %llu, stride: %llu capacity: %llu]", new_array->size, new_array->stride, new_array->capacity);

    new_array->size = loc_array->size;
    darray_destroy(array);

    CL_LOG_FUNC_END("")
    return new_array;
}