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

typedef struct driver_property_s driver_property_t;
typedef struct driver_properties_s driver_properties_t;

struct driver_property_s {
    const char* const name;                         //Name. Fixed.
    const type_variant_t property;                  //Property. Fixed.
};

struct driver_properties_s {
    const size_t count;                             //Number of properties. Fixed.
    const driver_property_t* list;                  //Array with properties. Fixed.
};



#endif //_PROPERTY_TYPES_H_
