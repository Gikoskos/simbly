 /********************
 *  AssociationList.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/

#ifndef VOIDS_ASSOCIATION_LIST_H__
#define VOIDS_ASSOCIATION_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"



typedef struct _DictListNode {
    KeyValuePair item;
    struct _DictListNode *nxt;
} DictListNode;



VOIDS_API DictListNode *DictList_insert(DictListNode **dictListHead,
                                        void *pData,
                                        void *pKey,
                                        vdsUserCompareFunc KeyCmp,
                                        vdsErrCode *err);

VOIDS_API DictListNode *DictList_append(DictListNode **dictListHead,
                                        void *pData,
                                        void *pKey,
                                        vdsUserCompareFunc KeyCmp,
                                        vdsErrCode *err);

VOIDS_API DictListNode *DictList_insertAfter(DictListNode *dictListPrev,
                                             void *pData,
                                             void *pKey,
                                             vdsErrCode *err);

VOIDS_API KeyValuePair DictList_deleteByKey(DictListNode **dictListHead,
                                            void *pKey,
                                            vdsUserCompareFunc KeyCmp,
                                            vdsErrCode *err);

VOIDS_API DictListNode *DictList_findByKey(DictListNode *dictListHead,
                                           void *pKey,
                                           vdsUserCompareFunc KeyCmp,
                                           vdsErrCode *err);

VOIDS_API void DictList_traverse(DictListNode *dictListHead,
                                 vdsUserDataFunc handleData,
                                 vdsErrCode *err);

VOIDS_API void DictList_destroy(DictListNode **dictListHead,
                                vdsUserDataFunc freeData,
                                vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_ASSOCIATION_LIST_H__
