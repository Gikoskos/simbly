 /********************
 *  DoubleEndedQueue.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_DEQUEUE__H__
#define VOIDS_DEQUEUE__H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "DoublyLinkedList.h"


typedef struct _Dequeue {
    DLListNode *head, *tail;
} Dequeue;


VOIDS_API Dequeue *Dequeue_init(vdsErrCode *err);

VOIDS_API DLListNode *Dequeue_push(Dequeue *dequeue,
                                   void *pData,
                                   vdsErrCode *err);

VOIDS_API DLListNode *Dequeue_inject(Dequeue *dequeue,
                                     void *pData,
                                     vdsErrCode *err);

VOIDS_API void *Dequeue_eject(Dequeue *dequeue,
                              vdsErrCode *err);

VOIDS_API void *Dequeue_pop(Dequeue *dequeue,
                            vdsErrCode *err);

VOIDS_API void Dequeue_destroy(Dequeue **dequeue,
                               vdsUserDataFunc freeData,
                               vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_DEQUEUE__H__
