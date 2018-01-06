 /********************
 *  SplayTree.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015-2018 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_SPLAYTREE_H__
#define VOIDS_SPLAYTREE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _SplayTreeNode {
    KeyValuePair item;
    struct _SplayTreeNode *left, *right, *parent;
} SplayTreeNode;

typedef struct _SplayTree {
    SplayTreeNode *root;
    vdsUserCompareFunc KeyCmp;
} SplayTree;


VOIDS_API SplayTree *SplayTree_init(vdsUserCompareFunc KeyCmp,
                                    vdsErrCode *err);

VOIDS_API SplayTreeNode *SplayTree_insert(SplayTree *spt,
                                          void *pData,
                                          void *pKey,
                                          vdsErrCode *err);

VOIDS_API KeyValuePair SplayTree_deleteNode(SplayTree *spt,
                                            SplayTreeNode *sptToDelete,
                                            vdsErrCode *err);

VOIDS_API KeyValuePair SplayTree_deleteByKey(SplayTree *spt,
                                             void *pKey,
                                             vdsErrCode *err);

VOIDS_API SplayTreeNode *SplayTree_findNode(SplayTree *spt,
                                            void *pKey,
                                            vdsErrCode *err);

VOIDS_API void *SplayTree_findData(SplayTree *spt,
                                   void *pKey,
                                   vdsErrCode *err);

VOIDS_API void SplayTree_traverse(SplayTree *spt,
                                  vdsTreeTraversal traversal,
                                  vdsUserDataFunc callback,
                                  vdsErrCode *err);

VOIDS_API void SplayTree_destroy(SplayTree **spt,
                                 vdsUserDataFunc freeData,
                                 vdsErrCode *err);


#ifdef __cplusplus
}
#endif

#endif //VOIDS_SPLAYTREE_H__
