#pragma once

#include <stddef.h>


// Adapted from https://github.com/jibsen/scv


#ifdef __cplusplus
extern "C" {
#endif


	typedef struct {
		void* data;			// Pointer to the underlying memory
		size_t stride;		// Size of each element in bytes
		size_t size;		// Used size in number of elements
		size_t capacity;	// Capacity in number of elements
	} c_vector;


	typedef enum {
		c_OK = 0,		// Success
		c_ERROR = -1,		// Generic error
		c_ENOMEM = -2,		// Out of memory
		c_ERANGE = -3,		// Overflow or out of range
		c_EINVAL = -4		// Invalid argument
	} c_error_code;

	// Create a new `c_vector` return pointer to [c_vector], [NULL] on error
	c_vector* c_vec_new(size_t stride, size_t capacity);

	//  Destroy [v], freeing the associated memory. [v] pointer to [c_vector]
	void c_vec_delete(c_vector* v);

	/**
	 * Return a pointer to element number `i` of `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @param i index
	 * @return pointer to element `i`, `NULL` on error
	 */
	void* c_vec_at(c_vector* v, size_t i);

	/**
	 * Return a pointer to the first element of `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @return pointer to first element, `NULL` on error
	 */
	void* c_vec_front(c_vector* v);

	/**
	 * Return a pointer to the last element of `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @return pointer to last element, `NULL` on error
	 */
	void* c_vec_back(c_vector* v);

	/**
	 * Return a pointer to the elements of `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @return pointer to elements, `NULL` if empty
	 */
	void* c_vec_data(c_vector* v);

	/**
	 * Check if `v` is empty.
	 *
	 * @param v pointer to `c_vector`
	 * @return non-zero if empty
	 */
	int c_vec_empty(const c_vector* v);

	/**
	 * Return the size of `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @return size in number of elements
	 */
	size_t c_vec_size(const c_vector* v);

	/**
	 * Return the size of each element in `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @return size of each element in bytes
	 */
	size_t c_vec_stride(const c_vector* v);

	/**
	 * Reserve space in `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @param capacity requested capacity
	 * @return zero on success, error code on error
	 */
	int c_vec_reserve(c_vector* v, size_t capacity);

	/**
	 * Return the capacity of `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @return capacity in number of elements
	 */
	size_t c_vec_capacity(const c_vector* v);

	/**
	 * Trim the capacity of `v` to the number of elements used.
	 *
	 * @param v pointer to `c_vector`
	 * @return zero on success, error code on error
	 */
	int c_vec_shrink_to_fit(c_vector* v);

	/**
	 * Remove all elements from `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @return zero on success, error code on error
	 */
	int c_vec_clear(c_vector* v);

	/**
	 * Replace the contents of `v` with `nobj` elements from `data`.
	 *
	 * If `data` is `NULL`, any assigned elements are not initialized.
	 *
	 * `data` must not point inside `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @param data pointer to data to copy into assigned elements
	 * @param nobj number of elements to assign
	 * @return zero on success, error code on error
	 */
	int c_vec_assign(c_vector* v, const void* data, size_t nobj);

	/**
	 * Replace elements from `i` up to, but not including, `j` in `v`, with
	 * `nobj` elements from `data`.
	 *
	 * If `data` is `NULL`, any inserted elements are not initialized.
	 *
	 * `i` can be `c_size(v)`, in which case elements are added at the end.
	 *
	 * `data` must not point inside `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @param i start index
	 * @param j end index
	 * @param data pointer to data to copy into new elements
	 * @param nobj number of elements to insert
	 * @return zero on success, error code on error
	 */
	int c_vec_replace(c_vector* v, size_t i, size_t j, const void* data, size_t nobj);

	/**
	 * Insert `nobj` elements from `data` before element number `i` of `v`.
	 *
	 * If `data` is `NULL`, inserted elements are not initialized.
	 *
	 * `i` can be `c_size(v)`, in which case elements are added at the end.
	 *
	 * `data` must not point inside `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @param i index
	 * @param data pointer to data to copy into new elements
	 * @param nobj number of elements to insert
	 * @return zero on success, error code on error
	 */
	int c_vec_insert(c_vector* v, size_t i, const void* data, size_t nobj);

	/**
	 * Remove elements from `i` up to, but not including, `j` from `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @param i start index
	 * @param j end index
	 * @return zero on success, error code on error
	 */
	int c_vec_erase(c_vector* v, size_t i, size_t j);

	/**
	 * Insert a single element from `data` at the end of `v`.
	 *
	 * If `data` is `NULL`, the inserted element is not initialized.
	 *
	 * `data` must not point inside `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @param data pointer to data to copy into new element
	 * @return zero on success, error code on error
	 */
	int c_vec_push_back(c_vector* v, const void* data);

	/**
	 * Remove the last element of `v`.
	 *
	 * @param v pointer to `c_vector`
	 * @return zero on success, error code on error
	 */
	int c_vec_pop_back(c_vector* v);

	/**
	 * Resize the number of elements in `v`.
	 *
	 * Any new elements are uninitialized.
	 *
	 * @param v pointer to `c_vector`
	 * @param size new size
	 * @return zero on success, error code on error
	 */
	int c_vec_resize(c_vector* v, size_t size);

	/**
	 * Copy elements from `src` to `dst`.
	 *
	 * @param dst pointer to destination `c_vector`
	 * @param src pointer to source `c_vector`
	 * @return zero on success, error code on error
	 */
	int c_vec_copy(c_vector* dst, const c_vector* src);

	/**
	 * Swap elements between `scv1` and `scv2`.
	 *
	 * @param scv1 pointer to `c_vector`
	 * @param scv2 pointer to `c_vector`
	 * @return zero on success, error code on error
	 */
	int c_vec_swap(c_vector* scv1, c_vector* scv2);

#ifdef __cplusplus
} /* extern "C" */
#endif






/*
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
*/