/**
 * @file    dyn_array.c
 * @brief   Dynamic array.
 *
 * @details
 * This module provides a dynamic array that can store pointers.
 * If the array ist full, it will be automaticly expanded.
 *
 * @author  Roman Buchert <roman.buchert@googlemail.com>
 * @date    2025-08-05
 * @version 0.1
 */

#include "dyn_array.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int dyn_array_add(dyn_array_t* array, void* element) {
    if ((array == NULL) || (element == NULL)) {
        errno = EINVAL;
        return -1;
    }

    /* Realloc, if list is full */
    if (array->used == array->elements) {
        void** new_ptr = realloc(array->list, (array->elements + DYN_ARRAY_REALLOC_ELEMENTS) * sizeof(void*));
        if (new_ptr == NULL) {
            errno = ENOMEM;
            return -1;
        }
        memset(&new_ptr[array->elements], 0, DYN_ARRAY_REALLOC_ELEMENTS * sizeof(void*));
        array->list = new_ptr;
        array->elements += DYN_ARRAY_REALLOC_ELEMENTS;
    }

    /* Get free index */
    ssize_t index = dyn_array_find_free_index(array);
    if (index < 0) {
        return -1;
    }

    array->list[index] = element;
    array->used++;
    return 0;
}

ssize_t dyn_array_find_free_index(dyn_array_t* array) {
    if (array == NULL) {
        errno = EINVAL;
        return -1;
    }
    
    for (size_t i = 0; i < array->elements; i++) {
        if (array->list[i] == NULL) {
            return i;
        }
    }
    errno = ENOENT;
    return -1;
}

ssize_t dyn_array_find_element(dyn_array_t* array, void* element) {
    if ((array == NULL) || (element == NULL)) {
        errno = EINVAL;
        return -1;
    }

    for (size_t i = 0; i < array->elements; i++) {
        if (array->list[i] == element) {
            return i;
        }
    }
    errno = ENOENT;
    return -1;
}

void* dyn_array_get_by_index(dyn_array_t* array, size_t index) {
    if (array == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (index < array->elements) {
        errno = 0;  // We do this here explicid, because a NULL-Pointer can be a valid pointer on an empty index.
        return array->list[index];
    }
    
    errno = EINVAL;
    return NULL;
}

int dyn_array_remove_element(dyn_array_t* array, void* element) {

    ssize_t index;

    if ((array == NULL) || (element == NULL)) {
        errno = EINVAL;
        return -1;
    }

    index = dyn_array_find_element(array, element);
    if (index < 0) {
        return -1;
    }

    array->list[index] = NULL;
    array->used--;

    if (array->used == 0) {
        free(array->list);
        array->list = NULL;
        array->elements = 0;
    }
    return 0;
}