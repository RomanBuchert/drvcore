/**
 * @file    dyn_array.h
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
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#define DYN_ARRAY_REALLOC_ELEMENTS (8U)

typedef struct dyn_array_s dyn_array_t;

struct dyn_array_s {
    size_t elements;
    size_t used;
    void** list;
};

int dyn_array_add(dyn_array_t* array, void* element);

ssize_t dyn_array_find_free_index(dyn_array_t* array);

ssize_t dyn_array_find_element(dyn_array_t* array, void* element);

void* dyn_array_get_by_index(dyn_array_t* array, size_t index);

int dyn_array_remove_element(dyn_array_t* array, void* element);
