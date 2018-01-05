 /********************
 *  BinomialHeap.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_BINOMIALHEAP_H__
#define VOIDS_BINOMIALHEAP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "BinaryTreeHeap.h" //vdsHeapProperty


typedef struct _BinomialTree {
    KeyValuePair item;
    struct _BinomialTree *child, *sibling, *parent;
    unsigned int order;
} BinomialTree;

typedef struct _BinomialHeap {
    BinomialTree *forest; //this is the first binomial tree on the forest of roots
    vdsUserCompareFunc KeyCmp;
    vdsHeapProperty property;
} BinomialHeap;


VOIDS_API BinomialHeap *BinomialHeap_init(vdsUserCompareFunc KeyCmp,
                                          vdsHeapProperty property,
                                          vdsErrCode *err);

VOIDS_API BinomialTree *BinomialHeap_push(BinomialHeap *binheap,
                                          void *pData,
                                          void *pKey,
                                          vdsErrCode *err);

VOIDS_API KeyValuePair BinomialHeap_pop(BinomialHeap *binheap,
                                        vdsErrCode *err);

VOIDS_API KeyValuePair BinomialHeap_getTop(BinomialHeap *binheap,
                                           vdsErrCode *err);

VOIDS_API void *BinomialHeap_replaceKey(BinomialHeap *binheap,
                                        BinomialTree *tree,
                                        void *pNewKey,
                                        vdsErrCode *err);

VOIDS_API void BinomialHeap_destroy(BinomialHeap **binheap,
                                    vdsUserDataFunc freeData,
                                    vdsErrCode *err);


#ifdef __cplusplus
}
#endif

#endif //VOIDS_BINOMIALHEAP_H__
