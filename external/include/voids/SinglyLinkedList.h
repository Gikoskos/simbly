 /********************
 *  SinglyLinkedList.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_SL_LIST_H__
#define VOIDS_SL_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _SLListNode {
    void *pData;
    struct _SLListNode *nxt;
} SLListNode;



VOIDS_API SLListNode *SLList_insert(SLListNode **sllHead,
                                    void *pData,
                                    vdsErrCode *err);

VOIDS_API SLListNode *SLList_append(SLListNode **sllHead,
                                    void *pData,
                                    vdsErrCode *err);

VOIDS_API SLListNode *SLList_insertAfter(SLListNode *sllPrev,
                                         void *pData,
                                         vdsErrCode *err);

VOIDS_API void *SLList_deleteNode(SLListNode **sllHead,
                                  SLListNode *sllToDelete,
                                  vdsErrCode *err);

VOIDS_API void *SLList_deleteData(SLListNode **sllHead,
                                  void *pData,
                                  vdsUserCompareFunc DataCmp,
                                  vdsErrCode *err);

VOIDS_API SLListNode *SLList_at(SLListNode *sllHead,
                                size_t idx,
                                vdsErrCode *err);

VOIDS_API SLListNode *SLList_concat(SLListNode *sll1,
                                    SLListNode *sll2,
                                    vdsErrCode *err);

VOIDS_API SLListNode *SLList_find(SLListNode *sllHead,
                                  void *pToFind,
                                  vdsUserCompareFunc DataCmp,
                                  vdsErrCode *err);

VOIDS_API void SLList_traverse(SLListNode *sllHead,
                               vdsUserDataFunc handleData,
                               vdsErrCode *err);

VOIDS_API void SLList_destroy(SLListNode **sllHead,
                              vdsUserDataFunc freeData,
                              vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_SL_LIST_H__
