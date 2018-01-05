#include "common.h"
#include "program.h"
#include "runtime.h"
#include "exec.h"
#include "scanner.h"
#include "global.h"
#include "error.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/sysinfo.h>

//constant value to use as a standard allocation size
#define MAX_ALLOC_SIZE 128
#define DEFAULT_THREAD_NUM 4

const char *help_msg[] = {
    "run executes simbly programs. command usage -> run <source_file_name> <optional_integer_args_separated_by_whitespace>",
    "kill stops the execution of the simbly program with the specified ID. command usage -> kill <non_negative_integer>",
    "list lists the program that's currently running, and the total number of programs, on each runtime. command usage -> list",
    "help prints this message. command usage -> help"
};



char *read_line(const char *prompt)
{
    char *buff = NULL;
    size_t idx = 0;
    size_t curr_size = 0;
    int c;

    if (prompt) {
        printf("%s ", prompt);
    }

    while ( ((c = fgetc(stdin)) != EOF) && isspace(c) ) {
        if ((c == '\n') && prompt)
            printf("%s ", prompt);
    }

    if (!feof(stdin)) {

        curr_size = MAX_ALLOC_SIZE;
        ENO(buff = malloc(curr_size));

        do {

            if (idx >= curr_size) {

                curr_size += MAX_ALLOC_SIZE;
                ENO(buff = realloc(buff, curr_size));

            }

            if (isblank(c)) {
                buff[idx++] = ' ';
                while ((c = fgetc(stdin)) != EOF && (c != '\n')) {
                    if (!isblank(c))
                        break;
                }

                if (idx >= curr_size) {

                    curr_size += MAX_ALLOC_SIZE;
                    ENO(buff = realloc(buff, curr_size));

                }

                if ((c == EOF) || (c == '\n'))
                    break;
            }

            buff[idx++] = (char)c;


        } while ((c = fgetc(stdin)) != EOF && (c != '\n'));

        if (c == EOF) {
            free(buff);
            buff = NULL;
        } else {
            if (idx >= curr_size) {
                ENO(buff = realloc(buff, curr_size + 1));
            }

            buff[idx] = '\0';
        }

    }

    return buff;
}

char *make_prompt_str(void)
{
    size_t prompt_len;
    char *prompt_append = "@<simbly>:", *prompt;
    struct passwd *user_id;

    ENO(user_id = getpwuid(getuid()));

    prompt_len = strlen(user_id->pw_name) + strlen(prompt_append) + 1;

    ENO(prompt = malloc(sizeof(char) * prompt_len));

    snprintf(prompt, prompt_len, "%s%s", user_id->pw_name, prompt_append);

    return prompt;
}

void mark_program_as_finished(runtime_s **rt_arr, int rt_cnt, int id)
{
    CDLListNode *curr;
    program_s *prog;

    for (int i = 0; i < rt_cnt; i++) {
        PTH(pthread_mutex_lock(&rt_arr[i]->lock));
        curr = rt_arr[i]->program_list;

        if (curr) {
            do {
                prog = (program_s*)curr->pData;

                if (prog->argv[0] == id) {
                    id = -1;
                    //wake up the thread if it's asleep to notify it that
                    //it's killed
                    if (prog->state == BLOCKED) {
                        global_var_s *var = (global_var_s*)prog->sem;

                        PTH(pthread_mutex_lock(&var->mtx));
                        var->count[prog->blocked_idx] = 1;
                        PTH(pthread_mutex_unlock(&var->mtx));
                    }
                    program_stop(prog, 1);
                    break;
                }
                curr = curr->nxt;
            } while (curr != rt_arr[i]->program_list);
        }
        PTH(pthread_mutex_unlock(&rt_arr[i]->lock));
    }

    if (id != -1) {
        shell_msg("couldn't find program with ID %d", id);
    }
}

void print_banner(const char *banner)
{
    size_t i, j, c = 0, len = strlen(banner);
    struct timespec sleeping_time = {0, 10 * 1000000};

    for (i = 0; i < 3; i++) {
        for (j = 0; j < (2*len); j++) {
            if (!j || !i || i == 2 || (j == (2 * len - 1))) {
                putchar('*');
            } else if (j >= ((2*len)/4) && c < len && i == 1) {
                putchar(banner[c++]);
            } else {
                putchar(' ');
            }
            fflush(stdout);
            nanosleep(&sleeping_time, NULL);
        }
        putchar('\n');
    }
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    exec_init();

    /*int a[3] = {1, 2, 3};

    program_s *p = program_state_init("../test_programs/Test.txt", 3, a);
    runtime_s *r = runtime_init();

    runtime_attach_program(r, p);
    getchar();
    runtime_stop(r);*/

    char *word, *line, *saveptr;
    runtime_s **rt_arr;
    int rt_cnt, rt_min_idx;
    char *prompt_str;

    rt_cnt = get_nprocs();

    if (rt_cnt <= 1) {
        rt_cnt = DEFAULT_THREAD_NUM;
    }

    prompt_str = make_prompt_str();

    ENO(rt_arr = malloc(sizeof(runtime_s*) * rt_cnt));

    for (int i = 0; i < rt_cnt; i++) {
        rt_arr[i] = runtime_init();
    }

    print_banner("Welcome to the Simbly interpreter!");
    printf("\nEnter a command, or 'help' to see a list of available commands\n\n");

    while (1) {
        line = read_line(NULL/*prompt_str*/);

        word = strtok_r(line, " ", &saveptr);

        if (!strcmp("q", word) || !strcmp("exit", word) || !strcmp("quit", word)) {
            break;
        }

        if (!strcmp("k", word) || !strcmp("kill", word)) {

            word = strtok_r(NULL, " ", &saveptr);

            if (!word) {

                shell_msg("kill command expects an argument with the non-negative ID number of the program you want to kill");

            } else {
                int i;

                for (i = 0; word[i]; i++) {
                    if (!isdigit(word[i])) {
                        shell_msg(help_msg[1]);
                        break;
                    }
                }

                if (!word[i]) {
                    if (i >= MAX_INT_STR_LEN) {
                        shell_msg("program ID can't be longer than %zu digits", MAX_INT_STR_LEN - 1);
                    } else {
                        int id = strtol(word, NULL, 10);
                        mark_program_as_finished(rt_arr, rt_cnt, id);
                    }
                }
            }

        } else if (!strcmp("r", word) || !strcmp("run", word)) {

            char *fname = strtok_r(NULL, " ", &saveptr);

            if (!fname) {

                shell_msg(help_msg[0]);

            } else {

                if (access(fname, R_OK) != -1) {
                    int _argc = 0, len = 12, *_argv;

                    ENO(_argv = malloc(sizeof(int) * len));

                    while (1) {
                        int i;
                        word = strtok_r(NULL, " ", &saveptr);

                        if (!word) {
                            break;
                        }

                        for (i = 0; word[i]; i++) {
                            if (!isdigit(word[i])) {
                                shell_msg(help_msg[0]);
                                break;
                            }
                        }

                        if (word[i]) {
                            _argc = -1;
                            break;
                        }

                        if (i >= MAX_INT_STR_LEN) {
                            shell_msg("integer value can't be longer than %zu digits", MAX_INT_STR_LEN - 1);
                            _argc = -1;
                            break;
                        } else {
                            _argv[_argc++] = strtol(word, NULL, 10);
                            if (_argc >= len) {
                                len += len;
                                ENO(_argv = realloc(_argv, sizeof(int) * len));
                            }
                        }
                    }

                    if (_argc >= 0) {

                        int i, min_prog_cnt;

                        PTH(pthread_mutex_lock(&rt_arr[0]->lock));
                        min_prog_cnt = rt_arr[0]->program_cnt;
                        PTH(pthread_mutex_unlock(&rt_arr[0]->lock));

                        rt_min_idx = 0;

                        for (i = 1; i < rt_cnt; i++) {
                            PTH(pthread_mutex_lock(&rt_arr[i]->lock));
                            if (rt_arr[i]->program_cnt < min_prog_cnt) {
                                min_prog_cnt = rt_arr[i]->program_cnt;
                                rt_min_idx = i;
                            }
                            PTH(pthread_mutex_unlock(&rt_arr[i]->lock));
                        }

                        runtime_attach_program(rt_arr[rt_min_idx], program_state_init(fname, _argc, _argv));
                    }

                    free(_argv);
                } else {
                    shell_msg("file \"%s\" doesn't exist", fname);
                }

            }
        } else if (!strcmp("l", word) || !strcmp("list", word)) {
            int i, tmp_id, tmp_cnt;

            for (i = 0; i < rt_cnt; i++) {
                PTH(pthread_mutex_lock(&rt_arr[i]->lock));

                if (rt_arr[i]->curr) {
                    tmp_id = ((program_s*)rt_arr[i]->curr->pData)->argv[0];
                    tmp_cnt = rt_arr[i]->program_cnt;
                } else {
                    tmp_id = -1;
                }

                PTH(pthread_mutex_unlock(&rt_arr[i]->lock));

                if (tmp_id == -1) {
                    shell_msg("No programs are running on runtime %ld", (long)rt_arr[i]->thrd_id);
                } else {
                    shell_msg("Program %d is currently running on runtime %ld. Total programs running %d.", tmp_id, (long)rt_arr[i]->thrd_id, tmp_cnt);
                }
            }
        } else if (!strcmp("h", word) || !strcmp("help", word)) {
            shell_msg("%s\n%s\n%s\n%s", help_msg[0], help_msg[1], help_msg[2], help_msg[3]);
        } else {
            shell_msg("unrecognized command");
        }

        free(line);
    }

    /* unecessary cleanup slows down program shutdown
     * but fits structured concurrency model */
    free(line);
    free(prompt_str);

    for (int i = 0; i < rt_cnt; i++) {
        runtime_stop(rt_arr[i]);
    }

    free(rt_arr);

    return 0;
}
