
#include "util/container/c_vec.h"

#include "util/IOCore/logger.h"




/*
* scv - Simple C Vector
*
*scv.c
*
*Copyright 2003 - 2014 Joergen Ibsen
*
*Licensed under the Apache License, Version 2.0 (the "License");
*you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
*Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
*limitations under the License.
*
* SPDX - License - Identifier: Apache - 2.0
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "c_vec.h"

/**
 * Return a pointer to element number `i` of `v`, without error checking.
 *
 * @param v pointer to `c_vector`
 * @param i index
 * @return pointer to element `i`
 */
#define c_AT(v, i) ((void *) ((char *) (v)->data + (i) * (v)->stride))

 /**
  * Minimum allocation size in bytes.
  */
#define c_MIN_ALLOC (64u)

  /**
   * Grow the capacity of `v` to at least `capacity`.
   *
   * If more space is needed, grow `v` to `capacity`, but at least by a factor
   * of 1.5.
   *
   * @param v pointer to `c_vector`
   * @param capacity requested capacity
   * @return zero on success, error code on error
   */
	static int c_i_grow(c_vector* v, size_t capacity) {
	void* newdata;
	size_t newcapacity;

	assert(v != NULL);
	assert(v->data != NULL);

	if (capacity <= v->capacity) {
		return c_OK;
	}

	assert(v->stride > 0);

	if (capacity >= (size_t)-1 / v->stride) {
		return c_ERANGE;
	}

	/* Growth factor 2 for small vectors, 1.5 for larger */
	if (v->capacity < 4096 / v->stride) {
		newcapacity = v->capacity + v->capacity + 1;
	} else {
		newcapacity = v->capacity + v->capacity / 2 + 1;
	}

	if (capacity > newcapacity || newcapacity >= (size_t)-1 / v->stride) {
		newcapacity = capacity;
	}

	newdata = realloc(v->data, newcapacity * v->stride);

	if (newdata == NULL) {
		return c_ENOMEM;
	}

	v->data = newdata;
	v->capacity = newcapacity;

	return c_OK;
}

c_vector* c_vec_new(size_t stride, size_t capacity) {
	c_vector* v;

	if (capacity == 0) {
		capacity = 1;
	}

	if (stride == 0 || capacity >= (size_t)-1 / stride) {
		return NULL;
	}

	v = (c_vector*)malloc(sizeof * v);

	if (v == NULL) {
		return NULL;
	}

	/* Minimum capacity is c_MIN_ALLOC bytes or 1 element */
	if (capacity * stride < c_MIN_ALLOC) {
		capacity = (c_MIN_ALLOC + (stride - 1)) / stride;
	}

	v->data = malloc(capacity * stride);

	if (v->data == NULL) {
		free(v);
		return NULL;
	}

	v->stride = stride;
	v->size = 0;
	v->capacity = capacity;

	return v;
}

void c_vec_delete(c_vector* v) {
	if (v == NULL) {
		return;
	}

	if (v->data != NULL) {
		free(v->data);
		v->data = NULL;
	}

	v->stride = 0;
	v->size = 0;
	v->capacity = 0;

	free(v);
}

void* c_vec_at(c_vector* v, size_t i) {
	assert(v != NULL);
	assert(v->data != NULL);

	if (i >= v->size) {
		return NULL;
	}

	return c_AT(v, i);
}

void* c_vec_front(c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	if (v->size == 0) {
		return NULL;
	}

	return v->data;
}

void* c_vec_back(c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	if (v->size == 0) {
		return NULL;
	}

	return c_AT(v, v->size - 1);
}

void* c_vec_data(c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	if (v->size == 0) {
		return NULL;
	}

	return v->data;
}

int c_vec_empty(const c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	return v->size == 0;
}

size_t c_vec_size(const c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	return v->size;
}

size_t c_vec_stride(const c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	return v->stride;
}

int c_vec_reserve(c_vector* v, size_t capacity) {
	void* newdata;

	assert(v != NULL);
	assert(v->data != NULL);

	if (capacity <= v->capacity) {
		return c_OK;
	}

	assert(v->stride > 0);

	if (capacity >= (size_t)-1 / v->stride) {
		return c_ERANGE;
	}

	newdata = realloc(v->data, capacity * v->stride);

	if (newdata == NULL) {
		return c_ENOMEM;
	}

	v->data = newdata;
	v->capacity = capacity;

	return c_OK;
}

size_t c_vec_capacity(const c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	return v->capacity;
}

int c_vec_shrink_to_fit(c_vector* v) {
	void* newdata;
	size_t newcapacity;

	assert(v != NULL);
	assert(v->data != NULL);

	if (v->capacity == v->size) {
		return c_OK;
	}

	newcapacity = v->size;

	assert(v->stride > 0);
	assert(newcapacity < (size_t)-1 / v->stride);

	/* Minimum capacity is c_MIN_ALLOC bytes or 1 element */
	if (newcapacity * v->stride < c_MIN_ALLOC) {
		newcapacity = (c_MIN_ALLOC + (v->stride - 1)) / v->stride;
	}

	newdata = realloc(v->data, newcapacity * v->stride);

	if (newdata == NULL) {
		return c_ENOMEM;
	}

	v->data = newdata;
	v->capacity = newcapacity;

	return c_OK;
}

int c_vec_clear(c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	v->size = 0;

	return c_OK;
}

int c_vec_assign(c_vector* v, const void* data, size_t nobj) {
	assert(v != NULL);
	assert(v->data != NULL);

	assert(v->stride > 0);

	if (nobj >= (size_t)-1 / v->stride) {
		return c_ERANGE;
	}

	if (nobj > v->capacity) {
		int res = c_i_grow(v, nobj);

		if (res != c_OK) {
			return res;
		}
	}

	if (data != NULL && nobj > 0) {
		memcpy(v->data, data, nobj * v->stride);
	}

	v->size = nobj;

	return c_OK;
}

int c_vec_replace(c_vector* v, size_t i, size_t j, const void* data, size_t nobj) {
	assert(v != NULL);
	assert(v->data != NULL);

	if (i > j || j > v->size) {
		return c_ERANGE;
	}

	assert(v->stride > 0);

	if (nobj >= (size_t)-1 / v->stride - (v->size - (j - i))) {
		return c_ERANGE;
	}

	if (v->size - (j - i) + nobj > v->capacity) {
		int res = c_i_grow(v, v->size - (j - i) + nobj);

		if (res != c_OK) {
			return res;
		}
	}

	if (j < v->size && i + nobj != j) {
		memmove(c_AT(v, i + nobj), c_AT(v, j), (v->size - j) * v->stride);
	}

	if (data != NULL && nobj > 0) {
		memcpy(c_AT(v, i), data, nobj * v->stride);
	}

	v->size = v->size - (j - i) + nobj;

	return c_OK;
}

int c_vec_insert(c_vector* v, size_t i, const void* data, size_t nobj) {
	return c_vec_replace(v, i, i, data, nobj);
}

int c_vec_erase(c_vector* v, size_t i, size_t j) {
	return c_vec_replace(v, i, j, NULL, 0);
}

int c_vec_push_back(c_vector* v, const void* data) {

	assert(v != NULL);
	assert(v->data != NULL);

	if (v->size + 1 > v->capacity) {
		int res = c_i_grow(v, v->size + 1);

		if (res != c_OK)
			return res;
	}
	assert(v->stride > 0);

	if (data != NULL)
		memcpy(c_AT(v, v->size), data, v->stride);

	v->size += 1;

	return c_OK;
}

int c_vec_pop_back(c_vector* v) {
	assert(v != NULL);
	assert(v->data != NULL);

	if (v->size == 0) {
		return c_ERANGE;
	}

	v->size -= 1;

	return c_OK;
}

int c_vec_resize(c_vector* v, size_t size) {
	assert(v != NULL);
	assert(v->data != NULL);

	assert(v->stride > 0);

	if (size >= (size_t)-1 / v->stride) {
		return c_ERANGE;
	}

	if (size > v->capacity) {
		int res = c_i_grow(v, size);

		if (res != c_OK) {
			return res;
		}
	}

	v->size = size;

	return c_OK;
}

int c_vec_copy(c_vector* dst, const c_vector* src) {
	assert(dst != NULL);
	assert(dst->data != NULL);
	assert(src != NULL);
	assert(src->data != NULL);

	if (dst == src || dst->stride != src->stride) {
		return c_EINVAL;
	}

	return c_vec_assign(dst, src->data, src->size);
}

int c_vec_swap(c_vector* scv1, c_vector* scv2) {
	c_vector tmp;

	assert(scv1 != NULL);
	assert(scv2 != NULL);

	tmp = *scv1;
	*scv1 = *scv2;
	*scv2 = tmp;

	return c_OK;
}



/*
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

    CL_LOG_FUNC_START("[size: %llu]  [capcazity: %llu]  [stride: %llu]", array->size, array->capacity, array->stride);
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

    CL_LOG_FUNC_START("[size: %llu]  [capcazity: %llu]  [stride: %llu]", array->size, array->capacity, array->stride);
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
}*/