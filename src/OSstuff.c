

#include "daf_util.h"
#include "daf_threads.h"
#include "stdio.h"

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif


#define MAX_MP 4096

int mp  = 0;
MUTEX   *trace_malloc_mutex = NULL;
void    *pa[MAX_MP];
size_t  ms[MAX_MP];
TID     tids[MAX_MP];

int bBigEndian = 0;

/************************************************************************/
/*                                                                      */
/* NAME:        setup_OSbase                                            */
/*                                                                      */
/* FUNCTION:    initialises OS specific environment                     */
/*                                                                      */
/* PARAMS IN:   none                                                    */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED:   bBigEndian     set to TRUE if this machine is     */
/*                                   big endian                         */
/*                                                                      */
/* RETURNS:           none                                              */
/*                                                                      */
/* DESCRIPTION  Initialises global variable to indicate if this         */
/*              a big endian or little endian machine.                  */
/*                                                                      */
/*                                                                      */
/************************************************************************/

extern void setup_OSbase(void)
{

    long int i = 1;
    const char *p = (const char *) &i;

    //  #if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
    //    run_system_cmd_and_print_single_line_output("uname -a", msg, MAX_MSG_LEN);
    //  #endif

    if (p[0] == 1)    // lowest address contains the least significant byte
    {
        bBigEndian = 0;
    }
    else
    {
        bBigEndian = 1;
    }
}


/************************************************************************/
/*                                                                      */
/* NAME:        convert_Iu16_to_bigendian                               */
/*                                                                      */
/* FUNCTION:                                                            */
/*                                                                      */
/* PARAMS IN:   value          a 2 byte value                           */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/*                                                                      */
/* RETURNS:     a 16 bit big endian representation of value             */
/*                                                                      */
/* DESCRIPTION  Converts (if necessary) the input value to a big        */
/*              endian representation.  If the host machine is a        */
/*              big endian architecture - then no byte swapping is      */
/*              necessary - if it is little endian then the two bytes   */
/*              in value are exchanged                                  */
/*                                                                      */
/*                                                                      */
/*              input value     host machine type      output value     */
/*                                                                      */
/*                0x3412        littlendian            0x1234           */
/*                0x1234        bigendian              0x1234           */
/*                                                                      */
/*                                                                      */
/************************************************************************/

Iu16 convert_Iu16_to_bigendian(Iu16 value)
{

    if ( bBigEndian)
    {
        return(value);
    }
    else
    {
        return( ((value & 0xff00) >> 8) |
                ((value & 0x00ff) << 8));
    }

}

/************************************************************************/
/*                                                                      */
/* NAME:        convert_Iu32_to_bigendian                               */
/*                                                                      */
/* FUNCTION:                                                            */
/*                                                                      */
/* PARAMS IN:   value          a 4 byte value                           */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/*                                                                      */
/* RETURNS:     a 32 bit big endian representation of value             */
/*                                                                      */
/* DESCRIPTION  Converts (if necessary) the input value to a big        */
/*              endian representation.  If the host machine is a        */
/*              big endian architecture - then no byte swapping is      */
/*              necessary - if it is little endian then the two bytes   */
/*              in value are exchanged                                  */
/*                                                                      */
/*                                                                      */
/*              input value     host machine type      output value     */
/*                                                                      */
/*                0x78563412    littlendian            0x12345678       */
/*                0x12345678    bigendian              0x12345678       */
/*                                                                      */
/*                                                                      */
/************************************************************************/

Iu32 convert_Iu32_to_bigendian(Iu32 value)
{

    if ( bBigEndian)
    {
        return(value);
    }
    else
    {
        return( ((value & 0xff000000) >> 24) |
                ((value & 0x000000ff) << 24) |
                ((value & 0x00ff0000) >>  8) |
                ((value & 0x0000ff00) <<  8) );
    }
}



/************************************************************************/
/*                                                                      */
/* NAME:        convert_Iu64_to_bigendian                               */
/*                                                                      */
/* FUNCTION:                                                            */
/*                                                                      */
/* PARAMS IN:   value          a 8 byte value                           */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/*                                                                      */
/* RETURNS:     a 64 bit big endian representation of value             */
/*                                                                      */
/* DESCRIPTION  Converts (if necessary) the input value to a big        */
/*              endian representation.  If the host machine is a        */
/*              big endian architecture - then no byte swapping is      */
/*              necessary - if it is little endian then the two bytes   */
/*              in value are exchanged                                  */
/*                                                                      */
/*                                                                      */
/*              input value     host machine type      output value     */
/*                                                                      */
/*       0x0123456789abcdef   littlendian         0xefcdab8967452301    */
/*       0x0123456789abcdef   bigendian           0x0123456789abcdef    */
/*                                                                      */
/*                                                                      */
/************************************************************************/

Iu64 convert_Iu64_to_bigendian(Iu64 value)
{

    if ( bBigEndian)
    {
        return(value);
    }
    else
    {
        return( ((value & 0xff00000000000000ULL) >> 56) |
                ((value & 0x00ff000000000000ULL) >> 40) |
                ((value & 0x0000ff0000000000ULL) >> 24) |
                ((value & 0x000000ff00000000ULL) >>  8) |
                ((value & 0x00000000ff000000ULL) <<  8) |
                ((value & 0x0000000000ff0000ULL) << 24) |
                ((value & 0x000000000000ff00ULL) << 40) |
                ((value & 0x00000000000000ffULL) << 56));
    }
}
/************************************************************************/
/*                                                                      */
/* NAME:        am_I_root()                                             */
/*                                                                      */
/* FUNCTION:                                                            */
/*                                                                      */
/* PARAMS IN:   none                                                    */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/*                                                                      */
/* RETURNS:     TRUE if the user is root, always true on Windows        */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/************************************************************************/

BOOL am_I_root(void)
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    if (getuid() == 0)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }

#elif defined WIN32 || defined NETWARE
    return (TRUE);
#else
#error Cannot compile, unknown target OS - define AIX, LINUX, LINUX_PPC64, SOLARIS, HPUX, WIN32 or NT
#endif

}


/************************************************************************/
/*                                                                      */
/* NAME:        get_my_pid()                                            */
/*                                                                      */
/* FUNCTION:                                                            */
/*                                                                      */
/* PARAMS IN:   none                                                    */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/*                                                                      */
/* RETURNS:     a 32 bit PID                                            */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/************************************************************************/

Iu32 get_my_pid(void)
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
    return (Iu32) getpid();
#elif defined WIN32 || defined NETWARE
    return (Iu32) _getpid();
#else
#error Cannot compile, unknown target OS - define AIX, LINUX, LINUX_PPC64, SOLARIS, HPUX, WIN32 or NT
#endif

}

/************************************************************************/
/*                                                                      */
/* NAME:        comment_on_OSbase                                       */
/*                                                                      */
/* FUNCTION:    comments on OS specific environment                     */
/*                                                                      */
/* PARAMS IN:   none                                                    */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED:   none                                              */
/*                                                                      */
/* RETURNS:           none                                              */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/*                                                                      */
/************************************************************************/

void comment_on_OSbase(BOOL quiet)
{

    char msg[MAX_MSG_LEN];

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
    /*   if (! quiet) {
         run_system_cmd_and_print_single_line_output("uname -a", msg, MAX_MSG_LEN);
       } */
#endif


    if (! quiet)
    {
        sprintf(msg, "Size of Iu8        is %lu\n", (unsigned long)sizeof(Iu8));
        print_msg_to_console(msg);
    }

    if (sizeof(Iu8) != 1)
    {
        print_msg_to_console("Size of Iu8 should be 1 - exiting\n");
        exit(1);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of Iu16       is %lu\n", (unsigned long)sizeof(Iu16));
        print_msg_to_console(msg);
    }

    if (sizeof(Iu16) != 2)
    {
        print_msg_to_console("Size of Iu16 should be 2 - exiting\n");
        exit(1);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of Iu32       is %lu\n", (unsigned long)sizeof(Iu32));
        print_msg_to_console(msg);
    }

    if (sizeof(Iu32) != 4)
    {
        print_msg_to_console("Size of Iu32 should be 4 - exiting\n");
        exit(1);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of Iu64       is %lu\n", (unsigned long)sizeof(Iu64));
        print_msg_to_console(msg);
    }

    if (sizeof(Iu64) != 8)
    {
        print_msg_to_console("Size of Iu64 should be 8 - exiting\n");
        exit(1);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of short      is %lu\n", (unsigned long)sizeof(short));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of int        is %lu\n", (unsigned long)sizeof(int));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of long       is %lu\n", (unsigned long)sizeof(long));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of long long  is %lu\n", (unsigned long)sizeof(long long));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of off_t      is %lu\n", (unsigned long)sizeof(off_t));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of size_t     is %lu\n", (unsigned long)sizeof(size_t));
        print_msg_to_console(msg);
    }

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    if (! quiet)
    {
        sprintf(msg, "Size of pthread_t  is %lu\n", (unsigned long)sizeof(pthread_t));
        print_msg_to_console(msg);
    }

#endif

    if (! quiet)
    {
        sprintf(msg, "Size of TID        is %lu\n", (unsigned long)sizeof(TID));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of PID        is %lu\n", (unsigned long)sizeof(PID));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of (Iu8 *)    is %lu\n", (unsigned long)sizeof(Iu8 *));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of (Iu32 *)   is %lu\n", (unsigned long)sizeof(Iu32 *));
        print_msg_to_console(msg);
    }

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    if (! quiet)
    {
        sprintf(msg, "Size of dev_t      is %lu\n", (unsigned long)sizeof(dev_t));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of ino_t      is %lu\n", (unsigned long)sizeof(ino_t));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of mode_t     is %lu\n", (unsigned long)sizeof(mode_t));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of nlink_t    is %lu\n", (unsigned long)sizeof(nlink_t));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of uid_t      is %lu\n", (unsigned long)sizeof(uid_t));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of gid_t      is %lu\n", (unsigned long)sizeof(gid_t));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of dev_t      is %lu\n", (unsigned long)sizeof(dev_t));
        print_msg_to_console(msg);
    }

#if defined LINUX_PPC64  || defined HPUX
#else

    if (! quiet)
    {
        sprintf(msg, "Size of blksize_t  is %lu\n", (unsigned long)sizeof(blksize_t));
        print_msg_to_console(msg);
    }

#endif

    if (! quiet)
    {
        sprintf(msg, "Size of blkcnt_t   is %lu\n", (unsigned long)sizeof(blkcnt_t));
        print_msg_to_console(msg);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of time_t     is %lu\n", (unsigned long)sizeof(time_t));
        print_msg_to_console(msg);
    }

#endif

#if defined WIN32 || defined NETWARE
#else

    if (sizeof(off_t) != 8)
    {
        print_msg_to_console("Size of off_t should be 8 on WIN32 onr NETWARE - exiting\n");
        exit(1);
    }

    if (! quiet)
    {
        sprintf(msg, "Size of timer_t    is %lu\n", (unsigned long)sizeof(timer_t));
        print_msg_to_console(msg);
    }

    //   if (sizeof(timer_t) != 4) {
    //     print_error_msg_to_console(20000, "SYSTEM_INTERNAL", "Size of timer should be 4 - exiting\n");
    //     exit(1);
    //   }
#endif

}



#if defined WIN32 || defined NETWARE

/************************************************************************/
/*                                                                      */
/* NAME:        convert_error_num_to_text                               */
/*                                                                      */
/* FUNCTION:                                                            */
/*                                                                      */
/* PARAMS IN:    pMsg           pointer to string that is to receive    */
/*                              the error message                       */
/*               maxlen                                                 */
/*               errorno        usually GetLastError()                  */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/*                                                                      */
/* RETURNS:           E_OK      if  ok                                  */
/*                    E_NOTOK   if a problem occurred and the           */
/*                              test should be terminated               */
/*                                                                      */
/* DESCRIPTION    concatenates error msg to pMsg but does not add a \n  */
/*                to the end of the string                              */
/*                                                                      */
/************************************************************************/

int convert_error_num_to_text(int errorno, char *pMsg, int maxlen)
{

    int rc = 0;

    LPVOID lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
                  NULL,
                  errorno,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  (LPTSTR) &lpMsgBuf,
                  0,
                  NULL);

    my_strncat( pMsg, lpMsgBuf, maxlen);
    LocalFree(lpMsgBuf);

    return(rc);
}

/************************************************************************/
/*                                                                      */
/* NAME:        format_error_num_into_error_message                     */
/*                                                                      */
/* FUNCTION:                                                            */
/*                                                                      */
/* PARAMS IN:    pMsg           pointer to string that is to receive    */
/*                              the error message                       */
/*               maxlen                                                 */
/*               errorno        usually GetLastError()                  */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/*                                                                      */
/* RETURNS:           E_OK      if  ok                                  */
/*                    E_NOTOK   if a problem occurred and the           */
/*                              test should be terminated               */
/*                                                                      */
/* DESCRIPTION    concatenates error msg to pMsg and adds a \n          */
/*                to the end of the string ready for printing           */
/*                                                                      */
/************************************************************************/

int format_error_num_into_error_message(int errorno, char *pMsg, int maxlen)
{

    int rc = 0;

    rc = convert_error_num_to_text(errorno, pMsg, maxlen);
    my_strncat(pMsg, "\n", maxlen);

    return(rc);
}


#endif
