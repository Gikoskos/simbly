#include "program.h"
#include "exec.h"
#include "error.h"
#include "scanner.h"


static int id_cnt = 1;
static pthread_mutex_t id_mtx = PTHREAD_MUTEX_INITIALIZER;


static void free_keyval_token(void *p);
static int generate_program_id(void);




int generate_program_id(void)
{
    int id;

    pthread_mutex_lock(&id_mtx);
    id = id_cnt++;
    pthread_mutex_unlock(&id_mtx);

    return id;
}

int symbol_name_cmp(const void *name1, const void *name2)
{
    char *sym1 = (char*)name1;
    char *sym2 = (char*)name2;

    if (sym1 && sym2)
        return strcmp(sym1, sym2);
    return 1;
}

void free_keyval_token(void *p)
{
    KeyValuePair item = *(KeyValuePair*)p;

    free(item.pKey);
    free(item.pData);
}

program_s *program_state_init(char *fname, int argc, int *argv)
{
    vdsErrCode verr;
    program_s *p = NULL;
    size_t fname_len;
    int argv_len;

    if (fname && (argc >= 0)) {

        ENO(p = malloc(sizeof(program_s)));
        ENO(p->fd = fopen(fname, "r"));

        argv_len = argc + 2;

        ENO(p->argv = malloc(sizeof(int) * argv_len));

        fname_len = strlen(fname) + 1;

        ENO(p->fname = malloc(sizeof(char) * fname_len));

        strcpy(p->fname, fname);

        p->argv[0] = generate_program_id();
        p->argv[1] = argc;

        for (int i = 2; i < argc + 2; i++) {
            p->argv[i] = argv[i - 2];
        }

        VDS(p->vartable = QuadHash_init(DEFAULT_VARTABLE_LEN, symbol_name_cmp, NULL, &verr), verr);
        VDS(p->translated_line = RingBuffer_init(DEFAULT_TRANSLATED_LINE_LEN, &verr), verr);

        p->prev_col = 0;
        p->line = p->column = 1;
        p->state = MAGIC_LINE;

        p->error_flag = 0;
    }

    return p;
}

void program_stop(program_s *p, int err)
{
    if (p) {
        p->state = FINISHED;
        if (err)
            p->error_flag = 1;
    }
}

void program_state_free(program_s *p)
{
    if (p) {
        QuadHash_destroy(&p->vartable, free_keyval_token, NULL);
        RingBuffer_destroy(&p->translated_line, free_token, NULL);

        fclose(p->fd);
        free(p->argv);
        free(p->fname);
        free(p);
    }
}

void print_program_state(program_s *p)
{
    if (p) {
        printf("File name: %s\n", p->fname);
        printf("Line: %u, Column: %u\n", p->line, p->column);
        printf("Argc: %d\n", p->argv[1]);

        for (int i = 2; i < p->argv[1]; i++) {
            printf("Argv[%d] = %d\n", i, p->argv[i]);
        }
    }
}
