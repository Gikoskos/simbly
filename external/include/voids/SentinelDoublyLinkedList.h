 /********************
 *  SentinelDoublyLinkedList.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_SENT_DL_LIST_H__
#define VOIDS_SENT_DL_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "DoublyLinkedList.h"


typedef struct _SDLList {
    DLListNode *head, *sentinel;
} SDLList;


VOIDS_API SDLList *SDLList_init(vdsErrCode *err);

VOIDS_API DLListNode *SDLList_insert(SDLList *sdllList,
                                     void *pData,
                                     vdsErrCode *err);

VOIDS_API DLListNode *SDLList_append(SDLList *sdllList,
                                     void *pData,
                                     vdsErrCode *err);

VOIDS_API void *SDLList_deleteNode(SDLList *sdllList,
                                   DLListNode *sdllToDelete,
                                   vdsErrCode *err);

VOIDS_API void *SDLList_deleteData(SDLList *sdllList,
                                   void *pToDelete,
                                   vdsUserCompareFunc DataCmp,
                                   vdsErrCode *err);

VOIDS_API DLListNode *SDLList_at(SDLList *sdllHead,
                                 size_t idx,
                                 vdsErrCode *err);

VOIDS_API SDLList *SDLList_concat(SDLList *sdll1,
                                  SDLList *sdll2,
                                  vdsErrCode *err);

VOIDS_API DLListNode *SDLList_find(SDLList *sdllList,
                                   void *pToFind,
                                   vdsUserCompareFunc DataCmp,
                                   vdsErrCode *err);

VOIDS_API void SDLList_traverse(SDLList *sdllList,
                                vdsUserDataFunc handleData,
                                vdsErrCode *err);

VOIDS_API void SDLList_destroy(SDLList **sdllList,
                               vdsUserDataFunc freeData,
                               vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_SENT_DL_LIST_H__
