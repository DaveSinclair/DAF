#ifndef _H_THREADS

#define _H_THREADS

#include <pthread.h>
#include "daf_protocol.h"
#include "OSstuff.h"


#define THREAD_WORK_ITEM_SIGNATURE     "THREADWORK"

#define MAX_TRACE_NUM 16           // counts number of entries in enum below  <<<<<<<<<<<<<
#define MAX_TRACE_NAME_LENGTH 20  // is this used ?????

enum { NOT_USED,
       PROC_SEG_LOCK,
       LOCK_MUTEX,      UNLOCK_MUTEX,
       PROC_SEG_UNLOCK,
       PROC_COND_WAIT1,
       WAIT_BEFORE,     WAIT_AFTER,
       PROC_COND_BROADCAST1, BROADCAST,
       PROC_LOCK_MUTEX, LOCK,
       PROC_UNLOCK_MUTEX, UNLOCK,
       BUFFER_TIMESTAMP, BUFFER_CREATE
     };



typedef struct thread_trace_element
{
    Iu32 timestamp;
    Iu32 tracepoint ;
    Iu32 type ;
    Iu32 p1 ;
    Iu32 p2 ;
    Iu32 p3 ;
    Iu32 rc ;
} thread_trace_element_t;

typedef union u_thread_trace_element
{
    thread_trace_element_t thread_trace_element;
} u_thread_trace_element_t;

typedef struct thread_return_code
{
    int value;
} thread_return_code_t;

typedef struct thread_workitem
{
    char    signature[sizeof(THREAD_WORK_ITEM_SIGNATURE)];
    bool_t    active;
    bool_t    terminated;
    HTHREAD thread;                            // the thread handle, as created in the
    // create_thread() routine
    int threadnum;                             // a tag, identifying this thread - each new
    // thread that is created should have a different
    // threadnum, but threadnum values can be re-used
    // after a thread has exited - this value must be set
    // by the user when calling the initialise_thread_workitem()
    // routine before calling the create_thread() routine
    thread_return_code_t  thread_exit_code;    // the exit code for the thread, value field = E_OK means success
    // anything else indicates the thread did not
    // complete successfully in some way - this value
    // is filled in by the exit_thread() routine
    u_thread_trace_element_t *ptrace_table;    // points to trace table
    Iu32                   trace_table_index;  // index of next free entry in table
    Iu32                   num_trace_entries;  // max number of entries in the trace table

} thread_workitem_t;


typedef struct response_monitor_wrk
{
    thread_workitem_t *pthread_workitem;
    void              *pparm;
} response_monitor_wrk_t;

typedef struct background_cmd_wrk
{
    char cmd[MAX_SYSTEM_CMD_LEN];
    bool_t quiet;
    thread_workitem_t *pthread_workitem;
    int this_tag;
} background_cmd_wrk_t;

void print_threads_version(void);
int init_timers(void);
Iu32 get_timestamp( void );
#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
float time_diff(time_control_t  *start, time_control_t  *end);
float time_compare(time_control_t *ptimer1, time_control_t *ptimer2);
void  time_sub_secs(time_control_t *ptimer1, float seconds, time_control_t *ptimer2);
int dskx_thread_block_async_signals(sigset_t *saved_set);
#elif defined WIN32 || defined NETWARE
float time_diff(time_control_t *start, time_control_t *stop);
float time_compare(time_control_t *ptimer1, time_control_t *ptimer2);
float time_sub_secs(time_control_t *ptimer1, float seconds, time_control_t *ptimer2);
#endif


int run_system_cmd_and_print_all_output(char *cmd);
int run_system_cmd_and_print_single_line_output(char *cmd, char *output, int max_output_line_length);
int run_system_cmd(char *cmd, bool_t quiet);
int run_system_cmd_in_background(char *cmd);

void initialise_thread_workitem( thread_workitem_t *thread_workitem, int threadnum, int num_trace_entries, bool_t reinitialise);
void deinitialise_thread_workitem( thread_workitem_t *pthread_workitem);
void check_thread_workitem( thread_workitem_t *thread_workitem, char *msg );

DWORD create_mutex( MUTEX **ptr_mutex, char *name );
DWORD lock_mutex( MUTEX *mutex , char *name, char *procname);
DWORD unlock_mutex( MUTEX *mutex, char *name, char *procname);

DWORD create_cond( COND **ptr_cond, char *name );
DWORD cond_wait1( COND *cond, MUTEX *mutex, char *cond_name, char *mutex_name, char *procname);
DWORD cond_broadcast1( COND *cond, char *name, char *procname );

int  create_thread       (void              *(*start_routine)(void *),
                          void              *parm,
                          thread_workitem_t *pthread_workitem);
void exit_thread         (thread_workitem_t *pthread_workitem, int return_code );
int  create_thread1      (void              *(*start_routine)(void *),
                          void              *parm);
void exit_thread1        (int return_code );

TID  thread_ID           (void );

DWORD init_thread_environment( void );

void sleep_for_seconds( int seconds, volatile bool_t *pexiting_flag);
void sleep_for_milliseconds( int milliseconds, volatile bool_t *pexiting_flag);
void sleep_for_microseconds(int microseconds, volatile bool_t *pexiting_flag);

Iu32 get_timestamp( void );

PID get_process_pid();

int debug1(const char *pdebug_flag);

/* Just a simple define for now to document the call-sites.
 * This needs to be a platform specific function/macro to
 * return a unique number based on thread ID as pthread_t is
 * an opaque type (on HPUX at least it's a struct I believe). */
#define printable_tid(x) ((unsigned long)(x))

int add_item_to_trace_table( thread_workitem_t *pthread_workitem,
                             Iu32 tracepoint,
                             Iu32 tracetype,
                             Iu32 p1,
                             Iu32 p2,
                             Iu32 p3,
                             Iu32 rc);

void print_trace_buffer( thread_workitem_t *pthread_workitem);



#endif
