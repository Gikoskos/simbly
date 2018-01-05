 /********************
 *  RingBuffer.h
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef VOIDS_CIRCULAR_BUFF_H__
#define VOIDS_CIRCULAR_BUFF_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"


typedef struct _RingBuffer {
    size_t size, write, read, available;
    void **buff;
} RingBuffer;



VOIDS_API RingBuffer *RingBuffer_init(size_t buff_size,
                                      vdsErrCode *err);

VOIDS_API void RingBuffer_resize(RingBuffer **cBuff,
                                 size_t new_size,
                                 vdsErrCode *err);

VOIDS_API void RingBuffer_write(RingBuffer *cBuff,
                                void *pData,
                                vdsErrCode *err);

VOIDS_API void *RingBuffer_read(RingBuffer *cBuff,
                                vdsErrCode *err);

VOIDS_API void RingBuffer_destroy(RingBuffer **cBuff,
                                  vdsUserDataFunc freeData,
                                  vdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //VOIDS_CIRCULAR_BUFF_H__
