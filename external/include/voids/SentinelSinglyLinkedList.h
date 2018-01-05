 /********************
 *  SentinelSinglyLinkedList.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_SENT_SL_LIST_H__
#define VOIDS_SENT_SL_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "SinglyLinkedList.h"


typedef struct _SSLList {
    SLListNode *head, *sentinel;
} SSLList;


VOIDS_API SSLList *SSLList_init(vdsErrCode *err);

VOIDS_API SLListNode *SSLList_insert(SSLList *ssllList,
                                     void *pData,
                                     vdsErrCode *err);

VOIDS_API SLListNode *SSLList_append(SSLList *ssllList,
                                     void *pData,
                                     vdsErrCode *err);

VOIDS_API void *SSLList_deleteData(SSLList *ssllList,
                                   void *pData,
                                   vdsUserCompareFunc DataCmp,
                                   vdsErrCode *err);

VOIDS_API void *SSLList_deleteNode(SSLList *ssllList,
                                   SLListNode *sllToDelete,
                                   vdsErrCode *err);

VOIDS_API SLListNode *SSLList_at(SSLList *ssllList,
                                 size_t idx,
                                 vdsErrCode *err);

VOIDS_API SSLList *SSLList_concat(SSLList *ssll1,
                                  SSLList *ssll2,
                                  vdsErrCode *err);

VOIDS_API SLListNode *SSLList_find(SSLList *ssllList,
                                   void *pToFind,
                                   vdsUserCompareFunc DataCmp,
                                   vdsErrCode *err);

VOIDS_API void SSLList_traverse(SSLList *ssllList,
                                vdsUserDataFunc handleData,
                                vdsErrCode *err);

VOIDS_API void SSLList_destroy(SSLList **ssllList,
                               vdsUserDataFunc freeData,
                               vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_SENT_SL_LIST_H__
