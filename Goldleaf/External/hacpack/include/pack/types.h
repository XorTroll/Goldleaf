#ifndef HACPACK_TYPES_H
#define HACPACK_TYPES_H

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint8_t byte;

typedef enum {
    VALIDITY_UNCHECKED = 0,
    VALIDITY_INVALID,
    VALIDITY_VALID
} validity_t;

#define GET_VALIDITY_STR(validity) ((validity == VALIDITY_VALID) ? "GOOD" : "FAIL")

#define le_dword(a) (a)
#define le_word(a) (a)

#endif
