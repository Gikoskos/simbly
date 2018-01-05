 /********************
 *  CircularDoublyLinkedList.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_CDL_LIST_H__
#define VOIDS_CDL_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _CDLListNode {
    void *pData;
    struct _CDLListNode *nxt, *prv;
} CDLListNode;



VOIDS_API CDLListNode *CDLList_insert(CDLListNode **cdllHead,
                                      void *pData,
                                      vdsErrCode *err);

VOIDS_API CDLListNode *CDLList_append(CDLListNode **cdllHead,
                                      void *pData,
                                      vdsErrCode *err);

VOIDS_API CDLListNode *CDLList_concat(CDLListNode *cdll_1,
                                      CDLListNode *cdll_2,
                                      vdsErrCode *err);

VOIDS_API CDLListNode *CDLList_insertAfter(CDLListNode *cdllPrev,
                                           void *pData,
                                           vdsErrCode *err);

VOIDS_API void *CDLList_deleteNode(CDLListNode **cdllHead,
                                   CDLListNode *cdllToDelete,
                                   vdsErrCode *err);

VOIDS_API void *CDLList_deleteData(CDLListNode **cdllHead,
                                   void *pToDelete,
                                   vdsUserCompareFunc DataCmp,
                                   vdsErrCode *err);

VOIDS_API CDLListNode *CDLList_at(CDLListNode *cdllHead,
                                  size_t idx,
                                  vdsErrCode *err);

VOIDS_API CDLListNode *CDLList_find(CDLListNode *cdllHead,
                                    void *pToFind,
                                    vdsUserCompareFunc DataCmp,
                                    vdsErrCode *err);

VOIDS_API void CDLList_traverse(CDLListNode *cdllHead,
                                vdsUserDataFunc handleData,
                                vdsErrCode *err);

VOIDS_API void CDLList_destroy(CDLListNode **cdllHead,
                               vdsUserDataFunc freeData,
                               vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_CDL_LIST_H__
