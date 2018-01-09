#ifndef SIMBLY_SCANNER_H__
#define SIMBLY_SCANNER_H__

#include "common.h"
#include "program.h"

typedef enum _token_type_e {
    LABEL_TOK,
    INSTRUCTION_TOK,
    INT_VAL_TOK,
    INT_VAR_TOK,
    INT_ARR_TOK,
    STRING_TOK
} token_type_e;

typedef union _varval_u {
    void *ptr;
    int value;
} varval_u;

typedef struct _token_s {
    varval_u data;
    size_t len;
    token_type_e type;
    unsigned int line, column, prev_col;
    long offset; //only used for labels (bad design choice)
} token_s;

typedef struct _int_arr_tok_s {
    varval_u idx;
    token_type_e idx_type;
    char *name;
} int_arr_tok_s;


void lexer_init(void);
void lexer_destroy(void);
void parse_magic(program_s *prog);
void tokenize_next_line(program_s *prog);
void free_token(void *p);
void free_int_arr_tok(int_arr_tok_s *arr_tok);

#endif //SIMBLY_SCANNER_H__
