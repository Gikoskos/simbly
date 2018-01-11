#include "exec.h"
#include "scanner.h"
#include "global.h"
#include "error.h"

#define SET_PARSER_IDX(prog, tok) \
do { \
    unsigned int ___tmp_line = prog->line, \
                 ___tmp_col = prog->column, \
                 ___tmp_prev_col = prog->prev_col; \
    prog->line = tok->line; \
    prog->column = tok->column; \
    prog->prev_col = tok->prev_col \

#define RESET_PARSER_IDX(prog) \
    prog->line = ___tmp_line; \
    prog->column = ___tmp_col; \
    prog->prev_col = ___tmp_prev_col; \
} while (0)

int exec_initialized = 0;
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

static int __varval_get_value(program_s *prog, token_type_e type, varval_u *data, size_t len, int *value);
static int __varval_set_value(program_s *prog, token_type_e type, varval_u *data, size_t len, int to_set);

static int varval_set_value(program_s *prog, token_s *tok, int to_set);
static int varval_get_value(program_s *prog, token_s *tok, int *to_get);

static label_data_s *insert_label_to_vartable(program_s *prog, token_s *lbl_tok);

//static void __dbg_print_vartable(QuadHashtable *table);

static void load_handler(program_s *prog, instruction_id_e ins_code);
static void store_handler(program_s *prog, instruction_id_e ins_code);
static void set_handler(program_s *prog, instruction_id_e ins_code);
static void primitive_op_handler(program_s *prog, instruction_id_e ins_code);
static void branch_handler(program_s *prog, instruction_id_e ins_code);
static void semaphore_handler(program_s *prog, instruction_id_e ins_code);
static void sleep_handler(program_s *prog, instruction_id_e ins_code);
static void print_handler(program_s *prog, instruction_id_e ins_code);
static void return_handler(program_s *prog, instruction_id_e ins_code);

static void exec_instruction_line(program_s *prog);
static void exec_destroy(void);

static const instruction_s instruction_array[] = {
    {"LOAD", LOAD_SYM, load_handler},
    {"STORE", STORE_SYM, store_handler},
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

int __varval_get_value(program_s *prog, token_type_e type, varval_u *data, size_t len, int *value)
{
    vdsErrCode verr;
    KeyValuePair *table_data;
    char *search_key;
    size_t key_len;
    int final_val;

    if (type == INT_VAL_TOK) {
        if (value)
            *value = data->value;
        return 1;
    }

    switch (type) {
        case INT_VAR_TOK:
            if (len) {
                key_len = len;
            } else {
                key_len = strlen(data->ptr) + 1;
            }
            search_key = data->ptr;

            if (!strcmp("argc", search_key)) {
                free(search_key);
                if (value)
                    *value = prog->argv[1];
                return 1;
            }
            break;
        case INT_ARR_TOK:
            search_key = ((int_arr_tok_s*)data->ptr)->name;
            key_len = strlen(search_key) + 1;

            if (!strcmp("argv", search_key)) {
                int tmp_idx;

                if (!__varval_get_value(prog, ((int_arr_tok_s*)data->ptr)->idx_type, &((int_arr_tok_s*)data->ptr)->idx, 0, &tmp_idx)) {
                    return 0;
                }

                if (tmp_idx >= prog->argv[1]) {
                    program_stop(prog, 1);
                    err_msg(prog, "tried to access area outside of argv array which is of size %d\n\t%s\n\t^", prog->argv[1], search_key);
                    return 0;
                }

                free(search_key);
                free(data->ptr);

                if (value)
                    *value = prog->argv[tmp_idx + 2];
                return 1;
            }
            break;
        default:
            return 0;
    }

    VDS(table_data = QuadHash_find(prog->vartable, (void*)search_key, key_len, &verr), verr);

    if (table_data) {

        if (*(int*)table_data->pData == -1) {
            program_stop(prog, 1);
            err_msg(prog, "there's already a label with the same name defined\n\t%s\n\t^", search_key);
            return 0;
        }

        int *arr = (int*)table_data->pData;

        switch (type) {
            case INT_VAR_TOK:
            {

                if (arr[0] > 1) {
                    program_stop(prog, 1);
                    err_msg(prog, "arrays can't be used by their names; only by their indices\n\t%s\n\t^", search_key);
                    return 0;
                }

                final_val = arr[1];
                free(search_key);
                break;
            }
            case INT_ARR_TOK:
            {

                int tmp;

                if (!__varval_get_value(prog, ((int_arr_tok_s*)data->ptr)->idx_type, &((int_arr_tok_s*)data->ptr)->idx, 0, &tmp)) {
                    return 0;
                }

                if (tmp < 0) {
                    program_stop(prog, 1);
                    err_msg(prog, "arrays can't have negative indices\n\t%s\n\t^", search_key);
                    return 0;
                }

                tmp++;
                if (tmp >= (arr[0] + 1)) {
                    ENO(arr = realloc(arr, sizeof(int) * (tmp + 1)));


                    for (int i = arr[0]; i < (tmp + 1); i++)
                        arr[i] = 0;

                    arr[0] = tmp;
                    final_val = 0;
                    table_data->pData = arr;//FU
                } else {
                    final_val = arr[tmp];
                }

                free(search_key);
                free(data->ptr);
                break;
            }
            default:
                return 0;
        }

    } else {

        int *new_array;

        final_val = 0;

        switch (type) {
            case INT_VAR_TOK:
            {
                ENO(new_array = malloc(sizeof(int) * 2));
                new_array[0] = 1;
                new_array[1] = 0;

                break;
            }
            case INT_ARR_TOK:
            {
                int tmp;

                if (!__varval_get_value(prog, ((int_arr_tok_s*)data->ptr)->idx_type, &((int_arr_tok_s*)data->ptr)->idx, 0, &tmp)) {
                    return 0;
                }

                if (tmp < 0) {
                    program_stop(prog, 1);
                    err_msg(prog, "arrays can't have negative indices\n\t%s\n\t^", search_key);
                    return 0;
                }

                tmp++;

                ENO(new_array = malloc(sizeof(int) * (tmp + 1)));

                new_array[0] = tmp;

                for (int i = 1; i < (tmp + 1); i++)
                    new_array[i] = 0;

                free(data->ptr);
                break;
            }
            default:
                return 0;
        }

        VDS(QuadHash_insert(prog->vartable, (void*)new_array, (void*)search_key, key_len, NULL, &verr), verr);

    }

    if (value) {
        *value = final_val;
    }

    return 1;
}

int __varval_set_value(program_s *prog, token_type_e type, varval_u *data, size_t len, int to_set)
{
    vdsErrCode verr;
    KeyValuePair *table_data;
    char *search_key;
    size_t key_len;

    switch (type) {
        case INT_VAR_TOK:
            if (len) {
                key_len = len;
            } else {
                key_len = strlen(data->ptr) + 1;
            }
            search_key = data->ptr;

            if (!strcmp("argc", search_key)) {
                program_stop(prog, 1);
                err_msg(prog, "the value of argc is constant; setting it to another value isn't allowed\n\t%s\n\t^", search_key);
                return 0;
            }
            break;
        case INT_ARR_TOK:
            search_key = ((int_arr_tok_s*)data->ptr)->name;
            key_len = strlen(search_key) + 1;

            if (!strcmp("argv", search_key)) {
                program_stop(prog, 1);
                err_msg(prog, "the value of argv is constant; setting it to another value isn't allowed\n\t%s\n\t^", search_key);
                return 0;
            }
            break;
        default:
            return 0;
    }

    VDS(table_data = QuadHash_find(prog->vartable, (void*)search_key, key_len, &verr), verr);

    if (table_data) {

        if (*(int*)table_data->pData == -1) {
            program_stop(prog, 1);
            err_msg(prog, "there's already a label with the same name defined\n\t%s\n\t^", search_key);
            return 0;
        }

        int *arr = (int*)table_data->pData;

        switch (type) {
            case INT_VAR_TOK:
            {

                if (arr[0] > 1) {
                    program_stop(prog, 1);
                    err_msg(prog, "arrays can't be used by their names; only by their indices\n\t%s\n\t^", search_key);
                    return 0;
                }

                arr[1] = to_set;
                free(search_key);
                break;
            }
            case INT_ARR_TOK:
            {

                int tmp;

                if (!__varval_get_value(prog, ((int_arr_tok_s*)data->ptr)->idx_type, &((int_arr_tok_s*)data->ptr)->idx, 0, &tmp)) {
                    return 0;
                }

                if (tmp < 0) {
                    program_stop(prog, 1);
                    err_msg(prog, "arrays can't have negative indices\n\t%s\n\t^", search_key);
                    return 0;
                }

                tmp++;
                if (tmp >= (arr[0] + 1)) {

                    ENO(arr = realloc(arr, sizeof(int) * (tmp + 1)));

                    for (int i = arr[0]; i < (tmp + 1); i++)
                        arr[i] = 0;

                    arr[0] = tmp;

                    table_data->pData = arr;
                }

                dbg_msg(prog, "setting the position %d of the array to the value %d", tmp, to_set);
                arr[tmp] = to_set;

                free(search_key);
                free(data->ptr);
                break;
            }
            default:
                return 0;
        }

    } else {

        int *new_array;

        switch (type) {
            case INT_VAR_TOK:
            {
                ENO(new_array = malloc(sizeof(int) * 2));
                new_array[0] = 1;
                new_array[1] = to_set;

                break;
            }
            case INT_ARR_TOK:
            {
                int tmp;

                if (!__varval_get_value(prog, ((int_arr_tok_s*)data->ptr)->idx_type, &((int_arr_tok_s*)data->ptr)->idx, 0, &tmp)) {
                    return 0;
                }

                if (tmp < 0) {
                    program_stop(prog, 1);
                    err_msg(prog, "arrays can't have negative indices\n\t%s\n\t^", search_key);
                    return 0;
                }

                tmp++;

                //calloc maybe?
                ENO(new_array = malloc(sizeof(int) * (tmp + 1)));

                new_array[0] = tmp;

                for (int i = 1; i < (tmp + 1); i++)
                    new_array[i] = 0;

                new_array[tmp] = to_set;
                free(data->ptr);
                break;
            }
            default:
                return 0;
        }

        VDS(QuadHash_insert(prog->vartable, (void*)new_array, (void*)search_key, key_len, NULL, &verr), verr);

    }

    return 1;
}

int varval_set_value(program_s *prog, token_s *tok, int to_set)
{
    int ret;

    SET_PARSER_IDX(prog, tok);
    ret = __varval_set_value(prog, tok->type, &tok->data, tok->len, to_set);
    RESET_PARSER_IDX(prog);

    if (ret) {
        free(tok);
    } else {
        free_token(tok);
    }

    return ret;
}

int varval_get_value(program_s *prog, token_s *tok, int *to_get)
{
    int ret;

    SET_PARSER_IDX(prog, tok);
    ret = __varval_get_value(prog, tok->type, &tok->data, tok->len, to_get);
    RESET_PARSER_IDX(prog);

    if (ret) {
        free(tok);
    } else {
        free_token(tok);
    }

    return ret;
}

label_data_s *insert_label_to_vartable(program_s *prog, token_s *lbl_tok)
{
    vdsErrCode verr;
    label_data_s *new_label;
    KeyValuePair *tmp;

    ENO(new_label = malloc(sizeof(label_data_s)));

    new_label->placeholder = -1;
    new_label->offset = lbl_tok->offset;
    new_label->line = lbl_tok->line;
    new_label->column = lbl_tok->column;
    new_label->prev_col = lbl_tok->prev_col;

    dbg_msg(prog, "label %s offset = %ld", (char*)lbl_tok->data.ptr, new_label->offset);
    tmp = QuadHash_insert(prog->vartable, (void*)new_label, (void*)lbl_tok->data.ptr, lbl_tok->len, NULL, &verr);

    if ((verr == VDS_KEY_EXISTS) && (((label_data_s*)tmp->pData)->offset != new_label->offset)) {
        program_stop(prog, 1);
        SET_PARSER_IDX(prog, lbl_tok);
        err_msg(prog, "can't redefine label with the same name!\n\t%s\n\t^", (char*)lbl_tok->data.ptr);
        RESET_PARSER_IDX(prog);
        free(new_label);
        new_label = NULL;
    }

    return new_label;
}

void exec_instruction_line(program_s *prog)
{
    token_s *instruction_tok = (token_s*)RingBuffer_read(prog->translated_line, NULL);

    if (instruction_tok) {
        if (instruction_tok->type == LABEL_TOK) {

            if (!insert_label_to_vartable(prog, instruction_tok))
                return;

            free(instruction_tok);
            instruction_tok = (token_s*)RingBuffer_read(prog->translated_line, NULL);
        }

        instruction_id_e code = (instruction_id_e)instruction_tok->data.value;

        free_token(instruction_tok);

        ASRT(code <= RETURN_SYM);
        instruction_array[code].handler(prog, code);
    } else {
        prog->state = FINISHED;
    }
}

void interpret_next_line(program_s *prog)
{
    ASRT(exec_initialized);

    if (prog) {

        switch (prog->state) {
            case MAGIC_LINE:
                parse_magic(prog);
                if (prog->state == FINISHED) {
                    break;
                }
                //fall through to parsing instructions
                //in case the magic string was matched
            case INSTRUCTION_LINE:
                tokenize_next_line(prog);
                break;
            default:
                break;
        }

        if (prog->state == INSTRUCTION_LINE || prog->state == LAST_LINE) {
            exec_instruction_line(prog);
            prog->state = (prog->state == LAST_LINE) ? FINISHED : prog->state;
            //__dbg_print_vartable(prog->vartable);
        }
    }
}
/*
void __dbg_print_vartable(QuadHashtable *table)
{
    for (unsigned int i = 0; i < table->size; i++) {

        if (table->array[i].state == 1) {
            printf("[%u] = (key=%s, ", i, (char*)table->array[i].item.pKey);

            if (table->array[i].item.pData) {
                int *arr = (int*)table->array[i].item.pData;

                printf("data=(");
                for (int j = 0; j < arr[0] + 1; j++) {
                    printf("%d,", arr[j]);
                }
                printf("\b))");
            } else {
                printf("data=(LABEL)");
            }
        } else {
            ASRT(table->array[i].state != 2);
            printf("[%u] = (EMPTY) ", i);
        }

        putchar('\n');
    }
}
*/
void load_handler(program_s *prog, instruction_id_e ins_code)
{
    (void)ins_code;
    vdsErrCode verr;
    token_s *global_tok, *varval_tok;
    int tmp;
    char *search_key;
    size_t idx = 0, key_len;

    VDS(varval_tok = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);
    VDS(global_tok = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);

    if (global_tok->type == INT_ARR_TOK) {
        if (!__varval_get_value(prog,
                                ((int_arr_tok_s*)global_tok->data.ptr)->idx_type,
                                &((int_arr_tok_s*)global_tok->data.ptr)->idx, 0, &tmp)) {
            free_token(global_tok);
            return;
        }

        idx = tmp;
        search_key = ((int_arr_tok_s*)global_tok->data.ptr)->name;
        key_len = strlen(search_key) + 1;
    } else {
        search_key = (char*)global_tok->data.ptr;
        key_len = global_tok->len;
    }

    global_var_load(search_key, key_len, idx, &tmp);

    if (!varval_set_value(prog, varval_tok, tmp)) {
        return;
    }
    free(global_tok);
}

void store_handler(program_s *prog, instruction_id_e ins_code)
{
    (void)ins_code;
    vdsErrCode verr;
    token_s *global_tok, *varval_tok;
    int tmp;
    char *search_key;
    size_t idx = 0, key_len;

    VDS(global_tok = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);
    VDS(varval_tok = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);

    if (global_tok->type == INT_ARR_TOK) {
        if (!__varval_get_value(prog,
                                ((int_arr_tok_s*)global_tok->data.ptr)->idx_type,
                                &((int_arr_tok_s*)global_tok->data.ptr)->idx, 0, &tmp)) {
            free_token(global_tok);
            return;
        }

        idx = tmp;
        search_key = ((int_arr_tok_s*)global_tok->data.ptr)->name;
        key_len = strlen(search_key) + 1;
    } else {
        search_key = (char*)global_tok->data.ptr;
        key_len = global_tok->len;
    }

    if (!varval_get_value(prog, varval_tok, &tmp)) {
        return;
    }

    global_var_store(search_key, key_len, idx, tmp);
    free(global_tok);
}

void set_handler(program_s *prog, instruction_id_e ins_code)
{
    (void)ins_code;
    vdsErrCode verr;
    token_s *var_tok, *val_tok;
    int val;

    VDS(var_tok = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);
    VDS(val_tok = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);

    if (!varval_get_value(prog, val_tok, &val)) {
        return;
    }

    if (!varval_set_value(prog, var_tok, val)) {
        return;
    }
}

void primitive_op_handler(program_s *prog, instruction_id_e ins_code)
{
    vdsErrCode verr;
    token_s *res, *varval1, *varval2;
    int resval, val1, val2;

    VDS(res = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);
    VDS(varval1 = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);
    VDS(varval2 = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);

    if (!varval_get_value(prog, varval1, &val1)) {
        return;
    }

    if (!varval_get_value(prog, varval2, &val2)) {
        return;
    }

    switch (ins_code) {
        case ADD_SYM:
            resval = val1 + val2;
            break;
        case SUB_SYM:
            resval = val1 - val2;
            break;
        case MUL_SYM:
            resval = val1 * val2;
            break;
        case DIV_SYM:
            resval = val1 / val2;
            break;
        case MOD_SYM:
            resval = val1 % val2;
            break;
        default:
            return;
    }

    if (!varval_set_value(prog, res, resval)) {
        return;
    }
}

void branch_handler(program_s *prog, instruction_id_e ins_code)
{
    vdsErrCode verr;
    token_s *label, *varval1, *varval2;
    int jump, val1, val2;

    if (ins_code != BRA_SYM) {
        VDS(varval1 = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);
        VDS(varval2 = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);

        if (!varval_get_value(prog, varval1, &val1)) {
            return;
        }

        if (!varval_get_value(prog, varval2, &val2)) {
            return;
        }
    }

    VDS(label = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);

    switch (ins_code) {
        case BRGT_SYM:
            jump = ( val1 > val2 );
            break;
        case BRGE_SYM:
            jump = ( val1 >= val2 );
            break;
        case BRLT_SYM:
            jump = ( val1 < val2 );
            break;
        case BRLE_SYM:
            jump = ( val1 <= val2 );
            break;
        case BREQ_SYM:
            jump = ( val1 == val2 );
            break;
        case BRA_SYM:
            jump = 1;
            break;
        default:
            return;
    }

    if (jump) {
        KeyValuePair *tmp_pair;

        VDS(tmp_pair = QuadHash_find(prog->vartable, label->data.ptr, label->len, &verr), verr);

        if (tmp_pair) {

            if (*(int*)tmp_pair->pData == -1) {
                label_data_s *lbl = (label_data_s*)tmp_pair->pData;

                prog->line = lbl->line;
                prog->column = lbl->column;
                prog->prev_col = lbl->prev_col;

                prog->c = ' ';
                ENO(fseek(prog->fd, lbl->offset, SEEK_SET));
            } else {
                program_stop(prog, 1);
                SET_PARSER_IDX(prog, label);
                err_msg(prog, "branching location name is already defined as a variable\n\t%s\n\t^", label->data.ptr);
                RESET_PARSER_IDX(prog);
            }

        } else {
            token_s *tmp_tok, *new_lbl;

            while (1) {
                tokenize_next_line(prog);

                new_lbl = (token_s*)RingBuffer_read(prog->translated_line, &verr);

                while (verr != VDS_BUFFER_EMPTY) {
                    tmp_tok = (token_s*)RingBuffer_read(prog->translated_line, &verr);
                    free_token(tmp_tok);
                }

                if (new_lbl->type == LABEL_TOK) {

                    if (!strcmp(new_lbl->data.ptr, label->data.ptr)) {

                        label_data_s *lbl = insert_label_to_vartable(prog, new_lbl);

                        if (!lbl) {
                            free_token(new_lbl);
                        } else {
                            prog->line = lbl->line;
                            prog->column = lbl->column;
                            prog->prev_col = lbl->prev_col;

                            prog->c = ' ';
                            ENO(fseek(prog->fd, lbl->offset, SEEK_SET));
                            free(new_lbl);
                        }

                        break;
                    }
                }

                free_token(new_lbl);

                if (prog->state == FINISHED) {
                    program_stop(prog, 1);
                    SET_PARSER_IDX(prog, label);
                    err_msg(prog, "couldn't jump to undefined label\n\t%s\n\t^", label->data.ptr);
                    RESET_PARSER_IDX(prog);
                    break;
                }
            }

        }
    }

    free(label->data.ptr);
    free(label);
}

void semaphore_handler(program_s *prog, instruction_id_e ins_code)
{
    vdsErrCode verr;
    token_s *global_tok;
    int tmp;
    size_t idx = 0, key_len;
    char *search_key;

    VDS(global_tok = (token_s*)RingBuffer_read(prog->translated_line, &verr), verr);
    if (global_tok->type == INT_ARR_TOK) {
        if (!__varval_get_value(prog,
                                ((int_arr_tok_s*)global_tok->data.ptr)->idx_type,
                                &((int_arr_tok_s*)global_tok->data.ptr)->idx, 0, &tmp)) {
            free_token(global_tok);
            return;
        }
        idx = tmp;
        search_key = ((int_arr_tok_s*)global_tok->data.ptr)->name;
        key_len = strlen(search_key) + 1;
    } else {
        search_key = (char*)global_tok->data.ptr;
        key_len = global_tok->len;
    }

    switch (ins_code) {
        case DOWN_SYM:
            global_var_down(prog, search_key, key_len, idx);
            break;
        case UP_SYM:
            global_var_up(search_key, key_len, idx);
            break;
        default:
            break;
    }

    free(global_tok);
}

void sleep_handler(program_s *prog, instruction_id_e ins_code)
{
    (void)ins_code;
    token_s *tok;
    int sleep_duration;

    tok = (token_s*)RingBuffer_read(prog->translated_line, NULL);

    SET_PARSER_IDX(prog, tok);

    if (!__varval_get_value(prog, tok->type, &tok->data, tok->len, &sleep_duration)) {
        free_token(tok);
        return;
    }

    dbg_msg(prog, "sleeping value %d", sleep_duration);

    if (sleep_duration > 0) {
        prog->state = SLEEPING;
        prog->sleep_left.tv_sec = (time_t)sleep_duration;
        prog->sleep_left.tv_nsec = 0;
    } else {
        warn_msg(prog, "negative parameter given to SLEEP instruction; nothing will happen");
    }

    RESET_PARSER_IDX(prog);

    free(tok);
}

void print_handler(program_s *prog, instruction_id_e ins_code)
{
    (void)ins_code;
    vdsErrCode verr;
    token_s *tok;
    int tmp;

    pthread_mutex_lock(&print_lock);
    printf("%sProgram %d says:%s", TERM_BONW, prog->argv[0], TERM_RESET);

    tok = (token_s*)RingBuffer_read(prog->translated_line, &verr);
    printf(" %s ", (char*)tok->data.ptr);
    free_token(tok);

    while (1) {
        tok = (token_s*)RingBuffer_read(prog->translated_line, &verr);

        if (verr == VDS_BUFFER_EMPTY) {
            break;
        }

        if (!varval_get_value(prog, tok, &tmp)) {
            break;
        }

        printf("%d ", tmp);
    }

    putchar('\n');

    pthread_mutex_unlock(&print_lock);
}

void return_handler(program_s *prog, instruction_id_e ins_code)
{
    (void)ins_code;

    prog->state = FINISHED;
}

void exec_init(void)
{
    if (!exec_initialized) {
        lexer_init();
        global_table_init();
        atexit(exec_destroy);
        exec_initialized = 1;
    }
}

void exec_destroy(void)
{
    lexer_destroy();
    global_table_destroy();
}
