 /********************
 *  LIFOstack.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_LIFO_STACK_H__
#define VOIDS_LIFO_STACK_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _LIFOnode {
    void *data;
    struct _LIFOnode *next;
} LIFOnode;

typedef struct _LIFOstack {
    LIFOnode *head;
    LIFOnode *tail;
    unsigned int total_nodes;
} LIFOstack;


VOIDS_API LIFOstack *LIFO_init(vdsErrCode *err);

VOIDS_API void LIFO_push(LIFOstack *stack,
                         void *node_data,
                         vdsErrCode *err);

VOIDS_API void *LIFO_pop(LIFOstack *stack,
                         vdsErrCode *err);

VOIDS_API void LIFO_destroy(LIFOstack **stack,
                            vdsUserDataFunc freeData,
                            vdsErrCode *err);

VOIDS_API void LIFO_traverse(LIFOstack *stack,
                             vdsUserDataFunc handleData,
                             vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_LIFO_STACK_H__
