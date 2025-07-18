#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    // Größe (Bit 0..2)
    TYPE_SIZE_8 = 0,
    TYPE_SIZE_16 = 1,
    TYPE_SIZE_32 = 2,
    TYPE_SIZE_64 = 3,

    // Unsigned / Signed (Bit 3)
    TYPE_UNSIGNED = 0,
    TYPE_SIGNED = (1 << 3),

    // Const (Bit 4)
    TYPE_CONST = (1 << 4),

    // Volatile (Bit 5)
    TYPE_VOLATILE = (1 << 5),

    // Pointer (Bit 6)
    TYPE_CLASS_PTR = (1 << 6),

    // Klasse (Bit 8..15)
    TYPE_CLASS_NONE = (0 << 8),
    TYPE_CLASS_BOOL = (1 << 8),
    TYPE_CLASS_INT = (2 << 8),
    TYPE_CLASS_FLOAT = (3 << 8),
    TYPE_CLASS_STR = (5 << 8),
    TYPE_CLASS_BYTE_STREAM = (6 << 8),

} type_variant_type_t;

typedef struct type_variant_s {
    type_variant_type_t type;
    union {
        bool boolean;           // store bool / bit
        uint64_t uval;          // store unsigned values in
        int64_t sval;           // store signed value in
        float fval;             // store float value in
        double dval;            // store double value in
        void* ptr;              // store all pointers in
        const char* str;        // store string in
        struct {
            char* bytes;        // Pointer to byte stream
            size_t len;         // Length of byte stream
        } byte_stream;          // store a bytestream
    };
} type_variant_t;

#endif //_TYPES_H_
