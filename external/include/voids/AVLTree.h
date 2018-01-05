 /********************
 *  AVLTree.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_AVLTREE_H__
#define VOIDS_AVLTREE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _AVLTreeNode {
    KeyValuePair item;
    struct _AVLTreeNode *left, *right, *parent;
    int height; //count of edges (not nodes)
} AVLTreeNode;

typedef struct _AVLTree {
    AVLTreeNode *root;
    vdsUserCompareFunc KeyCmp;
} AVLTree;


VOIDS_API AVLTree *AVLTree_init(vdsUserCompareFunc KeyCmp,
                                vdsErrCode *err);

VOIDS_API AVLTreeNode *AVLTree_insert(AVLTree *avlt,
                                      void *pKey,
                                      void *pData,
                                      vdsErrCode *err);

VOIDS_API KeyValuePair AVLTree_deleteNode(AVLTree *avlt,
                                          AVLTreeNode *avltToDelete,
                                          vdsErrCode *err);

VOIDS_API KeyValuePair AVLTree_deleteByKey(AVLTree *avlt,
                                           void *pKey,
                                           vdsErrCode *err);

VOIDS_API void *AVLTree_findData(AVLTree *avlt,
                                 void *pKey,
                                 vdsErrCode *err);

VOIDS_API AVLTreeNode *AVLTree_findNode(AVLTree *avlt,
                                        void *pKey,
                                        vdsErrCode *err);

VOIDS_API void AVLTree_traverse(AVLTree *avlt,
                                vdsTreeTraversal traversal,
                                vdsUserDataFunc callback,
                                vdsErrCode *err);

VOIDS_API void AVLTree_destroy(AVLTree **avlt,
                               vdsUserDataFunc freeData,
                               vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_AVLTREE_H__
