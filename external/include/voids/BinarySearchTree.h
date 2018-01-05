 /********************
 *  BinarySearchTree.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_BSTREE_H__
#define VOIDS_BSTREE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _BSTreeNode {
    KeyValuePair item;
    struct _BSTreeNode *left, *right, *parent;
} BSTreeNode;

typedef struct _BSTree {
    BSTreeNode *root;
    vdsUserCompareFunc KeyCmp;
} BSTree;


VOIDS_API BSTree *BSTree_init(vdsUserCompareFunc KeyCmp,
                              vdsErrCode *err);

VOIDS_API BSTreeNode *BSTree_insert(BSTree *bst,
                                    void *pData,
                                    void *pKey,
                                    vdsErrCode *err);

VOIDS_API KeyValuePair BSTree_deleteNode(BSTree *bst,
                                         BSTreeNode *bstToDelete,
                                         vdsErrCode *err);

VOIDS_API KeyValuePair BSTree_deleteByKey(BSTree *bst,
                                          void *pKey,
                                          vdsErrCode *err);

VOIDS_API BSTreeNode *BSTree_findNode(BSTree *bst,
                                      void *pKey,
                                      vdsErrCode *err);

VOIDS_API void *BSTree_findData(BSTree *bst,
                                void *pKey,
                                vdsErrCode *err);

VOIDS_API void BSTree_traverse(BSTree *bst,
                               vdsTreeTraversal traversal,
                               vdsUserDataFunc callback,
                               vdsErrCode *err);

VOIDS_API void BSTree_destroy(BSTree **bst,
                              vdsUserDataFunc freeData,
                              vdsErrCode *err);


#ifdef __cplusplus
}
#endif

#endif //VOIDS_BSTREE_H__
