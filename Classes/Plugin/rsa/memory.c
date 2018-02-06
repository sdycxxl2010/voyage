/* memory.c
 *
 * Copyright (C) 2006-2015 wolfSSL Inc.
 *
 * This file is part of wolfSSL. (formerly known as CyaSSL)
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "settings.h"

/* check old macros @wc_fips */
#if defined(USE_CYASSL_MEMORY) && !defined(USE_WOLFSSL_MEMORY)
    #define USE_WOLFSSL_MEMORY
#endif
#if defined(CYASSL_MALLOC_CHECK) && !defined(WOLFSSL_MALLOC_CHECK)
    #define WOLFSSL_MALLOC_CHECK
#endif


#include "./memory.h"
#include "error-crypt.h"


/* Set these to default values initially. */
static wolfSSL_Malloc_cb  malloc_function = 0;
static wolfSSL_Free_cb    free_function = 0;
static wolfSSL_Realloc_cb realloc_function = 0;

int wolfSSL_SetAllocators(wolfSSL_Malloc_cb  mf,
                          wolfSSL_Free_cb    ff,
                          wolfSSL_Realloc_cb rf)
{
    int res = 0;

    if (mf)
        malloc_function = mf;
    else
        res = BAD_FUNC_ARG;

    if (ff)
        free_function = ff;
    else
        res = BAD_FUNC_ARG;

    if (rf)
        realloc_function = rf;
    else
        res = BAD_FUNC_ARG;

    return res;
}


void* wolfSSL_Malloc(size_t size)
{
    void* res = 0;

    if (malloc_function)
        res = malloc_function(size);
    else
        res = malloc(size);

    #ifdef WOLFSSL_MALLOC_CHECK
        if (res == NULL)
            puts("wolfSSL_malloc failed");
    #endif
                
    return res;
}

void wolfSSL_Free(void *ptr)
{
    if (free_function)
        free_function(ptr);
    else
        free(ptr);
}

void* wolfSSL_Realloc(void *ptr, size_t size)
{
    void* res = 0;

    if (realloc_function)
        res = realloc_function(ptr, size);
    else
        res = realloc(ptr, size);

    return res;
}
