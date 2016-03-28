/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf automation agent                                                                                                 */
/*                                                                                                                      */
/* OSstuff.h                                                                                                            */
/*                                                                                                                      */
/* This file is part of the daf automation framework.                                                                   */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#ifndef _OSSTUFF_H
#define _OSSTUFF_H

#if defined AIX || defined LINUX || defined SOLARIS || defined HPUX

#include <pthread.h>
#include "daf_protocol.h"

  typedef unsigned long       TID;
  typedef int                 HANDLE;
  typedef pthread_cond_t      COND;
  typedef pthread_mutex_t     MUTEX;
  typedef int                 DWORD;
  typedef pthread_t           HTHREAD;
  typedef int                 socket_t;
  typedef struct itimerval    time_control_t;
  typedef pid_t               PID;

   #define E_FAIL 1

#elif defined WIN32

  typedef HANDLE              MUTEX;
  typedef HANDLE              COND;
  typedef unsigned long       TID;
  typedef HANDLE              HTHREAD;
  typedef __int64             Iu64;
  typedef int                 PID;
  typedef SOCKET              socket_t;
  typedef LARGE_INTEGER       time_control_t;

  #define strtoull            _strtoi64
  #define STATFS              statfs

#endif

/* define format specs for use when printing 64 bit integers */

#define IU64_FSPEC         "%llu"
#define IU64_12FSPEC       "%12llu" 

#define THREADIDSPEC  "%lu"
#define PROCESSID_FSPEC "%d"
#define PROCESSID_8FSPEC "%8d"

#define SIZE_TSPEC "%d"


void setup_OSbase(void);
Iu16 convert_Iu16_to_bigendian(Iu16 value);
Iu32 convert_Iu32_to_bigendian(Iu32 value);
Iu64 convert_Iu64_to_bigendian(Iu64 value);
BOOL am_I_root(void);
Iu32 get_my_pid(void);
void comment_on_OSbase(BOOL quiet);
void setup_mp(void);
void wrapup_mp(void);
void print_malloc_table(void);
void *my_malloc(size_t numbytes, char *caller);
void my_free(void *p, char *caller);


#if defined WIN32
int convert_error_num_to_text(int errorno, char *pMsg, int maxlen);
int format_error_num_into_error_message(int errorno, char *pMsg, int maxlen) {
#endif

#endif

