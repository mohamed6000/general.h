#pragma once
// Basic dynamic array implementation.
// I want to keep this separate from the general layer.
#include "general.h"

template<typename T>
struct Array {
    s64 allocated = 0;
    s64 count     = 0;
    T *data       = null;

    Allocator allocator = {heap_allocator, null};

    TINYRT_INLINE T &operator[] (s64 index) {
        assert(index >= 0);
        assert(index < this->count);
        assert(this->data != null);
        return this->data[index];
    }
};

template<typename T>
Array<T> array_new(s64 n, Allocator a = {heap_allocator, null}) {
    Array<T> result = {};

    if (!a.proc) {
        a.proc = heap_allocator;
        a.data = null;
    }

    result.allocator = a;

    result.allocated = n;
    result.count     = n;

    result.data = (T *)a.proc(ALLOCATOR_ALLOCATE, n * size_of(T), 0, null, a.data);

    return result;
}

template<typename T>
void array_free(Array<T> *arr) {
    if (arr->data) {
        Allocator a = arr->allocator;
        
        if (!a.proc) {
            a.proc = heap_allocator;
            a.data = null;
        }

        a.proc(ALLOCATOR_FREE, 0, 0, arr->data, a.data);
        arr->data = null;
    }

    arr->count     = 0;
    arr->allocated = 0;
}

template<typename T>
TINYRT_INLINE void array_reset(Array<T> *arr) {
    arr->count = 0;
}

template<typename T>
void array_copy(Array<T> *dest, Array<T> *src) {
    if (dest->allocated < src->count) {
        dest->allocated = src->count;

        Allocator a = dest->allocator;
        
        if (!a.proc) {
            a.proc = heap_allocator;
            a.data = null;
            
            dest->allocator = a;
        }


        if (dest->data)
            a.proc(ALLOCATOR_FREE, 0, 0, dest->data, a.data);
        
        dest->data = (T *)a.proc(ALLOCATOR_ALLOCATE, src->count * size_of(T), 0, null, a.data);
    }

    dest->count = src->count;
    memcpy(dest->data, src->data, (umm)(src->count * size_of(T)));
}

template<typename T>
void array_reserve(Array<T> *arr, s64 reserve) {
    // Only reserve memory if the array is empty.
    if (reserve <= arr->allocated) return;

    s64 size = size_of(T);
    s64 num_bytes = reserve * size;

    if (!arr->allocator.proc) {
        arr->allocator.proc = heap_allocator;
        arr->allocator.data = null;
    }

    Allocator a = arr->allocator;

    void *new_memory = a.proc(ALLOCATOR_RESIZE, num_bytes, arr->count * size, arr->data, a.data);
    assert(new_memory != null);

    if (!new_memory) return;

    arr->data = (T *)new_memory;
    arr->allocated = reserve;
}

template<typename T>
void array_add(Array<T> *arr, T item) {
    if (arr->count >= arr->allocated) {
        s64 reserve_count = 2 * arr->allocated;
        if (reserve_count < 8) reserve_count = 8;

        array_reserve(arr, reserve_count);
    }

    arr->data[arr->count] = item;
    arr->count += 1;
}

template<typename T>
T *array_add(Array<T> *arr) {
    if (arr->count >= arr->allocated) {
        s64 reserve_count = 2 * arr->allocated;
        if (reserve_count < 8) reserve_count = 8;

        array_reserve(arr, reserve_count);
    }

    T *result = arr->data + arr->count;
    arr->count += 1;

    return result;
}

template<typename T>
s64 array_find(Array<T> *array, T item) {
    for (s64 index = 0; index < array->count; ++index) {
        T it = array->data[index];
        if (it == item) return index;
    }

    return -1;  // Not found.
}

template<typename T>
void array_resize(Array<T> *array, s64 size) {
    /*if (array->allocated < size)*/
    {
        if (!array->allocator.proc) {
            array->allocator.proc = heap_allocator;
            array->allocator.data = null;
        }

        Allocator a = array->allocator;

        s64 stride = size_of(T);
        s64 bytes = size * stride;

        void *new_memory = a.proc(ALLOCATOR_RESIZE, bytes, array->allocated * stride, 
                                  array->data, a.data);
        assert(new_memory != null);

        array->data = (T *)new_memory;
        // array->count = size;
        array->allocated = size;
    }
}

template<typename T>
bool array_pop(Array<T> *array, T *value_return) {
    if (array->count == 0) {
        write_string("Panic: Attempt to pop an empty array.\n", /*bool to_standard_error=*/true);
        assert(0);
        return false;
    }

    *value_return = array->data[array->count-1];
    array->count -= 1;
    return true;
}
