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


typedef struct _RedBlackTreeNode {
    KeyValuePair item;
    struct _RedBlackTreeNode *left, *right, *parent;
    char color;
} RedBlackTreeNode;

typedef struct _RedBlackTree {
    RedBlackTreeNode *root, *nil;
    vdsUserCompareFunc KeyCmp;
} RedBlackTree;


VOIDS_API RedBlackTree *RBTree_init(vdsUserCompareFunc KeyCmp,
                                    vdsErrCode *err);

VOIDS_API RedBlackTreeNode *RBTree_insert(RedBlackTree *rbt,
                                          void *pKey,
                                          void *pData,
                                          vdsErrCode *err);

VOIDS_API KeyValuePair RBTree_deleteNode(RedBlackTree *rbt,
                                         RedBlackTreeNode *rbtToDelete,
                                         vdsErrCode *err);

VOIDS_API KeyValuePair RBTree_deleteByKey(RedBlackTree *rbt,
                                          void *pKey,
                                          vdsErrCode *err);

VOIDS_API RedBlackTreeNode *RBTree_findNode(RedBlackTree *rbt,
                                            void *pKey,
                                            vdsErrCode *err);

VOIDS_API void *RBTree_findData(RedBlackTree *rbt,
                                void *pKey,
                                vdsErrCode *err);

VOIDS_API void RBTree_traverse(RedBlackTree *rbt,
                               vdsTreeTraversal traversal,
                               vdsUserDataFunc callback,
                               vdsErrCode *err);

VOIDS_API void RBTree_destroy(RedBlackTree **rbt,
                              vdsUserDataFunc freeData,
                              vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_REDBLACKTREE_H__
