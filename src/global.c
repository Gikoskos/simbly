#include "global.h"
#include "exec.h"
#include "program.h"
#include "scanner.h"
#include "error.h"


static QuadHashtable *global_table;
static pthread_mutex_t global_table_lock = PTHREAD_MUTEX_INITIALIZER;

static int global_initialized = 0;


global_var_s *global_var_init(size_t total)
{
    global_var_s *ret = NULL;

    if (total) {
        pthread_mutexattr_t attr;
        PTH(pthread_mutexattr_init(&attr));
        PTH(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));

        ENO(ret = malloc(sizeof(global_var_s)));

        ret->len = total;

#ifdef INIT_SEMAPHORES_WITH_ONE
        ENO(ret->count = malloc(total * sizeof(int)));
        for (size_t i = 0; i < total; i++) {
            ret->count[i] = 1;
        }
#else
        ENO(ret->count = calloc(total, sizeof(int)));
#endif

        PTH(pthread_cond_init(&ret->cond, NULL));
        PTH(pthread_mutex_init(&ret->mtx, &attr));

        pthread_mutexattr_destroy(&attr);
    }

    return ret;
}

void global_var_destroy(global_var_s *p)
{
    if (p) {
        global_var_s *arr = (global_var_s*)p;

        pthread_mutex_destroy(&arr->mtx);
        pthread_cond_destroy(&arr->cond);

        free(arr->count);
        free(arr);
    }
}

void global_var_up(char *key, size_t key_len, size_t idx)
{
    ASRT(global_initialized);

    vdsErrCode verr;
    KeyValuePair *pair;
    global_var_s *var;

    PTH(pthread_mutex_lock(&global_table_lock));

    pair = QuadHash_find(global_table, key, key_len, &verr);

    if (pair) {
        PTH(pthread_mutex_unlock(&global_table_lock));
        free(key);

        var = (global_var_s*)pair->pData;

        PTH(pthread_mutex_lock(&var->mtx));
        if (idx < var->len) {

            var->count[idx]++;
            PTH(pthread_cond_broadcast(&var->cond));

        } else {

            ENO(var->count = realloc(var, sizeof(int) * (idx + 1)));

#ifdef INIT_SEMAPHORES_WITH_ONE
            for (size_t i = var->len; i < idx; i++) {
                var->count[i] = 1;
            }

            var->count[idx] = 2;
#else
            for (size_t i = var->len; i < idx; i++) {
                var->count[i] = 0;
            }

            var->count[idx] = 1;
#endif
            var->len = idx + 1;

        }
        PTH(pthread_mutex_unlock(&var->mtx));

    } else {

        var = global_var_init(idx + 1);
        var->count[idx] = 1;

        VDS(QuadHash_insert(global_table, var, key, key_len, NULL, &verr), verr);

        PTH(pthread_mutex_unlock(&global_table_lock));

    }
}

void global_var_down(program_s *prog, char *key, size_t key_len, size_t idx)
{
    ASRT(global_initialized);

    vdsErrCode verr;
    KeyValuePair *pair;
    global_var_s *var;

    PTH(pthread_mutex_lock(&global_table_lock));

    pair = QuadHash_find(global_table, key, key_len, &verr);

    if (pair) {
        PTH(pthread_mutex_unlock(&global_table_lock));
        free(key);

        var = (global_var_s*)pair->pData;


        PTH(pthread_mutex_lock(&var->mtx));
        if (idx >= var->len) {
            ENO(var->count = realloc(var, sizeof(int) * (idx + 1)));

            for (size_t i = var->len; i < idx; i++) {
#ifdef INIT_SEMAPHORES_WITH_ONE
                var->count[i] = 1;
#else
                var->count[i] = 0;
#endif
            }

            var->len = idx + 1;
        }

        PTH(pthread_mutex_unlock(&var->mtx));
    } else {
        var = global_var_init(idx + 1);

        VDS(QuadHash_insert(global_table, var, key, key_len, NULL, &verr), verr);

        PTH(pthread_mutex_unlock(&global_table_lock));
    }

    prog->blocked_idx = idx;
    prog->state = BLOCKED;
    prog->sem = (void*)var;
}

/* Should be called __only__ when the program is in BLOCKED state */
void program_state_blocked(program_s *prog, long sleep_nsec)
{
    struct timespec sleeping_time = {.tv_sec = 0, .tv_nsec = sleep_nsec};
    global_var_s *var = (global_var_s*)prog->sem;

    PTH(pthread_mutex_lock(&var->mtx));
    if (var->count[prog->blocked_idx] <= 0) {

        /*ret = */pthread_cond_timedwait(&var->cond, &var->mtx, &sleeping_time);

        if (var->count[prog->blocked_idx] > 0) {
            var->count[prog->blocked_idx]--;

            if (prog->state == BLOCKED) {
                prog->state = INSTRUCTION_LINE;
            }
        }

    } else {
        var->count[prog->blocked_idx]--;
        if (prog->state == BLOCKED) {
            prog->state = INSTRUCTION_LINE;
        }
    }
    PTH(pthread_mutex_unlock(&var->mtx));
}

void global_vartable_free_cb(void *p)
{
    KeyValuePair *param = (KeyValuePair*)p;

    free(param->pKey);
    global_var_destroy((global_var_s*)param->pData);
}

void global_var_load(char *key, size_t key_len, size_t idx, int *val)
{
    ASRT(global_initialized);

    vdsErrCode verr;
    KeyValuePair *pair;
    global_var_s *var;

    PTH(pthread_mutex_lock(&global_table_lock));

    pair = QuadHash_find(global_table, key, key_len, &verr);

    if (pair) {
        PTH(pthread_mutex_unlock(&global_table_lock));
        free(key);

        var = (global_var_s*)pair->pData;

        PTH(pthread_mutex_lock(&var->mtx));
        if (idx < var->len) {
            if (val) {
                *val = var->count[idx];
            }
        } else {

            ENO(var->count = realloc(var, sizeof(int) * (idx + 1)));

            for (size_t i = var->len; i < idx + 1; i++) {
#ifdef INIT_SEMAPHORES_WITH_ONE
                var->count[i] = 1;
#else
                var->count[i] = 0;
#endif
            }

            var->len = idx + 1;

            if (val)
                *val = 0;
        }
        PTH(pthread_mutex_unlock(&var->mtx));

    } else {

        VDS(QuadHash_insert(global_table, global_var_init(idx + 1), key, key_len, NULL, &verr), verr);

        PTH(pthread_mutex_unlock(&global_table_lock));

        if (val)
            *val = 0;
    }
}

void global_var_store(char *key, size_t key_len, size_t idx, int to_store)
{
    ASRT(global_initialized);

    vdsErrCode verr;
    KeyValuePair *pair;
    global_var_s *var;

    PTH(pthread_mutex_lock(&global_table_lock));

    pair = QuadHash_find(global_table, key, key_len, &verr);

    if (pair) {
        PTH(pthread_mutex_unlock(&global_table_lock));
        free(key);

        var = (global_var_s*)pair->pData;

        PTH(pthread_mutex_lock(&var->mtx));
        if (idx < var->len) {

            var->count[idx] = to_store;

        } else {

            ENO(var->count = realloc(var, sizeof(int) * (idx + 1)));

            for (size_t i = var->len; i < idx + 1; i++) {
#ifdef INIT_SEMAPHORES_WITH_ONE
                var->count[i] = 1;
#else
                var->count[i] = 0;
#endif
            }

            var->count[idx] = to_store;
            var->len = idx + 1;
        }
        PTH(pthread_mutex_unlock(&var->mtx));

    } else {

        var = global_var_init(idx + 1);
        var->count[idx] = to_store;

        VDS(QuadHash_insert(global_table, var, key, key_len, NULL, &verr), verr);

        PTH(pthread_mutex_unlock(&global_table_lock));

    }

}

void global_table_init(void)
{
    if (!global_initialized) {
        vdsErrCode verr;

        VDS(global_table = QuadHash_init(GLOBAL_TABLE_INIT_SIZE, symbol_name_cmp, NULL, &verr), verr);

        global_initialized = 1;

        atexit(global_table_destroy);
    }
}

void global_table_destroy(void)
{
    if (global_initialized) {
        QuadHash_destroy(&global_table, global_vartable_free_cb, NULL);
    }
}
