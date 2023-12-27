
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

#define INTERNAL_VALIDATE(expr, return_value)				if (!(expr)) {									\
																return return_value;						\
															}

#define VECTOR_ASSERT_INPUT									assert(v != NULL);								\
															assert(v->data != NULL)

#define VALIDATE_VACTOR_SIZE								if (v->size == 0) {								\
																return NULL;								\
															}

// ------------------------------------------------------------------------------------------ simple functions ------------------------------------------------------------------------------------------

//
int c_vec_empty(const c_vector* v) {

	VECTOR_ASSERT_INPUT;

	return v->size == 0;
}

//
size_t c_vec_size(const c_vector* v) {

	VECTOR_ASSERT_INPUT;

	return v->size;
}

//
size_t c_vec_stride(const c_vector* v) {

	VECTOR_ASSERT_INPUT;

	return v->stride;
}

// ------------------------------------------------------------------------------------------ functions ------------------------------------------------------------------------------------------


/**
 * Return a pointer to element number [i] of [v], without error checking.
 *
 * @param v pointer to [c_vector]
 * @param i index
 * @return pointer to element [i]
 */
#define __c_AT(v, i) ((void *) ((char *) (v)->data + (i) * (v)->stride))


// Minimum allocation size in bytes.
#define c_MIN_ALLOC (64u)

// Grow the capacity of [v] to at least [capacity]
// If more space is needed, grow [v] to [capacity], but at least by a factor of 1.5
static int __c_i_grow(c_vector* v, size_t capacity) {
	void* newdata;
	size_t newcapacity;

	VECTOR_ASSERT_INPUT;

	if (capacity <= v->capacity) {
		return c_OK;
	}

	assert(v->stride > 0);

	if (capacity >= (size_t)-1 / v->stride) {
		return c_ERANGE;
	}

	// Growth factor 2 for small vectors, 1.5 for larger
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

//
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

	// Minimum capacity is c_MIN_ALLOC bytes or 1 element
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

//
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

//
void* c_vec_at(c_vector* v, size_t i) {

	VECTOR_ASSERT_INPUT;

	if (i >= v->size) {
		return NULL;
	}

	return __c_AT(v, i);
}

//
void* c_vec_front(c_vector* v) {

	VECTOR_ASSERT_INPUT;
	VALIDATE_VACTOR_SIZE;

	return v->data;
}

//
void* c_vec_back(c_vector* v) {

	VECTOR_ASSERT_INPUT;
	VALIDATE_VACTOR_SIZE;

	return __c_AT(v, v->size - 1);
}

//
void* c_vec_data(c_vector* v) {

	VECTOR_ASSERT_INPUT;
	VALIDATE_VACTOR_SIZE;

	return v->data;
}

//
int c_vec_reserve(c_vector* v, size_t capacity) {

	VECTOR_ASSERT_INPUT;

	if (capacity <= v->capacity) {
		return c_OK;
	}

	assert(v->stride > 0);

	if (capacity >= (size_t)-1 / v->stride) {
		return c_ERANGE;
	}

	void* newdata;
	newdata = realloc(v->data, capacity * v->stride);

	if (newdata == NULL) {
		return c_ENOMEM;
	}

	v->data = newdata;
	v->capacity = capacity;

	return c_OK;
}

//
size_t c_vec_capacity(const c_vector* v) {

	VECTOR_ASSERT_INPUT;

	return v->capacity;
}

//
int c_vec_shrink_to_fit(c_vector* v) {

	VECTOR_ASSERT_INPUT;

	if (v->capacity == v->size) {
		return c_OK;
	}

	void* newdata;
	size_t newcapacity = v->size;

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

//
int c_vec_clear(c_vector* v) {

	VECTOR_ASSERT_INPUT;

	v->size = 0;

	return c_OK;
}

//
int c_vec_assign(c_vector* v, const void* data, size_t nobj) {

	VECTOR_ASSERT_INPUT;

	assert(v->stride > 0);

	if (nobj >= (size_t)-1 / v->stride) {
		return c_ERANGE;
	}

	if (nobj > v->capacity) {
		int res = __c_i_grow(v, nobj);

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

//
int c_vec_replace(c_vector* v, size_t i, size_t j, const void* data, size_t nobj) {

	VECTOR_ASSERT_INPUT;

	if (i > j || j > v->size) {
		return c_ERANGE;
	}

	assert(v->stride > 0);

	if (nobj >= (size_t)-1 / v->stride - (v->size - (j - i))) {
		return c_ERANGE;
	}

	if (v->size - (j - i) + nobj > v->capacity) {
		int res = __c_i_grow(v, v->size - (j - i) + nobj);

		if (res != c_OK) {
			return res;
		}
	}

	if (j < v->size && i + nobj != j) {
		memmove(__c_AT(v, i + nobj), __c_AT(v, j), (v->size - j) * v->stride);
	}

	if (data != NULL && nobj > 0) {
		memcpy(__c_AT(v, i), data, nobj * v->stride);
	}

	v->size = v->size - (j - i) + nobj;

	return c_OK;
}

//
int c_vec_insert(c_vector* v, size_t i, const void* data, size_t nobj) {
	return c_vec_replace(v, i, i, data, nobj);
}

//
int c_vec_erase(c_vector* v, size_t i, size_t j) {
	return c_vec_replace(v, i, j, NULL, 0);
}

//
int c_vec_push_back(c_vector* v, const void* data) {

	VECTOR_ASSERT_INPUT;

	if (v->size + 1 > v->capacity) {
		int res = __c_i_grow(v, v->size + 1);

		if (res != c_OK)
			return res;
	}
	assert(v->stride > 0);

	if (data != NULL)
		memcpy(__c_AT(v, v->size), data, v->stride);

	v->size += 1;

	return c_OK;
}

//
int c_vec_pop_back(c_vector* v) {

	VECTOR_ASSERT_INPUT;
	VALIDATE_VACTOR_SIZE;

	v->size -= 1;

	return c_OK;
}

//
int c_vec_resize(c_vector* v, size_t size) {

	assert(v != NULL);
	assert(v->data != NULL);
	assert(v->stride > 0);

	if (size >= (size_t)-1 / v->stride) {
		return c_ERANGE;
	}

	if (size > v->capacity) {
		int res = __c_i_grow(v, size);

		if (res != c_OK) {
			return res;
		}
	}

	v->size = size;

	return c_OK;
}

//
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

//
int c_vec_swap(c_vector* scv1, c_vector* scv2) {

	assert(scv1 != NULL);
	assert(scv2 != NULL);

	c_vector tmp;
	tmp = *scv1;
	*scv1 = *scv2;
	*scv2 = tmp;

	return c_OK;
}

