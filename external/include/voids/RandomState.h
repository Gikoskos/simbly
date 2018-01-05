 /********************
 *  RandomState.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_RANDOMSTATE_H__
#define VOIDS_RANDOMSTATE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


//I keep the PRNG types and API as opaque as possible, so that if someone
//wants to replace it with their own implementation they won't have to change
//code in the data structures, just the implementations in RandomFunctions.c


VOIDS_API void *RandomState_init(unsigned int seed, vdsErrCode *err);

VOIDS_API unsigned int RandomState_genUInt(void *state, vdsErrCode *err);

VOIDS_API int RandomState_genInt(void *state, vdsErrCode *err);

VOIDS_API unsigned long RandomState_genULong(void *state, vdsErrCode *err);

VOIDS_API long RandomState_genLong(void *state, vdsErrCode *err);

VOIDS_API void RandomState_destroy(void **state, vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_RANDOMSTATE_H__
