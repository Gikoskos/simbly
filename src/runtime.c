#include "runtime.h"
#include "exec.h"
#include "global.h"
#include "error.h"

//maximum time-slice amount in nano-seconds
//an instruction line normally takes about 10000000 nanoseconds to execute
#define TIME_SLICE_MAX_NSEC 10000000

static void *runtime_thread(void *param);
static void prog_free_cb(void *data);


void *runtime_thread(void *param)
{
    runtime_s *rt = (runtime_s*)param;
    struct timespec start_time, end_time;
    long int diff, time_slice;

    while (rt->running) {
        pthread_mutex_lock(&rt->lock);

        while (rt->running && !rt->program_list) {
            pthread_cond_wait(&rt->list_not_empty, &rt->lock);
        }

        rt->curr = rt->program_list;
        pthread_mutex_unlock(&rt->lock);

        program_s *prog;

        //this loop iterates through each entry in the program list
        //and interprets one or more instructions from each program.
        //round-robin scheduling is implemented so that each program
        //can get a fair percentage of CPU time. With this implementation,
        //regardless of how much the randomly generated time slice is,
        //at least a single instruction of each program will always execute
        while (rt->curr) {

            if (!rt->running) {
                break;
            }

            prog = (program_s*)rt->curr->pData;

            /* Calculate time slice */
            time_slice = RandomState_genLong(rt->rand_generator, NULL);

            /* make sure time slice is a non-negative value (could be zero but that doesn't matter with this algorithm) */
            if (time_slice < 0) {
                time_slice = 0 - time_slice;
            }

            time_slice = time_slice % TIME_SLICE_MAX_NSEC;

            switch (prog->state) {
                case MAGIC_LINE:
                case INSTRUCTION_LINE:

                    //printf("\nProgram %d is being executed!\n", prog->argv[0]);
                    do {

                        ENO(clock_gettime(CLOCK_REALTIME, &start_time));

                        interpret_next_line(prog);

                        ENO(clock_gettime(CLOCK_REALTIME, &end_time));

                        diff = end_time.tv_nsec - start_time.tv_nsec;

                        if (diff < 0) {
                            break;
                        } else {
                            time_slice -= diff;
                        }

                    } while ((time_slice > 0) && (prog->state == INSTRUCTION_LINE));

                    break;
                case SLEEPING:
                    diff = prog->sleep_left.tv_nsec - time_slice;

                    if (diff < 0) {

                        if (!prog->sleep_left.tv_sec) {

                            ENO(nanosleep(&prog->sleep_left, NULL));
                            prog->state = INSTRUCTION_LINE;

                        } else {

                            prog->sleep_left.tv_sec--;
                            prog->sleep_left.tv_nsec = diff + 1000000000;
                            //using start_time as a temp value here
                            start_time.tv_sec = 0;
                            start_time.tv_nsec = time_slice;
                            ENO(nanosleep(&start_time, NULL));
                        }

                    } else {
                        prog->sleep_left.tv_nsec = diff;

                        start_time.tv_sec = 0;
                        start_time.tv_nsec = time_slice;
                        ENO(nanosleep(&start_time, NULL));
                    }
                    break;
                case BLOCKED:
                    program_state_blocked(prog, time_slice);
                default:
                    break;
            }

            if (prog->state == FINISHED || prog->error_flag) {
                if (prog->error_flag)
                    shell_msg("Program %d was killed unexpectedly", prog->argv[0]);
                else
                    shell_msg("Program %d finished", prog->argv[0]);
                pthread_mutex_lock(&rt->lock);
                rt->curr = rt->curr->nxt;

                rt->program_cnt--;
                CDLList_deleteNode(&rt->program_list, rt->curr->prv, NULL);
                program_state_free(prog);
                pthread_mutex_unlock(&rt->lock);

                if (!rt->program_list) {
                    rt->curr = NULL;
                }
            } else {
                /* only reason for locking here is to make the 'list' command work
                 * properly. without the 'list' command, this locking can be removed */
                pthread_mutex_lock(&rt->lock);
                rt->curr = rt->curr->nxt;
                pthread_mutex_unlock(&rt->lock);
            }

        }
    }

    return NULL;
}

runtime_s *runtime_init(void)
{
    vdsErrCode verr;
    runtime_s *rt;
    pthread_mutexattr_t attr;

    ENO(rt = malloc(sizeof(runtime_s)));

    PTH(pthread_mutexattr_init(&attr));
    PTH(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
    PTH(pthread_mutex_init(&rt->lock, &attr));
    PTH(pthread_mutexattr_destroy(&attr));
    PTH(pthread_cond_init(&rt->list_not_empty, NULL));

    VDS(rt->rand_generator = RandomState_init((unsigned int)time(NULL), &verr), verr);

    rt->program_list = rt->curr = NULL;
    rt->program_cnt = 0;
    rt->running = 1;

    PTH(pthread_create(&rt->thrd_id, NULL, runtime_thread, (void*)rt));

    return rt;
}

void runtime_attach_program(runtime_s *rt, program_s *prog)
{
    if (rt && prog) {
        vdsErrCode err;

        pthread_mutex_lock(&rt->lock);

        rt->program_cnt++;
        VDS(CDLList_append(&rt->program_list, (void*)prog, &err), err);

        //if the list was empty before we added this program, there's a chance
        //that the interpreter thread will be sleeping on the condition, so we
        //have to signal it to wake up.
        if (rt->program_list->nxt == rt->program_list) {
            pthread_cond_signal(&rt->list_not_empty);
        }
        pthread_mutex_unlock(&rt->lock);
    }
}

void prog_free_cb(void *data)
{
    program_s *p = (program_s *)data;

    program_state_free(p);
}

void runtime_stop(runtime_s *rt)
{
    if (rt) {
        rt->running = 0;

        pthread_mutex_lock(&rt->lock);
        if (!rt->program_list) {
            pthread_cond_signal(&rt->list_not_empty);
        }
        pthread_mutex_unlock(&rt->lock);

        PTH(pthread_join(rt->thrd_id, NULL));

        CDLList_destroy(&rt->program_list, prog_free_cb, NULL);
        RandomState_destroy(&rt->rand_generator, NULL);
        pthread_cond_destroy(&rt->list_not_empty);
        pthread_mutex_destroy(&rt->lock);
        free(rt);
    }
}

/* taken from the GNU programming manual (but not used)
int
timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
  // Perform the carry for the later subtraction by updating y.
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  // Compute the time remaining to wait.
  // tv_usec is certainly positive.
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  //Return 1 if result is negative.
  return x->tv_sec < y->tv_sec;
}
*/
