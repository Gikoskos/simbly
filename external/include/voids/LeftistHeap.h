 /********************
 *  LeftistHeap.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_LEFTISTHEAP_H__
#define VOIDS_LEFTISTHEAP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "BinaryTreeHeap.h"

typedef struct _LeftistHeapNode {
    void *pData;
    struct _LeftistHeapNode *left, *right, *parent;
    unsigned int npl; //null path length
} LeftistHeapNode;

typedef struct _LeftistHeap {
    LeftistHeapNode *root;
    vdsUserCompareFunc DataCmp;
    vdsHeapProperty property;
} LeftistHeap;


VOIDS_API LeftistHeap *LeftistHeap_init(vdsUserCompareFunc DataCmp,
                                        vdsHeapProperty property,
                                        vdsErrCode *err);

VOIDS_API LeftistHeapNode *LeftistHeap_push(LeftistHeap *lheap,
                                            void *pData,
                                            vdsErrCode *err);

VOIDS_API void *LeftistHeap_pop(LeftistHeap *lheap,
                                vdsErrCode *err);

VOIDS_API void LeftistHeap_destroy(LeftistHeap **lheap,
                                   vdsUserDataFunc freeData,
                                   vdsErrCode *err);


#ifdef __cplusplus
}
#endif

#endif //VOIDS_LEFTISTHEAP_H__
