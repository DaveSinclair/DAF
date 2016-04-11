/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                        */
/*                                                                                                                      */
/* daf_threads.c                                                                                                        */
/*                                                                                                                      */
/* This file is part of the daf test automation programme.                                                              */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

//*****************************************************************************/
//
//  NOTE: When compiling for WINDOWS you must compile with the Multithreaded library
//        option - to do this select project or solution:
//           goto Configuration Properties
//              C++
//                Code Generation
//        and select the Multithreaded (/MT) library option.
//
//        This is because we use C run-time routine, with LIBCMT.LIB, and consequently
//        we are required to use  _beginthread and _endthread instead of CreateThread()
//        and ExitThread().  We also avoid the use of SuspendThread since this can
//        lead to a deadlock when more than one thread is blocked waiting for
//        the suspended thread to complete its access to a C run-time data structure.
//
/*****************************************************************************/

#include "daf_threads.h"
#include "daf_util.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define DEFAULT_MUTEX_ATTR_PTR  NULL
#define DEFAULT_COND_ATTR_PTR   NULL

pthread_attr_t  thread_attrs_detached;
pthread_attr_t  thread_attrs_joinable;

int thread_block_async_signals(sigset_t *saved_set);


#elif defined WIN32

#include <process.h>
LARGE_INTEGER Frequency;
Iu64 PerformanceFrequencyCounter_counts_per_second;
Iu64 PerformanceFrequencyCounter_counts_per_ms;

#else
#error Cannot compile, unknown target OS - define AIX, LINUX, LINUX_PPC64, SOLARIX, HPUX or WIN32
#endif

int threaddebug = 0;

#define TIMER_INIT_VALUE 1000000   //  timer wraps every 27 hours .... hmmm <<<<<

typedef struct trace_lookup_element
{
    int  trace_value;
    char trace_name[MAX_TRACE_NAME_LENGTH];
} trace_lookup_element_t;

static trace_lookup_element_t trace_lookup_table[MAX_TRACE_NUM] =
{

    { NOT_USED,        "not_used" },
    { PROC_SEG_LOCK,   "seg_lock" },
    { PROC_SEG_UNLOCK, "seg_unlock" },
    { LOCK_MUTEX,      "lock_mutex" },
    { UNLOCK_MUTEX,    "unlock_mutex" },
    { PROC_COND_WAIT1, "proc_cond_wait1" },
    { WAIT_BEFORE,     "wait_before" },
    { WAIT_AFTER,      "wait_after" },
    { PROC_COND_BROADCAST1, "proc_cond_broadcast1" },
    { BROADCAST,       "broadcast" },
    { PROC_LOCK_MUTEX, "proc_lock_mutex" },
    { LOCK,            "lock" },
    { PROC_UNLOCK_MUTEX, "proc_unlock_mutex" },
    { UNLOCK,           "unlock" },
    { BUFFER_TIMESTAMP, "buffer_timestamp" },
    { BUFFER_CREATE,    "created" }

};



/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        init_timers                                               */
/*                                                                        */
/* FUNCTION:    Initialises the real time timers for this operating       */
/*              system                                                    */
/*                                                                        */
/* PARAMS IN:   none                                                      */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     0 always for success                              .       */
/*                                                                        */
/*------------------------------------------------------------------------*/

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

struct itimerval rttimer;
struct itimerval old_rttimer;
struct itimerval time_now;

int init_timers(void)
{
    // won't a timer wrap upset the trace buffers??? <<<<<<<<<<<<<
    rttimer.it_value.tv_sec  = TIMER_INIT_VALUE;
    rttimer.it_value.tv_usec = 0;       // always set to 0
    rttimer.it_interval.tv_sec  = TIMER_INIT_VALUE;
    rttimer.it_interval.tv_usec = 0;    // always set to 0, code elsewhere assumes this

    setitimer(ITIMER_REAL, &rttimer, &old_rttimer);

    //printf("init_timers\n");
    //getitimer(ITIMER_REAL, &time_now);
    //print_time( &(time_now.it_value));

    return(0);
}

#elif defined WIN32

int init_timers(void)
{

    int rc=0;
    LPVOID lpMsgBuf;
    char msg[MAX_MSG_LEN];

    if (! QueryPerformanceFrequency(&Frequency))
    {
        rc = GetLastError();
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      rc,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR) &lpMsgBuf,
                      0,
                      NULL);

        sprintf(msg,  "ERROR: QueryPerformanceFrequency failed rc = %d : %s\n", rc,lpMsgBuf );
        print_msg_to_console(msg);

        LocalFree( lpMsgBuf );   // Free the buffer created by the FormatMessage() call
    }
    else
    {
        PerformanceFrequencyCounter_counts_per_second = (Iu64) Frequency.QuadPart;
        //sprintf(msg, "Windows high res counter FrequencyCount = " IU64_FSPEC "\n",
        //             PerformanceFrequencyCounter_counts_per_second);
        PerformanceFrequencyCounter_counts_per_ms = ((Iu64) Frequency.QuadPart) / 1000;
        //print_msg_to_console(msg);
    }

    return(rc);

}

#endif

/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        time_diff                                                 */
/*                                                                        */
/* FUNCTION:    Computes the time difference between two real time        */
/*              interval timers and returns this as floating point        */
/*              values in seconds.                                        */
/*                                                                        */
/*                                                                        */
/* PARAMS IN:   pstart    pointer to the value of the real time interval  */
/*                        timer at the start of the period to be measured */
/* PARAMS IN:   pend      pointer to the value of the real time interval  */
/*                        timer at the end of the period to be measured   */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     a float, the time between the two interval timers, in secs*/
/*                                                                        */
/*------------------------------------------------------------------------*/

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

float time_diff(struct itimerval *pstart,
                struct itimerval *pend)
{

    /*  The pstart and pend  interval timers count down from an initial value    */
    /*  held in the rttimer.it_interval structure and initially set up by the    */
    /* init_timer() routine.   When the counter reaches zero it wraps back to    */
    /* the value  specified in rttimer.it_interval stucture                      */
    /* returns the difference in seconds between the two times                   */
    /* Note, these are itimer times so count down as time goes on...             */

    struct timeval diff_time;
    float difference;

    assert(pend);
    assert(pstart);

    diff_time.tv_sec  = pstart->it_value.tv_sec  - pend->it_value.tv_sec;
    diff_time.tv_usec = pstart->it_value.tv_usec - pend->it_value.tv_usec;

    if (diff_time.tv_usec < 0)
    {
        diff_time.tv_sec  = diff_time.tv_sec-1;
        diff_time.tv_usec = diff_time.tv_usec+1000000;
    }

    difference = diff_time.tv_sec + ((float)diff_time.tv_usec)/1000000.0;

    if (difference < 0.0)
    {
        // timer must have wrapped, so we have to add back in the initial
        // value of the timer
        difference = difference + rttimer.it_interval.tv_sec;
    }

    //print_time( &(pstart->it_value));
    //print_time( &(pend->it_value));
    //printf("difference  %8.3f\n", difference);
    return difference;
}

#elif defined WIN32

float time_diff(time_control_t *pstart, time_control_t *pstop)
{

    return (float)(((Iu64) pstop->QuadPart) -
                   ((Iu64) pstart->QuadPart)) /
           (float)(PerformanceFrequencyCounter_counts_per_second);
}

#endif

/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        time_compare                                              */
/*                                                                        */
/* FUNCTION:    Compares two real time interval timers to see if the      */
/*              first is larger than the second, returns TRUE if it is    */
/*                                                                        */
/*                                                                        */
/* PARAMS IN:   ptimer1   pointer to the value of the first real time     */
/*                        interval timer                                  */
/*              ptimer2   pointer to the value of the second real time    */
/*                        interval timer                                  */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     TRUE if the value of ptimer1 > ptimer2                    */
/*                                                                        */
/*------------------------------------------------------------------------*/

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

float time_compare(time_control_t *ptimer1, time_control_t *ptimer2)
{

    assert(ptimer1);
    assert(ptimer2);

    if (ptimer1->it_value.tv_sec > ptimer2->it_value.tv_sec)
    {
        return(TRUE);
    }
    else if (ptimer1->it_value.tv_sec == ptimer2->it_value.tv_sec)
    {
        if (ptimer1->it_value.tv_usec > ptimer2->it_value.tv_usec)
        {
            return(TRUE);
        }
    }

    return(FALSE);
}

#elif defined WIN32

float time_compare(time_control_t *timer1, time_control_t *timer2)
{

    printf("Code is incomplete in time_compare\n");
    exit(1);

}

#endif

/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        time_sub_secs                                             */
/*                                                                        */
/* FUNCTION:    Substracts  a positive oloating seconds value from a real */
/*              time  interval timer and returns the new interval timer   */
/*              value                                                     */
/*                                                                        */
/* PARAMS IN:   ptimer1   pointer to the value of the real time interval  */
/*                        timer                                           */
/*              seconds   floating point number of seconds                */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     ptimer1 - seconds                                         */
/*                                                                        */
/*------------------------------------------------------------------------*/

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

void time_sub_secs(struct itimerval *ptimer1, float seconds, struct itimerval *ptimer2)
{

    long num_seconds;
    long num_usecs;

    assert(ptimer1);
    assert(seconds >= 0.0);

    num_seconds = (int) (seconds);
    num_usecs   = (int) ((seconds - (float) num_seconds) * 1000000);

    ptimer2->it_value.tv_sec  = ptimer1->it_value.tv_sec  - num_seconds;
    ptimer2->it_value.tv_usec = ptimer1->it_value.tv_usec - num_usecs;

    if (ptimer2->it_value.tv_usec < 0)
    {
        ptimer2->it_value.tv_sec  = ptimer2->it_value.tv_sec-1;
        ptimer2->it_value.tv_usec = ptimer2->it_value.tv_usec+1000000;
    }
}

#elif defined WIN32

float time_sub_secs(time_control_t *ptimer1, float seconds, time_control_t *ptimer2)
{

    return (float)(((Iu64) ptimer2->QuadPart) -
                   ((Iu64) ptimer1->QuadPart)) /
           (float)(PerformanceFrequencyCounter_counts_per_second);
}

#endif

/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        initialise_thread_workitem                                */
/*                                                                        */
/* FUNCTION:    Given a pre-existing thread_workitem, this routine puts   */
/*              this structure into a state that can be used to run a     */
/*              thread.  This means:                                      */
/*                                                                        */
/*              resetting the active and terminated flags                 */
/*              initialising the defualt thread exist value of 0          */
/*              allocating (if num_trace_entrie is non zero) some memory  */
/*              to hold a trace buffer of suitable size, or ensuring that */
/*              the pointer to the trace buffer is NULL  if no trace      */
/*              buffer is required, so other routines do not try to make  */
/*              any entries in a non existent buffer                      */
/*                                                                        */
/*              This routine will malloc() an area of memory for the      */
/*              trace buffer, sufficiently large to contain               */
/*              num_trace_entries - this memory area should be released   */
/*              by a call to deinitialise_thread_workitem() when the      */
/*              thread work item is no longer needed                      */
/*                                                                        */
/*                                                                        */
/* PARAMS IN:   pthread_workitem    pointer to the workitem structure     */
/*                                  that is used to 'manage' this thread  */
/*              threadnum           a unique number, allocated by the user*/
/*                                  that distinguishes this thread from   */
/*                                  any other thread item being run using */
/*                                  other thread_workitems                */
/*              num_trace_entries   the number of trace entries that are  */
/*                                  to be allocated in the trace buffer   */
/*                                  for this thread.                      */
/*              re_initialise       TRUE if we are re-initialising        */
/*                                  a thead_workitem that has been        */
/*                                  used before                           */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     void                                                      */
/*                                                                        */
/*------------------------------------------------------------------------*/

void initialise_thread_workitem( thread_workitem_t *pthread_workitem,
                                 int threadnum,
                                 int num_trace_entries,
                                 bool_t reinitialise)
{

    char msg[MAX_MSG_LEN];

    assert(pthread_workitem);

    /*
      if (reinitialise) {
        if (! pthread_workitem->terminated) {
             sprintf(msg, "initialise_thread_workitem: terminated flag is FALSE - maybe re-initialising an active thread - waiting 1 sec\n");
             print_msg_to_console(msg);
             if (num_times_to_wait_for_terminatation > 0) {
               num_times_to_wait_for_terminatation--;
               sleep(1);
             } else {
               sprintf(msg, "initialise_thread_workitem: terminated flag is FALSE - ERROR re-initialising an active thread\n");
               print_msg_to_console(msg);
               exit(1);
             }
        }
      }

    */

    pthread_workitem->trace_table_index      = 0;
    pthread_workitem->active                 = FALSE;
    pthread_workitem->terminated             = FALSE;
    pthread_workitem->thread_exit_code.value = 0;
    pthread_workitem->threadnum              = threadnum;

    if (num_trace_entries != 0)
    {

        pthread_workitem->ptrace_table = (u_thread_trace_element_t *) malloc((num_trace_entries + 1) *
                                         sizeof(u_thread_trace_element_t));

        if (pthread_workitem->ptrace_table == NULL)
        {
            sprintf(msg, "initialise_thread_workitem: Could not malloc space for trace table with %d entries\n", (num_trace_entries + 1));
            print_msg_to_console(msg);
            exit(1);
        }

        memset(pthread_workitem->ptrace_table, 0, (num_trace_entries+1) * sizeof(u_thread_trace_element_t));

        /* first entry in table is a special one. */
        pthread_workitem->ptrace_table[0].thread_trace_element.timestamp = get_timestamp();
        pthread_workitem->ptrace_table[0].thread_trace_element.tracepoint = BUFFER_TIMESTAMP;
        pthread_workitem->ptrace_table[0].thread_trace_element.type = BUFFER_CREATE;
        pthread_workitem->ptrace_table[0].thread_trace_element.p1 =  convert_Iu32_to_bigendian((Iu32) time(0));
        pthread_workitem->ptrace_table[0].thread_trace_element.p2 = pthread_workitem->ptrace_table[0].thread_trace_element.timestamp;
        pthread_workitem->ptrace_table[0].thread_trace_element.p3 = 0x0;
        pthread_workitem->ptrace_table[0].thread_trace_element.rc = 0x0;

        pthread_workitem->trace_table_index = 1;
        pthread_workitem->num_trace_entries = num_trace_entries+1;

    }
    else
    {

        pthread_workitem->ptrace_table = NULL;
        pthread_workitem->num_trace_entries = 0;

    }

    safecpy(pthread_workitem->signature,
            THREAD_WORK_ITEM_SIGNATURE,
            sizeof(pthread_workitem->signature));
    return;
}

/*------------------------------------------------------------------------

 NAME: deinitialise_thread_work_item

 PARAMS IN:   pthread_workitem    pointer to the workitem structure
                                  that is used to 'manage' this thread
 Output:    return_code           the return code for this thread

 we assume that we are no longer going to need the trace buffer
 in this thread's work item - and we need to deallocate the
 memory that was being used in the buffer

------------------------------------------------------------------------*/

void deinitialise_thread_workitem( thread_workitem_t *pthread_workitem)
{

    char msg[MAX_MSG_LEN];
    assert(pthread_workitem);

    if (threaddebug)
    {
        sprintf(msg, "deinitialise_thread_workitem:       thread=%lx \n", printable_tid(thread_ID()));
        print_msg_to_console(msg);
    }

    if (! MATCH(pthread_workitem->signature, THREAD_WORK_ITEM_SIGNATURE))
    {
        sprintf(msg, "Missing " THREAD_WORK_ITEM_SIGNATURE " in thread_workitem structure\n");
        print_msg_to_console(msg);
        exit(1);
    }

    /* sprintf(msg, "removing %d entries in trace table %p\n", pthread_workitem->num_trace_entries, pthread_workitem);
    print_msg_to_console(msg); */

    free((void *) pthread_workitem->ptrace_table);
    pthread_workitem->trace_table_index = 0;
    pthread_workitem->num_trace_entries = 0;

}


/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        thread_ID                                                 */
/*                                                                        */
/* FUNCTION:    returns the current thread ID.                            */
/*                                                                        */
/* PARAMS IN:   none                                                      */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     the ID of the thread (as reported by the OS) for the      */
/*              thread that called this routine.                          */
/*                                                                        */
/*------------------------------------------------------------------------*/
TID thread_ID( void )
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    TID thread_id = pthread_self();

    return( thread_id );

#elif  defined WIN32

    TID thread_id = GetCurrentThreadId();

    return( thread_id );

#endif

}

/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        thread_error                                              */
/*                                                                        */
/* FUNCTION:                                                              */
/*                                                                        */
/* PARAMS IN:   p                                                         */
/*                                                                        */
/*              rc                                                        */
/*                                                                        */
/*              pMsg                                                      */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     void                                                      */
/*                                                                        */
/*------------------------------------------------------------------------*/
#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

void thread_error( void *p, int rc, char *pMsg )
{

    char msg[MAX_MSG_LEN];
    assert(pMsg);

    sprintf(msg, "THREAD ERROR pointer = %p,  rc=%d (%s),  %s\n", p, rc, strerror(rc), pMsg);
    print_msg_to_console(msg);
    exit( -1 );
}

#elif defined WIN32

void thread_error( HANDLE handle, int rc, char *pMsg )
{
    char msg[MAX_MSG_LEN];
    assert(pMsg);
    sprintf(msg, "THREAD ERROR for thread handle %p  rc=%d, %s\n", handle, rc, pMsg );
    print_msg_to_console(msg);
    exit( -1 );
}

#endif


/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        init_thread_environment                                   */
/*                                                                        */
/* FUNCTION:    This routine must be called prior to calling any other    */
/*              routines in threads.c.  The routine initialises the       */
/*              thread environment for the program in this particular OS  */
/*                                                                        */
/*                                                                        */
/* PARAMS IN:   none                                                      */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     0 for success, non zero for failure                       */
/*                                                                        */
/*------------------------------------------------------------------------*/
DWORD init_thread_environment( void )
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    pthread_attr_init(&thread_attrs_detached);
    pthread_attr_setdetachstate(&thread_attrs_detached, PTHREAD_CREATE_DETACHED);
    pthread_attr_init(&thread_attrs_joinable);
    pthread_attr_setdetachstate(&thread_attrs_joinable, PTHREAD_CREATE_JOINABLE);

    // Solaris only appears to allow 2 to 3 threads to run in parallel by
    // default - this is not enough - so give it a hint as to how many
    // threads we really want to run in parallel (note, even with the hint,
    // there is no guarantee that all of these will run in parallel
#if defined SOLARIS
    pthread_setconcurrency(MAX_CONCURRENT_THREADS);
#endif

    return(E_OK);

#elif  defined WIN32

    return(E_OK);   // nothing needs initialising in Windows

#endif

}

/*------------------------------------------------------------------------

 Name: create_mutex

 Inputs:    name        pointer to null terminated string, that names
                        the mutex that is to be created.  This is used
                        only in debug messages in Unix but is required
                        to name the mutex in Windows.

 Ouputs:    handle      handle to the mutex object that is created

 Return code:           E_OK if mutex was successfully created
                        E_NOTOK if a problem occurred

 The routine calls exit via the thread_error routine, so will never return
 if the mutex create operation fails.

 ------------------------------------------------------------------------*/
DWORD create_mutex( MUTEX **handle, char *name )
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    int rc;
    pthread_mutex_t *pmutex;
    char msg[MAX_MSG_LEN];
    pthread_mutexattr_t attr;

    assert(handle);
    assert(name);

    pthread_mutexattr_init(&attr);
    //    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pmutex = (pthread_mutex_t * ) malloc(sizeof(pthread_mutex_t));

    if (pmutex == NULL)
    {
        sprintf(msg, "create_mutex: could not malloc space for new mutex: %s\n", name);
        print_msg_to_console(msg);
        exit(1);
    }

    // how does this mem get de-allocated? <<<<<<<<

    if (threaddebug)
    {
        sprintf(msg, "create_mutex: address of mutex %s is %p\n", name, pmutex);
        print_msg_to_console(msg);
        sprintf(msg, "create_mutex: %28s: thread=%lx  mutex *=%p  entered\n", "create_mutex",
                printable_tid(thread_ID()), *handle);
        print_msg_to_console(msg);
    }

    //    rc = pthread_mutex_init( pmutex, DEFAULT_MUTEX_ATTR_PTR );
    rc = pthread_mutex_init( pmutex, &attr);

    if( rc != 0 )
    {
        strcpy(msg, "create_mutex: ");
        strcat(msg, name);
        thread_error( (void *)  handle, rc, msg );
    }

    *handle = pmutex;

    if (threaddebug)
    {
        sprintf(msg, "create_mutex: %28s:  thread=%lx  mutex=%p  ended\n", "create_mutex",
                printable_tid(thread_ID()), *handle);
        print_msg_to_console(msg);
    }

    return(SUCCESS);

#elif defined WIN32

    char msg[MAX_MSG_LEN];
    DWORD rc = SUCCESS;
    HANDLE new_handle = CreateMutex( NULL, FALSE, (LPCTSTR)name );

    if (threaddebug)
    {
        sprintf(msg, "create_mutex: %s\n", name);
        print_msg_to_console(msg);
    }

    if( new_handle == NULL )
    {
        rc = GetLastError();
        strcpy(msg, "create_mutex: ");
        strcat(msg, name);
        thread_error( handle, rc, msg );
    }
    else
    {
        *handle = new_handle;
    }

    return rc;

#endif

}

/*------------------------------------------------------------------------

 lock_mutex

 Inputs:    handle      handle to the mutex object that is to be locked

            name        the name of the mutex.  This is used in debug
                        messages only and does not affect the functional
                        behaviour of the routine.

            procname    the name of the procedure that called this routine
                        - only used for debug

 Ouputs:    none

 Return code:           E_OK if mutex was successfully locked
                        E_NOTOK if a problem occurred

 The routine locks the mutex.

  ------------------------------------------------------------------------*/
DWORD lock_mutex( MUTEX *handle, char *name, char *procname )
{

    char msg[MAX_MSG_LEN];
#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
    int rc;
#elif defined WIN32
    DWORD rc;
    DWORD rc1;
#endif

    assert(handle);
    assert(name);
    assert(procname);


#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX


    if (threaddebug)
    {
        sprintf(msg, "%28s: %28s name=%28s thread=%lx  mutex=%p  entered\n",
                "lock_mutex", procname, name,
                printable_tid(thread_ID()), handle);
        print_msg_to_console(msg);
    }

    rc = pthread_mutex_lock(handle);

    if( rc != 0 )
    {
        sprintf(msg, "lock_mutex: %s %s", name, procname);
        thread_error( (void *)  handle, rc, msg );
    }

    if (threaddebug)
    {
        sprintf(msg, "%28s: %28s name=%28s thread=%lx  mutex=%p  ended\n",
                "lock_mutex", procname, name,
                printable_tid(thread_ID()), handle);
        print_msg_to_console(msg);
    }

    return(SUCCESS);

#elif defined WIN32

    rc1 =  WaitForSingleObject( handle, INFINITE );

    switch( rc1 )
    {
    case WAIT_OBJECT_0 :
        rc = SUCCESS;
        break;

    case WAIT_FAILED :
        rc = GetLastError();
        break;

    default :
        rc = FAILURE;
        break;
    }

    if (threaddebug)
    {
        sprintf(msg, "lock_mutex: %s proc=%s rc=%d\n", name, procname, rc);
        print_msg_to_console(msg);
    }

    if( rc != SUCCESS )
    {
        sprintf(msg, "lock_mutex: %s %s rc=%ld, rc1=%ld", name, procname, rc, rc1);
        thread_error( handle, rc, msg );
    }

    return rc;

#endif

}

/* ------------------------------------------------------------------------

 unlock_mutex

 Inputs:    handle      handle to the mutex object that is to be locked

            name        the name of the mutex.  This is used in debug
                        messages only and does not affect the functional
                        behaviour of the routine.

            procname    the name of the procedure that called this routine
                        - only used for debug

 Ouputs:    none

 Return code:           E_OK if mutex was successfully unlocked
                        E_NOTOK if a problem occurred

 The routine unlocks the mutex.

------------------------------------------------------------------------*/

DWORD unlock_mutex( MUTEX *handle, char *name, char *procname )
{

    char msg[MAX_MSG_LEN];
#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
    int rc;
#elif defined WIN32
    DWORD rc = SUCCESS;
#endif

    assert(handle);
    assert(name);
    assert(procname);

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    if (threaddebug)
    {
        sprintf(msg, "%28s: %28s name=%28s thread=%lx  mutex=%p\n",
                "unlock_mutex", procname, name,
                printable_tid(thread_ID()), handle);
        print_msg_to_console(msg);
    }

    rc = pthread_mutex_unlock(handle);

    if( rc != 0 )
    {
        sprintf(msg, "unlock_mutex: %s %s", name, procname);
        thread_error( (void *) handle, rc, msg );
    }

    return(SUCCESS);

#elif defined WIN32

    if( ReleaseMutex( handle ) == 0 )
    {
        rc = GetLastError();
        sprintf(msg, "unlock_mutex: %s %s", name, procname);
        thread_error( handle, rc, msg );
    }

    if (threaddebug)
    {
        sprintf(msg, "unlock_mutex: %s proc=%s rc=%d\n", name, procname, rc);
        print_msg_to_console(msg);
    }

    return rc;

#endif

}

/*------------------------------------------------------------------------

 create_cond

 Creates a condition variable for synchronisation purposes

 Inputs:    cond_name   pointer to null terminated string, that names
                        the condition - this is used only in debug
                        messages in Unix but is required
                        to name the condition variable in Windows.

 Ouputs:    handle      pointer to the condition variable that is created

 Return code:           SUCCESS if condition variable was successfully created

 The routine calls exit via the thread_error routine, so will never return
 if the condition variable create operation fails.

------------------------------------------------------------------------*/

DWORD create_cond( COND **cond, char *cond_name )
{

    char msg[MAX_MSG_LEN];

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    COND *pcondition;

    int rc;

    assert(cond);
    assert(cond_name);

    pcondition = (COND *)  malloc(sizeof(COND));

    if (pcondition == NULL)
    {
        sprintf(msg, "%28s: could not malloc space for new condition\n", "create_cond");
        print_msg_to_console(msg);
        exit(1);
    }


    if (threaddebug)
    {
        sprintf(msg, "%28s:  name=%s  thread=%lx  cond* =%p entered\n",
                "create_cond", cond_name, printable_tid(thread_ID()), cond);
        print_msg_to_console(msg);
    }

    rc = pthread_cond_init( pcondition, DEFAULT_COND_ATTR_PTR );

    if( rc != 0 )
    {
        strcpy(msg, "create_cond: ");
        strcat(msg, cond_name);
        thread_error( (void *) 0 , rc, msg );
    }

    *cond = pcondition;

    return(SUCCESS);

#elif  defined WIN32

    // assert(cond);
    // assert(cond_name);

    DWORD rc = SUCCESS;

    HANDLE handle = CreateEvent(NULL,                    // no security attributes
                                TRUE,                    // manual-reset event
                                FALSE,                   // initial state is non-signaled
                                (LPCTSTR)cond_name);     // object name

    if (threaddebug)
    {
        sprintf(msg, "create_cond: %s\n", cond_name);
        print_msg_to_console(msg);
    }

    if (handle == NULL)
    {
        rc = FAILURE;    // error
        strcpy(msg, "create_cond: ");
        strcat(msg, cond_name);
        thread_error( handle, rc, msg );
    }
    else
    {
        *cond = handle;
    }

    return( rc );

#endif

}

/*------------------------------------------------------------------------

 cond_wait1

 This routine atomically releases the mutex and causes the calling thread
 to block on the condition variable until the condition variable is set.

 Inputs:    cond        pointer to condition variable

            mutex       pointer to mutex

            cond_name   pointer to null terminated string that names
                        the condition - this is used only in debug
                        messages

            mutex_name  pointer to null terminated string that names
                        the mutex - this is used only in debug
                        messages

            procname    the name of the procedure that called this routine
                        - only used for debug

 Return code:           SUCCESS

 The routine calls exit if an error is detected, so will never return unless
 things work normally.

------------------------------------------------------------------------*/

DWORD cond_wait1( COND *cond, MUTEX *mutex, char *cond_name, char *mutex_name, char *proc_name )
{

    int rc;
    char msg[MAX_MSG_LEN];
    assert(cond);
    assert(mutex);
    assert(cond_name);
    assert(mutex_name);
    assert(proc_name);

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    if (threaddebug)
    {
        sprintf(msg, "%28s: %28s condition=%28s  mutex=%s  thread=%lx  cond=%p  entered\n",
                "cond_wait1", proc_name, cond_name, mutex_name,
                printable_tid(thread_ID()), cond);
        print_msg_to_console(msg);
    }

    rc = pthread_cond_wait(cond, mutex);

    if (rc != 0)
    {
        sprintf(msg, "cond_wait1: %s %s %s", proc_name, cond_name, mutex_name);
        thread_error( (void *) cond, rc, msg );
        exit(1);
    }

    if (threaddebug)
    {
        sprintf(msg, "%28s:    condition=%s  mutex=%s  thread=%lx  cond=%p  completed\n",
                "cond_wait1", cond_name, mutex_name,
                printable_tid(thread_ID()), cond);
        print_msg_to_console(msg);
    }

#elif  defined WIN32

    rc = ResetEvent( cond );    // set to non-signalled

    if (rc == 0)
    {
        thread_error( cond, rc, "cond_wait1" );
    }



    unlock_mutex( mutex, mutex_name, "cond_wait1" );

    if (threaddebug)
    {
        sprintf(msg, "cond_wait1: (unlocked mutex, waiting) cond=%s, mutex=%s, proc=%s\n", cond_name, mutex_name, proc_name);
        print_msg_to_console(msg);
    }

    do
    {
        rc = WaitForSingleObject( cond, INFINITE );
    }
    while( rc != WAIT_OBJECT_0 );

    if (threaddebug)
    {
        sprintf(msg, "cond_wait1: (wait complete) cond=%s, mutex=%s, proc=%s\n", cond_name, mutex_name, proc_name);
        print_msg_to_console(msg);
    }

    lock_mutex( mutex, mutex_name, "cond_wait1" );

    if (threaddebug)
    {
        sprintf(msg, "cond_wait1: (relocked mutex) cond=%s, mutex=%s, proc=%s\n", cond_name, mutex_name, proc_name);
        print_msg_to_console(msg);
    }


#endif

    return( SUCCESS );

}

/*------------------------------------------------------------------------

 cond_broadcast1


 Inputs:    cond        pointer to condition variable

            cond_name   pointer to null terminated string that names
                        the condition - this is used only in debug
                        messages

            proc_name   the name of the procedure that called this routine
                        - only used for debug

 Return code:           SUCCESS

 The routine calls exit if an error is detected, so will never return unless
 things work normally.

------------------------------------------------------------------------*/

DWORD cond_broadcast1( COND *cond , char *cond_name, char *proc_name)
{

    int rc;
    char msg[MAX_MSG_LEN];

    assert(cond);
    assert(cond_name);
    assert(proc_name);

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX


    if (threaddebug)
    {
        sprintf(msg, "%28s: %28s name=%28s  thread=%lx cond=%p\n",
                "cond_broadcast1", proc_name, cond_name,
                printable_tid(thread_ID()), cond);
        print_msg_to_console(msg);
    }

    rc = pthread_cond_broadcast(cond);

    if( rc != 0 )
    {
        sprintf(msg, "cond_wait1: %s %s\n", proc_name, cond_name);
        thread_error( (void *) cond, rc, msg );
    }


#elif  defined WIN32

    rc = SetEvent( cond );  // set state to signalled

    if( rc == 0 )
    {
        thread_error( cond, rc, "cond_broadcast1" );
    }

    if (threaddebug)
    {
        sprintf(msg, "cond_broadcast: cond=%s, proc=%s\n", cond_name, proc_name);
        print_msg_to_console(msg);
    }


#endif

    return(SUCCESS);
}

/*------------------------------------------------------------------------

 create_thread


 Inputs:    pstart_routine       pointer to the function that is to be run inside the
                                 the new thread

            pparm                pointer to a void, this usually points to a
                                 structure that contains the input parameters
                                 for start_routine

            pthread_workitem     pointer to the thread workitem area - this must
                                 have been created by a previous call to
                                 initialise_thread_workitem()

 Outputs:



 Return code:           E_OK if the thread was successfully created
                        E_NOTOK if a problem occurred.

 The routine launches the specified function a new thread.

------------------------------------------------------------------------*/

int create_thread( void *(*pstart_routine)(void *), void *pparm,
                   thread_workitem_t *pthread_workitem)
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    int rc;
    char msg[MAX_MSG_LEN];
    sigset_t saved_sigset;

    assert(pstart_routine);
    assert(pparm);
    assert(pthread_workitem);

    if (threaddebug)
    {
        sprintf(msg, "create_thread:    started\n");
        print_msg_to_console(msg);
    }


    if (! MATCH(pthread_workitem->signature, THREAD_WORK_ITEM_SIGNATURE))
    {
        sprintf(msg, "Missing signature (" THREAD_WORK_ITEM_SIGNATURE ") in thread_workitem structure\n");
        print_msg_to_console(msg);
        exit(1);
    }

    /*
     * The thread created here will inherit _this threads_ signal mask.
     * We create a temporary restrictive mask here, spawn the thread,
     * then restore out original state.
     * The newly created thread then has the responsibility for unmasking
     * any other signals it may be interested in.
     */
    rc = thread_block_async_signals(&saved_sigset);

    if (!rc)
    {
        rc = pthread_create(&(pthread_workitem->thread),
                            &thread_attrs_detached,
                            pstart_routine,
                            pparm);
        pthread_sigmask(SIG_SETMASK, &saved_sigset, NULL);
    }


    if (rc != 0)
    {
        sprintf(msg, "create_thread: THREAD ERROR rc = %d errno = %d (%s)\n",
                rc, errno, strerror(errno));
        pthread_workitem->active = FALSE;
        thread_error( &(pthread_workitem->thread), rc, msg );
    }
    else
    {
        pthread_workitem->active = TRUE;
    }

    return(SUCCESS);

#elif  defined WIN32

    DWORD rc = SUCCESS;
    char msg[MAX_MSG_LEN];

    assert(pstart_routine);
    assert(pparm);
    assert(pthread_workitem);


    if (threaddebug)
    {
        sprintf(msg, "create_thread: creating thread\n");
        print_msg_to_console(msg);
    }

    pthread_workitem->thread =
        (HANDLE) _beginthread( (void *) *pstart_routine,
                               0,   // default stack id
                               pparm); // pointer to parameter list

    if (pthread_workitem->thread == (HTHREAD) -1L)
    {
        pthread_workitem->active = FALSE;
        thread_error( (void *) 0, rc, msg );
    }
    else
    {
        pthread_workitem->active = TRUE;
    }

    if (threaddebug)
    {
        sprintf(msg, "create_thread: thread created rc=%d\n", rc);
        print_msg_to_console(msg);
    }

    return(rc);

#endif

}

/*------------------------------------------------------------------------

 exit_thread

 Output:    return_code           the return code for this thread


 The routine terminates the current thread, and passes the specified return
 code back to the calling function.

------------------------------------------------------------------------*/

void exit_thread( thread_workitem_t *pthread_workitem,  int return_code)
{

    char msg[MAX_MSG_LEN];
    assert(pthread_workitem);

    if (threaddebug)
    {
        sprintf(msg, "exit_thread:       thread=%lx rc=%d\n", printable_tid(thread_ID()), return_code);
        print_msg_to_console(msg);
    }

    if (! MATCH(pthread_workitem->signature, THREAD_WORK_ITEM_SIGNATURE))
    {
        sprintf(msg, "Missing " THREAD_WORK_ITEM_SIGNATURE " in thread_workitem structure\n");
        print_msg_to_console(msg);
        exit(1);
    }

    // we assume that we are no longer going to need the trace buffer
    // in this thread's work item - so we check to make sure it has already been
    // deallocated - it if has num_trace_entries will be 0.
    // memory that was being used in the buffer

    /*  if (pthread_workitem->num_trace_entries != 0) {
        sprintf(msg, "exit_thread: num_trace_entries in table at %p is %d but should be 0\n", pthread_workitem, pthread_workitem->num_trace_entries);
        print_msg_to_console(msg);
        exit(1);
      } */


    // we need to put the return code in a piece of memory that will
    // continue to exist when we exit this routine, so we use the
    // thread_work_item for this

    pthread_workitem->thread_exit_code.value = return_code;
    pthread_workitem->terminated             = TRUE;

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    pthread_exit( &(pthread_workitem->thread_exit_code.value) );

#elif  defined WIN32

    if (threaddebug)
    {
        sprintf(msg, "exit_thread: exiting thread %d\n", printable_tid(thread_ID()));
        print_msg_to_console(msg);
    }

    _endthread();

#endif

}

/*------------------------------------------------------------------------

 create_thread1


 Inputs:    start_routine        the function that is to be run inside the
                                 the new thread

            parm                 pointer to a void, this usually points to a
                                 structure that contains the input parameters
                                 for start_routine

                                 <<<<< must have a thread subfield

 Outputs:



 Return code:           E_OK if the thread was successfully created
                        E_NOTOK if a problem occurred.

 The routine launches the specified function a new thread.  The handle for
 this thread is returned back to the caller in phthread.

------------------------------------------------------------------------*/

int create_thread1( void *(*pstart_routine)(void *), void *pparm)
{

    char msg[MAX_MSG_LEN];
#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    HTHREAD thread;
    int rc;

    // cor lumme, what do we assert here?
    assert(pstart_routine);
    assert(pparm);

    if (threaddebug)
    {
        sprintf(msg, "create_thread:    started\n");
        print_msg_to_console(msg);
    }

    rc = pthread_create(&thread,
                        &thread_attrs_detached,
                        pstart_routine,
                        pparm);

    if (rc != 0)
    {
        sprintf(msg, "create_thread1: THREAD ERROR rc = %d errno = %d (%s)\n",
                rc, errno, strerror(errno));

        thread_error( &thread, rc, msg );
    }

    return(SUCCESS);

#elif  defined WIN32

    DWORD rc = SUCCESS;
    HTHREAD thread;

    assert(pstart_routine);
    assert(pparm);

    if (threaddebug)
    {
        sprintf(msg, "create_thread: creating thread\n");
        print_msg_to_console(msg);
    }

    thread =
        (HANDLE) _beginthread( (void *) *pstart_routine,
                               0,   // default stack id
                               pparm); // pointer to parameter list

    if (thread == (HTHREAD) -1L)
    {
        thread_error( (void *) 0, rc, msg );
    }

    if (threaddebug)
    {
        sprintf(msg, "create_thread: thread created rc=%d\n", rc);
        print_msg_to_console(msg);
    }

    return(rc);

#endif

}

/*------------------------------------------------------------------------

 exit_thread1

 Output:    return_code           the return code for this thread


 The routine terminates the current thread, and passes the specified return
 code back to the calling function.

------------------------------------------------------------------------*/

void exit_thread1( int rc )
{

    char msg[MAX_MSG_LEN];
    static int my_rc;

    my_rc = rc;

    if (threaddebug)
    {
        sprintf(msg, "exit_thread:       thread=%lx rc=%d\n", printable_tid(thread_ID()), rc);
        print_msg_to_console(msg);
    }

    // <<<<<< how do we return the return code????

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    pthread_exit( &my_rc );  // <<<<<<<<< is this kocke - what heepens when rc is deallcated?

#elif  defined WIN32

    if (threaddebug)
    {
        sprintf(msg, "exit_thread: exiting thread %d rc=%d\n", printable_tid(thread_ID()), rc);
        print_msg_to_console(msg);
    }

    // <<<<<< how do we return the return code????

    _endthread();

#endif

}

void check_thread_workitem( thread_workitem_t *pthread_workitem, char *msg )
{

    char msgtext[MAX_MSG_LEN];
    assert(pthread_workitem);
    assert(msg);

    printf("pthread_work item  %p  %s\n", pthread_workitem, msg);

    if (! MATCH(pthread_workitem->signature, THREAD_WORK_ITEM_SIGNATURE))
    {
        sprintf(msgtext, "%s Missing " THREAD_WORK_ITEM_SIGNATURE " in thread_workitem structure at address %p\n",
                msg, pthread_workitem);
        print_msg_to_console(msg);
        exit(1);
    }

    return;
}



/*------------------------------------------------------------------------

sleep_for_microseconds

Inputs:     microsecs            the number of microseconds that this thread
                                 will sleep for.

            pexit_flag           pointer to exit flag, if set then the
                                 sleep should exit as soon as possible

 Outputs:               none

 Return code:           none

 The routine sleeps for the specified number of microseconds.

------------------------------------------------------------------------*/
void sleep_for_microseconds( int microsecs, volatile bool_t *pexit_flag )
{

#if defined HPUX
    // sleep in microsecs...
    int usecs;

    // usleep in HPUX can accept intervals up to 1000000 (ie 1 sec)
    usecs = microsecs;

    while (usecs >= 1000000)
    {
        if ((*pexit_flag))
        {
            return;
        }

        sleep(1);
        usecs = usecs - 1000000;
    }

    if (usecs > 0)
    {
        usleep( usecs);
    }

#elif defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS

    int usecs;
    // split interval into seconds so that it can be CTRl-C'd in the middle of the sleep
    usecs = microsecs;

    while (usecs >= 1000000)
    {
        if ((*pexit_flag))
        {
            return;
        }

        sleep(1);
        usecs = usecs - 1000000;
    }

    if (usecs > 0)
    {
        usleep( usecs);
    }

#elif defined WIN32

    int msecs;

    // sleep in microsecs...

    // split interval into seconds so that it can be CTRl-C'd in the middle of the sleep
    msecs = microsecs / 1000L;

    while (msecs >= 1000)
    {
        if ((*pexit_flag))
        {
            return;
        }

        Sleep( (DWORD) msecs );   // Sleep() takes a millisecond argument
        msecs = msecs - 1000;
    }

    if (msecs > 0)
    {
        Sleep( (DWORD) msecs );
    }

#endif

}

/*------------------------------------------------------------------------

sleep_for_milliseconds

Inputs:     milliseconds         the number of milliseconds that this thread
                                 will sleep for.

            pexit_flag           pointer to exit flag, if set then the
                                 sleep should exit as soon as possible


 Outputs:               none

 Return code:           none

 The routine sleeps for the specified number of milliseconds.

  ------------------------------------------------------------------------*/

void sleep_for_milliseconds( int milliseconds, volatile bool_t *pexit_flag)
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    sleep_for_microseconds( 1000 * milliseconds, pexit_flag );

#elif defined WIN32

    // sleep in milliseconds...

    // split interval into seconds so that it can be CTRl-C'd in the middle of the sleep
    while (milliseconds >= 1000)
    {
        if ((*pexit_flag))
        {
            return;
        }

        Sleep( (DWORD) milliseconds );
        milliseconds = milliseconds - 1000;
    }

    if (milliseconds > 0)
    {
        Sleep( (DWORD) milliseconds );
    }

    Sleep( (DWORD) milliseconds );   // Sleep() takes a millisecond argument

#endif

}

/*------------------------------------------------------------------------

sleep_for_seconds

Inputs:     seconds             the number of seconds that this thread
                                will sleep for.
            pexit_flag          pointer to exit flag, if set then the
                                sleep should exit as soon as possible



 Outputs:               none

 Return code:           none

 The routine sleeps for the specified number of seconds.

------------------------------------------------------------------------*/

void sleep_for_seconds( int seconds, volatile bool_t *pexit_flag)
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    int i;

    // sleep in seconds ...
    // split the sleep up into 1 second intervals, so that we can accept a signal / interrupt
    // and CTRL-C the process within the overall sleep period if we want to
    for (i=0; i<seconds; i++)
    {
        if ((*pexit_flag))
        {
            return;
        }

        sleep( seconds );
    }

#elif defined WIN32

    int i;

    // sleep in seconds ...
    // split the sleep up into 1 second intervals, so that we can accept a signal / interrupt
    // and CTRL-C the process within the overall sleep period if we want to
    for (i=0; i<seconds; i++)
    {
        if ((*pexit_flag))
        {
            return;
        }

        Sleep( (DWORD) seconds * 1000L );     // Sleep() takes a millisecond argument
    }

#endif

}

/*------------------------------------------------------------------------

get_timestamp

 Inputs:                none

 Outputs:               none

 Return code:           returns the value of an elapsed timer - in milliseconds

 The timer will wrap every TIMER_INIT_VALUE seconds on Unix variants

------------------------------------------------------------------------*/

Iu32 get_timestamp( void )
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    struct itimerval time_now;
    Iu32   timer_ms;

    getitimer(ITIMER_REAL, &time_now);

    /*  seconds = time_now.it_value.tv_sec */
    /*  usecs   = time_now.it_value.tv_usec */

    timer_ms = (Iu32) ((TIMER_INIT_VALUE - time_now.it_value.tv_sec) * 1000
                       - (time_now.it_value.tv_usec / 1000) );

    return(timer_ms);

#elif defined WIN32

    LARGE_INTEGER time_count;
    DWORD rc        = 0;
    LPVOID lpMsgBuf;
    char msg[MAX_MSG_LEN];
    Iu32 time_count_in_ms;

    if (! QueryPerformanceCounter(&time_count))
    {
        rc = GetLastError();
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      rc,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR) &lpMsgBuf,
                      0,
                      NULL);

        sprintf(msg,  "ERROR:  QueryPerformanceCounter rc = %d : %s\n", rc,lpMsgBuf );
        print_msg_to_console(msg);

        // Free the buffer.
        LocalFree( lpMsgBuf );
        time_count.QuadPart = 0;
    }

    time_count_in_ms = (Iu32) ( time_count.QuadPart /PerformanceFrequencyCounter_counts_per_ms);
    return (Iu32) time_count_in_ms;

#endif

}

/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        run_system_cmd_in_thread                                  */
/*                                                                        */
/* FUNCTION:    Runs a system command, assuming this routine has been     */
/*              as a new thread that was launched by the caller           */
/*                                                                        */
/* PARAMS IN:   pwrk   pointer to                                         */
/*                                                                        */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     the exit code from the process that tried to run the      */
/*              system command.                                           */
/*------------------------------------------------------------------------*/

void run_system_cmd_in_thread(void *pparameter)
{

    int                   rc;
    int                   signal = 0;
    int                   cmd_rc = 0;
    char                  msg[MAX_MSG_LEN];
    background_cmd_wrk_t  *pbackground_cmd_wrk;
    thread_workitem_t     *pthread_workitem;
    char background_cmd[MAX_SYSTEM_CMD_LEN];

    pbackground_cmd_wrk   = (background_cmd_wrk_t *) pparameter;
    pthread_workitem      = pbackground_cmd_wrk->pthread_workitem;

    if (strlen(pbackground_cmd_wrk->cmd) > 0)
    {
        safecpy(background_cmd, pbackground_cmd_wrk->cmd, sizeof(background_cmd));

        if (! pbackground_cmd_wrk->quiet)
        {
            safecpy(msg, "Running system command: ", sizeof(msg));
            safecat(msg, background_cmd, sizeof(msg));
            safecat(msg, "\n", sizeof(msg));
            print_msg_to_console(msg);
        }

        rc = system(background_cmd);
#if defined AIX || defined LINUX || defined SOLARIS || defined HPUX
        cmd_rc = rc / 256;
        signal = rc & 0xff;
#elif defined WIN32

        if (rc == -1)
        {
            // problem running system command itself, maybe could not find command interpretr
            cmd_rc = errno;
        }
        else
        {
            cmd_rc = rc;
        }

#endif

        if (! pbackground_cmd_wrk->quiet)
        {
            sprintf(msg, "System command: %s, returned rc = 0x%02x, signal = 0x%02x\n",
                    background_cmd, cmd_rc, signal);
            print_msg_to_console(msg);
        }

        //  } else {
        //     sprintf(msg, "System command specified had zero length, so no command was actually run\n"
        //                  "(Note: a system command may be optionally run when a miscompare or i/o erro occurs or run as part of a Stanza\n"
        //                  "This message appears when a system command has beeb specified by the system command is the empty string)\n");
        //     print_msg_to_console(msg);
    }

    exit_thread( pthread_workitem, cmd_rc );

}


/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        run_system_cmd_in_background                              */
/*                                                                        */
/* FUNCTION:    Runs a system command in a new process in the background  */
/*              and return immediately                                    */
/*                                                                        */
/* PARAMS IN:   cmd   string containing the command to be executed        */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     0 for successful launch of commandin background, 1 for    */
/*              failure to launch command                                 */
/*------------------------------------------------------------------------*/

/* int run_system_cmd_in_background(char *cmd) {

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    // <<<<<<<<<< oh dear, this code does not look very good.... should use fork() I think
  int rc;
  int signal;
  int cmd_rc = 0;
  char msg[MAX_MSG_LEN];
  char background_cmd[MAX_SYSTEM_CMD_LEN];

  assert(cmd);

  if (strlen(cmd) > 0) {
     safecpy(background_cmd, cmd, sizeof(background_cmd));
     safecat(background_cmd, " >/dev/null 2>&1 &", sizeof(background_cmd));
     safecpy(msg, "Running system command in background: ", sizeof(msg));
     safecat(msg, background_cmd, sizeof(msg));
     safecat(msg, "\n", sizeof(msg));
     print_msg_to_console(msg);
     rc = system(background_cmd);
     cmd_rc = rc / 256;
     signal = rc & 0xff;
     sprintf(msg, "System command: %s, returned rc = 0x%02x, signal = 0x%02x\n", cmd, cmd_rc, signal);
     print_msg_to_console(msg);
  }

  return cmd_rc;

#elif defined WIN32

  char msg[MAX_MSG_LEN];
  thread_workitem_t    background_cmd_thread_workitem;  // <<<<<<<< ONLY WORKS WITHI ONE THEAD AT A TIME
  background_cmd_wrk_t background_cmd_wrk;

  assert(cmd);

  initialise_thread_workitem(&background_cmd_thread_workitem, 0, FALSE);

  if (strlen(cmd) > 0) {

     // we are going to start a new thread and then run the system() command in that thread

     safecpy(background_cmd_wrk.cmd, cmd, sizeof(background_cmd_wrk.cmd));
     background_cmd_wrk.quiet = 0;
     background_cmd_wrk.pthread_workitem = &background_cmd_thread_workitem;

     if (create_thread((void *(*)(void *)) &run_system_cmd_in_thread,
                       (void *) &background_cmd_wrk,
                       &background_cmd_thread_workitem ) != 0) {
        sprintf(msg, "threads.c: could not start background cmd thread %d (%s)\n", errno, strerror(errno));
        print_msg_to_console(msg);
        exit(1);
     }
  }

  return 0;

#endif


}  */


/*------------------------------------------------------------------------*/
/*                                                                        */
/* NAME:        get_process_pid                                           */
/*                                                                        */
/* FUNCTION:    Returns the process id of the current process             */
/*                                                                        */
/* PARAMS IN:   none                                                      */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     the process id of the current process                     */
/*                                                                        */
/*------------------------------------------------------------------------*/

/* PID get_process_pid() {

#if defined AIX || defined LINUX || defined SOLARIS || defined HPUX
   PID  pid;
   if ((pid = getpid()) < 0) {
      perror("unable to get pid");
   }
   return pid;
#else
   return _getpid();
#endif

}  */

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           thread_block_async_signals                          */
/*                                                                      */
/*  FUNCTION:       Blocks asynchronous signals for the calling thread  */
/*                                                                      */
/*  PARAMS IN       none                                                */
/*  PARAMS OUT:     none                                                */
/*      saved_set      The current signal mask will be saved to this    */
/*                     sigset_t if it's not NULL.                       */
/*  RETURNS:        void                                                */
/*                                                                      */
/*----------------------------------------------------------------------*/

/* Only useful on POSIX threads OSs */
#if defined AIX || defined LINUX || defined SOLARIS || defined HPUX
int thread_block_async_signals(sigset_t *saved_set)
{
    sigset_t sigset;
    int rc;

    /* Start with an all encompassing mask, then unblock a few signals
     * where we'd rather the thread get the signal delivered (e.g. SIGSEGV). */
    sigfillset(&sigset);
    sigdelset(&sigset, SIGSEGV);
    sigdelset(&sigset, SIGILL);
    sigdelset(&sigset, SIGFPE);
    sigdelset(&sigset, SIGBUS);

    rc = pthread_sigmask(SIG_BLOCK, &sigset, saved_set);

    if (rc)
    {
        print_msg_to_console("thread_block_async_signals: failed to block signals for thread\n");
    }

    return rc;
}

/***************************************************************************

 add_item_to_thread_trace_table


 Inputs:     pthread_workitem          pointer to thread workitem structure
                                       that contains the trace table that is
                                       being updated
             tracepoint                tracepoint to be entered
             type

 Outputs:    none


 Return code:           E_OK if the new trace table entry was successfully created
                        E_NOTOK if a problem occurred.



 **************************************************************************/
int add_item_to_trace_table( thread_workitem_t *pthread_workitem,
                             Iu32 tracepoint,
                             Iu32 tracetype,
                             Iu32 p1,
                             Iu32 p2,
                             Iu32 p3,
                             Iu32 rc)
{

    int return_rc = 0;
    thread_trace_element_t *pthread_trace_table_element;

    assert(pthread_workitem);

    // do not update the table if the table has 0 elements (that means that
    // we did not request a trace table when we initialised the thread).

    if (pthread_workitem->num_trace_entries != 0)
    {

        pthread_trace_table_element =
            &(pthread_workitem->ptrace_table[pthread_workitem->trace_table_index].thread_trace_element);

        assert(pthread_trace_table_element);

        pthread_trace_table_element->timestamp  = get_timestamp();
        pthread_trace_table_element->tracepoint = tracepoint;
        pthread_trace_table_element->type       = tracetype;
        pthread_trace_table_element->p1         = p1;
        pthread_trace_table_element->p2         = p2;
        pthread_trace_table_element->p3         = p3;
        pthread_trace_table_element->rc         = rc;

        //printf("wrote index %d timestamp 0x%08x tracepoint 0x%08x type 0x%08x p1,2,3 0x%08x 0x%08x 0x%08x\n",
        //       pthread_workitem->trace_table_index, pthread_trace_table_element->timestamp, tracepoint, type, p1,p2,p3);

        pthread_workitem->trace_table_index++;

        if (pthread_workitem->trace_table_index >= pthread_workitem->num_trace_entries)
        {
            pthread_workitem->trace_table_index = 0;
        }

        //printf("index %d  num_trace_entries %d\n", pthread_workitem->trace_table_index,
        //       pthread_workitem->num_trace_entries);

    }

    return return_rc;

}

/************************************************************************/
/*                                                                      */
/* NAME:        print_trace_buffer                                      */
/*                                                                      */
/* FUNCTION:    Prints a decoded version of the thread trace buffer     */
/*                                                                      */
/* PARAMS IN:   pthread_workitem  thread workitem structure             */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     void                                              .     */
/*                                                                      */
/*  tracepoint            type      p1        p2      p3       p4       */
/*  NOT_USED,                                                           */
/*  PROC_SEG_LOCK,                                                      */
/*  LOCK_MUTEX,           LOCK      *pmutex                    rc       */
/*  UNLOCK_MUTEX,         UNLOCK    *pmutex                    rc       */
/*  PROC_SEG_UNLOCK,                                                    */
/*  PROC_COND_WAIT1,                                                    */
/*  WAIT_BEFORE,          WAIT_AFTER,                                   */
/*  WAIT_AFTER,                                                         */
/*  PROC_COND_BROADCAST1, BROADCAST,                                    */
/*  PROC_LOCK_MUTEX,      LOCK,                                         */
/*  PROC_UNLOCK_MUTEX,    UNLOCK,                                       */
/*  BUFFER_TIMESTAMP,     BUFFER_CREATE                                 */
/*                                                                      */
/************************************************************************/
void print_trace_buffer( thread_workitem_t *pthread_workitem)
{

    int i,j;
    Iu32 tracepoint;
    Iu32 tracetype;
    Iu32 timestamp;
    char *tracepoint_name;
    char *tracetype_name;
    char not_found[MAX_TRACE_NAME_LENGTH];
    char msg[MAX_MSG_LEN];
    thread_trace_element_t *ptrace_table;
    int num_entries_in_table;

    num_entries_in_table = pthread_workitem->num_trace_entries;

    for (i=0; i< num_entries_in_table; i++)
    {

        ptrace_table = ((thread_trace_element_t *)  pthread_workitem->ptrace_table) + i;
        tracepoint   = ptrace_table->tracepoint;
        tracetype    = ptrace_table->type;
        timestamp    = ptrace_table->timestamp,
        safecpy(not_found, "not found", MAX_TRACE_NAME_LENGTH);

        tracepoint_name = not_found;

        for (j = 0; j < MAX_TRACE_NUM; j++)
        {
            if (tracepoint == trace_lookup_table[j].trace_value)
            {
                tracepoint_name = trace_lookup_table[j].trace_name;
                break;
            }
        }

        tracetype_name = not_found;

        for (j = 0; j < MAX_TRACE_NUM; j++)
        {
            if (tracetype == trace_lookup_table[j].trace_value)
            {
                tracetype_name = trace_lookup_table[j].trace_name;
                break;
            }
        }

        if (timestamp != 0)
        {

            sprintf(msg, "%08x %20s %20s %08x %08x %08x %08x\n",
                    timestamp,
                    tracepoint_name,
                    tracetype_name,
                    ptrace_table->p1,
                    ptrace_table->p2,
                    ptrace_table->p3,
                    ptrace_table->rc);
            print_string_to_console(msg);

        }

    }

    print_string_to_console("\n");

}


#endif
