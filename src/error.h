#ifndef SIMBLY_ERROR_H__
#define SIMBLY_ERROR_H__

#include "common.h"
#include "program.h"


#define __ENO(call, line, caller, file) \
do { \
    errno = 0; \
    call; \
    int __tmp_err_code = errno; \
    if (__tmp_err_code) { \
        fatal_handler(MAKE_STR(call), NULL, line, caller, file, __tmp_err_code, 0); \
    } \
} while (0)

#define __PTH(call, line, caller, file) \
do { \
    int __tmp_err_code = call; \
    if (__tmp_err_code) { \
        fatal_handler(MAKE_STR(call), NULL, line, caller, file, __tmp_err_code, 0); \
    } \
} while (0)

#define __ERR(call, fail_condition, line, caller, file) \
do { \
    errno = 0; \
    call; \
    int __tmp_err_code = errno; \
    if (fail_condition) { \
        fatal_handler(MAKE_STR(call), MAKE_STR(fail_condition), line, caller, file, __tmp_err_code, 0); \
    } \
} while (0)

#define __VDS(call, line, caller, file, err_var) \
do { \
    call; \
    if (err_var != VDS_SUCCESS) { \
        fatal_handler(MAKE_STR(call), NULL, line, caller, file, 0, err_var); \
    } \
} while (0)

#define __VDSERR(call, fail_condition, line, caller, file, err_var) \
do { \
    call; \
    if (fail_condition) { \
        fatal_handler(MAKE_STR(call), NULL, line, caller, file, 0, err_var); \
    } \
} while (0)

#define ENO(call) __ENO(call, __LINE__, __func__, __FILE__)
#define PTH(call) __PTH(call, __LINE__, __func__, __FILE__)
#define ERR(call, fail_condition) __ERR(call, fail_condition, __LINE__, __func__, __FILE__)
#define VDS(call, err_var) __VDS(call, __LINE__, __func__, __FILE__, err_var)
#define VDSERR(call, fail_condition, err_var) __VDSERR(call, fail_condition, __LINE__, __func__, __FILE__, err_var)
#define ASRT(fail_condition) ERR({}, !(fail_condition))

extern int debugging_messages;


void fatal_handler(const char *call, const char *failed_cond, int line,
                   const char *caller, const char *file, int errno_err,
                   vdsErrCode voids_err);
void err_msg(program_s *prog, const char *fmt, ...);
void warn_msg(program_s *prog, const char *fmt, ...);
void dbg_msg(program_s *prog, const char *fmt, ...);
void shell_msg(const char *fmt, ...);


#endif //SIMBLY_ERROR_H__
