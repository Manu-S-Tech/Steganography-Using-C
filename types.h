#ifndef TYPES_H

#define TYPES_H

#include <string.h>

/* User defined types */

/* Defines unsigned integer type used for image and file sizes */
typedef unsigned int uint;

/* Status values used as function return types */
typedef enum
{
    e_failure,
    e_success
} Status;

/* Defines the operation selected by the user */
typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif