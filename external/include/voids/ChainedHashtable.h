 /********************
 *  ChainedHashTable.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_CHAINEDHASHTABLE_H__
#define VOIDS_CHAINEDHASHTABLE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "AssociationList.h"


typedef struct _ChainedHashtable {
    DictListNode **chains;
    vdsUserCompareFunc KeyCmp;
    vdsUserHashFunc Hash;
    size_t size;
} ChainedHashtable;


VOIDS_API ChainedHashtable *ChainedHash_init(size_t size,
                                             vdsUserCompareFunc KeyCmp,
                                             vdsUserHashFunc Hash,
                                             vdsErrCode *err);

VOIDS_API DictListNode *ChainedHash_insert(ChainedHashtable *table,
                                           void *pData,
                                           void *pKey,
                                           size_t key_size,
                                           vdsErrCode *err);

VOIDS_API KeyValuePair ChainedHash_delete(ChainedHashtable *table,
                                          void *pKey,
                                          size_t key_size,
                                          vdsErrCode *err);

VOIDS_API DictListNode *ChainedHash_find(ChainedHashtable *table,
                                         void *pKey,
                                         size_t key_size,
                                         vdsErrCode *err);

VOIDS_API void ChainedHash_destroy(ChainedHashtable **table,
                                   vdsUserDataFunc freeData,
                                   vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_CHAINEDHASHTABLE_H__
