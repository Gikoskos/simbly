 /********************
 *  LinearHashtable.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_LINEARHASHTABLE_H__
#define VOIDS_LINEARHASHTABLE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _HashArrayElement {
    KeyValuePair item;
    unsigned char state;
    size_t key_hash; //saving this to make rehashing slightly faster
} HashArrayElement;

typedef struct _LinHashtable {
    HashArrayElement *array;
    vdsUserCompareFunc KeyCmp;
    vdsUserHashFunc Hash;
    int rehash;
    size_t total_elements;
    size_t size;
} LinHashtable;


VOIDS_API LinHashtable *LinHash_init(size_t size,
                                     vdsUserCompareFunc KeyCmp,
                                     vdsUserHashFunc Hash,
                                     int rehash,
                                     vdsErrCode *err);

VOIDS_API KeyValuePair *LinHash_insert(LinHashtable *table,
                                       void *pData,
                                       void *pKey,
                                       size_t key_size,
                                       vdsUserDataFunc freeData,
                                       vdsErrCode *err);

VOIDS_API KeyValuePair LinHash_delete(LinHashtable *table,
                                      void *pKey,
                                      size_t key_size,
                                      vdsErrCode *err);

VOIDS_API KeyValuePair *LinHash_find(LinHashtable *table,
                                     void *pKey,
                                     size_t key_size,
                                     vdsErrCode *err);

VOIDS_API void LinHash_destroy(LinHashtable **table,
                               vdsUserDataFunc freeData,
                               vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_LINEARHASHTABLE_H__
