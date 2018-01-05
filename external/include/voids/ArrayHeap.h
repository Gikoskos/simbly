 /********************
 *  ArrayHeap.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_ARRAYHEAP_H__
#define VOIDS_ARRAYHEAP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _ArrayHeap {
    void **array;
    size_t size, idx;
    vdsUserCompareFunc DataCmp;
    vdsHeapProperty property;
} ArrayHeap;


VOIDS_API ArrayHeap *ArrayHeap_init(vdsUserCompareFunc DataCmp,
                                    vdsHeapProperty property,
                                    size_t size,
                                    vdsErrCode *err);

VOIDS_API void *ArrayHeap_push(ArrayHeap *arrheap,
                               void *pData,
                               vdsErrCode *err);

VOIDS_API void *ArrayHeap_pop(ArrayHeap *arrheap,
                              vdsErrCode *err);

VOIDS_API void ArrayHeap_destroy(ArrayHeap **arrheap,
                                 vdsUserDataFunc freeData,
                                 vdsErrCode *err);


#ifdef __cplusplus
}
#endif

#endif //VOIDS_ARRAYHEAP_H__
