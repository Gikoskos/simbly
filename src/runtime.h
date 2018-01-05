#ifndef SIMBLY_RUNTIME_H__
#define SIMBLY_RUNTIME_H__

#include "common.h"
#include "program.h"

typedef struct _runtime_s {
    CDLListNode *program_list, *curr;
    pthread_t thrd_id;
    pthread_mutex_t lock;
    pthread_cond_t list_not_empty;
    int running, program_cnt;
    void *rand_generator;
} runtime_s;


runtime_s *runtime_init(void);
void runtime_attach_program(runtime_s *rt, program_s *prog);
void runtime_stop(runtime_s *rt);

#endif //SIMBLY_RUNTIME_H__
