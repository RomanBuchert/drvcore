/**
 * @file    registry_types.h
 * @brief   Types for driver registry implementation for (sub)driver management.
 *
 * @details
 * This module provides the required type definitions for the registry module.
 *
 * @author  Roman Buchert <roman.buchert@googlemail.com>
 * @date    2025-07-17
 * @version 0.1
 */
#ifndef _REGISTRY_TYPES_H_
#define _REGISTRY_TYPES_H_
#include "driver_types.h"

typedef struct registry_s {
    driver_t** driver_list;
    size_t driver_list_size;
    size_t driver_list_used;
} registry_t;


#endif // _REGISTRY_TYPES_H_