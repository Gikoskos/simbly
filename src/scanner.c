#include "scanner.h"
#include "exec.h"
#include "error.h"

#define LOCAL_VAR_TYPE 0
#define GLOBAL_VAR_TYPE 1

#define LINE_NOT_EMPTY 2

#define NEXT_LINE(prog) \
do { \
    prog->prev_col = prog->column; \
    prog->line++; prog->column = 1; \
} while (0)

#define NEXT_CHAR(prog) \
do { \
    prog->c = fgetc(prog->fd); \
\
    if (prog->c == '\n') { \
        NEXT_LINE(prog); \
    } else { \
        prog->column++; \
    } \
} while (0)



static int flush_up_to_char(program_s *prog);
static int flush_up_to_newline(program_s *prog);
static size_t get_next_word(program_s *prog, size_t max_len, int this_line);
static void add_new_token(program_s *prog, void *ptr, int value,
                          size_t data_len, token_type_e tok, instruction_id_e code);

static void loadstore_handler(program_s *prog, instruction_id_e ins_code);
static void set_handler(program_s *prog, instruction_id_e ins_code);
static void primitive_op_handler(program_s *prog, instruction_id_e ins_code);
static void branch_handler(program_s *prog, instruction_id_e ins_code);
static void semaphore_handler(program_s *prog, instruction_id_e ins_code);
static void sleep_handler(program_s *prog, instruction_id_e ins_code);
static void print_handler(program_s *prog, instruction_id_e ins_code);
static void return_handler(program_s *prog, instruction_id_e ins_code);

static int parse_varval_token(program_s *prog, size_t start_idx, token_type_e *type,
                              varval_u *tok_data, int is_array_idx, unsigned int *depth);



/* string array can be used as a hashtable with "hashes"
 * from the instruction_id_e enum that map on each instruction handler */
static const instruction_s instruction_array[] = {
    {"LOAD", LOAD_SYM, loadstore_handler},
    {"STORE", STORE_SYM, loadstore_handler},
    {"SET", SET_SYM, set_handler},
    {"ADD", ADD_SYM, primitive_op_handler},
    {"SUB", SUB_SYM, primitive_op_handler},
    {"MUL", MUL_SYM, primitive_op_handler},
    {"DIV", DIV_SYM, primitive_op_handler},
    {"MOD", MOD_SYM, primitive_op_handler},
    {"BRGT", BRGT_SYM, branch_handler},
    {"BRGE", BRGE_SYM, branch_handler},
    {"BRLT", BRLT_SYM, branch_handler},
    {"BRLE", BRLE_SYM, branch_handler},
    {"BREQ", BREQ_SYM, branch_handler},
    {"BRA", BRA_SYM, branch_handler},
    {"DOWN", DOWN_SYM, semaphore_handler},
    {"UP", UP_SYM, semaphore_handler},
    {"SLEEP", SLEEP_SYM, sleep_handler},
    {"PRINT", PRINT_SYM, print_handler},
    {"RETURN", RETURN_SYM, return_handler}
};

static QuadHashtable *instruction_table;
static const char magic_bytes[] = "#PROGRAM";
static int lexer_initialized = 0;


int flush_up_to_char(program_s *prog)
{
    do {

        NEXT_CHAR(prog);

        if (prog->c == EOF) {
            return 0;
        }

    } while (isspace(prog->c));

    return 1;
}

int flush_up_to_newline(program_s *prog)
{
    while (prog->c != '\n') {
        NEXT_CHAR(prog);

        if (prog->c == EOF) {
            return 0;
        }

        if (!isspace(prog->c)) {
            return LINE_NOT_EMPTY;
        }
    }

    return 1;
}

size_t get_next_word(program_s *prog, size_t max_len, int this_line)
{
    if (prog) {

        size_t i = 0;
        unsigned int prev_line = prog->line;

        if (this_line && (prog->c == '\n')) {
            return 0;
        }

        if (isspace(prog->c)) {
            if (!flush_up_to_char(prog)) {
                return 0;
            }
        }

        if (this_line && (prev_line != prog->line)) {
            return 0;
        }

        prog->input[i++] = prog->c;

        while (1) {
            NEXT_CHAR(prog);

            if (i == max_len) {
                program_stop(prog, 1);
                err_msg(prog, "symbol too big to parse; maximum symbol name length allowed is %zu\n\t%s...\n\t^",
                        max_len, prog->input);
                return 0;
            }

            if (prog->c == EOF) {
                prog->state = LAST_LINE;
                break;
            }

            if (isspace(prog->c)) {
                break;
            }

            prog->input[i++] = prog->c;

        }

        prog->input[i] = 0;

        dbg_msg(prog, "read word %s with length %zu", prog->input, i);

        return i;
    }

    return 0;
}

/* recursively parse each token in a word read from input,
 * that might represent a varval rule. obviously the recursion only happens
 * when we have array indices */
int parse_varval_token(program_s *prog, size_t start_idx, token_type_e *type,
                       varval_u *tok_data, int is_array_idx, unsigned int *depth)
{
    size_t i;
    int success = 0;
    varval_u ptr;

    if (prog) {

        char stop_character;

        if (!is_array_idx) {
            stop_character = '\0';
        } else {
            stop_character = ']';
        }

        if (prog->input[start_idx] == '$') {

            if (isalpha(prog->input[start_idx + 1])) {

                size_t symbol_limit = start_idx + 2 + MAX_ALLOWED_SYMBOL_LEN;

                for (i = start_idx + 2; (prog->input[i] != stop_character) && (prog->input[i] != '['); i++) {

                    if (!isalnum(prog->input[i])) {
                        program_stop(prog, 1);
                        err_msg(prog, "variable names always begin with a letter, followed by alphanumeric characters");
                        break;
                    }

                    if (i >= symbol_limit) {
                        program_stop(prog, 1);
                        err_msg(prog, "symbol exceeds maximum length of allowed symbol names: %zu", MAX_ALLOWED_SYMBOL_LEN);
                        break;
                    }

                }

                if (prog->input[i] == stop_character) {

                    if (type) {
                        *type = INT_VAR_TOK;
                    }

                    success = 1;
                    ptr.ptr = (void*)&prog->input[start_idx + 1];

                    if (!is_array_idx) {
                        add_new_token(prog, ptr.ptr, 0, i - start_idx, INT_VAR_TOK, 0);
                    } else {
                        char *name;
                        ENO(name = malloc(sizeof(char) * (i - start_idx + 1)));
                        memcpy(name, ptr.ptr, i - start_idx);
                        name[i - start_idx] = 0;
                        ptr.ptr = name;
                    }

                } else if (prog->input[i] == '[') {

                    int_arr_tok_s *arr;
                    size_t len = i - start_idx;

                    ENO(arr = malloc(sizeof(int_arr_tok_s)));

                    ENO(arr->name = malloc(sizeof(char) * len));

                    strncpy(arr->name, &prog->input[start_idx + 1], len - 1);

                    arr->name[len - 1] = 0;

                    if (type) {
                        *type = INT_ARR_TOK;
                    }

                    if (!is_array_idx) {
                        add_new_token(prog, arr, 0, 0, INT_ARR_TOK, 0);
                    }

                    //this weird thing below is to check whether all the opening array index brackets
                    //are also closed. hacky but it does the job.
                    //TODO: add recursion depth limit error checking, to remove stack overflow from the list
                    //of problems i'd have to worry about
                    if (!depth) {

                        unsigned int recur_depth = 1;
                        unsigned int closing_bracket_count = 0;

                        if (!parse_varval_token(prog, i + 1, &arr->idx_type, &arr->idx, 1, &recur_depth))
                            return 0;

                        for (size_t j = i + 1; prog->input[j]; j++) {
                            if (prog->input[j] == ']')
                                closing_bracket_count++;
                        }

                        if (closing_bracket_count != recur_depth) {
                            program_stop(prog, 1);
                            err_msg(prog, "couldn't parse array index closing brackets");
                            success = 0;
                        } else {
                            success = 1;
                        }
                    } else {
                        if (depth)
                            (*depth)++;

                        ptr.ptr = arr;
                        success = parse_varval_token(prog, i + 1, &arr->idx_type, &arr->idx, 1, depth);
                    }

                } else if (is_array_idx) {
                    program_stop(prog, 1);
                    err_msg(prog, "couldn't parse array index closing brackets");

                }


            } else {
                program_stop(prog, 1);
                err_msg(prog, "variable names always begin with a letter, followed by alphanumeric characters");
            }

        } else if (prog->input[start_idx] == '-' || isdigit(prog->input[start_idx])) {

            if (is_array_idx && prog->input[start_idx] == '-') {

                program_stop(prog, 1);
                err_msg(prog, "invalid symbol detected inside array index brackets");

            } else {

                size_t num_limit = MAX_INT_STR_LEN + start_idx;

                for (i = start_idx + 1; prog->input[i] != stop_character; i++) {

                    if (!isdigit(prog->input[i])) {
                        program_stop(prog, 1);
                        err_msg(prog, "invalid characters detected while parsing number");
                        break;
                    }

                    if (i >= num_limit) {
                        program_stop(prog, 1);
                        err_msg(prog, "integer exceeds maximum number of digits: %zu", MAX_INT_STR_LEN);
                        break;
                    }

                }

                if (prog->input[i] == stop_character) {

                    ptr.value = atoi(&prog->input[start_idx]);

                    if (type) {
                        *type = INT_VAL_TOK;
                    }

                    success = 1;

                    if (!is_array_idx) {
                        add_new_token(prog, NULL, ptr.value, 0, INT_VAL_TOK, 0);
                    }

                } else if (is_array_idx) {
                    warn_msg(prog, "couldn't parse closing array brackets");
                }

            }

        } else {
            program_stop(prog, 1);
            err_msg(prog, "unrecognized string isn't variable or integer value\n\t%s\n\t^", &prog->input[start_idx]);
        }
    }

    if (tok_data) {
        *tok_data = ptr;
    }

    return success;
}

void parse_magic(program_s *prog)
{
    size_t i, len = ARRAY_LEN(magic_bytes);

    //@FIXME: This should be a compile-time static assertion, not a runtime check
    ASRT(len <= ARRAY_LEN(prog->input));

    for (i = 0; i < len; i++) {
        NEXT_CHAR(prog);

        if (prog->c == EOF) {
            program_stop(prog, 0);
            return;
        }

        prog->input[i] = (char)prog->c;
    }

    prog->input[i - 1] = 0;

    if (strncmp(magic_bytes, prog->input, len)) {
        program_stop(prog, 1);
        err_msg(prog,
                "not a valid simbly program; valid simbly programs begin with the magic bytes \"%s\"",
                magic_bytes);
    }

    if (prog->c != '\n') {
        unsigned int this_line = prog->line;

        int ret = flush_up_to_char(prog);

        if (this_line == prog->line) {
            program_stop(prog, 1);
            err_msg(prog, "unexpected character encountered in the same line as the magic bytes");
        } else if (!ret) {
            prog->state = FINISHED;
        } else {
            prog->state = INSTRUCTION_LINE;
        }
    } else {
        prog->state = INSTRUCTION_LINE;
    }
}

int is_valid_label(program_s *prog, size_t len)
{
    if (prog && len) {

        if (prog->input[0] == 'L' && strcmp(prog->input, "LOAD")) {

            if (len == 1) {
                program_stop(prog, 1);
                err_msg(prog, "invalid label name");
                return 0;
            }

            for (size_t i = 1; i < len; i++) {
                if (!isalnum(prog->input[i])) {
                    program_stop(prog, 1);
                    err_msg(prog, "label names can only have alphanumeric characters");
                    return 0;
                }
            }

            return 1;
        }

    }

    return 0;
}

void add_new_token(program_s *prog, void *ptr, int value,
                   size_t data_len, token_type_e tok, instruction_id_e code)
{
    vdsErrCode verr;
    token_s *new_tok;

    ENO(new_tok = malloc(sizeof(token_s)));

    new_tok->type = tok;
    new_tok->line = prog->line;
    new_tok->column = prog->column;
    new_tok->prev_col = prog->prev_col;

    if (!ptr) {

        if (tok == INSTRUCTION_TOK)
            new_tok->data.value = code;
        else
            new_tok->data.value = value;

        new_tok->len = 0;

    } else {

        if (data_len) {

            new_tok->len = data_len;
            ENO(new_tok->data.ptr = malloc(data_len));
            memcpy(new_tok->data.ptr, ptr, data_len);

        } else {

            new_tok->data.ptr = ptr;
            new_tok->len = 0;

        }

    }

    if (tok == LABEL_TOK) {
        ENO(new_tok->offset = ftell(prog->fd));
    }

    /* FOR DEBUGGING */
    switch (tok) {
        case INSTRUCTION_TOK:
            dbg_msg(prog, "new instruction %s token recognized", prog->input);
            break;
        case LABEL_TOK:
            dbg_msg(prog, "new label %s token recognized", (char*)new_tok->data.ptr);
            break;
        case INT_VAL_TOK:
            dbg_msg(prog, "new integer value %d token recognized", new_tok->data.value);
            break;
        case INT_VAR_TOK:
            dbg_msg(prog, "new integer variable token with name %s recognized", (char*)new_tok->data.ptr);
            break;
        case INT_ARR_TOK:
        {
            int_arr_tok_s *arr = (int_arr_tok_s*)new_tok->data.ptr;

            dbg_msg(prog, "new integer array variable token with name %s recognized", arr->name);
            break;
        }
        default:
            break;
    }

    VDSERR(RingBuffer_write(prog->translated_line, (void*)new_tok, &verr),
           ((verr != VDS_SUCCESS) && (verr != VDS_BUFFER_FULL)),
           verr);

    if (verr == VDS_BUFFER_FULL) {
        VDS(RingBuffer_resize(&prog->translated_line, prog->translated_line->size + DEFAULT_TRANSLATED_LINE_LEN, &verr), verr);
        VDS(RingBuffer_write(prog->translated_line, (void*)new_tok, &verr), verr);
    }
}

void free_token(void *p)
{
    token_s *tok = (token_s*)p;

    if (tok) {

        if ((tok->type == LABEL_TOK) ||
            (tok->type == INT_VAR_TOK) ||
            (tok->type == STRING_TOK)) {

            free((void*)tok->data.ptr);

        } else if (tok->type == INT_ARR_TOK) {
            int_arr_tok_s *curr, *prev, *tmp = (int_arr_tok_s*)tok->data.ptr;

            curr = tmp;

            while (1) {

                if (curr->idx_type == INT_ARR_TOK) {
                    prev = curr;
                    curr = (int_arr_tok_s*)curr->idx.ptr;
                    free(prev->name);
                    free(prev);
                } else {
                    if (curr->idx_type != INT_VAL_TOK)
                        free(curr->idx.ptr);
                    free(curr);
                    break;
                }
            }
        }

        free(tok);
    }
}

int call_instruction_handler(program_s *prog, size_t len)
{
    if (prog && len) {

        KeyValuePair *key = QuadHash_find(instruction_table, (void *)prog->input, len + 1, NULL);

        if (key) {
            instruction_id_e code = *(instruction_id_e*)key->pData;

            add_new_token(prog, NULL, 0, 0, INSTRUCTION_TOK, code);

            instruction_array[code].handler(prog, code);

            return 1;
        } else {
            program_stop(prog, 1);
            err_msg(prog, "unrecognized instruction\n\t%s\n\t^", prog->input);
        }

    }

    return 0;
}

void tokenize_next_line(program_s *prog)
{
    ASRT(lexer_initialized);
    ASRT(prog);

    size_t i = 0;

    i = get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 0);

    if (is_valid_label(prog, i)) {

        add_new_token(prog, (void*)prog->input, 0, i + 1, LABEL_TOK, 0);

        i = get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1);

        if (!i) {
            program_stop(prog, 1);
            err_msg(prog, "line with label should be followed by instruction");
        }

    }

    if (prog->state != FINISHED) {
        (void)call_instruction_handler(prog, i);
    }
}

void loadstore_handler(program_s *prog, instruction_id_e ins_code)
{
    for (int i = 0; i < 2; i++) {
        if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
            program_stop(prog, 1);
            err_msg(prog, "%s instruction expects two arguments",
                    instruction_array[ins_code].name_str);
            return;
        }

        if (!i && (prog->input[0] == '-' || isdigit(prog->input[0]))) {
            program_stop(prog, 1);
            err_msg(prog, "%s instruction expects a variable name as its first argument",
                    instruction_array[ins_code].name_str);
            return;
        }

        if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;
    }

    if (flush_up_to_newline(prog) == LINE_NOT_EMPTY) {
        program_stop(prog, 1);
        err_msg(prog, "more arguments than expected, after %s instruction",
                instruction_array[ins_code].name_str);
    }
}

void set_handler(program_s *prog, instruction_id_e ins_code)
{
    if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects two arguments",
                instruction_array[ins_code].name_str);
        return;
    }

    if (prog->input[0] == '-' || isdigit(prog->input[0])) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects a variable name as its first argument",
                instruction_array[ins_code].name_str);
        return;
    }

    if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;

    if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects two arguments",
                instruction_array[ins_code].name_str);
        return;
    }

    if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;

    if (flush_up_to_newline(prog) == LINE_NOT_EMPTY) {
        program_stop(prog, 1);
        err_msg(prog, "more arguments than expected, after %s instruction",
                instruction_array[ins_code].name_str);
    }
}

void primitive_op_handler(program_s *prog, instruction_id_e ins_code)
{
    if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects three arguments",
                instruction_array[ins_code].name_str);
        return;
    }

    if (prog->input[0] == '-' || isdigit(prog->input[0])) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects a variable name as its first argument",
                instruction_array[ins_code].name_str);
        return;
    }

    if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;

    if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects three arguments",
                instruction_array[ins_code].name_str);
        return;
    }

    if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;

    if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects three arguments",
                instruction_array[ins_code].name_str);
        return;
    }

    if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;

    if (flush_up_to_newline(prog) == LINE_NOT_EMPTY) {
        program_stop(prog, 1);
        err_msg(prog, "more arguments than expected, after %s instruction",
                instruction_array[ins_code].name_str);
    }
}

void branch_handler(program_s *prog, instruction_id_e ins_code)
{
    if (ins_code != BRA_SYM) {

        if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
            program_stop(prog, 1);
            err_msg(prog, "%s instruction expects two arguments",
                    instruction_array[ins_code].name_str);
            return;
        }

        if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;

        if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
            program_stop(prog, 1);
            err_msg(prog, "%s instruction expects two arguments",
                    instruction_array[ins_code].name_str);
            return;
        }

        if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;
    }

    size_t i = get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1);

    if (is_valid_label(prog, i)) {

        add_new_token(prog, (void*)prog->input, 0, i + 1, LABEL_TOK, 0);

        if (flush_up_to_newline(prog) == LINE_NOT_EMPTY) {
            program_stop(prog, 1);
            err_msg(prog, "more arguments than expected, after %s instruction",
                    instruction_array[ins_code].name_str);
        }

        if (prog->c == EOF || prog->state == LAST_LINE) {
            prog->state = INSTRUCTION_LINE;
        }

    } else {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects a label as its last argument",
                instruction_array[ins_code].name_str);
    }

}

void semaphore_handler(program_s *prog, instruction_id_e ins_code)
{
    if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects one argument",
                instruction_array[ins_code].name_str);
        return;
    }

    if (prog->input[0] == '-' || isdigit(prog->input[0])) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects a global variable as its argument",
                instruction_array[ins_code].name_str);
        return;
    }

    if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;

    if (flush_up_to_newline(prog) == LINE_NOT_EMPTY) {
        program_stop(prog, 1);
        err_msg(prog, "more arguments than expected, after %s instruction",
                instruction_array[ins_code].name_str);
    }
}

void sleep_handler(program_s *prog, instruction_id_e ins_code)
{
    if (!get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction expects one argument",
                instruction_array[ins_code].name_str);
        return;
    }

    if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL)) return;

    if (flush_up_to_newline(prog) == LINE_NOT_EMPTY) {
        program_stop(prog, 1);
        err_msg(prog, "more arguments than expected, after %s instruction",
                instruction_array[ins_code].name_str);
    }
}

void print_handler(program_s *prog, instruction_id_e ins_code)
{
    (void)ins_code;

    if (!flush_up_to_char(prog) || (prog->c != '\"')) {
        program_stop(prog, 1);
        err_msg(prog, "%s instruction must be followed by a string and 0 or more arguments",
                instruction_array[PRINT_SYM].name_str);
        return;
    }

    unsigned int this_line = prog->line;
    size_t i = 0;

    //if we made it up to here then we expect to read a string
    while (1) {
        NEXT_CHAR(prog);

        if (prog->c == EOF) {
            program_stop(prog, 1);
            prog->input[i] = 0;

            err_msg(prog, "unexpected EOF encountered while parsing string\n\t%s\n\t^", prog->input);

            return;
        }

        if (i >= MAX_INPUT_STR_LEN) {
            program_stop(prog, 1);
            prog->input[i] = 0;
            err_msg(prog, "string too big to parse; maximum string name length allowed is %zu\n\t%s...\n\t^",
                    MAX_INPUT_STR_LEN, prog->input);
            return;
        }

        if (!isprint(prog->c)) {
            program_stop(prog, 1);
            prog->input[i] = 0;
            err_msg(prog, "non-printable character with ascii code %d encountered while parsing string\n\t%s\n\t^", prog->c, prog->input);
            return;
        }

        if (prog->c == '\"') {
            prog->input[i] = 0;
            dbg_msg(prog, "parsed string \"%s\"", prog->input);

            NEXT_CHAR(prog);

            if (prog->c == EOF) {
                program_stop(prog, 1);
                err_msg(prog, "unexpected EOF encountered while parsing %s instruction",
                        instruction_array[PRINT_SYM].name_str);
                return;
            }

            if (!isspace(prog->c)) {
                program_stop(prog, 1);
                err_msg(prog, "strings must be followed by whitespace");
                return;
            }

            break;
        }

        prog->input[i++] = prog->c;
    }

    add_new_token(prog, (void*)prog->input, 0, i + 1, STRING_TOK, 0);

    if (!flush_up_to_char(prog) || (this_line != prog->line)) {
        if (prog->c == EOF)
            prog->state = LAST_LINE;
        return;
    }

    while ( (i = get_next_word(prog, MAX_ALLOWED_SYMBOL_LEN, 1)) ) {

        dbg_msg(prog, "parsed %s in the same line as PRINT", prog->input);
        if (!parse_varval_token(prog, 0, NULL, NULL, 0, NULL))
            break;

    }

    //flush_up_to_newline(prog);
}

void return_handler(program_s *prog, instruction_id_e ins_code)
{
    (void)ins_code;(void)prog;

    /*if (flush_up_to_char(prog)) {
        warn_msg(prog, "characters after a %s statement will be ignored",
                 instruction_array[RETURN_SYM].name_str);
    }*/
}

void lexer_init(void)
{
    if (!lexer_initialized) {
        vdsErrCode verr;

        //allocate a hashtable that's two times the size of the instruction array, to prevent
        //possible reallocating due to load factor
        VDS(instruction_table = QuadHash_init((ARRAY_LEN(instruction_array) * 2) + 1, symbol_name_cmp, NULL, &verr), verr);

        for (size_t i = 0; i < ARRAY_LEN(instruction_array); i++) {
            VDS(QuadHash_insert(instruction_table,
                                (void *)&instruction_array[i].code,
                                (void *)instruction_array[i].name_str,
                                strlen(instruction_array[i].name_str) + 1,
                                NULL, &verr), verr);
        }

        lexer_initialized = 1;
    }
}

void lexer_destroy(void)
{
    if (lexer_initialized) {
        QuadHash_destroy(&instruction_table, NULL, NULL);
    }
}

