/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* Globals                                                                                                */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mysql.h>

#include "daf_protocol.h"
#include "daf_shared_memory.h"
#include "daf_util.h"

#define MAX_MSG_LEN         512  /* <<<<<<<< need to define in a common file */
#define E_OK                  0
#define E_NOTOK               1

char console_log_file_name[128] = "";
int  is_console_log_file_open   = FALSE;
FILE *console_log_fptr          = NULL;
static char saved_console_log_filename[MAX_PATHNAME_LEN] = "/tmp/dafconsole.log";

//static const struct cmd_log_state_entry cmd_log_state_array[] =
//{
//    { CMD_FREE,        "CMD_FREE"},
//    { CMD_INITIALISED, "CMD_INIT"},
//    { CMD_RUNNING,     "CMD_RUNNING"},
//    { CMD_BG_RUNNING,  "CMD_BG_RUNNING"},
//    { CMD_COMPLETED,   "CMD_COMPLETED"}
//};

struct remote_client_request_entry
{
    int          request_num;
    const char *request_name;
};

//static const struct remote_client_request_entry remote_client_request_array[] =
//{
//
//    {CLIENT_REMOTE_CLIENT_CNTRL, "CLIENT_REMOTE_CLIENT_CNTRL"},
//    {CLIENT_REMOTE_CLIENT_CMD,   "CLIENT_REMOTE_CLIENT_CMD"},
//    {CLIENT_REMOTE_CLIENT_PREPARE_CMD,   "CLIENT_REMOTE_CLIENT_PREPARE_CMD"},
//    {CLIENT_REMOTE_CLIENT_EXECUTE_CMD,   "CLIENT_REMOTE_CLIENT_EXECUTE_CMD"},
//    {CLIENT_REMOTE_CLIENT_QUERY_VERSION, "CLIENT_REMOTE_CLIENT_QUERY_VERSION"},
//    {CLIENT_REMOTE_CLIENT_QUERY_TAG, "CLIENT_REMOTE_CLIENT_QUERY_TAG"},
//    {CLIENT_REMOTE_CLIENT_QUERY_ALLTAGS, "CLIENT_REMOTE_CLIENT_QUERY_ALLTAGS"},
//    {CLIENT_REMOTE_CLIENT_QUERY_CMDLOG, "CLIENT_REMOTE_CLIENT_QUERY_CMDLOG"},
//    {CLIENT_REMOTE_CLIENT_QUERY_IDENT, "CLIENT_REMOTE_CLIENT_QUERY_IDENT"},
//    {CLIENT_REMOTE_CLIENT_QUERY_CMDS, "CLIENT_REMOTE_CLIENT_QUERY_CMDS"},
//    {CLIENT_REMOTE_CLIENT_CLEAR_TAG, "CLIENT_REMOTE_CLIENT_CLEAR_TAG"},
//    {CLIENT_REMOTE_CLIENT_CLEAR_IDENT, "CLIENT_REMOTE_CLIENT_CLEAR_IDENT"},
//    {CLIENT_REMOTE_CLIENT_CLEAR_ALLTAGS, "CLIENT_REMOTE_CLIENT_CLEAR_ALLTAGS"},
//    {CLIENT_REMOTE_CLIENT_CANCEL_TAG, "CLIENT_REMOTE_CLIENT_CANCEL_TAG",},
//    {CLIENT_REMOTE_CLIENT_CANCEL_TAG, "CLIENT_REMOTE_CLIENT_CANCEL_IDENT",},
//    {CLIENT_REMOTE_CLIENT_COPYFILE, "CLIENT_REMOTE_CLIENT_COPYFILE",},
//};


struct clnt_stat_entry
{
    enum clnt_stat clnt_stat_num;
    const char *clnt_stat_name;
};

static const struct clnt_stat_entry clnt_stat_array[] =
{
    { RPC_SUCCESS,          "RPC_SUCCESS - call succeeded"},
    { RPC_CANTENCODEARGS,   "RPC_CANTENCODEARGS - can't encode arguments"},
    { RPC_CANTDECODERES,    "RPC_CANTDECODERES - can't decode results"},
    { RPC_CANTSEND,         "RPC_CANTSEND - failure in sending call"},
    { RPC_CANTRECV,         "RPC_CANTRECV - failure in receiving result"},
    { RPC_TIMEDOUT,         "RPC_TIMEOUT -  call timed out"},
    { RPC_VERSMISMATCH,     "RPC_VERSMISMATCH - rpc versions not compatible"},
    { RPC_AUTHERROR,        "RPC_AUTHERROR - authentication error"},
    { RPC_PROGUNAVAIL,      "RPC_PROGUNAVAIL - program not available"},
    { RPC_PROGVERSMISMATCH, "RPC_PROGVERSMISMATCH - program version mismatched"},
    { RPC_PROCUNAVAIL,      "RPCPROCUNAVAIL - procedure unavailable"},
    { RPC_CANTDECODEARGS,   "RPC_CANTDECODEARGS - decode arguments error"},
    { RPC_SYSTEMERROR,      "RPC_SYSTEMERROR - generic \"other problem\""},
    { RPC_NOBROADCAST,      "RPC_NOBROADCAST - Broadcasting not supported"},
    { RPC_UNKNOWNHOST,      "RPC_UNKNOWNHOST - unknown host name"},
    { RPC_UNKNOWNPROTO,     "RPC_UNKNOWNPROTO - unknown protocol"},
    { RPC_UNKNOWNADDR,      "RPC_UNKNOWNADDR - Remote address unknown"},
    { RPC_RPCBFAILURE,      "RPC_RPCBFAILTURE - portmapper failed in its call"},
    { RPC_PROGNOTREGISTERED,"RPC_PROGNOTREGISTERED - remote program is not registered"},
    { RPC_N2AXLATEFAILURE,  "RPC_N2AXLATEFAIURE - Name to addr translation failed"},
    { RPC_FAILED,           "RPC_FAILED"},
    { RPC_INTR,             "RPC_INTR"},
    { RPC_TLIERROR,         "RPC_TLIERROR"},
    { RPC_UDERROR,          "RPC_UDERROR"},
    { RPC_INPROGRESS,       "RPC_INPROGRESS"},
    { RPC_STALERACHANDLE,   "RPC_STALERACHANDLE"}
};




/*--------------------------------------------------------------------------------------------------------*/
/*  Globals                                                                                               */
/*--------------------------------------------------------------------------------------------------------*/

cmd_log_object_t        *p_service_cmd_log_object;
active_steps_object_t   *p_service_active_steps_object;
shm_daf_t               *p_shared_memory_base_pointer;

/*--------------------------------------------------------------------------------------------------------*/
/*  External Globals (in daf.c)                                                                           */
/*--------------------------------------------------------------------------------------------------------*/

//extern int  daf_major_version;
//extern int  daf_minor_version;
//extern char *daf_version_string;

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void clnt_stat_decode(enum clnt_stat clnt_stat, char *msg, int msg_size)
{
    int i;
    msg[0] = 0;

    for (i=0; i < (sizeof(clnt_stat_array)/sizeof(struct clnt_stat_entry)); i++)
    {
        if (clnt_stat == clnt_stat_array[i].clnt_stat_num)
        {
            safecpy(msg, (char *) clnt_stat_array[i].clnt_stat_name, msg_size);
            break;
        }
    }
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

Iu8 *duplicate_bytes(unsigned char *source, int length)
{

    Iu8 *target = NULL;

    if (source != NULL)
    {
        if ((target = (Iu8 *) malloc(length)) != NULL)
        {
            memcpy(target, source, length);
        }
    }

    return(target);
}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

char *duplicate_string(char *source)
{

    int len = strlen(source);
    char *target = NULL;

    if (source != NULL)
    {
        if ((target = (char *) malloc(len+1)) != NULL)             /* how does this malloc get freed  ?  memory leadk ??  see similar below <<<<<<<<< */
        {
            safecpy(target, source, len+1);                      /* <<<<<<<<<<<<<<<<<<<<< */
        }
    }

    return(target);
}


/************************************************************************/
/*                                                                      */
/* NAME:        convert_epoch_time_to_string                            */
/*                                                                      */
/* FUNCTION:    converts an epoch time in seconds to a printable        */
/*              string                                                  */
/*                                                                      */
/* PARAMS IN:   epoch_time       epoch time in seconds since Jan 1 1970 */
/*              time_str         pointer to string to hold result       */
/*              time_str_length  max length of string                   */
/*                                                                      */
/* PARAMS OUT:  time_str         string representing epoch_time         */
/*                                                                      */
/* GLOBALS CHANGED: none.                                               */
/*                                                                      */
/* RETURNS:     time_str                                          .     */
/*                                                                      */
/*----------------------------------------------------------------------*/

char *convert_epoch_time_to_string(time_t epoch_time, char *time_str, int time_str_length)
{
    struct tm timestruct;
    struct tm *ptm = &timestruct;

    localtime_r(&epoch_time, ptm);
    strftime( time_str, time_str_length, "%c %Z", ptm);
    return(time_str);

}


/**************************************************************************/
/*                                                                        */
/* NAME:        timeval_sum                                               */
/*                                                                        */
/* FUNCTION:    Computes the sum of two timeval interval timers           */
/*                                                                        */
/* PARAMS IN:   start    value of the real time interval                  */
/*                        timer at the start of the period to be measured */
/* PARAMS IN:   end      value of the real time interval                  */
/*                        timer at the end of the period to be measured   */
/*                                                                        */
/* PARAMS OUT:  sum                                                       */
/*                                                                        */
/* RETURNS:     void                                                      */
/*                                                                        */
/* FUNCTION:    Compute start + end and return sum.                       */
/*                                                                        */
/**************************************************************************/

void timeval_sum (struct timeval start,
                  struct timeval end,
                  struct timeval *sum)
{

    sum->tv_sec  = start.tv_sec  + end.tv_sec;
    sum->tv_usec = start.tv_usec + end.tv_usec;

    if (sum->tv_usec >= 1000000)
    {
        sum->tv_sec++;
        sum->tv_usec -= 1000000;
    }

    return;
}

/**************************************************************************/
/*                                                                        */
/* NAME:        timeval_diff                                              */
/*                                                                        */
/* FUNCTION:    Computes the time difference between two timeval          */
/*              interval timers                                           */
/*                                                                        */
/* PARAMS IN:   start    value of the real time interval                  */
/*                        timer at the start of the period to be measured */
/* PARAMS IN:   end      value of the real time interval                  */
/*                        timer at the end of the period to be measured   */
/*                                                                        */
/* PARAMS OUT:  difference                                                */
/*                                                                        */
/* RETURNS:     1 if end < start, 0 if not                                */
/*                                                                        */
/* FUNCTION:    Compute end - start and return result.                    */
/*                                                                        */
/**************************************************************************/

int timeval_diff(struct timeval start,
                 struct timeval end,
                 struct timeval *difference)
{

    /* Perform the carry for the later subtraction by updating start. */
    if (end.tv_usec < start.tv_usec)
    {
        int nsec = (start.tv_usec - end.tv_usec) / 1000000 + 1;
        start.tv_usec -= 1000000 * nsec;
        start.tv_sec += nsec;
    }

    if (end.tv_usec - start.tv_usec > 1000000)
    {
        int nsec = (end.tv_usec - start.tv_usec) / 1000000;
        end.tv_usec += 1000000 * nsec;
        end.tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.  tv_usec is certainly positive. */
    difference->tv_sec  = end.tv_sec - start.tv_sec;
    difference->tv_usec = end.tv_usec - start.tv_usec;

    /* Return 1 if result is negative. */
    return difference->tv_sec < difference->tv_sec;
}


/*------------------------------------------------------------------------*/
/* get_current_time_as_timestamp                                          */
/*                                                                        */
/* Input:                                                                 */
/*                                                                        */
/*  timstamp              pointer to string where timestamp is to         */
/*                        be stored                                       */
/*  max_time_stamplength  max length of timestamp string                  */
/*                                                                        */
/* Output:                                                                */
/*                                                                        */
/*  timstamp           pointer to string where timestamp is to be stored  */
/*                                                                        */
/* RETURNS:     time_stamp                                          .     */
/*                                                                        */
/* Description:                                                           */
/*                                                                        */
/* Formats the current time as a time stamp that is suitable for use in   */
/* filenames etc as a timestamp                                           */
/*                                                                        */
/* This time stamp is in the format:                                      */
/*                                                                        */
/*      YYYYMMDD.hhmmss    eg 20071227.130354                             */
/*                                                                        */
/*------------------------------------------------------------------------*/

char *get_current_time_as_timestamp(char *timestamp,
                                    int max_timestamp_length,
                                    char separator)
{

    time_t     time_now;
    struct tm  timestruct;
    struct tm  *ptm = &timestruct;
    char msg[40];

    time_now = time(NULL);
    ptm = localtime_r(&time_now, ptm);
    sprintf(msg, "%04d%02d%02d%c%02d%02d%02d",
            1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday,
            separator,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    safecpy(timestamp, msg, max_timestamp_length);
    return timestamp;
}

/*------------------------------------------------------------------------*/
/* time_msg                                                               */
/*                                                                        */
/* Input:                                                                 */
/*                                                                        */
/*  none                                                                  */
/*                                                                        */
/* Output:                                                                */
/*                                                                        */
/*   msg     pointer to a string containing a printable representation    */
/*           of the current time                                          */
/*                                                                        */
/* Description:                                                           */
/*                                                                        */
/* Formats the current time as a time stamp that is suitable for printing */
/* This time stamp is in the format:                                      */
/*                                                                        */
/*      YYYYMMDD hhmmss    eg 20071227 130354                             */
/*                                                                        */
/*------------------------------------------------------------------------*/

char *time_msg(char *msg, int max_msg_len)
{
    time_t time_now;
    struct tm timestruct;
    struct tm *ptm = &timestruct;

    time_now = time(NULL);
    ptm = localtime_r(&time_now, ptm);
    snprintf(msg, max_msg_len, "%04d%02d%02d %02d%02d%02d",
             1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday,
             ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    return msg;
}

/*----------------------------------------------------------------------*/
/*                                                                      */
/* NAME:        print_string_to_console                                 */
/*                                                                      */
/* FUNCTION:    Prints a formatted message to the console               */
/*              Does not print a \n unless one is supplied in msg.      */
/*              Also prints the message to the console log if one is    */
/*              open.                                                   */
/*                                                                      */
/* PARAMS IN:   msg      the text of the message to be displayed        */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     void                                              .     */
/*                                                                      */
/*----------------------------------------------------------------------*/

void print_string_to_console(char *text)
{

    struct stat statbuf;

    if (stat(saved_console_log_filename, &statbuf) == -1)
    {
        /* the current consolelog file seems to have disappeared - so close the
           stream and re-open it */

        close_consolelog();
        open_consolelog(NULL, FALSE);
    }

    printf("%s", text );
    fflush(stdout);

    if (console_log_fptr != NULL)
    {
        fprintf(console_log_fptr, "%s", text );
        fflush(console_log_fptr);
    }
    else
    {
        printf("%s", text);
    }

}

/*----------------------------------------------------------------------*/
/*                                                                      */
/* NAME:        print_msg_to_console                                    */
/*                                                                      */
/* FUNCTION:    Prints a time stamped, formatted message to the console */
/*              Does not print a \n unless one is supplied in msg.      */
/*              Also prints the message to the console log if one is    */
/*              open.                                                   */
/*                                                                      */
/* PARAMS IN:   msg      the text of the message to be displayed        */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     void                                              .     */
/*                                                                      */
/*----------------------------------------------------------------------*/

void print_msg_to_console(char *text)
{

    char time_string[16];
    char outputline[1024];
    struct stat statbuf;

    if  ((console_log_fptr != NULL) && (stat(saved_console_log_filename, &statbuf) == -1))
    {
        /* the current consolelog file seems to have disappeared - so close the
           stream and re-open it */

        close_consolelog();
        open_consolelog(NULL, FALSE);
    }

    time_msg(time_string, sizeof(time_string));

    time_msg(time_string, sizeof(time_string));
    snprintf(outputline, sizeof(outputline), "%s: %s", time_string, text );
    printf("%s", outputline );
    fflush(stdout);

    if (console_log_fptr != NULL)
    {
        fprintf(console_log_fptr, "%s", outputline );
        fflush(console_log_fptr);
    }

}

/*----------------------------------------------------------------------*/
/*                                                                      */
/* NAME:        print_msg_to_console_log                                */
/*                                                                      */
/* FUNCTION:    Prints a time stamped, formatted message to the console */
/*              log only                                                */
/*              Does not print a \n unless one is supplied in msg.      */
/*              If the console log file is not open prints the message  */
/*              to the console                                          */
/*                                                                      */
/* PARAMS IN:   msg      the text of the message to be displayed        */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     void                                              .     */
/*                                                                      */
/*----------------------------------------------------------------------*/

void print_msg_to_console_log(char *text)
{

    char time_string[16];
    char outputline[1024];
    struct stat statbuf;

    if (stat(saved_console_log_filename, &statbuf) == -1)
    {
        /* the current consolelog file seems to have disappeared - so close the
           stream and re-open it */

        close_consolelog();
        open_consolelog(NULL, FALSE);
    }

    time_msg(time_string, sizeof(time_string));
    snprintf(outputline, sizeof(outputline), "%s: %s", time_string, text );

    if (console_log_fptr != NULL)
    {
        fprintf(console_log_fptr, "%s", outputline );
        fflush(console_log_fptr);
    }
    else
    {
        printf("%s", outputline );
        fflush(stdout);
    }

}

/*----------------------------------------------------------------------*/
/*                                                                      */
/* NAME:        open_consolelog                                         */
/*                                                                      */
/* FUNCTION:    Opens a file specified by the user.  Any messages       */
/*              then printed by the print_msg_to_console                */
/*              will also be written to this file too                   */
/*                                                                      */
/* PARAMS IN:   console_log_filename   the name of the file to be opened*/
/*              quiet                  if TRUE, informational messages  */
/*                                     are not printed to stdout        */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                     .      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int open_consolelog(char *console_log_filename, bool_t quiet)
{
    char msg[MAX_MSG_LEN];

    if (console_log_filename != NULL)
    {
        safecpy(saved_console_log_filename, console_log_filename, sizeof(saved_console_log_filename));
    }

    if ( ((console_log_fptr = fopen(saved_console_log_filename, "w")) == NULL) )
    {
        sprintf(msg, "Error opening console log filename: %s\n", saved_console_log_filename);
        /* do not use print_msg_to_console(msg); as this would be infinite recursion !! */
        printf("%s", msg);
        return(1);
    }

    if (! quiet)
    {
        fprintf(console_log_fptr, "%s console log file opened\n", saved_console_log_filename );
        sprintf(msg, "console log filename successfully opened: %s\n",
                saved_console_log_filename);
        print_msg_to_console(msg);
    }

    return(0);

}

/*----------------------------------------------------------------------*/
/*                                                                      */
/* NAME:        close_consolelog                                        */
/*                                                                      */
/* FUNCTION:    Closes the console log file previously specified by the */
/*              Any messages now printed by the print_msg_to_console or */
/*              will no longer be written to this file too.             */
/*                                                                      */
/* PARAMS IN:   none                                                    */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                     .      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int close_consolelog(void)
{
    if  (console_log_fptr != NULL)
    {
        fclose(console_log_fptr);
        console_log_fptr = NULL;
    }

    return(0);

}




int log_msg_to_scenario_console_log(char *agent_log_pathname, char *prefix, char *msg, bool_t add_newline)
{

    FILE *agent_console_log_fptr;
    char time_string[16];

    if ( ((agent_console_log_fptr = fopen(agent_log_pathname, "a")) == NULL) )
    {
        sprintf(msg, "Error opening agent log pathname: %s\n", agent_log_pathname);
        print_msg_to_console(msg);
        return(1);
    }
    else
    {
        time_msg(time_string, sizeof(time_string));
        fprintf(agent_console_log_fptr, "%s: %36s:  %s", time_string, prefix, msg );

        if (add_newline)
        {
            fprintf(agent_console_log_fptr, "\n");
        }

        fflush(stdout);
        fclose(agent_console_log_fptr);
    }

    return 0;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  split_pathname                                                                             */
/*                                                                                                        */
/* PARAMS IN:  pathname             a string containing the pathname of a file - eg "/fred/jim/test1"     */
/*                                                                                                        */
/* PARAMS OUT: directory_pathname   a string containing the directory part of the pathname, in the above  */
/*                                  example, this would be "/fred/jim"                                    */
/*                                                                                                        */
/*             filename             a string containing the filename part of the pathname, in the above   */
/*                                  example, this would be "test1"                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:   Splits a pathname into a directory portion and a filename portion.                         */
/*                                                                                                        */
/*  Examples:                                                                                             */
/*                                                                                                        */
/*     pathname                      directory_pathname                    filename                       */
/*                                                                                                        */
/*     /dir1/dir2/dir3/test          /dir1/dir2/dir3                       test                           */
/*     /test1                        /                                     test                           */
/*     /dir1/dir2/dir3/              /dir1/dir2/dir3                       empty string                   */
/*     /                             /                                     empty string                   */
/*--------------------------------------------------------------------------------------------------------*/

/*<<<<< what happens if pathname is presented as 'data' and does not contain a leading / ? */

void split_pathname(char *pathname, char *directory_pathname, char *filename)
{

    int i, j=0, k;

    for (i=0; i < strlen(pathname); i++)
    {
        if (pathname[i] == '/')
        {
            j = i;
        }
    }

    for (i=0; i < ((j == 0) ? 1 : j); i++)
    {
        directory_pathname[i] = pathname[i];
    }

    directory_pathname[ ((j == 0) ? 1 : j)] = 0;
    k = 0;

    for (i=j+1; i < strlen(pathname); i++)
    {
        filename[k] = pathname[i];
        k++;
    }

    filename[k] = 0;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  ensure_directory_path_exists                                                               */
/*                                                                                                        */
/* PARAMS IN:  pathname             a string containing the pathname of a directory, eg /fred/jim/test1"  */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    0 for success, 1 for failure                                                               */
/*                                                                                                        */
/* FUNCTION:   Ensures that the specified directory exists - if it does not or one of the components in   */
/*             the directory pathname do not exist, the necessary directories are created                 */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int ensure_directory_path_exists(char *pathname)
{

    char path[MAX_PATHNAME_LEN];
    struct stat buf;

    int i = 0;
    int l = strlen(pathname);
    int checksubdir;

    memset(path, 0, sizeof(path));

    while (i < l)
    {

        path[i] = pathname[i];
        i++;

        checksubdir = 0;

        if (i < l)
        {
            if (pathname[i] == '/')
            {
                checksubdir = 1;
            }
        }
        else
        {
            checksubdir = 1;
        }

        if (checksubdir)
        {

            if (stat(path, &buf) == 0)
            {
                if (S_ISDIR(buf.st_mode))
                {
                    /* sub directory exists */
                }
                else
                {
                    /* path to subdirectory did exist, but it was not a directory - problem */
                    return 1;
                }
            }
            else
            {
                /* sub direcotry did not exist - or there was a problem stating it
                  try to make the new sub directory anyway */
                if (mkdir(path, buf.st_mode & 0777) != 0)
                {
                    /* could not create subdir */
                    return 1;
                }
            }
        }
    }

    return 0;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  does_file_exist                                                                            */
/*                                                                                                        */
/* PARAMS IN:  pathname             a string containing the pathname of a file, eg /fred/jim/testfile"    */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    TRUE if the file exists, FALSE if it does not                                              */
/*                                                                                                        */
/* FUNCTION:   Tests to see if a specific file exists                                                     */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int does_file_exist(char *pathname)
{

    struct stat buf;
    int rc = FALSE;

    if (stat(pathname, &buf) == 0)
    {
        if (S_ISREG(buf.st_mode))
        {
            /* file exists */
            rc = TRUE;
        }
        else
        {
            /* path to subdirectory did exist, but it was not a directory */
        }
    }

    return rc;

}


/*----------------------------------------------------------------------*/
/*                                                                      */
/* NAME:        create_empty_file                                       */
/*                                                                      */
/* FUNCTION:    Creates an empty file in an existing directory          */
/*              If the file already exists, it is truncated to 0 length */
/*                                                                      */
/* PARAMS IN:   pathname     the name of the file to be opened          */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                     .      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int create_empty_file(char *pathname)
{
    char msg[MAX_MSG_LEN];
    FILE *fptr;

    if ( ((fptr = fopen(pathname, "w")) == NULL) )
    {
        sprintf(msg, "Error opening pathname: %s\n", pathname);
        print_msg_to_console(msg);
        return(1);
    }

    fclose(fptr);
    return(0);

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  safecpy                                                                                    */
/*                                                                                                        */
/* PARAMS IN:  dest          the destination string                                                       */
/*                                                                                                        */
/*             src           the source string                                                            */
/*                                                                                                        */
/*             len           the maximum storage space in dest in characters.   This includes the         */
/*                           space for the terminating \0                                                 */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    dest          a pointer to the destination string                                          */
/*                                                                                                        */
/* FUNCTION:   Copies the source string to the destination string.  The dest string is guaranteed to be   */
/*             returned as a null terminated string, so the src string is too long to fit in dest then,   */
/*             at most, len-1 chars will be copied.                                                       */
/*                                                                                                        */
/*  The destination and source strings should not overlap.  len should not be 0.                          */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

char *safecpy(char *dest, char *src, unsigned int len)
{

    unsigned int i = 0;

    if (len > 0)
    {

        while (i < (len-1))
        {

            if (src[i] != 0)
            {
                dest[i] = src[i];
                i++;
            }
            else
            {
                break;
            }
        }

        dest[i] = 0;

        // printf("safecpy: src: %s  dest: %s\n", src, dest);
    }

    return dest;

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  safecat                                                                                   */
/*                                                                                                        */
/* PARAMS IN:  dest          the destination string                                                       */
/*                                                                                                        */
/*             src           the source string                                                            */
/*                                                                                                        */
/*             len           the maximum storage space in dest in characters.   This includes the         */
/*                           space for the terminating \0                                                 */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    dest          a pointer to the destination string                                          */
/*                                                                                                        */
/* FUNCTION:   Concatenatesp the source string to the destination string.  The dest string is guaranteed  */
/*             to be returned as a null terminated string, so if the concatenated string would exceed     */
/*             the length available in dest, it is truncated to fit.                                      */
/*                                                                                                        */
/*  The destination and source strings should not overlap.  len should not be 0.                          */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

char *safecat(char *dest, char *src, unsigned int len)
{

    unsigned int i = 0;
    unsigned int j = strlen(dest);

    // printf("safecat: src: %s  dest: %s\n", src, dest);

    if (len > 0)
    {

        while ((i+j) < (len-1))
        {

            if (src[i] != 0)
            {
                dest[i+j] = src[i];
                i++;
            }
            else
            {
                break;
            }
        }

        dest[i+j] = 0;

    }

    // printf("safecat: src: %s  dest: %s\n", src, dest);

    return dest;

}


/**************************************************************************/
/*                                                                        */
/* NAME:        run_system_cmd_and_print_all_output                       */
/*                                                                        */
/* FUNCTION:    Runs a system command in a new process                    */
/*                                                                        */
/* PARAMS IN:   cmd   string containing the command to be executed        */
/*                                                                        */
/*                                                                        */
/* RETURNS:     the exit code from the process that tried to run the      */
/*              system command.  If the command was run, then this will   */
/*              the exit code of the command, if the command could not be */
/*              found or some other problem occurred, then the exit code  */
/*              will relate to that problem                               */
/**************************************************************************/

int run_system_cmd_and_print_all_output(char *cmd)
{
    int rc;
    int signal;
    int cmd_rc = 0;
    FILE *pf;
    char cmd1[MAX_MSG_LEN];
    char msg[MAX_MSG_LEN];
    char temppathname[65] = "/tmp/temp.dskx.dskx.dskx.temp";
    char tstring[65];              // <<<<<<<<<

    sprintf(tstring, "%d", (int)get_process_pid());

    strncat(temppathname, tstring, sizeof(temppathname));


    if (strlen(cmd) > 0)
    {

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
        unlink(temppathname);
#elif defined WIN32
        _unlink(temppathname);
#endif

        safecpy(cmd1, cmd, sizeof(cmd1));
        safecat(cmd1, " >", sizeof(cmd1));
        safecat(cmd1, temppathname, sizeof(cmd1));
        safecat(cmd1, " 2>&1", sizeof(cmd1));

        rc = system(cmd1);
        cmd_rc = rc / 256;
        signal = rc & 0xff;
        sprintf(msg, "System command: %s, returned rc = 0x%02x, signal = 0x%02x\n", cmd, cmd_rc, signal);

        if (cmd_rc != 0)
        {
            print_msg_to_console(msg);
        }
        else
        {
            print_msg_to_console(msg);

            if ((pf = fopen(temppathname,"r")) != NULL)
            {
                while (fgets(msg, sizeof(msg)-1, pf) != NULL)
                {
                    print_string_to_console(msg);
                }

                fclose(pf);
#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
                unlink(temppathname);
#elif defined WIN32
                _unlink(temppathname);
#endif
            }
        }

        print_string_to_console("\n");
    }

    return cmd_rc;
}


/**************************************************************************/
/*                                                                        */
/* NAME:        run_system_cmd_and_capture_single_line_output             */
/*                                                                        */
/* FUNCTION:    Runs a system command in a new process                    */
/*                                                                        */
/* PARAMS IN:   cmd         string containing the command to be executed  */
/*                          Note: this should not contain any redirection */
/*                          characters such as >                          */
/*                                                                        */
/*              output      first line of output of command               */
/*              max_output_line_length   max size of output string        */
/*                                                                        */
/*                                                                        */
/* PARAMS OUT:  output   first line of output from cmd - any terminating  */
/*                       \n is removed                                    */
/*                                                                        */
/* RETURNS:     the exit code from the process that tried to run the      */
/*              system command.  If the command was run, then this will   */
/*              the exit code of the command, if the command could not be */
/*              found or some other problem occurred, then the exit code  */
/*              will relate to that problem                               */
/**************************************************************************/

int run_system_cmd_and_capture_single_line_output(char *cmd,
        char *output,
        int max_output_line_length)
{
    int rc;
    int signal;
    int cmd_rc = 0;
    FILE *pf;
    char cmd1[MAX_MSG_LEN];
    char msg[MAX_MSG_LEN];
    char temppathname[65] = "/tmp/temp.dskx.dskx.temp";
    char tstring[65];              // <<<<<<<<<

    sprintf(tstring, "%d", (int)get_process_pid());

    strncat(temppathname, tstring, sizeof(temppathname));

    if (strlen(cmd) > 0)
    {

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
        unlink(temppathname);
#elif defined WIN32
        _unlink(temppathname);
#endif
        safecpy(cmd1, cmd, sizeof(cmd1));
        safecat(cmd1, " >", sizeof(cmd1));
        safecat(cmd1, temppathname, sizeof(cmd1));
        safecat(cmd1, " 2>&1", sizeof(cmd1));

        rc = system(cmd1);
        cmd_rc = rc / 256;
        signal = rc & 0xff;

        if (cmd_rc != 0)
        {
            sprintf(msg, "System command: %s, returned rc = 0x%02x, signal = 0x%02x\n", cmd, cmd_rc, signal);
            print_msg_to_console(output);
        }
        else
        {
            if ((pf = fopen(temppathname,"r")) != NULL)
            {
                if (fgets(output, max_output_line_length, pf) == NULL)
                {
                    output[0] = 0;
                }

                fclose(pf);
#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX
                unlink(temppathname);
#elif defined WIN32
                _unlink(temppathname);
#endif
            }

            if (output[strlen(output)] == '\n')
            {
                output[strlen(output)] = 0;
            }

            sprintf(msg, "%s: %s\n", cmd, output);
            print_msg_to_console(msg);
        }
    }

    return cmd_rc;
}

/**************************************************************************/
/*                                                                        */
/* NAME:        run_system_cmd                                            */
/*                                                                        */
/* FUNCTION:    Runs a system command in a new process                    */
/*                                                                        */
/* PARAMS IN:   cmd   string containing the command to be executed        */
/*              quiet set to TRUE to suppress print statements            */
/*                                                                        */
/* PARAMS OUT:  none                                                      */
/*                                                                        */
/* RETURNS:     the exit code from the process that tried to run the      */
/*              system command.                                           */
/**************************************************************************/

int run_system_cmd(char *cmd, bool_t quiet)
{
    int rc;
    int signal = 0;
    int cmd_rc = 0;
    char msg[MAX_MSG_LEN];

    if (strlen(cmd) > 0)
    {

        if (! quiet)
        {
            safecpy(msg, "Running system command: ", sizeof(msg));
            safecat(msg, cmd, sizeof(msg));
            safecat(msg, "\n", sizeof(msg));
            print_msg_to_console(msg);
        }

        rc = system(cmd);

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

        if (! quiet)
        {
            sprintf(msg, "System command: %s, returned rc = 0x%02x, signal = 0x%02x\n", cmd, cmd_rc, signal);
            print_msg_to_console(msg);
        }

        //  } else {
        //     sprintf(msg, "System command specified had zero length, so no command was actually run\n"
        //                  "(Note: a system command may be optionally run when a miscompare or i/o erro occurs or run as part of a Stanza\n"
        //                  "This message appears when a system command has beeb specified by the system command is the empty string)\n");
        //     print_msg_to_console(msg);
    }

    return cmd_rc;
}

/**************************************************************************/
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
/**************************************************************************/

int run_system_cmd_in_background(char *cmd)
{

#if defined AIX || defined LINUX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    // <<<<<<<<<< oh dear, this code does not look very good.... should use fork() I think
    int rc;
    int signal;
    int cmd_rc = 0;
    char msg[MAX_MSG_LEN];
    char background_cmd[MAX_SYSTEM_CMD_LEN];

    if (strlen(cmd) > 0)
    {
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

#elif defined WIN32 || defined NETWARE

    char msg[MSG_TEXT_SIZE];
    thread_workitem_t    background_cmd_thread_workitem;  // <<<<<<<< ONLY WORKS WITHI ONE THEAD AT A TIME
    background_cmd_wrk_t background_cmd_wrk;

    assert(cmd);

    initialise_thread_workitem(&background_cmd_thread_workitem, 0, FALSE);

    if (strlen(cmd) > 0)
    {

        // we are going to start a new thread and then run the system() command in that thread

        safecpy(background_cmd_wrk.cmd, cmd, sizeof(background_cmd_wrk.cmd));
        background_cmd_wrk.quiet = 0;
        background_cmd_wrk.pthread_workitem = &background_cmd_thread_workitem;

        if (create_thread((void *(*)(void *)) &run_system_cmd_in_thread,
                          (void *) &background_cmd_wrk,
                          &background_cmd_thread_workitem ) != 0)
        {
            sprintf(msg, "threads.c: could not start background cmd thread %d (%s)\n",
                    errno, strerror(errno));
            print_error_msg_to_console(20000, "SYSTEM", msg);
            exit(1);
        }
    }

    return 0;

#endif

}


/************************************************************************/
/*                                                                      */
/* NAME:        copy_file                                               */
/*                                                                      */
/* FUNCTION:    copies the contents of one file into another file,      */
/*              overwriting the contents of the destination file        */
/*                                                                      */
/* PARAMS IN:   src         pathname of the source file                 */
/*              dest        pathname of the destination file            */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: none.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                      .     */
/*                                                                      */
/*----------------------------------------------------------------------*/
int copy_file(char *src, char *dest)
{

    int    rc = 0;
    char   cmd[1024];
    char   msg[MAX_MSG_LEN];

    safecpy(cmd, "cp -f ", sizeof(cmd));
    safecat(cmd, src, sizeof(cmd));
    safecat(cmd, " ", sizeof(cmd));
    safecat(cmd, dest, sizeof(cmd));

    if (run_system_cmd(cmd, 0) != 0)
    {
        snprintf(msg, sizeof(msg), "copy command failed: %s - errno=%d (%s)\n",
                 cmd, errno, strerror(errno));
        print_msg_to_console(msg);
        return(1);
    }

    return rc;

}

/************************************************************************/
/*                                                                      */
/* NAME:        chmod_file                                              */
/*                                                                      */
/* FUNCTION:    Runs a chmod command on the specified file              */
/*                                                                      */
/* PARAMS IN:   src         pathname of the file to be chmod'd          */
/*              permissions chmod setting, eg "u+S"                     */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: none.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                      .     */
/*                                                                      */
/*----------------------------------------------------------------------*/
int chmod_file(char *src, char *permissions)
{

    int    rc = 0;
    char   cmd[1024];
    char   msg[MAX_MSG_LEN];

    safecpy(cmd, "chmod ", sizeof(cmd));
    safecat(cmd, permissions, sizeof(cmd));
    safecat(cmd, " ", sizeof(cmd));
    safecat(cmd, src, sizeof(cmd));

    if (run_system_cmd(cmd, 0) != 0)
    {
        snprintf(msg, sizeof(msg), "chmod command failed: %s - errno=%d (%s)\n",
                 cmd, errno, strerror(errno));
        print_msg_to_console(msg);
        return(1);
    }

    return rc;

}

/************************************************************************/
/*                                                                      */
/* NAME:        delete_file                                             */
/*                                                                      */
/* FUNCTION:    delets a file                                           */
/*                                                                      */
/* PARAMS IN:   path        pathname of the file to be deleted          */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED: none.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                      .     */
/*                                                                      */
/*----------------------------------------------------------------------*/
int delete_file(char *path)
{

    int rc = 0;
    char   cmd[1024];
    char   msg[MAX_MSG_LEN];

    safecpy(cmd, "rm -f ", sizeof(cmd));
    safecat(cmd, path, sizeof(cmd));

    if ((rc = system(cmd)) != 0)
    {
        snprintf(msg, sizeof(msg), "rm command failed: %s - rc = %d, errno=%d (%s)\n",
                 cmd, rc, errno, strerror(errno));
        print_msg_to_console(msg);
        return(1);
    }

    return rc;

}


/**************************************************************************/
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
/**************************************************************************/

pid_t get_process_pid()
{

#if defined AIX || defined LINUX || defined SOLARIS || defined HPUX
    pid_t  pid;

    if ((pid = getpid()) < 0)
    {
        perror("unable to get pid");
    }

    return pid;
#else
    return _getpid();
#endif

}

/************************************************************************/
/*                                                                      */
/* NAME:        chomp                                                   */
/*                                                                      */
/* FUNCTION:                                                            */
/*                                                                      */
/* PARAMS IN:   mystring  a string, possibly with a \n at the end       */
/*                                                                      */
/* PARAMS OUT:  mystring  the same string, without any terminated \n    */
/*                                                                      */
/*                                                                      */
/* RETURNS:     void                                                    */
/*                                                                      */
/* DESCRIPTION  Takes as input a null terminated string, and if the     */
/*              last character in the string is a \n it removes the \n  */
/*                                                                      */
/************************************************************************/

void chomp(char *mystring)
{

    int i = (int) strlen(mystring);

    if (i>0)
    {
        i--;

        if (mystring[i] == '\n')
        {
            mystring[i] = 0;
        }
    }

    return;
}


/************************************************************************/
/*                                                                      */
/* NAME:        set_ulimit_c_unlimited                                  */
/*                                                                      */
/* FUNCTION:    Perform the equivalent of ulimit -c unlimited, thus     */
/*              ensuring that if the program does segfault, we get a    */
/*              core dump                                               */
/*                                                                      */
/* PARAMS IN:   max_errmsg_len   max number of chars in errmsg          */
/*                                                                      */
/* PARAMS OUT:  errmsg                                                  */
/*                                                                      */
/* RETURNS:     E_OK for success, E_NOTOK for failure                   */
/*                                                                      */
/* DESCRIPTION  This routine is a no-op on Windows                      */
/*                                                                      */
/************************************************************************/

int set_ulimit_c_unlimited(char *errmsg, int max_errmsg_len)
{

#if defined AIX || defined LINUX || defined MACOSX || defined LINUX_PPC64 || defined SOLARIS || defined HPUX

    struct rlimit new;
    int rc = E_OK;

    errmsg[0] = 0;

    new.rlim_cur = RLIM_INFINITY;
    new.rlim_max = RLIM_INFINITY;

    if (setrlimit(RLIMIT_CORE, &new) != 0)
    {
        sprintf(errmsg, "set_ulimit_c_unlimited: could not set ulimit -c unlimited, errno=%d\n", errno);
        rc = E_NOTOK;
    }

    return(rc);

#elif defined WIN32
    return(E_OK);
#else
#error Cannot compile, unknown target OS - define AIX, LINUX, MACOSX, LINUX_PPC64, SOLARIS, HPUX, WIN32
#endif

}

/************************************************************************/
/*                                                                      */
/* NAME:        split_namevalue_parameter                               */
/*                                                                      */
/* FUNCTION:    Splits a string of the form 'name=value' into its       */
/*              two separate parts                                      */
/*                                                                      */
/* PARAMS IN:   namevalue    the string that is to be split up          */
/*                                                                      */
/*              name        pointer to the string that will contain the */
/*                          name part of the input string               */
/*                                                                      */
/*              name_len    max number of bytes in the name string      */
/*                          including the terminating \0                */
/*                                                                      */
/*              value       pointer to the string that will contain the */
/*                          value part of the input string              */
/*                                                                      */
/*              value_len   max number of bytes in the value string     */
/*                          including the terminating \0                */
/*                                                                      */
/* PARAMS OUT:  name        the name part of the input string           */
/*                                                                      */
/*              value       the value part of the input string          */
/*                                                                      */
/* RETURNS:                 void                                        */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/************************************************************************/

void split_namevalue_parameter(char *namevalue,
                               char *name,
                               int name_len,
                               char *value,
                               int value_len)
{

    int i = 0;
    int j = 0;
    int k = 0;

    char separater = '=';

    bool_t inName = TRUE;

    while (i < strlen(namevalue))
    {

        if (namevalue[i] == separater)
        {
            inName = FALSE;
        }
        else
        {
            if (inName)
            {
                if (j<(name_len-1))
                {
                    name[j++] = namevalue[i];
                }
            }
            else
            {
                if (k<(value_len-1))
                {
                    value[k++] = namevalue[i];
                }
            }
        }

        i++;

    }

    name[j] = 0;
    value[k] = 0;

}

/************************************************************************/
/*                                                                      */
/* NAME:        get_hostname                                            */
/*                                                                      */
/* FUNCTION:    Perform the equivalent of hostname, determing the       */
/*              name of the current host excludig its domain name       */
/*                                                                      */
/* PARAMS IN:   hostname    pointer to the char string that will        */
/*                          contain the full host name                  */
/*              max_hostname_len   max number of bytes in the hostname  */
/*                                 including the terminating \0         */
/*                                                                      */
/* PARAMS OUT:  hostname    pointer to the char string that will        */
/*                          contain the host name                       */
/*                                                                      */
/* RETURNS:                 pointer to the char string that will        */
/*                          contain the host name                       */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/************************************************************************/

char *get_hostname(char *hostname, int max_hostname_len)
{

    gethostname(hostname, max_hostname_len);

    return hostname;
}


/************************************************************************/
/*                                                                      */
/* NAME:        get_short_hostname                                      */
/*                                                                      */
/* FUNCTION:    Perform the equivalent of hostname -s, determing the    */
/*              name of the current host excludig its domain name       */
/*                                                                      */
/* PARAMS IN:   hostname    pointer to the char string that will        */
/*                          contain the host name                       */
/*              max_hostname_len   max number of bytes in the hostname  */
/*                                 including the terminating \0         */
/*                                                                      */
/* PARAMS OUT:  hostname    pointer to the char string that will        */
/*                          contain the host name                       */
/*                                                                      */
/* RETURNS:                 pointer to the char string that will        */
/*                          contain the host name                       */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/************************************************************************/

char *get_short_hostname(char *hostname, int max_hostname_len)
{

    int i;

    gethostname(hostname, max_hostname_len);

    for (i=0; i<strlen(hostname); i++)
    {
        if (hostname[i] == '.')
        {
            hostname[i] = 0;
            break;
        }
    }

    return hostname;
}
