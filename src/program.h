#ifndef SIMBLY_PROGRAM_H__
#define SIMBLY_PROGRAM_H__

#include "common.h"


#define DEFAULT_VARTABLE_LEN 8
#define DEFAULT_TRANSLATED_LINE_LEN 8

typedef enum _program_state_e {
    MAGIC_LINE,
    INSTRUCTION_LINE,
    LAST_LINE,
    SLEEPING,
    BLOCKED,
    FINISHED
} program_state_e;

typedef struct _program_s {
    FILE *fd;
    char input[MAX_INPUT_STR_LEN + 1], *fname;
    unsigned int line, column, prev_col;
    QuadHashtable *vartable;
    struct timespec sleep_left;
    int *argv, c;
    program_state_e state;
    RingBuffer *translated_line;
    int error_flag;
    void *sem;
    size_t blocked_idx;
} program_s;


program_s *program_state_init(char *fname, int argc, int *argv);
void program_state_free(program_s *p);
void program_stop(program_s *p, int err);
void print_program_state(program_s *p);
int symbol_name_cmp(const void *name1, const void *name2);
void clear_translated_line(program_s *p);

#endif //SIMBLY_PROGRAM_H__
