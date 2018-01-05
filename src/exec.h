#ifndef SIMBLY_EXEC_H__
#define SIMBLY_EXEC_H__

#include "common.h"
#include "program.h"


typedef enum _instruction_id_e {
    LOAD_SYM = 0,
    STORE_SYM,
    SET_SYM,
    ADD_SYM,
    SUB_SYM,
    MUL_SYM,
    DIV_SYM,
    MOD_SYM,
    BRGT_SYM,
    BRGE_SYM,
    BRLT_SYM,
    BRLE_SYM,
    BREQ_SYM,
    BRA_SYM,
    DOWN_SYM,
    UP_SYM,
    SLEEP_SYM,
    PRINT_SYM,
    RETURN_SYM //in this enum, RETURN always has to be
               //defined as the last instruction
} instruction_id_e;

typedef void (*i_handler_cb)(program_s *prog, instruction_id_e ins_code);

typedef struct _instruction_s {
    char *name_str;
    instruction_id_e code;
    i_handler_cb handler;
} instruction_s;

typedef struct _label_data_s {
    int placeholder;
    unsigned int line, column, prev_col;
    long offset;
} label_data_s;


void interpret_next_line(program_s *prog);
void exec_init(void);

#endif //SIMBLY_EXEC_H__
