#include "core/container/darray.h"

#include "core/logger.h"


// ------------------------------------------------------------------------------------------ private functions ------------------------------------------------------------------------------------------

void __darray_resize(DArray* array, const bool Scale_up);


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

// copys [stride] amound of bytes from content of [value_ptr] to end of array
void darray_push(DArray* array, const void* value_ptr) {

    // CL_LOG_FUNC_START("[size: %llu]  [capcazity: %llu]  [stride: %llu]", array->size, array->capacity, array->stride);
    DArray* loc_buffer = array;
    if (array->size >= array->capacity)
        __darray_resize(array, true);

    u64 addr = (u64)array->array;
    addr += (array->size * array->stride);
    memcpy((void*)addr, value_ptr, array->stride);
    array->size++;

    // CL_LOG_FUNC_END("[size: %llu]  [capcazity: %llu]  [stride: %llu]", array->size, array->capacity, array->stride)
}

void darray_push_at(DArray* array, const u64 index, const void* value_ptr) {

    CL_VALIDATE(index < array->size, return, "", "Index outside the bounds of this array! Index/Length: [%llu/%llu], ", index, array->size);

    if (array->size >= array->capacity)
        __darray_resize(array, true);

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
void darray_pop(DArray* array, void* dest) {

    CL_VALIDATE(array->size > 0, return, "", "Tryed to pop from an empty array");

    // CL_LOG_FUNC_START("[size: %llu]  [capcazity: %llu]  [stride: %llu]", array->size, array->capacity, array->stride);
    DArray* loc_buffer = array;
    if (array->capacity - array->size >= GROW_AMOUNT && array->capacity - array->size > MIN_ARRAY_SIZE)
        __darray_resize(array, false);

    u64 addr = (u64)array->array;
    addr += ((array->size - 1) * array->stride);
    memcpy(dest, (void*)addr, array->stride);
    array->size--;

    // CL_LOG_FUNC_END("[size: %llu]  [capcazity: %llu]  [stride: %llu]", array->size, array->capacity, array->stride)
}

//
void darray_pop_at(DArray* array, const u64 index, void* dest) {

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
void __darray_resize(DArray* array, const bool Scale_up) {
    
    u64 grow__size = (array->capacity * array->stride) + (GROW_AMOUNT * array->stride);
    u64 srink_size = (array->capacity * array->stride) - (GROW_AMOUNT * array->stride);
    u64 new_size = Scale_up ? grow__size : srink_size;

    void* new_array = (void*)malloc(new_size);
    CL_ASSERT(new_array != 0, "Allocated memory for [new_array] [size: %llu] [elements: %llu]", "Failed to allocate memory for [new_array] [used size: %llu] [elements: %llu]", new_size, new_size / array->stride);

    if (Scale_up)
        array->capacity += GROW_AMOUNT;
    else
        array->capacity -= GROW_AMOUNT;

    memcpy(new_array, array->array, (array->size * array->stride));
    free(array->array);

    array->array = new_array;
}