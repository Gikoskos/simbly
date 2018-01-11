#include "error.h"
#include <stdarg.h>

int debugging_messages = 0;

static char errbuff[1024];


void fatal_handler(const char *call, const char *failed_cond, int line,
                   const char *caller, const char *file, int errno_err,
                   vdsErrCode voids_err)
{
    if (errno_err) {

        if (!strerror_r(errno_err, errbuff, 256)) {
            fprintf(stderr, "%sFailed call:\t%s%s\n"
                            "%son condition:\t%s%s\n"
                            "%sat %s -> %s():%d%s\n"
                            "%sReason: \"%s\"%s\n",
                            TERM_BONW, call, TERM_RESET,
                            TERM_BONW, failed_cond, TERM_RESET,
                            TERM_BONW, file, caller, line, TERM_RESET,
                            TERM_BONW, errbuff, TERM_RESET);
        } else {
            fprintf(stderr, "%sFailed call:\t%s%s\n"
                            "%son condition:\t%s%s\n"
                            "%sat %s -> %s():%d%s\n",
                            TERM_BONW, MAKE_STR(call), TERM_RESET,
                            TERM_BONW, failed_cond, TERM_RESET,
                            TERM_BONW, file, caller, line, TERM_RESET);
        }

    } else if (voids_err) {
        fprintf(stderr, "%sFailed call:\t%s%s\n"
                        "%sat %s -> %s():%d%s\n"
                        "%sReason: \"%s\"%s\n",
                        TERM_BONW, call, TERM_RESET,
                        TERM_BONW, file, caller, line, TERM_RESET,
                        TERM_BONW, VdsErrString(voids_err), TERM_RESET);
    } else {
        fprintf(stderr, "%sFailed call:\t%s%s\n"
                        "%son condition:\t%s%s\n"
                        "%sat %s -> %s():%d%s\n",
                        TERM_BONW, call, TERM_RESET,
                        TERM_BONW, failed_cond, TERM_RESET,
                        TERM_BONW, file, caller, line, TERM_RESET);
    }

    exit(EXIT_FAILURE);
}

void err_msg(program_s *prog, const char *fmt, ...)
{
    va_list args;
    unsigned int lin, col;

    if (prog) {
        if (prog->column == 1) {
            col = prog->prev_col;
            lin = prog->line - 1;
        } else {
            col = prog->column - 1;
            lin = prog->line;
        }

        fprintf(stderr, "%s:%u:%u: " TERM_RED "error: " TERM_RESET, prog->fname, lin, col);
        va_start(args, fmt);

        vfprintf(stderr, fmt, args);

        va_end(args);
        fputc('\n', stderr);
    }
}

void warn_msg(program_s *prog, const char *fmt, ...)
{
    va_list args;
    unsigned int lin, col;

    if (prog) {
        if (prog->column == 1) {
            col = prog->prev_col;
            lin = prog->line - 1;
        } else {
            col = prog->column - 1;
            lin = prog->line;
        }

        fprintf(stderr, "%s:%u:%u: " TERM_YEL "warning: " TERM_RESET, prog->fname, lin, col);

        va_start(args, fmt);

        vfprintf(stderr, fmt, args);

        va_end(args);
        fputc('\n', stderr);
    }
}

void dbg_msg(program_s *prog, const char *fmt, ...)
{
    va_list args;
    unsigned int lin, col;

    if (prog && debugging_messages) {
        if (prog->column == 1) {
            col = prog->prev_col;
            lin = prog->line - 1;
        } else {
            col = prog->column - 1;
            lin = prog->line;
        }

        fprintf(stderr, "%s:%u:%u: " TERM_CYAN " debug: " TERM_RESET, prog->fname, lin, col);

        va_start(args, fmt);

        vfprintf(stderr, fmt, args);

        va_end(args);
        fputc('\n', stderr);
    }
}

void shell_msg(const char *fmt, ...)
{
    va_list args;

    fprintf(stdout, TERM_YEL);

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    fprintf(stdout, TERM_RESET "\n");
}
