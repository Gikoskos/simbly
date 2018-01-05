 /********************
 *  DoublyLinkedList.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_DL_LIST_H__
#define VOIDS_DL_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _DLListNode {
    void *pData;
    struct _DLListNode *nxt, *prv;
} DLListNode;



VOIDS_API DLListNode *DLList_insert(DLListNode **dllHead,
                                    void *pData,
                                    vdsErrCode *err);

VOIDS_API DLListNode *DLList_append(DLListNode **dllHead,
                                    void *pData,
                                    vdsErrCode *err);

VOIDS_API DLListNode *DLList_concat(DLListNode *dll_1,
                                    DLListNode *dll_2,
                                    vdsErrCode *err);

VOIDS_API DLListNode *DLList_insertAfter(DLListNode *dllPrev,
                                         void *pData,
                                         vdsErrCode *err);

VOIDS_API void *DLList_deleteNode(DLListNode **dllHead,
                                  DLListNode *dllToDelete,
                                  vdsErrCode *err);

VOIDS_API void *DLList_deleteData(DLListNode **dllHead,
                                  void *pToDelete,
                                  vdsUserCompareFunc DataCmp,
                                  vdsErrCode *err);

VOIDS_API DLListNode *DLList_at(DLListNode *dllHead,
                                size_t idx,
                                vdsErrCode *err);

VOIDS_API DLListNode *DLList_find(DLListNode *dllHead,
                                  void *pToFind,
                                  vdsUserCompareFunc DataCmp,
                                  vdsErrCode *err);

VOIDS_API void DLList_traverse(DLListNode *dllHead,
                               vdsUserDataFunc handleData,
                               vdsErrCode *err);

VOIDS_API void DLList_destroy(DLListNode **dllHead,
                              vdsUserDataFunc freeData,
                              vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_DL_LIST_H__
