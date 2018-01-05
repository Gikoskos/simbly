 /********************
 *  CircularSinglyLinkedList.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_CSL_LIST_H__
#define VOIDS_CSL_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _CSLListNode {
    void *pData;
    struct _CSLListNode *nxt;
} CSLListNode;



VOIDS_API CSLListNode *CSLList_insert(CSLListNode **csllHead,
                                      void *pData,
                                      vdsErrCode *err);

VOIDS_API CSLListNode *CSLList_append(CSLListNode **csllHead,
                                      void *pData,
                                      vdsErrCode *err);

VOIDS_API CSLListNode *CSLList_concat(CSLListNode *csll_1,
                                      CSLListNode *csll_2,
                                      vdsErrCode *err);

VOIDS_API CSLListNode *CSLList_insertAfter(CSLListNode *csllPrev,
                                           void *pData,
                                           vdsErrCode *err);

VOIDS_API void *CSLList_deleteNode(CSLListNode **csllHead,
                                   CSLListNode *csllToDelete,
                                   vdsErrCode *err);

VOIDS_API void *CSLList_deleteData(CSLListNode **csllHead,
                                   void *pToDelete,
                                   vdsUserCompareFunc DataCmp,
                                   vdsErrCode *err);

VOIDS_API CSLListNode *CSLList_at(CSLListNode *csllHead,
                                  size_t idx,
                                  vdsErrCode *err);

VOIDS_API CSLListNode *CSLList_find(CSLListNode *csllHead,
                                    void *pToFind,
                                    vdsUserCompareFunc DataCmp,
                                    vdsErrCode *err);

VOIDS_API void CSLList_traverse(CSLListNode *csllHead,
                                vdsUserDataFunc handleData,
                                vdsErrCode *err);

VOIDS_API void CSLList_destroy(CSLListNode **csllHead,
                               vdsUserDataFunc freeData,
                               vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_CSL_LIST_H__
