#ifndef _U_NX_DEBUG_H
#define _U_NX_DEBUG_H

// clang-format off

#include "nx_api.h"

/* This file contains NetX-specific debug helpers. */

/* API */
const char *nx_status_toString(UINT status); /* Converts a NetX status macro to a printable string. Meant to be used with DEBUG_PRINTLN() (defined in another file). */

// clang-format on
#endif /* u_nx_debug.h */