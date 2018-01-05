 /********************
 *  Treap.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_TREAP_H__
#define VOIDS_TREAP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "RandomState.h"


typedef struct _TreapNode {
    KeyValuePair item;
    struct _TreapNode *left, *right, *parent;
    int priority; //heap priority for each node. randomized on the node's creation
} TreapNode;

typedef struct _Treap {
    TreapNode *root;
    void *rand_gen_state; //opaque type to store the state for the PRNG
    vdsHeapProperty property;
    vdsUserCompareFunc KeyCmp;
} Treap;


VOIDS_API Treap *Treap_init(vdsUserCompareFunc KeyCmp,
                            vdsHeapProperty property,
                            unsigned int seed,
                            vdsErrCode *err);

VOIDS_API TreapNode *Treap_insert(Treap *treap,
                                  void *pKey,
                                  void *pData,
                                  vdsErrCode *err);

VOIDS_API KeyValuePair Treap_deleteNode(Treap *treap,
                                        TreapNode *treapToDelete,
                                        vdsErrCode *err);

VOIDS_API KeyValuePair Treap_deleteByKey(Treap *treap,
                                         void *pKey,
                                         vdsErrCode *err);

VOIDS_API TreapNode *Treap_findNode(Treap *treap,
                                    void *pKey,
                                    vdsErrCode *err);

VOIDS_API void *Treap_findData(Treap *treap,
                               void *pKey,
                               vdsErrCode *err);

VOIDS_API void Treap_traverse(Treap *treap,
                              vdsTreeTraversal traversal,
                              vdsUserDataFunc callback,
                              vdsErrCode *err);

VOIDS_API void Treap_destroy(Treap **treap,
                             vdsUserDataFunc freeData,
                             vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_TREAP_H__
