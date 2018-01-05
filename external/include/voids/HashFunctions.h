 /********************
 *  HashFunctions.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_HASHFUNCTIONS_H__
#define VOIDS_HASHFUNCTIONS_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


VOIDS_API size_t HashCode(const void *pKey, size_t key_size);

VOIDS_API size_t HashDiv(size_t key_hash, size_t array_len);

VOIDS_API size_t HashMult(size_t key_hash, size_t array_len);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_HASHFUNCTIONS_H__
