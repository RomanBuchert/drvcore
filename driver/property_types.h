/**
 * @file    property_types.h
 * @brief   Property data types.
 *
 
 * @author  Roman Buchert <roman.buchert@googlemail.com>
 * @date    2025-07-17
 * @version 0.1
 */

#ifndef _PROPERTY_TYPES_H_
#define _PROPERTY_TYPES_H_

#include <types.h>

typedef struct property_s property_t;
typedef struct property_list_s property_list_t;

typedef enum property_type_e {
    PROP_UNKNOWN,
    PROP_LIST
} property_type_t;

struct property_s {
    const property_type_t type;                         //Name. Fixed.
    const type_variant_t property;                      //Property. Fixed.
};

struct property_list_s {
    const size_t count;                             //Number of properties. Fixed.
    const property_t* list;                         //Array with properties. Fixed.
};

#endif //_PROPERTY_TYPES_H_
