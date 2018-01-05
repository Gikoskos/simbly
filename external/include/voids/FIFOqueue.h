 /********************
 *  FIFOqueue.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_FIFO_QUEUE_H__
#define VOIDS_FIFO_QUEUE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _FIFOnode {
    void *data;
    struct _FIFOnode *next;
} FIFOnode;

typedef struct _FIFOqueue {
    FIFOnode *head;
    FIFOnode *tail;
    unsigned int total_nodes;
} FIFOqueue;


VOIDS_API FIFOqueue *FIFO_init(vdsErrCode *err);

VOIDS_API void FIFO_enqueue(FIFOqueue *queue,
                            void *node_data,
                            vdsErrCode *err);

VOIDS_API void *FIFO_dequeue(FIFOqueue *queue,
                             vdsErrCode *err);

VOIDS_API void FIFO_destroy(FIFOqueue **queue,
                            vdsUserDataFunc freeData,
                            vdsErrCode *err);

VOIDS_API void FIFO_traverse(FIFOqueue *queue,
                             vdsUserDataFunc handleData,
                             vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_FIFO_QUEUE_H__
