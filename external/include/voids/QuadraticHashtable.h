 /********************
 *  QuadraticHashtable.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_QUADRATICHASHTABLE_H__
#define VOIDS_QUADRATICHASHTABLE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "LinearHashtable.h" //for HashArrayElement


typedef struct _QuadHashtable {
    HashArrayElement *array;
    vdsUserCompareFunc KeyCmp;
    vdsUserHashFunc Hash;
    size_t size;
    size_t total_elements;
} QuadHashtable;


VOIDS_API QuadHashtable *QuadHash_init(size_t size,
                                       vdsUserCompareFunc KeyCmp,
                                       vdsUserHashFunc Hash,
                                       vdsErrCode *err);

VOIDS_API KeyValuePair *QuadHash_insert(QuadHashtable *table,
                                        void *pData,
                                        void *pKey,
                                        size_t key_size,
                                        vdsUserDataFunc freeData,
                                        vdsErrCode *err);

VOIDS_API KeyValuePair QuadHash_delete(QuadHashtable *table,
                                       void *pKey,
                                       size_t key_size,
                                       vdsErrCode *err);

VOIDS_API KeyValuePair *QuadHash_find(QuadHashtable *table,
                                      void *pKey,
                                      size_t key_size,
                                      vdsErrCode *err);

VOIDS_API void QuadHash_destroy(QuadHashtable **table,
                                vdsUserDataFunc freeData,
                                vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_QUADRATICHASHTABLE_H__
