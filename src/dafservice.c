/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                        */
/*                                                                                                                      */
/* dafservice.c                                                                                                         */
/*                                                                                                                      */
/* This file is part of the daf test automation programme.                                                              */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/dir.h>
#include <mysql.h>
#include "OSstuff.h"
#include "af_mysql.h"
#include "af_defns.h"
#include "afmysql.h"
#include "daf_protocol.h"
#include "daf_util.h"
#include "daf_service_environment.h"
#include "dafservice.h"
#include "dafclient.h"
#include "daf_shared_memory.h"
#include "daf_threads.h"

#define MAX_MSG_LEN         512  /* <<<<<<<< need to define in a common file */
#define E_OK                  0
#define E_NOTOK               1

int msglevel = 0;

static const struct cmd_log_state_entry cmd_log_state_array[] =
{
    { CMD_FREE,        "CMD_FREE"},
    { CMD_INITIALISED, "CMD_INITIALISED"},
    { CMD_RUNNING,     "CMD_RUNNING"},
    { CMD_BG_RUNNING,  "CMD_BG_RUNNING"},
    { CMD_COMPLETED,   "CMD_COMPLETED"},
    { CMD_CANCELED_TIMELIMIT,    "CMD_CANCEL_TIMELIMIT"},
    { CMD_CANCELED_TIMEDOUT,     "CMD_CANCEL_TIMEDOUT"},
    { CMD_COMPLETED_TIMELIMIT,   "CMD_COMPLETED_TIMELIMIT"},
    { CMD_COMPLETED_TIMEDOUT,    "CMD_COMPLETED_TIMEDOUT"}
};

struct remote_client_request_entry
{
    int          request_num;
    const char *request_name;
};

static const struct remote_client_request_entry remote_client_request_array[] =
{

    {CLIENT_REMOTE_CLIENT_CMD, "CLIENT_REMOTE_CLIENT_CMD"},
    {CLIENT_REMOTE_CLIENT_QUERY_VERSION, "CLIENT_REMOTE_CLIENT_QUERY_VERSION"},
    {CLIENT_REMOTE_CLIENT_QUERY_TAG, "CLIENT_REMOTE_CLIENT_QUERY_TAG"},
    {CLIENT_REMOTE_CLIENT_QUERY_ALLTAGS, "CLIENT_REMOTE_CLIENT_QUERY_ALLTAGS"},
    {CLIENT_REMOTE_CLIENT_QUERY_CMDLOG, "CLIENT_REMOTE_CLIENT_QUERY_CMDLOG"},
    {CLIENT_REMOTE_CLIENT_QUERY_IDENT, "CLIENT_REMOTE_CLIENT_QUERY_IDENT"},
    {CLIENT_REMOTE_CLIENT_QUERY_CMDS, "CLIENT_REMOTE_CLIENT_QUERY_CMDS"},
    {CLIENT_REMOTE_CLIENT_CLEAR_TAG, "CLIENT_REMOTE_CLIENT_CLEAR_TAG"},
    {CLIENT_REMOTE_CLIENT_CLEAR_IDENT, "CLIENT_REMOTE_CLIENT_CLEAR_IDENT"},
    {CLIENT_REMOTE_CLIENT_CLEAR_ALLTAGS, "CLIENT_REMOTE_CLIENT_CLEAR_ALLTAGS"},
    {CLIENT_REMOTE_CLIENT_CANCEL_TAG, "CLIENT_REMOTE_CLIENT_CANCEL_TAG",},
    {CLIENT_REMOTE_CLIENT_CANCEL_TAG, "CLIENT_REMOTE_CLIENT_CANCEL_IDENT",},
    {CLIENT_REMOTE_CLIENT_COPYFILE, "CLIENT_REMOTE_CLIENT_COPYFILE",},
    {CLIENT_REMOTE_CLIENT_DIRLIST, "CLIENT_REMOTE_CLIENT_DIRLIST",},
    {CLIENT_REMOTE_CLIENT_UPDATE_WORKQUEUE_STATUS, "CLIENT_REMOTE_CLIENT_UPDATE_WORKQUEUE_STATUS",}
};



/*--------------------------------------------------------------------------------------------------------*/
/*  Globals                                                                                               */
/*--------------------------------------------------------------------------------------------------------*/

cmd_log_object_t        *p_service_cmd_log_object;
active_steps_object_t   *p_service_active_steps_object;
shm_daf_t               *p_shared_memory_base_pointer;

char ThisHostName[64] = "unknown";

extern struct
{
    char  sql_servername[MAX_SQLSERVERNAME_LEN];
    char  sql_username[MAX_SQLUSERNAME_LEN];
    char  sql_password[MAX_SQLPASSWORD_LEN];
    char  sql_databasename[MAX_SQLDATABASENAME_LEN];
    Iu16  sql_port;
    char  sql_socketname[MAX_SQLSOCKETNAME_LEN];
} sql_server;

int myzombiereaperdebug = 0;

void set_zombie_reaper_debug_flag(bool_t zombie_reaper_debug_flag)
{

    myzombiereaperdebug = zombie_reaper_debug_flag;

}



/*--------------------------------------------------------------------------------------------------------*/
/*  External Globals (in daf.c)                                                                          */
/*--------------------------------------------------------------------------------------------------------*/

extern int  daf_major_version;
extern int  daf_minor_version;
extern char *daf_version_string;
extern char *daf_buildmachine;

/*--------------------------------------------------------------------------------------------------------*/
/*  make_host_and_time_specific_pathname_prefixu                                                           */
/*                                                                                                        */
/*                                                                                                        */
/*  The pathname that is formed is in the following format:                                               */
/*                                                                                                        */
/*  pathdir / hh..hh_YYYYMMDD_HHMMSS_tt.tt.xxxxx                                                          */
/*                                                                                                        */
/*  where hh..hh is the rightmost 16 chars of the host name running this test agent                       */
/*        ii..ii is the identifier in the parameters expressed as a numeric string                        */
/*        tt..tt is the rightmost 32 chars of the testcase                                                */
/*        YYYYYMMDD_HHMMSS is our standard timestamp  eg 20110826_14:30:0 for 2.30pm on 26 Aug 2011       */
/*        xxxxx is the postfix provided in the parameters, which should not exceed 8 chars                */
/*                                                                                                        */
/*  eg    /tmp/daflogs/suse11a_123_nfst.stdout                                                            */
/*                                                                                                        */
/*        any '/' or '\' present in the testcase are substituted with '_'                                 */
/*--------------------------------------------------------------------------------------------------------*/

void make_host_and_time_specific_pathname_prefix(char *pathdir, char *hostname, int identifier, char *testcasename, char *postfix,
        char *pathname, int max_pathname_len)
{

    char temp[256];
    //   int i,j,k;
    int j;

    safecpy(pathname, pathdir, max_pathname_len);
    safecat(pathname, "/", max_pathname_len);

    if (strlen(hostname) > 16)
    {
        j = strlen(hostname) - 16;
    }
    else
    {
        j = 0;
    }

    safecat(pathname, &(hostname[j]), max_pathname_len);
    safecat(pathname, "_", max_pathname_len);
    snprintf(temp, sizeof(temp), "%u", identifier);
    safecat(pathname, temp, max_pathname_len);
    safecat(pathname, "_", max_pathname_len);

    get_current_time_as_timestamp(temp, sizeof(temp),  '.');
    safecat(pathname, temp, max_pathname_len);

    //    safecat(pathname, "_", max_pathname_len);
    //
    //    if (strlen(testcasename) > 32)
    //    {
    //        j = strlen(testcasename) - 32;
    //    }
    //    else
    //    {
    //        j = 0;
    //    }
    //
    //    k = strlen(pathname);
    //
    //    for (i=j; ((i<strlen(testcasename)) && (k<(max_pathname_len-1))); i++)
    //    {
    //        if ((testcasename[i] == '/') || (testcasename[i] == '\\'))      /* In case testcase is a full path,eg /opt/bin/testcase1 */
    //        {
    //            pathname[k] = '_';
    //        }
    //        else
    //        {
    //            pathname[k] = testcasename[i];
    //        }
    //
    //        k++;
    //    }
    //
    //    pathname[k] = 0;

    safecat(pathname, ".", max_pathname_len);
    safecat(pathname, postfix, max_pathname_len);
}



/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void remote_client_request_decode(int remote_client_request, char *msg, int msg_size)
{
    int i;
    msg[0] = 0;

    for (i=0; i < (sizeof(remote_client_request_array)/sizeof(struct remote_client_request_entry)); i++)
    {
        if (remote_client_request == remote_client_request_array[i].request_num)
        {
            safecpy(msg, (char *) remote_client_request_array[i].request_name, msg_size);
            break;
        }
    }
}



/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  cmd_log_state_decode                                                                       */
/*                                                                                                        */
/* PARAMS IN:  cmd_log_state                                                                              */
/*                                                                                                        */
/* PARAMS OUT: msg                                                                                        */
/*                                                                                                        */
/*             msg_size                                                                                   */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void cmd_log_state_decode(enum cmd_log_state cmd_log_state, char *msg, int msg_size)
{

    int i;
    msg[0] = 0;

    for (i=0; i < (sizeof(cmd_log_state_array)/sizeof(struct cmd_log_state_entry)); i++)
    {
        if (cmd_log_state == cmd_log_state_array[i].cmd_log_state_num)
        {
            safecpy(msg, (char *) cmd_log_state_array[i].cmd_log_state_name, msg_size);
            break;
        }
    }

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  get_next_free_tag                                                                          */
/*                                                                                                        */
/* PARAMS IN:  none                                                                                       */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    int                                                                                        */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_next_free_tag(void)
{

    /* assume we don't have to lock this as it is a 4 byte location ??? <<<<<<<< */
    return(p_shared_memory_base_pointer->current_cmd_tag++);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  lock_cmd_log                                                                               */
/*                                                                                                        */
/* PARAMS IN:  cmd_log_object                                                                             */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void lock_cmd_log(cmd_log_object_t *cmd_log, char *subname)
{

    char msg[MAX_MSG_LEN];

    /*      Iu32 secs, msecs, usecs;

          Iu32 *p = (Iu32 *) (&cmd_log->mp);
          get_timestamp(&secs, &msecs, &usecs);
          printf("%4u:%4u:%4u: attempting   lock by %15s of %p %08x %08x %08x %08x\n",
                 secs, msecs, usecs, subname, &cmd_log->mp, *p, *(p+1), *(p+2), *(p+3) ); */

    if (pthread_mutex_lock(&(cmd_log->mp)) != 0)
    {
        snprintf(msg, sizeof(msg), "%s: problem locking mutex in lock_cmd_log_cache, errno = %d - exiting\n",
                 subname, errno);
        print_msg_to_console(msg);
        exit(1);
    }
    else
    {
        /*
              get_timestamp(&secs, &msecs, &usecs);
              printf("%4u:%4u:%4u:    locked by         %15s                   %08x %08x %08x %08x\n",
                     secs, msecs, usecs, subname,  *p, *(p+1), *(p+2), *(p+3)); */
    }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  unlock_cmd_log                                                                             */
/*                                                                                                        */
/* PARAMS IN:  cmd_log_object                                                                             */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void unlock_cmd_log(cmd_log_object_t *cmd_log, char *subname)
{

    char msg[MAX_MSG_LEN];

    /*      Iu32 secs, msecs, usecs;

          Iu32 *p = (Iu32 *) (&cmd_log->mp);
          get_timestamp(&secs, &msecs, &usecs);
          printf("%4u:%4u:%4u: attempting unlock by %15s of %p %08x %08x %08x %08x\n",
                 secs, msecs, usecs, subname, &cmd_log->mp, *p, *(p+1), *(p+2), *(p+3) ); */

    if (pthread_mutex_unlock(&(cmd_log->mp)) != 0)
    {
        snprintf(msg, sizeof(msg), "%s: problem unlocking mutex in lock_cmd_log_cache, errno = %d - exiting\n",
                 subname, errno);
        print_msg_to_console(msg);
        exit(1);
    }
    else
    {
        /*    get_timestamp(&secs, &msecs, &usecs);
            printf("%4u:%4u:%4u: unlocked by          %15s                   %08x %08x %08x %08x\n",
                   secs, msecs, usecs, subname,  *p, *(p+1), *(p+2), *(p+3)); */
    }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  add_cmd_log_entry                                                                          */
/*                                                                                                        */
/* PARAMS IN:  cmd_log                                                                                    */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    int                                                                                        */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int add_cmd_log_entry(cmd_log_object_t *cmd_log_object,
                      char  *cmdstring,
                      char  *identstring,
                      char  *agent_log_pathname,
                      Iu32  tag,
                      Iu32  workqueueID,
                      Iu32  pid,
                      Iu32  state,
                      Iu32  status,
                      Iu32  start_time,
                      Iu32  end_time,
                      Iu32  timeout_at_time,
                      bool_t fail_on_timeout,
                      char  *errmsg,
                      int   max_msg_len)
{

#undef  SUBNAME
#define SUBNAME "add_cmd_log_entry"

    int           i = 0;
    int           j = 0;
    Iu32          oldest_completed_time = 0xffffffff;
    bool_t          found = FALSE;
    bool_t          foundcompleted = FALSE;
    int           rc = 0;

    lock_cmd_log(cmd_log_object, SUBNAME);

    while ( i < DAF_MAX_COMMANDS)
    {

        if (cmd_log_object->cmd_array[i].state == CMD_FREE )
        {

            // printf(">>> add_cmlog actionnresultID for tag %d is %u\n", tag, workqueueID);
            // printf("(cmd_log_object->cmd_array[0]  is at %p\n", &(cmd_log_object->cmd_array[0]));
            // printf("(cmd_log_object->cmd_array[0].workqueueID  is at %p\n", &(cmd_log_object->cmd_array[0].workqueueID));

            found = TRUE;
            break;

        }
        else if (cmd_log_object->cmd_array[i].state == CMD_COMPLETED)
        {

            if (cmd_log_object->cmd_array[i].end_time < oldest_completed_time)
            {
                j = i;
                oldest_completed_time = cmd_log_object->cmd_array[i].end_time;
                foundcompleted = TRUE;
            }

        }

        i++;

    }

    if ((! found) && (foundcompleted))
    {
        i = j;
        found = TRUE;
    }

    if (found)
    {

        safecpy(cmd_log_object->cmd_array[i].cmdstring, cmdstring, sizeof(cmd_log_object->cmd_array[i].cmdstring));
        safecpy(cmd_log_object->cmd_array[i].identstring, identstring, sizeof(cmd_log_object->cmd_array[i].identstring));
        safecpy(cmd_log_object->cmd_array[i].agent_log_pathname, agent_log_pathname, sizeof(cmd_log_object->cmd_array[i].agent_log_pathname));
        cmd_log_object->cmd_array[i].state           = state;
        cmd_log_object->cmd_array[i].status          = status;
        cmd_log_object->cmd_array[i].start_time      = start_time;
        cmd_log_object->cmd_array[i].end_time        = end_time;
        cmd_log_object->cmd_array[i].timeout_at_time = timeout_at_time;
        cmd_log_object->cmd_array[i].fail_on_timeout = fail_on_timeout;
        cmd_log_object->cmd_array[i].tag             = tag;
        cmd_log_object->cmd_array[i].workqueueID     = workqueueID;
        cmd_log_object->cmd_array[i].pid             = pid;

    }
    else
    {

        /* problem - could not allocated a slot to the command */   /* <<<<<<<<<<< */
        rc = 1;

    }

    unlock_cmd_log(cmd_log_object, SUBNAME);

    return rc;
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  add_cmd_log_fragment                                                                       */
/*                                                                                                        */
/* PARAMS IN:  cmd_log                                                                                    */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    int                                                                                        */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int add_cmd_log_fragment(cmd_log *cmd_log,
                         char  *cmdstring,
                         char  *identstring,
                         Iu32  tag,
                         Iu32  workqueueID,
                         Iu32  pid,
                         Iu32  state,
                         Iu32  status,
                         Iu32  start_time,
                         Iu32  end_time,
                         Iu32  timeout_at_time,
                         bool_t fail_on_timeout,
                         char  *errmsg,
                         int   max_msg_len)
{

#undef  SUBNAME
#define SUBNAME "add_cmd_log_fragment"

    cmd_log_entry *new_entry;

    if ((new_entry = malloc(sizeof(cmd_log_entry))) == NULL)
    {
        snprintf(errmsg, max_msg_len ,"%s: malloc() call failed", SUBNAME);
        return E_NOTOK;
    }

    memset(new_entry, 0, sizeof(cmd_log_entry));

    new_entry->next            = NULL;   /* redundant ? */

    safecpy(new_entry->cmdstring, cmdstring, sizeof(new_entry->cmdstring));
    safecpy(new_entry->identstring, identstring, sizeof(new_entry->identstring));
    new_entry->state            = state;
    new_entry->status           = status;
    new_entry->start_time       = start_time;
    new_entry->end_time         = end_time;
    new_entry->timeout_at_time  = timeout_at_time;
    new_entry->fail_on_timeout  = fail_on_timeout;
    new_entry->tag              = tag;
    new_entry->workqueueID      = workqueueID;
    new_entry->pid              = pid;

    if (cmd_log->first == NULL)
    {
        cmd_log->first = new_entry;
    }

    if (cmd_log->last != NULL)
    {
        cmd_log->last->next = new_entry;
    }

    cmd_log->last = new_entry;

    return E_OK;
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remove_cmd_log_entry_by_tag                                                                */
/*                                                                                                        */
/* PARAMS IN:  cmd_log                                                                                    */
/*                                                                                                        */
/*             tag                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    int                                                                                        */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t remove_cmd_log_entry_by_tag(cmd_log_object_t *cmd_log_object, Iu32 tag)
{

#undef  SUBNAME
#define SUBNAME "remove_cmd_log_entry_by_tag"

    bool_t          found = FALSE;
    int             i;

    for  (i = 0; (i< DAF_MAX_COMMANDS) && (! found); i++)
    {

        if (tag == cmd_log_object->cmd_array[i].tag)
        {
            found = TRUE;
            safecpy(cmd_log_object->cmd_array[i].cmdstring,   "", sizeof(cmd_log_object->cmd_array[i].cmdstring));
            safecpy(cmd_log_object->cmd_array[i].identstring, "", sizeof(cmd_log_object->cmd_array[i].identstring));
            cmd_log_object->cmd_array[i].state             = 0;
            cmd_log_object->cmd_array[i].status            = 0;
            cmd_log_object->cmd_array[i].start_time        = 0;
            cmd_log_object->cmd_array[i].end_time          = 0;
            cmd_log_object->cmd_array[i].timeout_at_time   = 0;
            cmd_log_object->cmd_array[i].fail_on_timeout   = 0;
            cmd_log_object->cmd_array[i].tag               = 0;
            cmd_log_object->cmd_array[i].workqueueID       = 0;
            cmd_log_object->cmd_array[i].pid               = 0;

        }

    }

    return found;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remove_cmd_log_entry_by_ident                                                              */
/*                                                                                                        */
/* PARAMS IN:  cmd_log                                                                                    */
/*                                                                                                        */
/*             ident                                                                                      */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    int                                                                                        */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t remove_cmd_log_entry_by_ident(cmd_log_object_t *cmd_log_object, char *identstring)
{

#undef  SUBNAME
#define SUBNAME "remove_cmd_log_entry_by_ident"

    bool_t          found = FALSE;
    int             i;

    for  (i = 0; (i< DAF_MAX_COMMANDS) && (! found); i++)
    {

        if (strncmp(cmd_log_object->cmd_array[i].identstring, identstring, sizeof(cmd_log_object->cmd_array[i].identstring)) == 0)
        {
            found = TRUE;
            safecpy(cmd_log_object->cmd_array[i].cmdstring, "", sizeof(cmd_log_object->cmd_array[i].cmdstring));
            safecpy(cmd_log_object->cmd_array[i].identstring, "", sizeof(cmd_log_object->cmd_array[i].identstring));
            cmd_log_object->cmd_array[i].state      = 0;
            cmd_log_object->cmd_array[i].status     = 0;
            cmd_log_object->cmd_array[i].start_time = 0;
            cmd_log_object->cmd_array[i].end_time   = 0;
            cmd_log_object->cmd_array[i].timeout_at_time = 0;
            cmd_log_object->cmd_array[i].fail_on_timeout = 0;
            cmd_log_object->cmd_array[i].tag        = 0;
            cmd_log_object->cmd_array[i].workqueueID = 0;
            cmd_log_object->cmd_array[i].pid        = 0;

        }

    }

    return found;

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  deallocate_dirlist                                                                         */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void deallocate_dirlist(dirlist pdl)
{

    dirlist current;
    dirlist next;

    current = pdl;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  deallocate_cmd_log_fragment                                                                */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void deallocate_cmd_log_fragment(cmd_log *cmd_log)
{

    cmd_log_entry *current_entry;
    cmd_log_entry *next_entry;

    current_entry = cmd_log->first;

    while (current_entry != NULL)
    {
        next_entry = current_entry->next;
        free(current_entry);
        current_entry = next_entry;
    }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  deallocate_cmd_log                                                                         */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void deallocate_cmd_log(cmd_log_object_t  *cmd_log_object)
{


}





/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  is_tag_in_cmd_log                                                                          */
/*                                                                                                        */
/* PARAMS IN:  cmd_log                                                                                    */
/*                                                                                                        */
/*             tag                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    -1 if not found                                                                            */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int is_tag_in_cmd_log(cmd_log_object_t *cmd_log_object, Iu32 tag)
{

#undef  SUBNAME
#define SUBNAME "is_tag_in_cmd_log"

    int i;

    for (i = 0; i< DAF_MAX_COMMANDS; i++)
    {

        if (tag == cmd_log_object->cmd_array[i].tag)
        {
            if (CMD_FREE != cmd_log_object->cmd_array[i].state)
            {
                return i;
            }
        }

    }

    return -1;
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  get_index_in_cmd                                                                           */
/*                                                                                                        */
/* PARAMS IN:  cmd_log                                                                                    */
/*                                                                                                        */
/*             tag                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    -1 if not found                                                                            */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_index_in_cmd_log(cmd_log_object_t *cmd_log_object, Iu32 index)
{

#undef  SUBNAME
#define SUBNAME "get_index_in_cmd_log"

    Iu32 j;
    int i;

    for  (i = 0, j=index; i< DAF_MAX_COMMANDS; i++)
    {
        if (cmd_log_object->cmd_array[i].state != CMD_FREE )
        {
            if (i == j)
            {
                return i;
            }
        }
    }

    return -1;
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  update_cmd_log_entry                                                                       */
/*                                                                                                        */
/* PARAMS IN:  cmd_log                                                                                    */
/*                                                                                                        */
/*             tag                                                                                        */
/*                                                                                                        */
/*             state                                                                                      */
/*                                                                                                        */
/*             status                                                                                     */
/*                                                                                                        */
/*             update_pid                                                                                 */
/*                                                                                                        */
/*             pid                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int update_cmd_log_entry(cmd_log_object_t *cmd_log_object,
                         char *agent_log_pathname,
                         Iu32 tag,
                         Iu32 state,
                         Iu32 status,
                         bool_t update_pid,
                         Iu32 pid)
{

#undef  SUBNAME
#define SUBNAME "update_cmd_log_entry"

    int index;
    char msg[MAX_MSG_LEN];
    char prefix[MAX_MSG_LEN];
    int rc = 0;

    // sprintf(msg, "updat_cmd_log_entry called - state %d\n", state);
    // print_msg_to_console(msg);

    lock_cmd_log(cmd_log_object, SUBNAME);

    if ((index = is_tag_in_cmd_log(cmd_log_object, tag)) != -1)
    {

        snprintf(prefix, sizeof(prefix), "Update cmd_log: tag=%u", tag);

        if (update_pid)
        {
            sprintf(msg, "tag=%d, pid=%d, setting state=%d, status=%d\n", tag, pid, state, status);
        }
        else
        {
            sprintf(msg, "tag=%d, pid=%d, setting state=%d, status=%d\n", tag, cmd_log_object->cmd_array[index].pid, state, status);
        }

        log_msg_to_scenario_console_log(agent_log_pathname, prefix, msg, FALSE);

        cmd_log_object->cmd_array[index].tag        = tag;
        cmd_log_object->cmd_array[index].state      = state;
        cmd_log_object->cmd_array[index].status     = status;

        if ((state == CMD_COMPLETED) || (state == CMD_COMPLETED_TIMELIMIT))
        {
            cmd_log_object->cmd_array[index].end_time   = time(0);
        }

        if (update_pid)
        {
            cmd_log_object->cmd_array[index].pid = pid;
        }

    }
    else
    {

        printf("update_cmd_log_entry: problem - search for tag %u not found in cmd log\n", tag);  /* <<<<<<<<<<< */
        rc = 1;

    }

    unlock_cmd_log(cmd_log_object, SUBNAME);

    return rc;

}




/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  print_cmd_log_fragment                                                                     */
/*                                                                                                        */
/* PARAMS IN:  cmd_log                                                                                    */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_cmd_log_fragment(cmd_log  *cmd_log,
                            bool_t   no_headers,
                            bool_t   use_delimiter,
                            char     *delimiter_string)
{

#undef  SUBNAME
#define SUBNAME "print_cmd_log_fragment"

    struct tm         timestruct;
    char              time_string1[40];
    char              time_string2[40];
    char              time_string3[40];
    time_t            this_time;
    cmd_log_entry     *next_entry;
    char              msg[MAX_MSG_LEN];
    char              state_msg[MAX_MSG_LEN];
    int               system_exit_status;
    int               system_signal;
    char              system_exit_string[16];
    char              system_signal_string[16];

    char              *fmt_title       = "%28s  %28s  %28s  %16s  %8s %8s  %13s  %6s  %12s  %6s  %16s  %12s %s\n";
    char              *fmt1            = "%28s  %28s  %28s  %16d  %8d %8d  %13s  0x%04x  %12s  %6s  %16s  %12d %s\n";
    char              *fmt_title_delim = "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n";
    char              *fmt1_delim      = "%s%s%s%s%s%s%d%s%d%s%d%s%s%s%d%s%s%s%s%s%s%s%d%s%s\n";

    next_entry = cmd_log->first;

    if (! no_headers)
    {
        if (use_delimiter)
        {
            snprintf(msg, sizeof(msg), fmt_title_delim, "start", delimiter_string, "end", delimiter_string, "timeout_at", delimiter_string, "fail_on_timeout", delimiter_string,
                     "tag", delimiter_string, "pid",
                     delimiter_string, "state", delimiter_string, "status", delimiter_string,
                     "exit_code", delimiter_string, "signal", delimiter_string, "ident", delimiter_string, "workqueueID", delimiter_string, "cmd");
        }
        else
        {
            snprintf(msg, sizeof(msg), fmt_title, "start", "end", "timeout_at", "fail_on_timeout", "tag", "pid", "state", "status", "exit_code", "signal", "ident", "workqueueID", "cmd");
        }

        print_string_to_console(msg);
    }

    while  (next_entry != NULL)
    {

        if (next_entry->start_time == 0)
        {
            safecpy(time_string1, "", sizeof(time_string1));
        }
        else
        {
            this_time = (time_t) next_entry->start_time;

            if (localtime_r( &this_time, &timestruct) == NULL)
            {
                sprintf(msg, "bad localtime_r %d %s\n", errno, strerror(errno));
                print_msg_to_console(msg);
            }
            else
            {
                strftime(time_string1, sizeof(time_string1), "%c %Z", &timestruct);
            }
        }

        if (next_entry->end_time == 0)
        {
            safecpy(time_string2, "", sizeof(time_string2));
        }
        else
        {
            this_time = (time_t) next_entry->end_time;

            if (localtime_r( &this_time, &timestruct) == NULL)
            {
                sprintf(msg, "bad localtime_r %d %s\n", errno, strerror(errno));
                print_msg_to_console(msg);
            }
            else
            {
                strftime(time_string2, sizeof(time_string2), "%c %Z", &timestruct);
            }
        }

        if (next_entry->timeout_at_time == 0)
        {
            safecpy(time_string3, "", sizeof(time_string3));
        }
        else
        {
            this_time = (time_t) next_entry->timeout_at_time;

            if (localtime_r( &this_time, &timestruct) == NULL)
            {
                sprintf(msg, "bad localtime_r %d %s\n", errno, strerror(errno));
                print_msg_to_console(msg);
            }
            else
            {
                strftime(time_string3, sizeof(time_string3), "%c %Z", &timestruct);
            }
        }

        cmd_log_state_decode(next_entry->state, state_msg, sizeof(state_msg));

        safecpy(system_exit_string,   "na", sizeof(system_exit_string));
        safecpy(system_signal_string, "na", sizeof(system_signal_string));

        if (WIFEXITED(next_entry->status))
        {
            system_exit_status = WEXITSTATUS(next_entry->status);
            snprintf(system_exit_string, sizeof(system_exit_string), "%d", system_exit_status);
        }
        else if (WIFSIGNALED(next_entry->status))
        {
            system_signal = WTERMSIG(next_entry->status);
            snprintf(system_signal_string, sizeof(system_signal_string), "%d", system_signal);
        }

        if (use_delimiter)
        {
            snprintf(msg, sizeof(msg), fmt1_delim, time_string1, delimiter_string, time_string2, delimiter_string,
                     next_entry->tag, delimiter_string, time_string3, delimiter_string,
                     next_entry->fail_on_timeout, delimiter_string,
                     next_entry->pid, delimiter_string,
                     state_msg, delimiter_string, next_entry->status, delimiter_string,
                     system_exit_string, delimiter_string, system_signal_string, delimiter_string,
                     next_entry->identstring, delimiter_string, next_entry->workqueueID,
                     delimiter_string, next_entry->cmdstring);
        }
        else
        {
            snprintf(msg, sizeof(msg), fmt1, time_string1, time_string2, time_string3, next_entry->fail_on_timeout, next_entry->tag, next_entry->pid,
                     state_msg, next_entry->status,
                     system_exit_string, system_signal_string,
                     next_entry->identstring, next_entry->workqueueID, next_entry->cmdstring);
        }

        print_string_to_console(msg);

        next_entry = next_entry->next;

    }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  print_cmd_log                                                                              */
/*                                                                                                        */
/* PARAMS IN:  cmd_log_object                                                                             */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_cmd_log(cmd_log_object_t  *cmd_log_object,
                   bool_t            no_headers,
                   bool_t            use_delimiter,
                   char              *delimiter_string)
{

#undef  SUBNAME
#define SUBNAME "print_cmd_log"

    int               i;
    struct tm         timestruct;
    char              time_string1[40];
    char              time_string2[40];
    char              time_string3[40];
    time_t            this_time;
    char              msg[MAX_MSG_LEN];
    char              state_msg[MAX_MSG_LEN];
    int               system_exit_status;
    int               system_signal;
    char              system_exit_string[16];
    char              system_signal_string[16];

    char              *fmt_title       = "%28s  %28s  %28s  %16s  %8s  %5s  %13s  %6s  %12s  %6s %14s %s\n";
    char              *fmt1            = "%28s  %28s  %28s  %16 d %8d  %5d %13s 0x%04x  %12s  %6s %14d %s\n";
    char              *fmt_title_delim = "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n";
    char              *fmt1_delim      = "%s%s%s%s%s%s%d%s%d%s%d%s%s%s%d%s%s%s%s%s%s%d%s\n";


    if (! no_headers)
    {
        if (use_delimiter)
        {
            snprintf(msg, sizeof(msg), fmt_title_delim, "start", delimiter_string, "end", delimiter_string, "timeout_at", delimiter_string, "fail_on_timeout", delimiter_string,
                     "pid", delimiter_string, "state", delimiter_string, "status", delimiter_string,
                     "exit_code", delimiter_string, "signal", delimiter_string, "workqueueID", "cmd");
        }
        else
        {
            snprintf(msg, sizeof(msg), fmt_title, "start", "end", "timeout_at", "fail_on_timeout", "pid", "tag", "state", "status", "exit_code", "signal", "workqueueID", "cmd");
        }

        print_string_to_console(msg);
    }

    lock_cmd_log(cmd_log_object, SUBNAME);

    for  (i = 0; i< DAF_MAX_COMMANDS; i++)
    {

        if (cmd_log_object->cmd_array[i].start_time == 0)
        {
            safecpy(time_string1, "", sizeof(time_string1));
        }
        else
        {
            this_time = (time_t) cmd_log_object->cmd_array[i].start_time;;

            if (localtime_r(&this_time, &timestruct) == NULL)
            {
                sprintf(msg, "bad localtime_r %d %s\n", errno, strerror(errno));
                print_msg_to_console(msg);
            }
            else
            {
                strftime(time_string1, sizeof(time_string1), "%c %Z", &timestruct);
            }
        }

        if (cmd_log_object->cmd_array[i].end_time == 0)
        {
            safecpy(time_string2, "", sizeof(time_string2));
        }
        else
        {
            this_time = (time_t) cmd_log_object->cmd_array[i].end_time;;

            if (localtime_r(&this_time, &timestruct) == NULL)
            {
                sprintf(msg, "bad localtime_r %d %s\n", errno, strerror(errno));
                print_msg_to_console(msg);
            }
            else
            {
                strftime(time_string2, sizeof(time_string2), "%c %Z", &timestruct);
            }
        }

        if (cmd_log_object->cmd_array[i].timeout_at_time == 0)
        {
            safecpy(time_string3, "", sizeof(time_string3));
        }
        else
        {
            this_time = (time_t) cmd_log_object->cmd_array[i].end_time;;

            if (localtime_r(&this_time, &timestruct) == NULL)
            {
                sprintf(msg, "bad localtime_r %d %s\n", errno, strerror(errno));
                print_msg_to_console(msg);
            }
            else
            {
                strftime(time_string3, sizeof(time_string3), "%c %Z", &timestruct);
            }
        }

        cmd_log_state_decode(cmd_log_object->cmd_array[i].state, state_msg, sizeof(state_msg));

        cmd_log_state_decode(cmd_log_object->cmd_array[i].state, state_msg, sizeof(state_msg));

        safecpy(system_exit_string,   "na", sizeof(system_exit_string));
        safecpy(system_signal_string, "na", sizeof(system_signal_string));

        if (WIFEXITED(cmd_log_object->cmd_array[i].status))
        {
            system_exit_status = WEXITSTATUS((int) cmd_log_object->cmd_array[i].status);
            snprintf(system_exit_string, sizeof(system_exit_string), "%d", system_exit_status);
        }
        else if (WIFSIGNALED(cmd_log_object->cmd_array[i].status))
        {
            system_signal = WTERMSIG(cmd_log_object->cmd_array[i].status);
            snprintf(system_signal_string, sizeof(system_signal_string), "%d", system_signal);
        }

        if (use_delimiter)
        {
            snprintf(msg, sizeof(msg), fmt1_delim, time_string1, delimiter_string, time_string2, delimiter_string,
                     time_string3, delimiter_string,  cmd_log_object->cmd_array[i].fail_on_timeout, delimiter_string,
                     cmd_log_object->cmd_array[i].pid, delimiter_string,
                     cmd_log_object->cmd_array[i].tag, delimiter_string,
                     state_msg, delimiter_string,
                     cmd_log_object->cmd_array[i].status, delimiter_string,
                     system_exit_string, delimiter_string, system_signal_string, delimiter_string,
                     cmd_log_object->cmd_array[i].workqueueID, delimiter_string, cmd_log_object->cmd_array[i].cmdstring);
        }
        else
        {
            snprintf(msg, sizeof(msg), fmt1, time_string1, time_string2, time_string3, cmd_log_object->cmd_array[i].fail_on_timeout,
                     cmd_log_object->cmd_array[i].pid,
                     cmd_log_object->cmd_array[i].tag,
                     state_msg, cmd_log_object->cmd_array[i].status,
                     system_exit_string, system_signal_string,
                     cmd_log_object->cmd_array[i].workqueueID,
                     cmd_log_object->cmd_array[i].cmdstring);
        }

        print_string_to_console(msg);

    }

    unlock_cmd_log(cmd_log_object, SUBNAME);

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  get_next_unique_ident                                                                      */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void get_next_unique_ident(char *NewIdentValue, int max_NewIdentValue_len)
{

    static unsigned int IdentifierPoolCount = 0;
    int hoststringlen, xstringlen, i,j;
    char IdentNumber[10];

    memset(NewIdentValue, 'x', max_NewIdentValue_len);

    sprintf(IdentNumber, "%05d", IdentifierPoolCount);    /* <<<<<<<<<<< need a lock around these two statements */
    IdentifierPoolCount++;

    hoststringlen = (max_NewIdentValue_len-1) - strlen(IdentNumber);

    if (strlen(ThisHostName) <= (size_t) hoststringlen)
    {
        xstringlen    = hoststringlen - strlen(ThisHostName);
        hoststringlen = strlen(ThisHostName);
        j = 0;
    }
    else
    {
        xstringlen = 0;
        j = strlen(ThisHostName) - 1 - hoststringlen;
    }

    for (i=0; i < hoststringlen; i++)
    {
        NewIdentValue[i] = ThisHostName[j+i];
    }

    for (i=0; i < xstringlen; i++ )
    {
        NewIdentValue[i+hoststringlen] = 'x';
    }

    for (i=0; i < (int) strlen(IdentNumber); i++ )
    {
        NewIdentValue[i+hoststringlen+xstringlen] = IdentNumber[i];
    }

    NewIdentValue[max_NewIdentValue_len-1] = 0;

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  initialise_service_environment                                                             */
/*                                                                                                        */
/* PARAMS IN:  none                                                                                       */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void initialise_service_environment(void)
{

    char msg[MAX_MSG_LEN];
    char errmsg[MAX_MSG_LEN];

    //  init_timers();   TODO do we need this somewhere ?

    if (! get_shared_segment_process_pointer(&p_shared_memory_base_pointer, errmsg, sizeof(errmsg)))
    {

        snprintf(msg, sizeof(msg), "Problem starting daf service: %s - exiting\n", errmsg);
        print_msg_to_console(msg);
        exit(1);

    }

    if (! find_cmd_log(&p_service_cmd_log_object, errmsg, sizeof(errmsg)))
    {

        snprintf(msg, sizeof(msg), "Problem starting daf service: %s - exiting\n", errmsg);
        print_msg_to_console(msg);
        exit(1);

    }

    if (! find_active_steps(&p_service_active_steps_object, errmsg, sizeof(errmsg)))
    {

        snprintf(msg, sizeof(msg), "Problem starting daf service: %s - exiting\n", errmsg);
        print_msg_to_console(msg);
        exit(1);

    }

    get_short_hostname(ThisHostName, sizeof(ThisHostName));

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_query_cntrl_1_svc                                                            */
/*                                                                                                        */
/* PARAMS IN:  rpc_client     structure that describes the rpc client and the NFS server it is to be      */
/*                                                                                                        */
/*             rqstp                                                                                      */
/*                                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cmd_res                                                                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_cntrl_1_svc(remote_client_cntrl_args  *args,
                                        remote_client_cntrl_res   *result,
                                        struct svc_req            *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_cntrl_1_svc"

    char  msg[MAX_MSG_LEN];
    char  errmsg[MAX_MSG_LEN];
    int   rc;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    if (args->clear_shared_memory_flag)
    {

        if ((rc = initialise_shared_segment_values(errmsg, sizeof(errmsg))) != E_OK)
        {
            snprintf(msg, sizeof(msg), "%s: problem with clear_shared_memory(): %s\n", SUBNAME, errmsg);
            print_msg_to_console(msg);
        }

    }

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

void make_agent_log_pathname(char *hostname, int actionresultID, char *agent_log_pathname, int max_agent_log_pathname_len)
{

    snprintf(agent_log_pathname, max_agent_log_pathname_len, DAF_SERVICE_LOG_DIR "/agent_log_%s_%u.log",
             hostname, actionresultID);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_cmd_client_prepare_cmd_1_svc                                                 */
/*                                                                                                        */
/* PARAMS IN:  rpc_client     structure that describes the rpc client and the NFS server it is to be      */
/*                                                                                                        */
/*             rqstp                                                                                      */
/*                                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cmd_res                                                                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_prepare_cmd_1_svc(remote_client_prepare_cmd_args  *args,
        remote_client_prepare_cmd_res   *result,
        struct svc_req                  *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_prepare_cmd_1_svc"

    char   prefix[MAX_MSG_LEN];
    char   errmsg[MAX_MSG_LEN];
    char   msg[MAX_MSG_LEN];
    int    tag;
    Iu32   timeout_at_time;

    char agent_log_pathname[MAX_PATHNAME_LEN];

    make_agent_log_pathname(ThisHostName, args->actionresultID, agent_log_pathname, sizeof(agent_log_pathname));

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */
    tag = get_next_free_tag();

    snprintf(prefix, sizeof(prefix), "Preparing: tag=%u", tag);
    log_msg_to_scenario_console_log(agent_log_pathname, prefix, args->cmdstring, TRUE);


    /* --------------------------------------------------------------------------------------------------------------- */
    /* A timeout may be applied by the user to the system command that is to be run.  This can be                      */
    /* 0  - the timeout is in effect infinitiy, and the system command will be allowed to run forever until it either  */
    /*      terminates itself, or until a cancel request is received by the daf daemon                                 */
    /* N  - the timeout is N seconds, so we make an entry in the command log that has the value T+N, where T is the    */
    /*      the current time in seconds, to indicate when the command is to be timed out.  The zombie_reaper() routine */
    /*      then can periodically examine the command log queue and see if there are any entries in it that have       */
    /*      timeout_at entries that are less than the current time - if so, these commands have exceeded their timeout */
    /*      and will be killed by the zombie_reaper().                                                                 */
    /* A timeout can be applied in two ways:                                                                           */
    /* fail_on_timeout = 1     indicates the command will be treated as a failed command if it exceeds its timeout     */
    /* fail_on_timeout = 0     indicates that the user wanted the command to run for a set period and if it is still   */
    /*                         running at the end of this period, that is not to be considered an error (eg you might  */
    /*                         run a testcase that does read-write i/o operations and simply want it to run for 10     */
    /*                         10 mins, but you don't know how many read-writes would actually be done, so you set     */
    /*                         the number to a very big number and use the timeout mechanism to end the testcase after */
    /*                         the desired 10 mins                                                                     */
    /* --------------------------------------------------------------------------------------------------------------- */

    if (args->timeout == 0)
    {
        timeout_at_time = 0;
    }
    else
    {
        timeout_at_time = (Iu32) time(0) + args->timeout;
    }

    if (add_cmd_log_entry(p_service_cmd_log_object, args->cmdstring, args->identstring, agent_log_pathname, tag, args->workqueueID,
                          getpid(), CMD_INITIALISED, 0, time(0), 0, timeout_at_time, args->fail_on_timeout,
                          errmsg, sizeof(errmsg)) != E_OK)
    {
        snprintf(msg, sizeof(msg), "could not add cmd %s to command log: %s\n", args->cmdstring, errmsg);
        log_msg_to_scenario_console_log(agent_log_pathname, SUBNAME, msg, FALSE);
        result->status = REMOTE_CMD_COULD_NOT_ADD_TO_CMD_LOG;
        snprintf(msg, sizeof(msg), "prepare_cmd: internal error: add_cmd_log_entry() failed): tag=%u, actionresultID=%u\n",
                 tag, args->actionresultID);
        log_msg_to_scenario_console_log(agent_log_pathname, prefix, msg, TRUE);
        /* problem - what do we do now ?? <<<< */
    }
    else
    {
        result->status = REMOTE_CMD_OK;
    }

    result->remote_client_prepare_cmd_res_u.outcome.tag = tag;
    return (TRUE);

}

#if defined AIX || defined LINUX || defined MACOSX || defined SOLARIS || defined HPUX
/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           ServiceCloseFDs                                     */
/*                                                                      */
/*  FUNCTION:       Closes all open file descriptors upwards of a       */
/*                  given starting point.                               */
/*                                                                      */
/*  PARAMS IN:      from         The first file descriptor value to     */
/*                               consider closing.                      */
/*                                                                      */
/*  RETURNS:        A count of file descriptors closed.                 */
/*                                                                      */
/*----------------------------------------------------------------------*/
int ServiceCloseFDs(int from)
{
    int result = 0;
    int to = sysconf(_SC_OPEN_MAX);

    // assert(from >= 0);

    for (; from < to ; ++from)
    {
        /* attempt to close(2) fd from ignoring errors, but keeping
         * count of successes. */
        if (!close(from))
        {
            result++;
        }
    }

    return result;
}
#endif


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_cmd_client_execute_cmd_1_svc                                                 */
/*                                                                                                        */
/*             rqstp                                                                                      */
/*                                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cmd_res                                                                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_execute_cmd_1_svc(remote_client_execute_cmd_args  *args,
        remote_client_execute_cmd_res   *result,
        struct svc_req                  *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_execute_cmd_1_svc"

    char   msg[MAX_MSG_LEN];
    char   prefix[MAX_MSG_LEN];
    int    rc;
    pid_t  pid, session_id;

#define MAX_NUM_ENVIRONMENT_VALUES 100
    char *envp[MAX_NUM_ENVIRONMENT_VALUES+1];

    char agent_log_pathname[MAX_PATHNAME_LEN];
    make_agent_log_pathname(ThisHostName, args->actionresultID, agent_log_pathname, sizeof(agent_log_pathname));

    snprintf(msg, sizeof(msg), "execute_cmd: tag=%u, actionresultID=%u\n", args->tag, args->actionresultID);
    log_msg_to_scenario_console_log(agent_log_pathname, SUBNAME, msg, FALSE);

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */


    if ((pid = fork()) == 0)
    {

        /* -------------------------------------------------------------------------------- */
        /* here we are running in the child - we cannot rely on the zombie_reaper()         */
        /* routine to monitor the progress of this child as in the -multiprocess_server     */
        /* mode we are a child of a child and the zombie_reaper cannot monitor this process */
        /* So we allow the child process to update the cmd_log itself - however if this     */
        /* child process dies we won't be able to update the cmd_log so we may need to run  */
        /* a reaper at for this child level too                                             */ /* <<<<<<<<<<<<<<<<<<< */
        /* -------------------------------------------------------------------------------- */

        if (! get_shared_segment_process_pointer(&p_shared_memory_base_pointer, msg, sizeof(msg)))
        {

            snprintf(msg, sizeof(msg), "client_remote_client_execute_cmd_1_svc: Problem in daf service getting shared segment pointer: %s - exiting\n", msg);
            print_msg_to_console(msg);
            exit(1);

        }

        if (! find_cmd_log(&p_service_cmd_log_object, msg, sizeof(msg)))
        {

            snprintf(msg, sizeof(msg), "client_remote_client_execute_cmd_1_svc: Problem in daf service finding command log: %s - exiting\n", msg);
            print_msg_to_console(msg);
            exit(1);

        }

        /* -------------------------------------------------------------------------------- */
        /* Make ourselves the process group leader - we will then be the only process in    */
        /* this new process and any child process that we create will also belong to        */
        /* our process group by default.  So if we need to kill all the processes           */
        /* associated with this fork we can do it by referencing the process group id       */
        /* -------------------------------------------------------------------------------- */

        session_id = setsid();

        if (update_cmd_log_entry(p_service_cmd_log_object, agent_log_pathname, args->tag, CMD_BG_RUNNING, 0, TRUE, getpid()) != 0)
        {
            snprintf(msg, sizeof(msg), "execute_cmd: internal error: update cmd_log() failed: tag=%u, actionresultID=%u\n", args->tag, args->actionresultID);
            log_msg_to_scenario_console_log(agent_log_pathname, SUBNAME, msg, FALSE);
            /* problem - what do we do now ?? <<<< */
        }

        /* --------------------------------------------------------------------------------
           * Close all open fds except for std{in,out,err}.  Most importantly
           * we want to ensure that this process (or sub-thread thereof) does
           * not keep the services socket(s) open.
           * ----------------------------------------------------------------------------- */
        ServiceCloseFDs(STDERR_FILENO + 1);

        int fd_stdout; /*file descriptor to the file we will redirect stdout to in the exec */
        int fd_stderr; /*file descriptor to the file we will redirect stderr to in the exec */
        char stdout_pathname[64];
        char stderr_pathname[64];
        char testcasename[MAX_SYSTEM_CMD_LEN];

        make_host_and_time_specific_pathname_prefix(DAF_SERVICE_LOG_DIR, ThisHostName, args->actionresultID, testcasename, "stdout", stdout_pathname, sizeof(stdout_pathname));

        if ((fd_stdout = open(stdout_pathname, O_RDWR | O_CREAT))==-1)  /*open the file */
        {
            snprintf(msg, sizeof(msg), "execute_cmd: internal error: open() for stdout failed (errno=%u, %s): tag=%u, actionresultID=%u\n",
                     errno, strerror(errno), args->tag, args->actionresultID);
            log_msg_to_scenario_console_log(agent_log_pathname, SUBNAME,  msg, FALSE);
            /* problem - what do we do now ?? <<<< */
        }
        else
        {
            snprintf(prefix, sizeof(prefix), "Executing: stdout_pathname=");
            log_msg_to_scenario_console_log(agent_log_pathname, prefix, stdout_pathname, TRUE);
        }

        make_host_and_time_specific_pathname_prefix(DAF_SERVICE_LOG_DIR, ThisHostName, args->actionresultID, testcasename, "stderr", stderr_pathname, sizeof(stderr_pathname));

        if ((fd_stderr = open(stderr_pathname, O_RDWR | O_CREAT))==-1)  /*open the file */
        {
            snprintf(msg, sizeof(msg), "execute_cmd: internal error: open() for stderr failed (errno=%u, %s): tag=%u, actionresultID=%u\n",
                     errno, strerror(errno), args->tag, args->actionresultID);
            log_msg_to_scenario_console_log(agent_log_pathname, SUBNAME, msg, FALSE);
            /* problem - what do we do now ?? <<<< */
        }
        else
        {
            snprintf(prefix, sizeof(prefix), "Executing: stderr_pathname=");
            log_msg_to_scenario_console_log(agent_log_pathname, prefix, stderr_pathname, TRUE);
        }

        /* -------------------------------------------------------------------------------- */
        /* make a copy of the current state of stdout and sderr so that we can restore this */
        /* state later                                                                      */
        /* -------------------------------------------------------------------------------- */
        //    int savedstdoutfd;
        //    int savedstderrfd;
        //    savedstdoutfd = dup(1);
        //    savedstderrfd = dup(2);


        dup2(fd_stdout, STDOUT_FILENO); /*copy the file descriptor fd into standard output*/
        close(fd_stdout);
        dup2(fd_stderr, STDERR_FILENO); /* same, for the standard error */
        close(fd_stderr);

        if (args->run_in_shell )
        {

            snprintf(msg, sizeof(msg), "Executing in shell: tag=%u, cmd=", args->tag);
            log_msg_to_scenario_console_log(agent_log_pathname, msg, args->cmdstring, TRUE);

            envlist p = args->environment_settings;

            if (p == NULL)
            {
                log_msg_to_scenario_console_log(agent_log_pathname, "Executing in shell:", "No environment variables provided", TRUE);
            }

            while (p != NULL)
            {
                snprintf(prefix, sizeof(prefix), "Environment: ");
                log_msg_to_scenario_console_log(agent_log_pathname, prefix, p->entry, TRUE);
                putenv(p->entry);
                p = p->next;
            }

            rc = system(args->cmdstring);

            snprintf(prefix, sizeof(prefix), "Completed: tag=%u, ", args->tag);
            snprintf(msg, sizeof(msg), "rc=0x%02x, signal=0x%02x for session id=%u\n", (rc / 256 ), (rc && 0xff), (Iu32) session_id);
            log_msg_to_scenario_console_log(agent_log_pathname, prefix, msg, TRUE);

        }
        else
        {

            snprintf(prefix, sizeof(prefix), "Executing in environment: tag=%u", args->tag);
            log_msg_to_scenario_console_log(agent_log_pathname, prefix, args->cmdstring, TRUE);

            envlist p = args->environment_settings;
            int i = 0;

            snprintf(prefix, sizeof(prefix), "Environment: tag=%u", args->tag);

            while (p != NULL)
            {
                envp[i] = p->entry;
                snprintf(prefix, sizeof(prefix), "Environment: ");
                log_msg_to_scenario_console_log(agent_log_pathname, prefix, p->entry, TRUE);
                i++;

                if (i == MAX_NUM_ENVIRONMENT_VALUES)
                {
                    snprintf(prefix, sizeof(prefix), "Environment: Maximum number (%d) of Environment values reached - further values will be discarded", MAX_NUM_ENVIRONMENT_VALUES);
                    log_msg_to_scenario_console_log(agent_log_pathname, prefix, p->entry, TRUE);
                }

                p = p->next;
            }

            envp[i] = NULL;

            int cmdstring_length;

            i = 0;
            cmdstring_length = strlen(args->cmdstring);

            if (cmdstring_length >= sizeof(testcasename))
            {
                cmdstring_length = sizeof(testcasename- 1);
            }

            while ((i <= cmdstring_length) && (args->cmdstring[i] != ' '))
            {
                testcasename[i] = args->cmdstring[i];
                i++;
            }

            testcasename[i] = 0;

            rc = execle("/bin/sh", "sh", "-c", args->cmdstring, NULL, &envp);

            printf("rc = %d, errno %d\n", rc, errno);   /* this will go to where fd_stdout was opened */
            /* so if we did not exec - the screen output is no longer to the screen - should we put it back ? <<<< */
            /* <<<<< do we need to update the CMD log here with a completion status? , or should we skp the following exit? */
            // _exit(rc);

            snprintf(prefix, sizeof(prefix), "INTERNAL ERROR: tag=%u", args->tag);
            snprintf(msg, sizeof(msg), "%s: execle gave rc = %d, errno = %d\n", SUBNAME, rc, errno);
            log_msg_to_scenario_console_log(agent_log_pathname, prefix, msg, FALSE);

        }

        // we only get here if we were running with system()

        /* -------------------------------------------------------------------------------- */
        /* Close the files that contain the output from this system command                 */
        /* We don't need the copy of the saved handles for the original stdout/stderr either*/
        /* so those are closed too                                                          */
        /* -------------------------------------------------------------------------------- */

        close(fd_stdout);                    // <<<<<< ?????????????
        close(fd_stderr);
        //         close(savedstdoutfd);
        //         close(savedstderrfd);


        unsigned int pid_exit_code = rc / 256;  /* <<<<<<<<< note, we are not returning the signal if one exists - who catches this? <<<<*/
        /* unsigned int pid_signal    = rc & 0xff;   */

        _exit(pid_exit_code);

    }

    /* in the parent, we don't see the command complete as it is still running - so just return good status */
    /* <<< what should we do if the fork fails ??? */

    /* -------------------------------------------------------------------------------- */
    /* Close the files that contain the output from this system command                 */
    /* We don't need the copy of the saved handles for the original stdout/stderr either*/
    /* so those are closed too                                                          */
    /* -------------------------------------------------------------------------------- */

    //     close(fd_stdout);
    //     close(fd_stderr);

    //     dup2(savedstdoutfd, 1);
    //     dup2(savedstderrfd, 2);
    //
    //     close(savedstdoutfd);
    //     close(savedstdoutfd);

    result->remote_client_execute_cmd_res_u.outcome.valid = 0;
    result->status = REMOTE_CLIENT_OK;
    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_cmd_client_cmd_1_svc                                                         */
/*                                                                                                        */
/* PARAMS IN:  rpc_client     structure that describes the rpc client and the NFS server it is to be      */
/*                                                                                                        */
/*             rqstp                                                                                      */
/*                                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cmd_res                                                                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_cmd_1_svc(remote_client_cmd_args  *args,
                                      remote_client_cmd_res   *result,
                                      struct svc_req          *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_cmd_1_svc"

    char   msg[MAX_MSG_LEN];
    char   errmsg[MAX_MSG_LEN];
    int    rc;
    int    system_exit_status;
    int    system_signal;
    pid_t  pid, session_id;
    Iu32   tag;
    Iu32   timeout_at_time;

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */
    tag = get_next_free_tag();

    snprintf(msg, sizeof(msg), "started: tag=%d, cmdstring=%s\n", tag, args->cmdstring);
    print_msg_to_console(msg);


    /* --------------------------------------------------------------------------------------------------------------- */
    /* A timeout may be applied by the user to the system command that is to be run.  This can be                      */
    /* 0  - the timeout is in effect infinitiy, and the system command will be allowed to run forever until it either  */
    /*      terminates itself, or until a cancel request is received by the daf daemon                                 */
    /* N  - the timeout is N seconds, so we make an entry in the command log that has the value T+N, where T is the    */
    /*      the current time in seconds, to indicate when the command is to be timed out.  The zombie_reaper() routine */
    /*      then can periodically examine the command log queue and see if there are any entries in it that have       */
    /*      timeout_at entries that are less than the current time - if so, these commands have exceeded their timeout */
    /*      and will be killed by the zombie_reaper().                                                                 */
    /* A timeout can be applied in two ways:                                                                           */
    /* fail_on_timeout = 1     indicates the command will be treated as a failed command if it exceeds its timeout     */
    /* fail_on_timeout = 0     indicates that the user wanted the command to run for a set period and if it is still   */
    /*                         running at the end of this period, that is not to be considered an error (eg you might  */
    /*                         run a testcase that does read-write i/o operations and simply want it to run for 10     */
    /*                         10 mins, but you don't know how many read-writes would actually be done, so you set     */
    /*                         the number to a very big number and use the timeout mechanism to end the testcase after */
    /*                         the desired 10 mins                                                                     */
    /* --------------------------------------------------------------------------------------------------------------- */

    if (args->timeout == 0)
    {
        timeout_at_time = 0;
    }
    else
    {
        timeout_at_time = time(0) + args->timeout;
    }

    if (add_cmd_log_entry(p_service_cmd_log_object, args->cmdstring, args->identstring, "", tag, args->workqueueID,
                          getpid(), CMD_INITIALISED, 0, time(0), 0, timeout_at_time, args->fail_on_timeout,
                          errmsg, sizeof(errmsg)) != E_OK)
    {
        snprintf(msg, sizeof(msg), "could not add cmd %s to command log: %s\n", args->cmdstring, errmsg);
        print_msg_to_console(msg);
    }

    if (! args->run_in_background)
    {

        /* -------------------------------------------------------------------------------- */
        /* Even though we are not running in the background, we fork a process so that      */
        /* the system() command runs in a child process of the the service process - that   */
        /* means it is in a separate process to the service process and can be killed       */
        /* if desired without killing the service process - (if we just called system()     */
        /* directly from the service process we would not know the child process that       */
        /* system was using to run the system cmd and so could not easily kill the child)   */
        /* -------------------------------------------------------------------------------- */

        if ((pid = fork()) == 0)
        {

            /* -------------------------------------------------------------------------------- */
            /* here we are running in the child                                                 */
            /* -------------------------------------------------------------------------------- */

            /* -------------------------------------------------------------------------------- */
            /* Make ourselves the process group leader - we will then be the only process in    */
            /* this new process and any child process that we create will also belong to        */
            /* our process group by default.  So if we need to kill all the processes           */
            /* associated with this fork we can do it by referencing the process group id       */
            /* -------------------------------------------------------------------------------- */

            session_id = setsid();

            rc = system(args->cmdstring);

            if (WIFEXITED(rc))
            {
                system_exit_status = WEXITSTATUS(rc);
                snprintf(msg, sizeof(msg), "command: %s, session_id %d exited with rc = 0x%x\n", args->cmdstring, (Iu32) session_id, system_exit_status);
                print_msg_to_console(msg);
            }
            else if (WIFSIGNALED(rc))
            {
                system_signal = WTERMSIG(rc);

                if (WCOREDUMP(rc))
                {
                    snprintf(msg, sizeof(msg), "command: %s, session_id %d was terminated with signal 0x%x and a system dump was produced\n",
                             args->cmdstring, (Iu32) session_id, system_signal);
                }
                else
                {
                    snprintf(msg, sizeof(msg), "command: %s, session_id %d was terminated with signal 0x%x - no system dump was produced\n",
                             args->cmdstring, (Iu32) session_id, system_signal);
                }

                print_msg_to_console(msg);
            }

            _exit(system_exit_status);

        }

        /* -------------------------------------------------------------------------------- */
        /* here we are running in the parent, having just started a child with PID = pid    */
        /* we will wait for the child to complete before returning status to the client     */
        /* machine that requested us to run the command                                     */
        /* -------------------------------------------------------------------------------- */

        if (update_cmd_log_entry(p_service_cmd_log_object, "", tag, CMD_RUNNING, 0, TRUE, pid) != 0)
        {
            printf("error 2\n");
        }

        waitpid(pid, &rc, 0);                                                                   /* <<< could this race with the waitpid in reaper ? */

        if (update_cmd_log_entry(p_service_cmd_log_object, "", tag, CMD_COMPLETED, rc, FALSE, 0) != 0)
        {
            /* <<< do we even need the waitpid in reaper */   printf("error 3\n");
        }

    }
    else
    {

        if ((pid = fork()) == 0)
        {

            /* -------------------------------------------------------------------------------- */
            /* here we are running in the child - we cannot rely on the zombie_reaper()         */
            /* routine to monitor the progress of this child as in the -multiprocess_server     */
            /* mode we are a child of a child and the zombie_reaper cannot monitor this process */
            /* So we allow the child process to update the cmd_log itself - however if this     */
            /* child process dies we won't be able to update the cmd_log so we may need to run  */
            /* a reaper at for this child level too                                             */ /* <<<<<<<<<<<<<<<<<<< */
            /* -------------------------------------------------------------------------------- */

            /* -------------------------------------------------------------------------------- */
            /* Make ourselves the process group leader - we will then be the only process in    */
            /* this new process and any child process that we create will also belong to        */
            /* our process group by default.  So if we need to kill all the processes           */
            /* associated with this fork we can do it by referencing the process group id       */
            /* -------------------------------------------------------------------------------- */

            session_id = setsid();

            if (update_cmd_log_entry(p_service_cmd_log_object, "", tag, CMD_BG_RUNNING, 0, TRUE, getpid()) != 0)
            {
                printf("error 4\n");
            }

            rc = system(args->cmdstring);

            snprintf(msg, sizeof(msg), "background command: %s, tag = %u, session_id %d returned rc = 0x%02x, signal = 0x%02x\n",
                     args->cmdstring, tag, (Iu32) session_id, (rc / 256 ), (rc && 0xff));
            print_msg_to_console(msg);

            //update_cmd_log_entry(p_service_cmd_log_object, tag, CMD_COMPLETED, rc, FALSE, 0);

            unsigned int pid_exit_code = rc / 256;  /* <<<<<<<<< note, we are not returning the signal if one exists - who catches this? <<<<*/
            unsigned int pid_signal    = rc & 0xff;

            snprintf(msg, sizeof(msg), "process %u completed with rc=%d, pid_exit_code=%u, pid_signal=%u\n", getpid(), rc, pid_exit_code, pid_signal);
            print_msg_to_console(msg);

            _exit(pid_exit_code);

        }

        /* -------------------------------------------------------------------------------- */
        /* here we are running in the parent, having just started a child with PID = pid    */
        /* -------------------------------------------------------------------------------- */

        rc = 0;  /* we don't have the child exit status yet - so just return 0 */

    }

    result->status = REMOTE_CLIENT_OK;
    result->remote_client_cmd_res_u.outcome.tag             = tag;
    result->remote_client_cmd_res_u.outcome.cmd_exit_code   = rc / 256;
    result->remote_client_cmd_res_u.outcome.cmd_exit_signal = rc & 0xff;
    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_cmd_client_run_cmd_1_svc                                                     */
/*                                                                                                        */
/* PARAMS IN:  rpc_client     structure that describes the rpc client and the NFS server it is to be      */
/*                                                                                                        */
/*             rqstp                                                                                      */
/*                                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cmd_res                                                                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_run_cmd_1_svc(remote_client_run_cmd_args  *args,
        remote_client_run_cmd_res   *result,
        struct svc_req              *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_run_cmd_1_svc"

    char   msg[MAX_MSG_LEN];
    int    rc = 0;
    FILE   *fp;
    int    status;
    char   line[1024];        /* <<<<<<<<<< 1024 ?????? */
    int    num_lines = 0;

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    if ((fp = popen(args->cmdstring, "r")) == NULL)
    {

        rc = 1;

    }
    else
    {

        while (fgets(line, sizeof(line), fp) != NULL)
        {
            num_lines++;
        }

        if (ferror(fp))
        {
            rc = 1;
        }

        status = pclose(fp);

        if (status == -1)
        {
            /* Error reported by pclose() */
            rc = 2;
        }
        else if (status != 0)
        {
            if (WIFEXITED(status))
            {
                /* child exited, status= WEXITSTATUS(status) */
                rc = 3;
            }
            else if (WIFSIGNALED(status))
            {
                /* child killed (signal = WTERMSIG(status) */
                rc = 4;
            }
            else if (WIFSTOPPED(status))
            {
                /* child stopped (signal %d)\n", WSTOPSIG(status) */
                rc = 5;
            }
            else        /* Non-standard case -- may never happen */
            {
                /* Unexpected status (0x%x), status */
                rc = 6;
            }
        }
    }

    if (num_lines != 1)
    {
        rc = 7;
    }

    if (rc != 0)
    {

        result->remote_client_run_cmd_res_u.outcome.valid      = 0;
        result->remote_client_run_cmd_res_u.outcome.outputline = duplicate_string("");  /* not allowed to return null pointer */

    }
    else
    {

        /* now we have a single line of output we can return, just need to strip the return at the end */
        chomp(line);
        result->remote_client_run_cmd_res_u.outcome.outputline = duplicate_string(line);
        result->remote_client_run_cmd_res_u.outcome.valid      = 1;

    }

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return(TRUE);

}

#ifndef DAF_AGENT

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  client_remote_client_start_scenario_1_svc                                                  */
/*                                                                                                        */
/*             rqstp                                                                                      */
/*                                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_start_scenario_res                                                           */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_start_scenario_1_svc(remote_client_start_scenario_args  *args,
        remote_client_start_scenario_res   *result,
        struct svc_req                     *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_start_scenario_1_svc"

    char   msg[MAX_MSG_LEN];
    char   errmsg[MAX_MSG_LEN];
    int    rc = 0;

    char query[2048];
    int testerID = 0;
    int scenarioID = 0;
    int scenarioresultID = 0;
    int testlevelID = 0;
    int teststandID = 0;
    int teststandrecordID = 0;
    int testlevelrecordID = 0;
    int num_actions_in_scenario;
    MYSQL *sql_connection;

    char status_database_name[100]; // TODO
    char status_table_name[100];   // TODO
    char loglocation[100];  // TODO

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    result->remote_client_start_scenario_res_u.outcome.valid_start_scenario = REMOTE_START_SCENARIO_OK;
    result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string("no error");

    // here we need to kick off the scenario
    snprintf(msg, sizeof(msg), "-------------------Start scenario requested: Project: %s, Phase: %s, Scenario: %s, loglocation: %s, scenariologfile: %s, teststand: %s, level:%s\n",
             args->project, args->phase, args->jobname, args->loglocation, args->scenariologfile, args->teststand, args->testlevel);
    print_string_to_console(msg);

    determine_status_database_name(args->project, args->phase, status_database_name, sizeof(status_database_name));

    sql_connection = do_connect(SUBNAME,
                                sql_server.sql_servername,
                                sql_server.sql_username,
                                sql_server.sql_password,
                                "daf",
                                sql_server.sql_port,
                                sql_server.sql_socketname,
                                0,
                                errmsg,
                                sizeof(errmsg));

    if (sql_connection != NULL)
    {

        // need to check return code - TODO

        //     rc = lookupID(sql_connection, "user", args->username, &testerID, errmsg, sizeof(errmsg));
        //TODO
        testerID = 1;

        if (rc == 0)
        {
            if ((rc = lookupID(sql_connection, "scenario", args->jobname, &scenarioID, errmsg, sizeof(errmsg))) != 0)
            {
                snprintf(msg, sizeof(msg), "lookupID for scenario failed (rc=%d) - %s\n", rc, errmsg);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
            }

            if (scenarioID == 0)
            {
                snprintf(msg, sizeof(msg), "scenario ID not found for scenario %s\n", args->jobname);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                rc = 1;
            }
        }

        if (rc == 0)
        {
            if ((rc = lookupID(sql_connection, "testlevel", args->testlevel, &testlevelID, errmsg, sizeof(errmsg))) != 0)
            {
                snprintf(msg, sizeof(msg), "lookupID for testlevel failed (rc=%d) - %s\n", rc, errmsg);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
            }

            if (testlevelID == 0)
            {
                snprintf(msg, sizeof(msg), "testlevel ID not found for testlevel %s\n", args->testlevel);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                rc = 1;
            }
        }

        if (rc == 0)
        {
            if ((rc = lookupID(sql_connection, "teststand", args->teststand, &teststandID, errmsg, sizeof(errmsg))) != 0)
            {
                snprintf(msg, sizeof(msg), "lookupID for teststand failed (rc=%d) - %s\n", rc, errmsg);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                rc = 1;
            }

            if (teststandID == 0)
            {
                snprintf(msg, sizeof(msg), "teststand ID not found for teststand %s\n", args->teststand);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                rc = 1;
            }
        }

        if (rc == 0)
        {
            // TODO Scenario and Jobname both exist - should they be the same ?
            determine_status_table_name(status_database_name, "scenarioresult", status_table_name, sizeof(status_table_name));
            snprintf(query, sizeof(query), "INSERT INTO %s (Project, Phase, Scenarioname, Jobname, Teststand, Testlevel, Tester, Comments, State, Scenariologfilename) "
                     "VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",
                     status_table_name, args->project, args->phase, args->jobname, args->jobname, args->teststand, args->testlevel, args->username, args->comments, "Initialised",  args->scenariologfile);

            if ((scenarioresultID = perform_insert_from_query_string(SUBNAME, sql_connection, query, errmsg, sizeof(errmsg))) == 0)
            {
                snprintf(msg, sizeof(msg), "insert of %s table failed\n", status_table_name);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                rc = 1;
            }
        }

        if (rc == 0)
        {
            determine_status_table_name(status_database_name, "testlevelrecord", status_table_name, sizeof(status_table_name));
            snprintf(query, sizeof(query), "INSERT INTO %s (ScenarioresultID, Name) "
                     "VALUES('%d','%s')",
                     status_table_name, scenarioresultID, args->testlevel);

            if ((testlevelrecordID = perform_insert_from_query_string(SUBNAME, sql_connection, query, errmsg, sizeof(errmsg))) == 0)
            {
                snprintf(msg, sizeof(msg), "insert of %s table failed\n", status_table_name);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                rc = 1;
            }
        }

        if (rc == 0)
        {
            determine_status_table_name(status_database_name, "teststandrecord", status_table_name, sizeof(status_table_name));
            snprintf(query, sizeof(query), "INSERT INTO %s (ScenarioresultID, Name) "
                     "VALUES('%d','%s')",
                     status_table_name, scenarioresultID, args->teststand);

            if ((teststandrecordID = perform_insert_from_query_string(SUBNAME, sql_connection, query, errmsg, sizeof(errmsg))) == 0)
            {
                snprintf(msg, sizeof(msg), "insert of %s table failed\n", status_table_name);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                rc = 1;
            }
        }

        if (rc == 0)
        {
            num_actions_in_scenario = count_actions_in_scenario(sql_connection, scenarioID);

            snprintf(msg, sizeof(msg), "num_actions_in_scenario = %d, scenarioID = %d\n", num_actions_in_scenario, scenarioID);
            print_msg_to_console(msg);
            //   $logdirectory = $gp['logdirectory'];
            //       $loglocation = $logdirectory . '/' . 'scenario_' . $scenarioresultID . '_' . make_legal_filename($gp['jobname'], 32);
            snprintf(loglocation, sizeof(loglocation), "%s/scenario_%d_%s", args->loglocation, scenarioresultID, args->jobname);               // TODO = what about windows? what about odd chars in args->jobname ?
            determine_status_table_name(status_database_name, "scenarioresult", status_table_name, sizeof(status_table_name));
            snprintf(query, sizeof(query), "UPDATE %s SET Loglocation = '%s', TeststandrecordID = '%d', "
                     "TestlevelrecordID = '%d', Actionsinscenario = '%d' WHERE ID='%d'",
                     status_table_name, loglocation, teststandrecordID, testlevelrecordID, num_actions_in_scenario, scenarioresultID);

            if (perform_query(SUBNAME, sql_connection, query) != 0)      // we are not doing an audit log update !
            {
                snprintf(msg, sizeof(msg), "update of of %s table failed\n", status_table_name);
                print_msg_to_console(msg);
                result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                rc = 1;
            }

            // Handle UDP here - assemble a string containing any user supplied parameters
            parameterlist p = args->parameters;
            char userDefinedParameters[DAF_PARAMETERS_LEN];
            safecpy(userDefinedParameters, "", sizeof(userDefinedParameters));

            while (p != NULL)
            {
                safecat(userDefinedParameters, p->name, sizeof(userDefinedParameters));
                safecat(userDefinedParameters, "=", sizeof(userDefinedParameters));
                safecat(userDefinedParameters, p->value, sizeof(userDefinedParameters));
                p = p->next;

                if (p !=NULL)
                {
                    safecat(userDefinedParameters, " ", sizeof(userDefinedParameters));
                }
            }

            if (rc == 0)
            {
                snprintf(query, sizeof(query), "INSERT INTO daf.workrequest(Project, Phase, RequestType, TesterID, Tablename1, ObjectID1, Tablename2, ObjectID2, Tablename3, ObjectID3, Tablename4, ObjectID4, Description1, Description1Type, Logdirectory, Scenariologfilename) "
                         "VALUES('%s','%s','Scenario','%d','scenario','%d','scenarioresult','%d','testlevel','%d','teststand','%d','%s','UserDefinedParameters', '%s','%s')",
                         args->project, args->phase, testerID, scenarioID, scenarioresultID, testlevelID, teststandID, userDefinedParameters, args->loglocation, args->scenariologfile);

                if (perform_insert_from_query_string(SUBNAME, sql_connection, query, errmsg, sizeof(errmsg)) == 0)
                {
                    result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
                    rc = 1;
                }
            }
        }

        do_disconnect(SUBNAME, sql_connection);

    }
    else
    {

        snprintf(msg, sizeof(msg), "could not connect to SQL datbase\n");
        print_msg_to_console(msg);
        result->remote_client_start_scenario_res_u.outcome.errmsg = duplicate_string(msg);
        rc = 1;

    }

    result->remote_client_start_scenario_res_u.outcome.testerID = testerID;
    result->remote_client_start_scenario_res_u.outcome.scenarioID = scenarioID;
    result->remote_client_start_scenario_res_u.outcome.scenarioresultID = scenarioresultID;
    result->remote_client_start_scenario_res_u.outcome.testlevelID = testlevelID;
    result->remote_client_start_scenario_res_u.outcome.teststandID = teststandID;
    result->remote_client_start_scenario_res_u.outcome.teststandrecordID = teststandrecordID;
    result->remote_client_start_scenario_res_u.outcome.testlevelrecordID = testlevelrecordID;

    snprintf(msg, sizeof(msg), "client_remote_client_start_scenario_1_svc completed (rc=%d, scenarioresultID=%d)\n", rc, scenarioresultID);
    print_msg_to_console(msg);

    if (rc != 0)
    {
        result->remote_client_start_scenario_res_u.outcome.valid_start_scenario = REMOTE_START_SCENARIO_FAILED;
    }

    result->status = REMOTE_CLIENT_OK;
    return (TRUE);

}

#endif

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_query_cmdlog_1_svc                                                           */
/*                                                                                                        */
/* PARAMS IN:  rpc_client     structure that describes the rpc client and the NFS server it is to be      */
/*                                                                                                        */
/*             rqstp                                                                                      */
/*                                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cmd_res                                                                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_query_cmdlog_1_svc(remote_client_query_cmdlog_args  *args,
        remote_client_query_cmdlog_res   *result,
        struct svc_req                   *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_query_cmdlog_1_svc"

    char msg[MAX_MSG_LEN];
    int  index;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: index=%d\n", SUBNAME, args->index);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    if ((index = get_index_in_cmd_log(p_service_cmd_log_object, args->index)) == -1)
    {

        result->remote_client_query_cmdlog_res_u.outcome.cmdstring  = duplicate_string("");
        result->remote_client_query_cmdlog_res_u.outcome.identstring  = duplicate_string("");
        result->remote_client_query_cmdlog_res_u.outcome.last = TRUE;

    }
    else
    {

        result->remote_client_query_cmdlog_res_u.outcome.last         = FALSE;
        result->remote_client_query_cmdlog_res_u.outcome.cmdstring    = duplicate_string(p_service_cmd_log_object->cmd_array[index].cmdstring);
        result->remote_client_query_cmdlog_res_u.outcome.identstring  = duplicate_string(p_service_cmd_log_object->cmd_array[index].identstring);
        result->remote_client_query_cmdlog_res_u.outcome.tag          = p_service_cmd_log_object->cmd_array[index].tag;
        result->remote_client_query_cmdlog_res_u.outcome.workqueueID  = p_service_cmd_log_object->cmd_array[index].workqueueID;
        result->remote_client_query_cmdlog_res_u.outcome.pid          = p_service_cmd_log_object->cmd_array[index].pid;
        result->remote_client_query_cmdlog_res_u.outcome.state        = p_service_cmd_log_object->cmd_array[index].state;
        result->remote_client_query_cmdlog_res_u.outcome.status       = p_service_cmd_log_object->cmd_array[index].status;
        result->remote_client_query_cmdlog_res_u.outcome.start_time   = p_service_cmd_log_object->cmd_array[index].start_time;
        result->remote_client_query_cmdlog_res_u.outcome.end_time     = p_service_cmd_log_object->cmd_array[index].end_time;
        result->remote_client_query_cmdlog_res_u.outcome.timeout_at_time   = p_service_cmd_log_object->cmd_array[index].timeout_at_time;
        result->remote_client_query_cmdlog_res_u.outcome.fail_on_timeout   = p_service_cmd_log_object->cmd_array[index].fail_on_timeout;

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}


int get_OS_info(char *os_description, char *os_type, char *os_version, char *cpu_architecture, int max_string_len)
{

    FILE *fp;
    int status;
    char line[256];
    int rc = 0;
    int num_lines = 0;
    char *token;
    char *p;

    safecpy(os_description,   "not_known", max_string_len);
    safecpy(os_type,          "not_known", max_string_len);
    safecpy(os_version,       "not_known", max_string_len);
    safecpy(cpu_architecture, "not_known", max_string_len);

    if ((fp = popen("uname -a", "r")) == NULL)
    {
        rc = 1;
    }
    else
    {
        while (fgets(line, sizeof(line), fp) != NULL)
        {
            num_lines++;
        }

        if (ferror(fp))
        {
            rc = 1;
        }

        status = pclose(fp);

        if (status == -1)
        {
            /* Error reported by pclose() */
            rc = 2;
        }
        else if (status != 0)
        {
            if (WIFEXITED(status))
            {
                /* child exited, status= WEXITSTATUS(status) */
                rc = 3;
            }
            else if (WIFSIGNALED(status))
            {
                /* child killed (signal = WTERMSIG(status) */
                rc = 4;
            }
            else if (WIFSTOPPED(status))
            {
                /* child stopped (signal %d)\n", WSTOPSIG(status) */
                rc = 5;
            }
            else        /* Non-standard case -- may never happen */
            {
                /* Unexpected status (0x%x), status */
                rc = 6;
            }
        }
    }

    if (num_lines != 1)
    {
        rc = 7;
    }
    else
    {

        /* now we have to parse a line like */
        /* Linux suse11a 2.6.37.1-1.2-desktop #1 SMP PREEMPT 2011-02-21 10:34:10 +0100 x86_64 x86_64 x86_64 GNU/Linux */
        chomp(line);
        safecpy(os_description, line, max_string_len);

        token = strtok_r(line, " ", &p);
        safecpy(os_type, token, max_string_len);

        if ((token = strtok_r(NULL, " ", &p)) != NULL)
        {
            if ((token = strtok_r(NULL, " ", &p)) != NULL)
            {
                safecpy(os_version, token, max_string_len);

                if ((token = strtok_r(NULL, " ", &p)) != NULL)
                {
                    if ((token = strtok_r(NULL, " ", &p)) != NULL)
                    {
                        if ((token = strtok_r(NULL, " ", &p)) != NULL)
                        {
                            if ((token = strtok_r(NULL, " ", &p)) != NULL)
                            {
                                if ((token = strtok_r(NULL, " ", &p)) != NULL)
                                {
                                    if ((token = strtok_r(NULL, " ", &p)) != NULL)
                                    {
                                        if ((token = strtok_r(NULL, " ", &p)) != NULL)
                                        {
                                            safecpy(cpu_architecture, token, max_string_len);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    return rc;

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_query_version_1_svc                                                          */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing */
/*                            the query version request                                                   */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_query_version_res   a structure that is passed back to the daf client       */
/*                                               machine and which contains a description of the outcode  */
/*                                               of the query tag operation                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*   Returns the version of the daf server that is running to a remote client.                           */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_query_version_1_svc(remote_client_query_version_args  *args,
        remote_client_query_version_res   *result,
        struct svc_req                    *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_version_tag_1_svc"

#define OS_STRING_LEN 128
    char  msg[MAX_MSG_LEN];
    char  os_type[OS_STRING_LEN];
    char  os_version[OS_STRING_LEN];
    char  os_description[OS_STRING_LEN];
    char  cpu_architecture[OS_STRING_LEN];

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    if (args->set_zombie_reaper_debug_flag)
    {
        set_zombie_reaper_debug_flag(1);
    }

    if (args->reset_zombie_reaper_debug_flag)
    {
        set_zombie_reaper_debug_flag(0);
    }


    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    result->remote_client_query_version_res_u.outcome.agent_major_version  = daf_major_version;
    result->remote_client_query_version_res_u.outcome.agent_minor_version  = daf_minor_version;
    result->remote_client_query_version_res_u.outcome.agent_version_string = duplicate_string(daf_version_string);
    result->remote_client_query_version_res_u.outcome.agent_build_machine  = duplicate_string(&daf_buildmachine[0]);

    result->remote_client_query_version_res_u.outcome.p_name = duplicate_string(ThisHostName);

    if (get_OS_info(os_description, os_type, os_version, cpu_architecture, OS_STRING_LEN) == 0)
    {
        result->remote_client_query_version_res_u.outcome.p_osdata_is_valid = TRUE;
    }

    result->remote_client_query_version_res_u.outcome.p_os_type          = duplicate_string(os_type);
    result->remote_client_query_version_res_u.outcome.p_os_version       = duplicate_string(os_version);
    result->remote_client_query_version_res_u.outcome.p_os_description   = duplicate_string(os_description);
    result->remote_client_query_version_res_u.outcome.p_cpu_architecture = duplicate_string(cpu_architecture);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

#ifndef DAF_AGENT

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_query_scenarioresult_1_svc                                                   */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing  */
/*                            the query tag request                                                       */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_query_scenarioresult_res    a structure that is passed back to the daf       */
/*                                            daf client and which contains a description of the outcome  */
/*                                            of the query scenarioresultID operation                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*    Examines the contents of the specified scenarioresultID row in the scenarioresult table in the DAF  */
/*    SQL database and returns the state of the scenario                                                  */
/*                                                                                                        */
/*   If the ID of the scenarioresult  cannot found in the scenarioresult table then the 'valid' flag      */
/*   is set to FALSE in the returned result structure.   The 'status' field in the returned result        */
/*   structure is always set to good (ie E_OK)                                                            */
/*--------------------------------------------------------------------------------------------------------*/

bool_t  client_remote_client_query_scenarioresult_1_svc(remote_client_query_scenarioresult_args  *args,
        remote_client_query_scenarioresult_res   *result,
        struct svc_req                           *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_query_scenarioresult_1_svc"

    char  msg[MAX_MSG_LEN];
    char  errmsg[MAX_MSG_LEN];
    scenarioresult_t scenarioresult;
    char status_database_name[100]; // TODO
    char status_table_name[100];   // TODO
    int   rc;

    MYSQL *sql_connection;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: project=%s, phase=%s, ID=%d\n", SUBNAME, args->project, args->phase, args->scenarioresultID);
        print_msg_to_console(msg);
    }

    result->remote_client_query_scenarioresult_res_u.outcome.state = duplicate_string("unknown");

    sql_connection = do_connect(SUBNAME,
                                sql_server.sql_servername,
                                sql_server.sql_username,
                                sql_server.sql_password,
                                "daf",
                                sql_server.sql_port,
                                sql_server.sql_socketname,
                                0,
                                errmsg,
                                sizeof(errmsg));

    if (sql_connection != NULL)
    {
        determine_status_database_name(args->project, args->phase, status_database_name, sizeof(status_database_name));
        determine_status_table_name(status_database_name, "scenarioresult", status_table_name, sizeof(status_table_name));

        if ((rc = get_scenarioresult(sql_connection, args->scenarioresultID, status_table_name, &scenarioresult)) != 0)
        {
            snprintf(msg, sizeof(msg), "%s: could not find scenarioresult ID=%d in %s\n",
                     SUBNAME, args->scenarioresultID, status_table_name);
            print_msg_to_console(msg);
        }
        else
        {
            result->remote_client_query_scenarioresult_res_u.outcome.project = duplicate_string(scenarioresult.Project);
            result->remote_client_query_scenarioresult_res_u.outcome.phase = duplicate_string(scenarioresult.Phase);
            result->remote_client_query_scenarioresult_res_u.outcome.scenarioname = duplicate_string(scenarioresult.Scenarioname);
            result->remote_client_query_scenarioresult_res_u.outcome.jobname = duplicate_string(scenarioresult.Jobname);
            result->remote_client_query_scenarioresult_res_u.outcome.state = duplicate_string(scenarioresult.State);           // TODO how does this get freed  etc?
            result->remote_client_query_scenarioresult_res_u.outcome.actionsinscenario = scenarioresult.Actionsinscenario;
            result->remote_client_query_scenarioresult_res_u.outcome.actionsattempted = scenarioresult.Actionsattempted;
            result->remote_client_query_scenarioresult_res_u.outcome.actionscompleted = scenarioresult.Actionscompleted;
            result->remote_client_query_scenarioresult_res_u.outcome.actionspassed = scenarioresult.Actionspassed;
            result->remote_client_query_scenarioresult_res_u.outcome.actionsfailed = scenarioresult.Actionsfailed;
            result->remote_client_query_scenarioresult_res_u.outcome.pass = scenarioresult.Pass;
            result->remote_client_query_scenarioresult_res_u.outcome.start = duplicate_string(scenarioresult.Start);
            result->remote_client_query_scenarioresult_res_u.outcome.end = duplicate_string(scenarioresult.End);
            result->remote_client_query_scenarioresult_res_u.outcome.teststand = duplicate_string(scenarioresult.Teststand);
            result->remote_client_query_scenarioresult_res_u.outcome.teststandrecordID = scenarioresult.TeststandrecordID;
            result->remote_client_query_scenarioresult_res_u.outcome.testlevel = duplicate_string(scenarioresult.Testlevel);
            result->remote_client_query_scenarioresult_res_u.outcome.tester = duplicate_string(scenarioresult.Tester);
            result->remote_client_query_scenarioresult_res_u.outcome.loglocation = duplicate_string(scenarioresult.Loglocation);
            result->remote_client_query_scenarioresult_res_u.outcome.scenariologfilename = duplicate_string(scenarioresult.Scenariologfilename);
            result->remote_client_query_scenarioresult_res_u.outcome.comments = duplicate_string(scenarioresult.Comments);
        }

    }
    else
    {
        snprintf(msg, sizeof(msg), "%s: could not connect to mysql database: %s %s %s %s %d %s: %s\n",
                 SUBNAME,
                 sql_server.sql_servername,
                 sql_server.sql_username,
                 sql_server.sql_password,
                 sql_server.sql_databasename,
                 sql_server.sql_port,
                 sql_server.sql_socketname,
                 errmsg);             // TODO - remove password in clear text ?
        print_msg_to_console(msg);
        rc = 1;
    }

    if (rc  != 0)
    {
        result->remote_client_query_scenarioresult_res_u.outcome.valid = FALSE;
        result->remote_client_query_scenarioresult_res_u.outcome.errmsg = duplicate_string(msg);

    }
    else
    {

        result->remote_client_query_scenarioresult_res_u.outcome.valid = TRUE;
        result->remote_client_query_scenarioresult_res_u.outcome.errmsg = duplicate_string("no error");

    }

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

#endif

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_query_tag_1_svc                                                              */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing */
/*                            the query tag request                                                       */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_query_tag_res    a structure that is passed back to the daf client machine   */
/*                                            and which contains a description of the outcode of the      */
/*                                            query tag operation                                         */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*    Examines the state of a specified command tag and returns information about the state of that       */
/*    tag (eg CMD_INIT, CMD_RUNNING, CMD_COMPLETED) and the status of the process associated              */
/*    with that command - this status is only valid if the state of the command is CMD_COMPLETED or       */
/*    CMD_COMPLETED_TIMELIMIT.                                                                            */
/*                                                                                                        */
/*   If the tag cannot found in the list of known commands (ie the command log) then the 'valid' flag     */
/*   is set to FALSE in the returned result structure.   The 'status' field in the returned result        */
/*   structure is always set to good (ie E_OK)                                                            */
/*--------------------------------------------------------------------------------------------------------*/

bool_t  client_remote_client_query_tag_1_svc(remote_client_query_tag_args  *args,
        remote_client_query_tag_res   *result,
        struct svc_req                *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_query_tag_1_svc"

    char  msg[MAX_MSG_LEN];
    int   index;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: tag=%d\n", SUBNAME, args->tag);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */
    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    index = is_tag_in_cmd_log(p_service_cmd_log_object, args->tag);

    if (index == -1)
    {

        result->remote_client_query_tag_res_u.outcome.valid = FALSE;

    }
    else
    {

        result->remote_client_query_tag_res_u.outcome.valid = TRUE;
        result->remote_client_query_tag_res_u.outcome.state = p_service_cmd_log_object->cmd_array[index].state;
        result->remote_client_query_tag_res_u.outcome.cmd_exit_code = p_service_cmd_log_object->cmd_array[index].status / 256;;
        result->remote_client_query_tag_res_u.outcome.cmd_exit_signal = p_service_cmd_log_object->cmd_array[index].status & 0xff;

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_get_inique_ident_1_svc                                                       */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing */
/*                            the get unique ident                                                        */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_get_unique_ident_res    a structure that is passed back to the daf client   */
/*                                            and which contains a description of the outcome of the      */
/*                                            get_uniqueident_res                                         */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t  client_remote_client_get_unique_ident_1_svc(remote_client_get_unique_ident_args  *args,
        remote_client_get_unique_ident_res   *result,
        struct svc_req                       *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_get_unique_ident_1_svc"

    char msg[MAX_MSG_LEN];
    char identstring[MAX_IDENT_LEN];

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    get_next_unique_ident(identstring, sizeof(identstring));

    result->remote_client_get_unique_ident_res_u.outcome.identstring =
        duplicate_string(identstring);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: ident = %s, result->status=%d\n",
                 result->remote_client_get_unique_ident_res_u.outcome.identstring,
                 SUBNAME,
                 result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_query_ident_1_svc                                                            */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing */
/*                            the query ident request                                                     */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_query_tag_res    a structure that is passed back to the daf client machine   */
/*                                            and which contains a description of the outcode of the      */
/*                                            query ident operation                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*    Examines the state of all the commands associated with the specified ident and returns information  */
/*    about the state of that                                                                             */
/*    tag (eg CMD_INIT, CMD_RUNNING, CMD_COMPLETED) and the status of the process associated              */
/*    with that command - this status is only valid if the state of the command is CMD_COMPLETED or       */
/*    CMD_COMPLETED_TIMELIMIT                                                                             */
/*                                                                                                        */
/*   If the ident cannot found in the list of known commands (ie the command log) then the 'valid' flag   */
/*   is still set to TRUE in the returned result structure and the cmd_log fragment in the returned       */
/*   data will be empty.  The result.status in the result structure is always set to good (ie E_OK)       */
/*--------------------------------------------------------------------------------------------------------*/

bool_t  client_remote_client_query_ident_1_svc(remote_client_query_ident_args  *args,
        remote_client_query_ident_res   *result,
        struct svc_req                  *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_query_ident_1_svc"

    char           msg[MAX_MSG_LEN];
    int            i;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: ident=%s\n", SUBNAME, args->identstring);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->remote_client_query_ident_res_u.outcome.cmd_log_fragment.first = NULL;
    result->remote_client_query_ident_res_u.outcome.cmd_log_fragment.last = NULL;

    for (i=0; i < DAF_MAX_COMMANDS; i++)
    {

        if (MATCH(p_service_cmd_log_object->cmd_array[i].identstring, args->identstring))
        {

            if (add_cmd_log_fragment(&(result->remote_client_query_ident_res_u.outcome.cmd_log_fragment),
                                     p_service_cmd_log_object->cmd_array[i].cmdstring,
                                     p_service_cmd_log_object->cmd_array[i].identstring,
                                     p_service_cmd_log_object->cmd_array[i].tag,
                                     p_service_cmd_log_object->cmd_array[i].workqueueID,
                                     p_service_cmd_log_object->cmd_array[i].pid,
                                     p_service_cmd_log_object->cmd_array[i].state,
                                     p_service_cmd_log_object->cmd_array[i].status,
                                     p_service_cmd_log_object->cmd_array[i].start_time,
                                     p_service_cmd_log_object->cmd_array[i].end_time,
                                     p_service_cmd_log_object->cmd_array[i].timeout_at_time,
                                     p_service_cmd_log_object->cmd_array[i].fail_on_timeout,
                                     msg,
                                     sizeof(msg)) != E_OK)
            {
                snprintf(msg, sizeof(msg), "%s: problem adding cmd log entry to cmd_log_fragment\n", SUBNAME);
                print_msg_to_console(msg);
                result->status = 1;  /* <<<<<<<<<<<<< */
            }

        }

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_query_cmds_1_svc                                                             */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing */
/*                            the query ident request                                                     */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_query_cmds_res   a structure that is passed back to the daf client machine   */
/*                                            and which contains a description of the outcode of the      */
/*                                            query cmds operation                                        */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*    Examines the state of all the commands in the command log and returns a count of the number of      */
/*    commands that are in the various different command states, eg (CMD_INIT, CMD_RUNNING,               */
/*    CMD_COMPLETED).   The number of commands that have completed with good or bad status is also        */
/*    returned.  Note: CMD_COMPLETED and CMD_COMPLETED_TIMELIMIT are both added to the CMD_COMPLETED      */
/*    total                                                                                               */
/*--------------------------------------------------------------------------------------------------------*/

bool_t  client_remote_client_query_cmds_1_svc(remote_client_query_cmds_args  *args,
        remote_client_query_cmds_res   *result,
        struct svc_req                 *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_query_cmds_1_svc"

    char           msg[MAX_MSG_LEN];
    int            i;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    for (i=0; i < DAF_MAX_COMMANDS; i++)
    {

        if (p_service_cmd_log_object->cmd_array[i].state == CMD_INITIALISED)
        {
            result->remote_client_query_cmds_res_u.outcome.num_CMD_INITIALISED_commands++;
        }
        else if (p_service_cmd_log_object->cmd_array[i].state == CMD_RUNNING)
        {
            result->remote_client_query_cmds_res_u.outcome.num_CMD_RUNNING_commands++;
        }
        else if (p_service_cmd_log_object->cmd_array[i].state == CMD_BG_RUNNING)
        {
            result->remote_client_query_cmds_res_u.outcome.num_CMD_RUNNING_commands++;
        }
        else if ((p_service_cmd_log_object->cmd_array[i].state == CMD_COMPLETED) ||
                 (p_service_cmd_log_object->cmd_array[i].state == CMD_COMPLETED_TIMELIMIT))
        {
            result->remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_commands++;

            if (p_service_cmd_log_object->cmd_array[i].status != 0)
            {
                result->remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_bad_status_commands++;
            }
            else
            {
                result->remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_good_status_commands++;
            }
        }

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_clear_tag_1_svc                                                              */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing  */
/*                            the clear tag request                                                       */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_clear_tag_res    a structure that is passed back to the daf client machine   */
/*                                            and which contains a description of the outcome of the      */
/*                                            clear tag operation                                         */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*    Finds the entry in the command log that corresponds to the specified tag and removes it from the    */
/*    command log if it is in the CMD_COMPLETED or CMD_COMPLETED_TIMELIMIT state.   If the command is     */
/*    still running then the                                                                              */
/*   'valid_clear' field in the result structure is set to indicate that the clear operation could not be */
/*   carried out, or if there is no entry in the command log for this tag, then the 'valid_clear' field   */
/*   is set to indicate the entry could not be found.                                                     */
/*   The 'status' field in the returned result structure is always set to good                            */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_clear_tag_1_svc(remote_client_clear_tag_args  *args,
        remote_client_clear_tag_res   *result,
        struct svc_req                *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_clear_tag_1_svc"

    char          msg[MAX_MSG_LEN];
    int           index;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result.status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    index = is_tag_in_cmd_log(p_service_cmd_log_object, args->tag);

    if (index == -1)
    {

        result->remote_client_clear_tag_res_u.outcome.valid_clear = REMOTE_CLEAR_TAG_DOES_NOT_EXIST;

    }
    else if ((p_service_cmd_log_object->cmd_array[index].state == CMD_COMPLETED) ||
             (p_service_cmd_log_object->cmd_array[index].state == CMD_COMPLETED_TIMELIMIT))
    {

        if (remove_cmd_log_entry_by_tag(p_service_cmd_log_object, args->tag))
        {
            result->remote_client_clear_tag_res_u.outcome.valid_clear = REMOTE_CLEAR_TAG_OK;
        }
        else
        {
            result->remote_client_clear_tag_res_u.outcome.valid_clear = REMOTE_CLEAR_TAG_FAILED;
        }

    }
    else
    {
        result->remote_client_clear_tag_res_u.outcome.valid_clear = REMOTE_CLEAR_TAG_NOT_IN_COMPLETED_STATE;
    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result.status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_clear_ident_1_svc                                                            */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing  */
/*                            the clear ident request                                                     */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_clear_ident_res  a structure that is passed back to the daf client machine   */
/*                                            and which contains a description of the outcome of the      */
/*                                            clear ident operation                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*    Finds the entries in the command log that correspond to the specified ident and removes them from   */
/*    the command log if they are in the CMD_COMPLETED or CMD_COMPLETED_TIMELIMIT state.   If the         */
/*    command is still running then the                                                                   */
/*   'valid_clear' field in the result structure is set to indicagte that the clear operation could not be*/
/*   carried out, or if there is no entry in the command log for this tag, then the 'valid_clear' field   */
/*   is set to indicate the entry could not be found.                                                     */
/*   An attempt is made to clear all tags that have the specified ident, even if some of the clears       */
/*   fail - if multiple errors do occur, then the last occuring error is the one that is returned in the  */
/*   result structure.                                                                                    */
/*   The 'status' field in the returned result structure is always set to good.                           */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_clear_ident_1_svc(remote_client_clear_ident_args  *args,
        remote_client_clear_ident_res   *result,
        struct svc_req                  *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_clear_ident_1_svc"

    char          msg[MAX_MSG_LEN];
    int           i;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started, ident=%s:\n", SUBNAME, args->identstring);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    for (i=0; i < DAF_MAX_COMMANDS; i++)
    {

        if (strncmp(p_service_cmd_log_object->cmd_array[i].identstring, args->identstring, sizeof(p_service_cmd_log_object->cmd_array[i].identstring)) == 0)
        {

            if ((p_service_cmd_log_object->cmd_array[i].state == CMD_COMPLETED) ||
                    (p_service_cmd_log_object->cmd_array[i].state == CMD_COMPLETED_TIMELIMIT))
            {

                if (remove_cmd_log_entry_by_ident(p_service_cmd_log_object, args->identstring))
                {
                    result->remote_client_clear_ident_res_u.outcome.valid_clear = REMOTE_CLEAR_IDENT_OK;
                }
                else
                {
                    result->remote_client_clear_ident_res_u.outcome.valid_clear = REMOTE_CLEAR_IDENT_FAILED;
                }

            }
            else
            {

                result->remote_client_clear_ident_res_u.outcome.valid_clear = REMOTE_CLEAR_IDENT_NOT_IN_COMPLETED_STATE;

            }

        }

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);


    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_clear_alltags_1_svc                                                          */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing  */
/*                            the clear tag request                                                       */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_clear_alltags_res    a structure that is passed back to the daf client       */
/*                                                machine and which contains a description of the outcome */
/*                                                outcome of the clear tag operation                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*   Looks at each entry in the command log and removes it from the command log if it is in the           */
/*   CMD_COMPLETED or CMD_COMPLETED_TIMELIMIT state.  If, for some reason, the command entry cannot be    */
/*    removed from the command log 'valid_clear' field in the result structure is set to indicate this    */
/*   failure.                                                                                             */
/*   The 'status' field in the returned result structure is always set to good                            */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_clear_alltags_1_svc(remote_client_clear_alltags_args  *args,
        remote_client_clear_alltags_res   *result,
        struct svc_req                    *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_clear_alltags_1_svc"

    int             index;
    Iu32            next = 0;
    char            msg[MAX_MSG_LEN];

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->remote_client_clear_alltags_res_u.outcome.valid_clear = REMOTE_CLEAR_TAG_OK;

    while ((index = get_index_in_cmd_log(p_service_cmd_log_object, next)) != -1)
    {

        if ((p_service_cmd_log_object->cmd_array[index].state == CMD_COMPLETED) ||
                (p_service_cmd_log_object->cmd_array[index].state == CMD_COMPLETED_TIMELIMIT))
        {
            if (! remove_cmd_log_entry_by_tag(p_service_cmd_log_object, p_service_cmd_log_object->cmd_array[index].tag))
            {
                result->remote_client_clear_alltags_res_u.outcome.valid_clear = REMOTE_CLEAR_TAG_FAILED;
            }
        }
        else
        {
            next++;
        }

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_query_alltags_1_svc                                                          */
/*                                                                                                        */
/* PARAMS IN:  rpc_client     structure that describes the rpc client and the NFS server it is to be      */
/*                                                                                                        */
/*             rqstp                                                                                      */
/*                                                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cmd_res                                                                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_query_alltags_1_svc(remote_client_query_alltags_args *args,
        remote_client_query_alltags_res  *result,
        struct svc_req                   *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_query_alltags_1_svc"

    char            msg[MAX_MSG_LEN];
    int             i;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->remote_client_query_alltags_res_u.outcome.cmd_log_fragment.first = NULL;
    result->remote_client_query_alltags_res_u.outcome.cmd_log_fragment.last = NULL;

    for (i=0; i < DAF_MAX_COMMANDS; i++)
    {

        if (p_service_cmd_log_object->cmd_array[i].state != CMD_FREE)
        {

            if (add_cmd_log_fragment(&(result->remote_client_query_alltags_res_u.outcome.cmd_log_fragment),
                                     p_service_cmd_log_object->cmd_array[i].cmdstring,
                                     p_service_cmd_log_object->cmd_array[i].identstring,
                                     p_service_cmd_log_object->cmd_array[i].tag,
                                     p_service_cmd_log_object->cmd_array[i].workqueueID,
                                     p_service_cmd_log_object->cmd_array[i].pid,
                                     p_service_cmd_log_object->cmd_array[i].state,
                                     p_service_cmd_log_object->cmd_array[i].status,
                                     p_service_cmd_log_object->cmd_array[i].start_time,
                                     p_service_cmd_log_object->cmd_array[i].end_time,
                                     p_service_cmd_log_object->cmd_array[i].timeout_at_time,
                                     p_service_cmd_log_object->cmd_array[i].fail_on_timeout,
                                     msg,
                                     sizeof(msg)) != E_OK)
            {
                snprintf(msg, sizeof(msg), "%s: problem adding cmd log entry to cmd_log_fragment\n", SUBNAME);
                print_msg_to_console(msg);
                result->status = 1;  /* <<<<<<<<<<<<< */
            }
        }

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return (TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_cancel_tag_1_svc                                                             */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing */
/*                            the clear tag request                                                       */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cancel_tag_res   a structure that is passed back to the daf client machine  */
/*                                            and which contains a description of the outcome of the      */
/*                                            cancel tag operation                                        */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*   Finds the entry in the command log that corresponds to the specified tag and looks to see if the     */
/*   command is still running.  If it is, then a kill -9 is sent to the procress group associated with    */
/*   the  command process. If the command  was not running, or the tag was not associated with a command  */
/*   log entry, then the 'valid_cancel' field in the result structure is set accordingly.                 */
/*   The 'status' field in the returned result structure is always set to good                            */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_cancel_tag_1_svc(remote_client_cancel_tag_args  *args,
        remote_client_cancel_tag_res   *result,
        struct svc_req                 *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_cancel_tag_1_svc"

    int              rc;
    char             msg[MAX_MSG_LEN];
    int              index;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: tag=%d\n", SUBNAME, args->tag);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    index = is_tag_in_cmd_log(p_service_cmd_log_object, args->tag);

    if (index == -1)
    {

        result->remote_client_cancel_tag_res_u.outcome.valid_cancel = REMOTE_CANCEL_TAG_DOES_NOT_EXIST;

    }
    else if ((p_service_cmd_log_object->cmd_array[index].state == CMD_COMPLETED) ||
             (p_service_cmd_log_object->cmd_array[index].state == CMD_COMPLETED_TIMELIMIT))
    {

        result->remote_client_cancel_tag_res_u.outcome.valid_cancel = REMOTE_CANCEL_TAG_COMMAND_ALREADY_COMPLETED;

    }
    else
    {

        if ((rc = killpg(p_service_cmd_log_object->cmd_array[index].pid, 9)) == 0)
        {
            result->remote_client_cancel_tag_res_u.outcome.valid_cancel = REMOTE_CANCEL_TAG_OK;
        }
        else
        {
            result->remote_client_cancel_tag_res_u.outcome.valid_cancel = REMOTE_CANCEL_TAG_KILLPG_FAILED;
        }

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    return(TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_cancel_ident_1_svc                                                           */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing */
/*                            the clear ident request                                                     */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_cancel_ident_res a structure that is passed back to the daf client machine  */
/*                                            and which contains a description of the outcome of the      */
/*                                            cancel ident operation                                      */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*   Finds the entries in the command log that corresponds to the specified ident and looks to see if the */
/*   associated commands is still running.  If it is, then a kill -9 is sent to the procress group        */
/*   associated with                                                                                      */
/*   the  command process. If the command  was not running, or the ident was not associated with a command*/
/*   log entry, then the 'valid_cancel' field in the result structure is set accordingly.                 */
/*   An attempt is made to cancel all tags that have the specified ident, even if some of the cancels     */
/*   fail - if multiple errors do occur, then the last occuring error is the one that is returned in the  */
/*   result structure.                                                                                    */
/*   The 'status' field in the returned result structure is always set to good.                           */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_cancel_ident_1_svc(remote_client_cancel_ident_args  *args,
        remote_client_cancel_ident_res   *result,
        struct svc_req                   *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_cancel_ident_1_svc"

    int  rc;
    char msg[MAX_MSG_LEN];
    int  i;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: ident=%s\n", SUBNAME, args->identstring);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0;  */

    lock_cmd_log(p_service_cmd_log_object, SUBNAME);

    for (i=0; i < DAF_MAX_COMMANDS; i++)
    {

        if (strncmp(p_service_cmd_log_object->cmd_array[i].identstring, args->identstring, sizeof(p_service_cmd_log_object->cmd_array[i].identstring)) == 0)
        {

            if ((p_service_cmd_log_object->cmd_array[i].state == CMD_COMPLETED) ||
                    (p_service_cmd_log_object->cmd_array[i].state == CMD_COMPLETED_TIMELIMIT))
            {

                result->remote_client_cancel_ident_res_u.outcome.valid_cancel = REMOTE_CANCEL_IDENT_COMMAND_ALREADY_COMPLETED;

            }
            else
            {

                if ((rc = killpg(p_service_cmd_log_object->cmd_array[i].pid, 9)) == 0)
                {
                    result->remote_client_cancel_ident_res_u.outcome.valid_cancel = REMOTE_CANCEL_TAG_OK;
                }
                else
                {
                    result->remote_client_cancel_ident_res_u.outcome.valid_cancel = REMOTE_CANCEL_TAG_KILLPG_FAILED;
                }

            }

        }

    }

    unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

    result->status = REMOTE_CLIENT_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return(TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_copyfile_1_svc                                                               */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing */
/*                            the copyfile request                                                        */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_copyfile_res     a structure that is passed back to the daf client machine  */
/*                                            and which contains a description of the outcome of the      */
/*                                            copyfile operation                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*   Copies a file between the client and server machine.                                                 */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_copyfile_1_svc(remote_client_copyfile_args  *args,
        remote_client_copyfile_res *result,
        struct svc_req               *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_copyfile_1_svc"

    Iu8              *pbuffer = NULL;
    char             msg[MAX_MSG_LEN];
    FILE             *fptr = NULL;
    int              rc;
    struct stat      statbuf;
    int              num_bytes_written;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: fromremote=%d, remotepathname=%s, \n", SUBNAME, args->fromremote, args->remotepathname);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0 and valid_copyfile good;  */

    if ((pbuffer = malloc(sizeof(Iu8) * args->requestedlength)) == NULL)
    {
        snprintf(msg, sizeof(msg), "%s: could not malloc %d bytes : ", SUBNAME, args->requestedlength);
        print_msg_to_console(msg);
        result->remote_client_copyfile_res_u.outcome.valid_copyfile = REMOTE_COPYFILE_MALLOC_ERROR;
        return (FALSE);
    }

    if (args->fromremote)
    {

        if ((rc = stat(args->remotepathname, &statbuf)) != 0)
        {
            result->remote_client_copyfile_res_u.outcome.valid_copyfile = REMOTE_COPYFILE_FILE_DOES_NOT_EXIST;
            return(FALSE);
        }

        result->remote_client_copyfile_res_u.outcome.data.data_val = (char *) pbuffer;

        if ((fptr = fopen(args->remotepathname, "rb")) == NULL )
        {

            snprintf(msg, sizeof(msg), "%s: Could not open %s\n", SUBNAME, args->remotepathname);
            print_msg_to_console(msg);
            result->remote_client_copyfile_res_u.outcome.valid_copyfile = REMOTE_COPYFILE_FILE_DOES_NOT_EXIST;
            return(FALSE);

        }

        fseek(fptr, args->offset, SEEK_SET);
        result->remote_client_copyfile_res_u.outcome.num_bytes_in_transfer =
            fread(pbuffer, sizeof(Iu8), args->requestedlength, fptr);
        result->remote_client_copyfile_res_u.outcome.data.data_len =
            result->remote_client_copyfile_res_u.outcome.num_bytes_in_transfer;

        if (feof(fptr))
        {

            result->remote_client_copyfile_res_u.outcome.eof = TRUE;

        }
        else
        {

            if (args->requestedlength != result->remote_client_copyfile_res_u.outcome.num_bytes_in_transfer)
            {
                result->remote_client_copyfile_res_u.outcome.valid_copyfile = REMOTE_COPYFILE_FAILED;
                return(FALSE);
            }

        }

    }
    else
    {

        result->remote_client_copyfile_res_u.outcome.data.data_val = (char *) NULL;
        result->remote_client_copyfile_res_u.outcome.data.data_len = 0;

        if ((fptr = fopen(args->remotepathname, "wb")) == NULL )
        {

            snprintf(msg, sizeof(msg), "%s: Could not open %s\n", SUBNAME, args->remotepathname);
            print_msg_to_console(msg);
            result->remote_client_copyfile_res_u.outcome.valid_copyfile = REMOTE_COPYFILE_COULD_NOT_OPEN_FOR_WRITE;
            return(FALSE);

        }

        num_bytes_written = fwrite(args->data.data_val, sizeof(Iu8), args->data.data_len, fptr);

        if (num_bytes_written != args->requestedlength)
        {
            snprintf(msg, sizeof(msg), "%s: Could not write requested number of bytes (%d) to %s - only %d written\n",
                     SUBNAME, args->requestedlength, args->remotepathname, num_bytes_written);
            print_msg_to_console(msg);
            result->remote_client_copyfile_res_u.outcome.valid_copyfile = REMOTE_COPYFILE_COULD_NOT_WRITE_REQUESTED_LENGTH;
            return(FALSE);
        }

    }

    if (fptr != NULL)
    {
        fclose(fptr);
    }

    result->status = E_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return(TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_deletefile_1_svc                                                             */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing  */
/*                            the deletefile request                                                      */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    result                         a structure that is passed back to the daf client machine   */
/*                                            and which contains a description of the outcome of the      */
/*                                            deletefile operation                                        */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*   Deletes a file on the server machine, if the file did not exist before the delete operation, then    */
/*   an error is returned.                                                                                */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_deletefile_1_svc(remote_client_deletefile_args  *args,
        remote_client_deletefile_res   *result,
        struct svc_req                 *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_deletefile_1_svc"

    char             msg[MAX_MSG_LEN];
    int              rc;
    struct stat      statbuf;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: remotepathname=%s, \n", SUBNAME, args->remotepathname);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0 and valid_deletefile good;  */

    if ((rc = stat(args->remotepathname, &statbuf)) != 0)
    {
        result->remote_client_deletefile_res_u.outcome.valid_deletefile = REMOTE_DELETEFILE_FILE_DOES_NOT_EXIST;
        return(FALSE);
    }

    if ((rc = delete_file(args->remotepathname)) != 0)
    {

        snprintf(msg, sizeof(msg), "%s: Could not open %s\n", SUBNAME, args->remotepathname);
        print_msg_to_console(msg);
        result->remote_client_deletefile_res_u.outcome.valid_deletefile = REMOTE_DELETEFILE_COULD_NOT_DELETE_FILE;
        return(FALSE);

    }

    result->status = E_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return(TRUE);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_dirlist_1_svc                                                                */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing  */
/*                            the dirlist request                                                         */
/*                                                                                                        */
/*             rqstp          a structure describing the dirlist request - this is provided by the        */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    remote_client_dirlist_res      a structure that is passed back to the daf client machine   */
/*                                            and which contains a description of the outcome of the      */
/*                                            dirlist operation                                           */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*   Copies a directory listing from the test client machine to the server                                */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_dirlist_1_svc(remote_client_dirlist_args  *args,
        remote_client_dirlist_res   *result,
        struct svc_req              *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_dirlist_1_svc"

    char             msg[MAX_MSG_LEN];
    int              rc;
    struct stat      statbuf;
    DIR              *dirp;
    dirlist          dl;       // typedef struct dirnode *dirlist;
    dirlist          *pdl;
    struct direct    *d;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started: remotepathname=%s, \n", SUBNAME, args->remotepathname);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0  */

    if ((rc = stat(args->remotepathname, &statbuf)) != 0)
    {
        result->remote_client_dirlist_res_u.outcome.valid_dirlist = REMOTE_DIRLIST_DIRECTORY_DOES_NOT_EXIST;
        return(FALSE);
    }

    dirp = opendir(args->remotepathname);

    if (dirp == NULL)
    {
        result->remote_client_dirlist_res_u.outcome.valid_dirlist = REMOTE_DIRLIST_COULD_NOT_OPEN_DIR;
        return(FALSE);
    }

    pdl = &result->remote_client_dirlist_res_u.outcome.directory;

    while ((d = readdir(dirp)) != NULL)
    {
        *pdl = dl = (dirnode *) malloc(sizeof(dirnode));   /* how does this memory get freed ? */
        dl->entry = strdup(d->d_name);
        pdl = &(dl->next);

    }

    *pdl = NULL;

    /* whathappens if there are mor directory entries than can fit in the RPC buffer ?? <<<<<<<<<< */

    closedir(dirp);

    result->status = E_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return(TRUE);

}

#ifndef DAF_AGENT

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  remote_client_update_workqueue_status_1_svc                                                */
/*                                                                                                        */
/* PARAMS IN:  args           structure that contains information from the daf client machine describing  */
/*                            the request to update the workqueue record                                  */
/*                                                                                                        */
/*             rqstp          a structure describing the service request - this is provided by the        */
/*                            service dispatcher routine but is not needed by this routine                */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    result                         a structure that is passed back to the daf client machine   */
/*                                            and which contains a description of the outcome of the      */
/*                                            update workqueue status operation                           */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*   Updates selected fields in a workqueue record in the DAF database                                    */
/*--------------------------------------------------------------------------------------------------------*/

bool_t client_remote_client_update_workqueue_status_1_svc(remote_client_update_workqueue_status_args  *args,
        remote_client_update_workqueue_status_res   *result,
        struct svc_req                 *rqstp)
{

#undef  SUBNAME
#define SUBNAME "client_remote_client_update_workqueue_status_1_svc"

    char             msg[MAX_MSG_LEN];

#ifndef POLLING_SERVER
    char             errmsg[MAX_MSG_LEN];
    MYSQL            *sql_connection;
    bool_t           comma_needed;
    char             query[512];
    char             temp[128];
#endif

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: started:\n", SUBNAME);
        print_msg_to_console(msg);
    }

    memset(result, 0, sizeof(*result));   /* implicitly sets result->status = 0 and valid_update good;  */

    /* ----------------------------------------------------------------------------------------------- */
    /* Turn this code off if the agent's can't see the server (eg fulla)  POLLING OPTION               */
    /* When this code is off, the the workqueue record will be updated by the process_workqueue()      */
    /* routine in the server (see afmysql.c)                                                           */
    /* ----------------------------------------------------------------------------------------------- */

#ifndef POLLING_SERVER

    if (args->update_tag || args->update_state || args->update_pass)
    {

        sql_connection = do_connect(SUBNAME,
                                    sql_server.sql_servername,
                                    sql_server.sql_username,
                                    sql_server.sql_password,
                                    "daf",
                                    sql_server.sql_port,
                                    sql_server.sql_socketname,
                                    0,
                                    errmsg,
                                    sizeof(errmsg));

        if (sql_connection != NULL)
        {

            comma_needed = FALSE;
            safecpy(query, "UPDATE daf.workqueue SET ", sizeof(query));

            if (args->update_tag)
            {
                snprintf(temp, sizeof(temp), "Tag = '%u'", args->tag);
                safecat(query, temp, sizeof(query));
                comma_needed = TRUE;
            }

            if (args->update_state)
            {
                if (comma_needed)
                {
                    safecat(query, ", ", sizeof(query));
                }

                snprintf(temp, sizeof(temp), "State = '%s'", args->state);
                safecat(query, temp, sizeof(query));
                comma_needed = TRUE;
            }

            if (args->update_pass)
            {
                if (comma_needed)
                {
                    safecat(query, ", ", sizeof(query));
                }

                snprintf(temp, sizeof(temp), "Pass = '%u'", args->pass);
                safecat(query, temp, sizeof(query));
                comma_needed = TRUE;
            }

            if (args->update_end)
            {
                if (comma_needed)
                {
                    safecat(query, ", ", sizeof(query));
                }

                safecat(query, "End = NOW()", sizeof(query));
            }

            snprintf(temp, sizeof(temp), " WHERE ID = '%u'", args->workqueueID);
            safecat(query, temp, sizeof(query));

            if (perform_query(SUBNAME, sql_connection, query) != 0)
            {
                result->remote_client_update_workqueue_status_res_u.outcome.valid_update = E_NOTOK;
            }
            else
            {
                result->remote_client_update_workqueue_status_res_u.outcome.valid_update = E_OK;
            }

            do_disconnect(SUBNAME, sql_connection);

        }
        else
        {
            snprintf(msg, sizeof(msg), "client_remote_client_update_workqueue_status_1_svc: could not connect to SQL:  %s %s %s %s %d %s: %s\n",
                     sql_server.sql_servername,
                     sql_server.sql_username,
                     sql_server.sql_password,
                     "daf",
                     sql_server.sql_port,
                     sql_server.sql_socketname,
                     errmsg);
            print_msg_to_console(msg);
        }
    }

#endif

    result->status = E_OK;

    if (args->msglevel > 2)
    {
        snprintf(msg, sizeof(msg), "%s: completed: result->status=%d\n", SUBNAME, result->status);
        print_msg_to_console(msg);
    }

    return(TRUE);

}

#endif

void log_zombie_reaper_msg(char *pathname, char *prefix, char *msg)
{

    if (strlen(pathname) == 0)
    {
        print_msg_to_console(msg);
    }
    else
    {
        log_msg_to_scenario_console_log(pathname, prefix, msg, FALSE);
    }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  zombie_reaper                                                                              */
/*                                                                                                        */
/* PARAMS IN:  void                                                                                       */
/*                                                                                                        */
/* PARAMS OUT: void                                                                                       */
/*                                                                                                        */
/* RETURNS:    none         (never returns)                                                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/* Periodically looks to see if any commands that have been issued from the daf service on this machine   */
/* have completed - if they have, then their completion status is recorded in the command log and the     */
/* state for that command is moved to the CMD_COMPLETED or the CMD_COMPLETED_TIMELIMIT states             */
/*                                                                                                        */
/* This routine should be started and run in a seperate backgrond thread when the daf service is          */
/* started - the routine will then run until the daf service is terminated                                */
/*--------------------------------------------------------------------------------------------------------*/

void *zombie_reaper(void *p)
{

#undef  SUBNAME
#define SUBNAME "zombie_reaper"

    char                    msg[MAX_MSG_LEN];
    char                    prefix[MAX_MSG_LEN];
    char                    state_msg[MAX_MSG_LEN];
    pid_t                   pid;
    int                     i;
    int                     statuslocation;
    int                     rc;
    bool_t                    complete_this;

#ifndef POLLING_SERVER

    typedef struct completion_request
    {
        int workqueueID;
        int pass;
    } completion_request_t;

    completion_request_t completion_requests[DAF_MAX_COMMANDS];
    int num_completion_requests;

    af_daemon_thread_data_t *p_af_daemon_thread_data;
    p_af_daemon_thread_data  = (struct af_daemon_thread_data *) p;

#endif

    pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */

    p_shared_memory_base_pointer->daemon_pid = (Iu64) getpid();

    while (1)
    {

        /* show that zombie_reaper (and hence the DAF or DAFAGENT service), is still alive */
        p_shared_memory_base_pointer->daemon_tick = (Iu32) time(0);

        if (myzombiereaperdebug)
        {
            printf("%s: tick=%u\n", SUBNAME,  p_shared_memory_base_pointer->daemon_tick);
        }

        lock_cmd_log(p_service_cmd_log_object, SUBNAME);
#ifndef POLLING_SERVER
        num_completion_requests = 0;
#endif

        for (i=0; i< DAF_MAX_COMMANDS; i++ )
        {

            if (myzombiereaperdebug)
            {
                if ((p_service_cmd_log_object->cmd_array[i].state == CMD_RUNNING) ||
                        (p_service_cmd_log_object->cmd_array[i].state == CMD_INITIALISED) ||
                        (p_service_cmd_log_object->cmd_array[i].state == CMD_BG_RUNNING))
                {
                    cmd_log_state_decode(p_service_cmd_log_object->cmd_array[i].state, state_msg, sizeof(state_msg));
                    printf("%s: PID %u state %s fail_on_timeout %d timeout_at_time %u\n",
                           SUBNAME, p_service_cmd_log_object->cmd_array[i].pid, state_msg,
                           p_service_cmd_log_object->cmd_array[i].fail_on_timeout,
                           p_service_cmd_log_object->cmd_array[i].timeout_at_time);
                }
            }

            /* -------------------------------------------------------------------------------------------*/
            /* is a command running and has it exceeded it's timeout - if so, terminate it                */
            /* -------------------------------------------------------------------------------------------*/

            if ((p_service_cmd_log_object->cmd_array[i].state == CMD_RUNNING) ||
                    (p_service_cmd_log_object->cmd_array[i].state == CMD_BG_RUNNING))
            {

                if (p_service_cmd_log_object->cmd_array[i].timeout_at_time != 0)
                {

                    if (time(0) > p_service_cmd_log_object->cmd_array[i].timeout_at_time)
                    {

                        if (p_service_cmd_log_object->cmd_array[i].fail_on_timeout)
                        {

                            /* -------------------------------------------------------------------------------------------*/
                            /* command has run for too long, so cancel command and record bad status for the command      */
                            /* -------------------------------------------------------------------------------------------*/

                            rc = killpg(p_service_cmd_log_object->cmd_array[i].pid, 9);
                            p_service_cmd_log_object->cmd_array[i].state = CMD_CANCELED_TIMEDOUT;
                            cmd_log_state_decode(p_service_cmd_log_object->cmd_array[i].state, state_msg, sizeof(state_msg));
                            snprintf(msg, sizeof(msg), "process pid=%d for tag %u killed as timeout expired, state set to %d (%s)\n",
                                     (Iu32) (p_service_cmd_log_object->cmd_array[i].pid), p_service_cmd_log_object->cmd_array[i].tag,
                                     p_service_cmd_log_object->cmd_array[i].state, state_msg);
                        }
                        else
                        {

                            /* ------------------------------------------------------------------------------------------------------*/
                            /* command has run up to its allocated timeout, so cancel command but record good status for the command */
                            /* ------------------------------------------------------------------------------------------------------*/

                            rc = killpg(p_service_cmd_log_object->cmd_array[i].pid, 9);
                            p_service_cmd_log_object->cmd_array[i].state = CMD_CANCELED_TIMELIMIT;
                            cmd_log_state_decode(p_service_cmd_log_object->cmd_array[i].state, state_msg, sizeof(state_msg));
                            snprintf(msg, sizeof(msg), "process pid=%d for tag %u killed as timeout reached, state set to %d (%s)\n",
                                     (Iu32) (p_service_cmd_log_object->cmd_array[i].pid), p_service_cmd_log_object->cmd_array[i].tag,
                                     p_service_cmd_log_object->cmd_array[i].state, state_msg);
                        }

                        log_zombie_reaper_msg(p_service_cmd_log_object->cmd_array[i].agent_log_pathname, SUBNAME, msg);
                    }

                }
            }

            complete_this = FALSE;

            if (p_service_cmd_log_object->cmd_array[i].state == CMD_CANCELED_TIMELIMIT)
            {

                pid = p_service_cmd_log_object->cmd_array[i].pid;
                rc = waitpid(pid, &statuslocation, WNOHANG);

                if (rc > 0)
                {
                    if ( (((rc && 0xff) / 256) == 0) && ((rc & 0xff) == 9))
                    {
                        statuslocation = 0;  /* command was intentionally killed as it reached its allocated time limit */
                        /* so we treat this as a good completion situation */
                    }

                    p_service_cmd_log_object->cmd_array[i].state    = CMD_COMPLETED_TIMELIMIT;
                    p_service_cmd_log_object->cmd_array[i].status   = statuslocation = 0;
                    p_service_cmd_log_object->cmd_array[i].end_time = time(0);
                    complete_this = TRUE;
                }
                else if (rc < 0)
                {
                    snprintf(msg, sizeof(msg), "waitpid(%d, WNOHANG) gave rc=%d, errno=%d\n", (Iu32) pid, rc, errno);
                    log_zombie_reaper_msg(p_service_cmd_log_object->cmd_array[i].agent_log_pathname, SUBNAME, msg);

                    if (errno == 10)
                    {
                        cmd_log_state_decode(p_service_cmd_log_object->cmd_array[i].state, state_msg, sizeof(state_msg));
                        snprintf(msg, sizeof(msg), "pid %u was in state %s but command completion was lost - command status has "
                                 " been returned as CMD_COMPLETION_LOST, status=0xffff\n", (Iu32) pid, state_msg);
                        log_zombie_reaper_msg(p_service_cmd_log_object->cmd_array[i].agent_log_pathname, SUBNAME, msg);
                        p_service_cmd_log_object->cmd_array[i].state    = CMD_COMPLETION_LOST;
                        p_service_cmd_log_object->cmd_array[i].status   = statuslocation = 0xffff;
                        p_service_cmd_log_object->cmd_array[i].end_time = time(0);
                        complete_this = TRUE;
                    }
                }

            }
            else if ((p_service_cmd_log_object->cmd_array[i].state == CMD_RUNNING) ||
                     (p_service_cmd_log_object->cmd_array[i].state == CMD_BG_RUNNING))
            {

                pid = p_service_cmd_log_object->cmd_array[i].pid;
                rc  = waitpid(pid, &statuslocation, WNOHANG);

                if (myzombiereaperdebug)
                {
                    printf("%s: waitpid for %u gave rc=%d, statuslocation=%d\n", SUBNAME, pid, rc, statuslocation);
                }

                if (rc > 0)
                {

                    /* --------------------------------------------------------*/
                    /* pid has completed                                       */
                    /* --------------------------------------------------------*/

                    p_service_cmd_log_object->cmd_array[i].state    = CMD_COMPLETED;
                    p_service_cmd_log_object->cmd_array[i].status   = statuslocation;
                    p_service_cmd_log_object->cmd_array[i].end_time = time(0);
                    complete_this = TRUE;

                }
                else if (rc < 0)
                {

                    /* ------------------------------------------------------------------------------------------------*/
                    /* waitpid has failed - maybe the pid process has completed and the status has already been reaped */
                    /* outside the zombie_reaper() routine - if that is the case we get errno=10 and we have lost      */
                    /* any knowledge of the completion status of the command, so there is not much we can do except    */
                    /* record that fact                                                                                */
                    /* ------------------------------------------------------------------------------------------------*/

                    int errnox = errno;
                    snprintf(msg, sizeof(msg), "waitpid(%d, WNOHANG) gave rc=%d, errno=%d\n", (Iu32) pid, rc, errnox);
                    log_zombie_reaper_msg(p_service_cmd_log_object->cmd_array[i].agent_log_pathname, SUBNAME, msg);

                    if (errno == 10)
                    {
                        cmd_log_state_decode(p_service_cmd_log_object->cmd_array[i].state, state_msg, sizeof(state_msg));
                        snprintf(msg, sizeof(msg), "pid %u was in state %s but command completion was lost - command status has "
                                 " been returned as CMD_COMPLETION_LOST, status=0xffff\n", (Iu32) pid, state_msg);
                        log_zombie_reaper_msg(p_service_cmd_log_object->cmd_array[i].agent_log_pathname, SUBNAME, msg);
                        p_service_cmd_log_object->cmd_array[i].state    = CMD_COMPLETION_LOST;
                        p_service_cmd_log_object->cmd_array[i].status   = statuslocation = 0xffff;
                        p_service_cmd_log_object->cmd_array[i].end_time = time(0);
                        complete_this = TRUE;
                    }
                }
            }

            /* ------------------------------------------------------------------------------------------------*/
            /* record the fact that the i'th entry in the cmd_arry has completed so that we can ask the        */
            /* the remote DAF service to update the status for this action in the DAF status database - we     */
            /* need to do this outside this loop, as we we have the command log locked at the moment, and we   */
            /* are expecting the DAF remote RPC service to be single threaded - so if we send it an RPC        */
            /* request and that remote DAF service is already processing a different RPC request that          */
            /* required the command log to be locked as part of processing that request - then we would have   */
            /* a deadlock - so we must release the command log lock before sending our RPC request from the    */
            /* zombie_reaper()                                                                                 */
            /* ------------------------------------------------------------------------------------------------*/

            if (complete_this)
            {
                cmd_log_state_decode(p_service_cmd_log_object->cmd_array[i].state, state_msg, sizeof(state_msg));
                snprintf(prefix, sizeof(prefix), "Completed: tag=%u", p_service_cmd_log_object->cmd_array[i].tag);
                snprintf(msg, sizeof(msg), "pid=%d completed, state=%d (%s), status=%d (exitcode=%d, signal=%d)\n",
                         (Iu32) pid, p_service_cmd_log_object->cmd_array[i].state, state_msg,
                         p_service_cmd_log_object->cmd_array[i].status, (p_service_cmd_log_object->cmd_array[i].status/256) & 0xFF,
                         p_service_cmd_log_object->cmd_array[i].status & 0x7F);
                log_zombie_reaper_msg(p_service_cmd_log_object->cmd_array[i].agent_log_pathname, SUBNAME, msg);

                if (p_service_cmd_log_object->cmd_array[i].status == 0)
                {
                    snprintf(prefix, sizeof(prefix), "PASS: tag=%u", p_service_cmd_log_object->cmd_array[i].tag);
                }
                else
                {
                    snprintf(prefix, sizeof(prefix), "FAIL: tag=%u", p_service_cmd_log_object->cmd_array[i].tag);
                }

                snprintf(msg, sizeof(msg), "%s\n", p_service_cmd_log_object->cmd_array[i].cmdstring);
                log_zombie_reaper_msg(p_service_cmd_log_object->cmd_array[i].agent_log_pathname, SUBNAME, msg);

                if (p_service_cmd_log_object->cmd_array[i].workqueueID != 0)
                {

                    if  (p_service_cmd_log_object->cmd_array[i].status == 0)
                    {
                        if (myzombiereaperdebug)
                        {
                            printf("zombie_reaper: remote_client_update_queue_status for workqueueID %d, Completed, 100%%\n",
                                   p_service_cmd_log_object->cmd_array[i].workqueueID);
                        }

#ifndef POLLING_SERVER
                        completion_requests[num_completion_requests].workqueueID = p_service_cmd_log_object->cmd_array[i].workqueueID;
                        completion_requests[num_completion_requests].pass        = 100;
#endif
                    }
                    else
                    {
                        if (myzombiereaperdebug)
                        {
                            printf("zombie_reaper: remote_client_update_queue_status for workqueueID %d, Completed, 0%%\n",
                                   p_service_cmd_log_object->cmd_array[i].workqueueID);
                        }

#ifndef POLLING_SERVER
                        completion_requests[num_completion_requests].workqueueID = p_service_cmd_log_object->cmd_array[i].workqueueID;
                        completion_requests[num_completion_requests].pass        = 0;
#endif
                    }

#ifndef POLLING_SERVER
                    num_completion_requests++;
#endif

                }

            }

        }

        unlock_cmd_log(p_service_cmd_log_object, SUBNAME);

#ifndef POLLING_SERVER

        for (i=0; i<num_completion_requests; i++)
        {
            remote_client_update_workqueue_status(p_af_daemon_thread_data->sql_servername,
                                                  completion_requests[i].workqueueID, FALSE, 0,
                                                  TRUE, "Pendingcompletion", TRUE , completion_requests[i].pass, TRUE);
        }

#endif

        sleep(1);
    }

}


