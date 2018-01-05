 /********************
 *  BinaryTreeHeap.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_BINARYTREEHEAP_H__
#define VOIDS_BINARYTREEHEAP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"

typedef struct _BTHeapNode {
    void *pData;
    struct _BTHeapNode *left, *right, *parent;
} BTHeapNode;

typedef struct _BTHeap {
    BTHeapNode *root;
    unsigned int total_nodes;
    vdsUserCompareFunc DataCmp;
    vdsHeapProperty property;
} BTHeap;


VOIDS_API BTHeap *BTHeap_init(vdsUserCompareFunc DataCmp,
                              vdsHeapProperty property,
                              vdsErrCode *err);

VOIDS_API BTHeapNode *BTHeap_push(BTHeap *btheap,
                                  void *pData,
                                  vdsErrCode *err);

VOIDS_API void *BTHeap_pop(BTHeap *btheap,
                           vdsErrCode *err);

VOIDS_API void *BTHeap_replace(BTHeap *btheap,
                               void *pData,
                               vdsErrCode *err);

VOIDS_API void BTHeap_destroy(BTHeap **btheap,
                              vdsUserDataFunc freeData,
                              vdsErrCode *err);


#ifdef __cplusplus
}
#endif

#endif //VOIDS_BINARYTREEHEAP_H__
