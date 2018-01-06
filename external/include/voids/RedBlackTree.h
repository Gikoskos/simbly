 /********************
 *  RedBlackTree.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_REDBLACKTREE_H__
#define VOIDS_REDBLACKTREE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _RBTreeNode {
    KeyValuePair item;
    struct _RBTreeNode *left, *right, *parent;
    char color;
} RBTreeNode;

typedef struct _RBTree {
    RBTreeNode *root, *nil;
    vdsUserCompareFunc KeyCmp;
} RBTree;


VOIDS_API RBTree *RBTree_init(vdsUserCompareFunc KeyCmp,
                              vdsErrCode *err);

VOIDS_API RBTreeNode *RBTree_insert(RBTree *rbt,
                                    void *pKey,
                                    void *pData,
                                    vdsErrCode *err);

VOIDS_API KeyValuePair RBTree_deleteNode(RBTree *rbt,
                                         RBTreeNode *rbtToDelete,
                                         vdsErrCode *err);

VOIDS_API KeyValuePair RBTree_deleteByKey(RBTree *rbt,
                                          void *pKey,
                                          vdsErrCode *err);

VOIDS_API RBTreeNode *RBTree_findNode(RBTree *rbt,
                                      void *pKey,
                                      vdsErrCode *err);

VOIDS_API void *RBTree_findData(RBTree *rbt,
                                void *pKey,
                                vdsErrCode *err);

VOIDS_API void RBTree_traverse(RBTree *rbt,
                               vdsTreeTraversal traversal,
                               vdsUserDataFunc callback,
                               vdsErrCode *err);

VOIDS_API void RBTree_destroy(RBTree **rbt,
                              vdsUserDataFunc freeData,
                              vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_REDBLACKTREE_H__
