#ifndef SIMBLY_GLOBAL_H__
#define SIMBLY_GLOBAL_H__

#include "common.h"
#include "program.h"

typedef struct _global_var_s {
    int *count;
    size_t len;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
} global_var_s;


global_var_s *global_var_init(size_t total);
void global_var_destroy(global_var_s *p);

void global_var_up(char *key, size_t key_len, size_t idx);
void global_var_down(program_s *prog, char *key, size_t key_len, size_t idx);

void program_state_blocked(program_s *prog, long sleep_nsec);

void global_var_load(char *key, size_t key_len, size_t idx, int *val);
void global_var_store(char *key, size_t key_len, size_t idx, int to_store);

void global_table_init(void);
void global_table_destroy(void);

#endif //SIMBLY_GLOBAL_H__
