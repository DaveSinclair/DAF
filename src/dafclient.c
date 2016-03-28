/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                       */
/*                                                                                                                      */
/* dafclient.c                                                                                                         */
/*                                                                                                                      */
/* This file is part of the daf NFS test exerciser programme.                                                          */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include <assert.h>
#include "OSstuff.h"
#include "daf_protocol.h" 
#include "daf_util.h"
#include "dafclient.h" 
#include "dafservice.h" 

static struct timeval TIMEOUT = { 100, 0 };                       /* <<<<<<<<<<<<<<<<<<<<<< */

/*--------------------------------------------------------------------------------------------------------*/
/*  External Globals (in daf.c)                                                                          */
/*--------------------------------------------------------------------------------------------------------*/

extern int daf_major_version;
extern int daf_minor_version;
extern char *daf_version_string;

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_cntrl                                                              */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  stop_flag                                                                       */
/*  pause_flag                                                                      */
/*  resume_flag                                                                     */
/*  sleep_flag                                                                      */
/*  sleep_duration_secs                                                             */
/*  clear_auditlog_flag                                                             */
/*  clear_shared_memory_flag                                                        */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK                                                                            */
/*  E_NOTOK                                                                         */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/*                                                                                  */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_cntrl(char *remotehostname,
                        bool_t clear_shared_memory_flag) {

#undef  SUBNAME
#define SUBNAME "remote_client_cntrl"

  remote_client_cntrl_args  args;
  remote_client_cntrl_res   result;
  enum clnt_stat            stat;
  char                      msg[MAX_MSG_LEN];
  char                      errmsg[MAX_MSG_LEN];
  int                       return_rc = E_OK;
  CLIENT *                  remote_client;

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,      0, sizeof(args));
    memset(&result,    0, sizeof(result));
 
    args.clear_shared_memory_flag  = clear_shared_memory_flag;
    args.msglevel                  = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_CNTRL,
                          (xdrproc_t) xdr_remote_client_cntrl_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_cntrl_res,  (caddr_t) &result,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_CNTRL, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (result.status != E_OK) {
          return_rc = E_NOTOK;
       } 

    }
 
    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 


/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_cmd                                                                */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client         structure described in the rmote_client_cntrl() routine above*/
/*  run_in_shell       <<<<<<<<<                                                    */
/*  cmdstring          the system command that is to be run                         */
/*  identstring        a ident string that is to be associated with this command,   */
/*                     set to the null string if no ident is in use                 */
/*  run_in_background  TRUE if this command is to be run in the background,         */
/*                     in which case the routine will return immediately the        */
/*                     the command has been launched                                */
/* workqueueID_flag                                                                 */
/* workqueueID                                                                      */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/*                                                                                  */
/*  Runs a system command on the remote client. The command is run by forking       */
/*  a subprocess and if the run in background flag is not set, then the routine     */
/*  will wait for the command to finish and enter the result in the command log.    */
/*  If the run_in_background flag is set, we rely on the zombie_reaper() routine    */
/*  to notice when the command process has completed and update the command log     */
/*  with the completion status.                                                     */
/* -------------------------------------------------------------------------------- */

int remote_client_cmd(char *remotehostname, int run_in_shell, char *cmdstring, char *identstring,  
                      bool_t run_in_background, bool_t workqueueID_flag, Iu32 workqueueID, Iu32 actionresultID,
                      char *sql_servername, char *sql_username, char *sql_password,
                      char *sql_databasename, Iu16 sql_port) { 

  remote_client_cmd_args  args;
  remote_client_cmd_res   remoteresult;
  enum clnt_stat          stat;
  char                    msg[MAX_MSG_LEN];
  char                    errmsg[MAX_MSG_LEN];
  int                     return_rc = E_OK;
  CLIENT *                remote_client;

#undef  SUBNAME
#define SUBNAME "remote_client_cmd"

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,      0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.run_in_shell      = run_in_shell;
    args.cmdstring         = cmdstring;
    args.identstring       = identstring;
    args.run_in_background = run_in_background;
    args.timeout           = 0;                      /* <<<<<<<<<<<<<< */
    args.fail_on_timeout   = 0;                      /* <<<<<<<<<<<<<< */
    args.workqueueID_flag  = workqueueID_flag;
    args.workqueueID       = workqueueID;
    args.workqueueID       = actionresultID;
    args.sql_servername    = sql_servername;
    args.sql_username      = sql_username;
    args.sql_password      = sql_password;
    args.sql_databasename  = sql_databasename;
    args.sql_port          = sql_port;
    args.msglevel          = 0;

    /* stringlist x;

    x.item = NULL;
    x.next = NULL;  */

    args.environment_settings.item = NULL;   /* <<<<<<<< temp */
    args.environment_settings.next = NULL;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_CMD,
                          (xdrproc_t) xdr_remote_client_cmd_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_cmd_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, (long) CLIENT_REMOTE_CLIENT_CMD, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;
    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  printf("system command: %s:  ident=%s, rc=%d, signal=%d\n", args.cmdstring, identstring, remoteresult.remote_client_cmd_res_u.outcome.cmd_exit_code,  remoteresult.remote_client_cmd_res_u.outcome.cmd_exit_signal);

  return_rc = return_rc | remoteresult.remote_client_cmd_res_u.outcome.cmd_exit_code | remoteresult.remote_client_cmd_res_u.outcome.cmd_exit_signal;

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_query_cmdlog                                                       */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client        structure described in the rmote_client_cntrl() routine above */
/*  no_headers        set to TRUE to suppress the printing of column headers in the */
/*                    command log output - this mode is useful when parsing the     */
/*                    command log output in a script                                */
/*  use_delimiter     set to TRUE to supress any blanks between the columns in the  */
/*                    command log output and a delimiting string between the        */
/*                    columns - this mode is useful when parsing the command log    */
/*                    output in a script                                            */
/*  delimiter_string  the string used to separate columns in the output             */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/*  Retrieves and prints out the command log from the remote client. This is the    */
/*  list of system commands that have been run using the -execute option and were   */
/*  run using the remote_client_cmd() routine above.                                */
/* -------------------------------------------------------------------------------- */

int remote_client_query_cmdlog(char         *remotehostname,
                               bool_t       no_headers,
                               bool_t       use_delimiter,
                               char         *delimiter_string) {

#undef  SUBNAME
#define SUBNAME "remote_client_query_cmdlog"

  remote_client_query_cmdlog_args  args;
  remote_client_query_cmdlog_res   remoteresult;
  enum clnt_stat                   stat;
  char                             msg[MAX_MSG_LEN];
  char                             errmsg[MAX_MSG_LEN];
  int                              return_rc = E_OK;
  Iu32                             index = 0;
  CLIENT *  remote_client;

  cmd_log  cmd_log = {NULL, NULL };  

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,      0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));
 
    do {

       args.index     = index;
       args.msglevel  = 0;

       if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_QUERY_CMDLOG,
                             (xdrproc_t) xdr_remote_client_query_cmdlog_args, (caddr_t) &args,
                             (xdrproc_t) xdr_remote_client_query_cmdlog_res,  (caddr_t) &remoteresult,
                             TIMEOUT)) != RPC_SUCCESS) {
          clnt_stat_decode(stat, errmsg, sizeof(errmsg));
          snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                   SUBNAME, (long) CLIENT_REMOTE_CLIENT_QUERY_CMDLOG, stat, errmsg);
          print_msg_to_console(msg);
          return_rc = E_NOTOK;

       } else {

           if (! remoteresult.remote_client_query_cmdlog_res_u.outcome.last) {

             if (add_cmd_log_fragment(&cmd_log, 
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.cmdstring,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.identstring,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.tag,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.workqueueID,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.pid,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.state,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.status,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.start_time,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.end_time,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.timeout_at_time,
                                      remoteresult.remote_client_query_cmdlog_res_u.outcome.fail_on_timeout,
                                      errmsg, sizeof(errmsg)) != E_OK) {
                snprintf(msg, sizeof(msg) ,"%s: problem with add_cmd_log_fragment: %s\n", SUBNAME, errmsg);
                print_msg_to_console(msg);
                return_rc = E_NOTOK;
             }

          } 

       }
       index++;
 
    } while ((! remoteresult.remote_client_query_cmdlog_res_u.outcome.last) && (return_rc == E_OK)); 

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  if (return_rc == E_OK) {
     print_cmd_log_fragment(&cmd_log, no_headers, use_delimiter, delimiter_string);
  }

  deallocate_cmd_log_fragment(&cmd_log);

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_query_version                                                      */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client        structure described in the rmote_client_cntrl() routine above */
/*  no_headers        set to TRUE to suppress the printing of column headers in the */
/*                    command log output - this mode is useful when parsing the     */
/*                    command log output in a script                                */
/*  use_delimiter     set to TRUE to supress any blanks between the columns in the  */
/*                    command log output and a delimiting string between the        */
/*                    columns - this mode is useful when parsing the command log    */
/*                    output in a script                                            */
/*  delimiter_string  the string used to separate columns in the output             */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/*  Retrieves the version information for the daf service running on the remote    */
/*  client.                                                                         */
/* -------------------------------------------------------------------------------- */

int remote_client_query_version(char *remotehostname, 
                                bool_t set_zombie_reaper_debug_flag, 
                                bool_t reset_zombie_reaper_debug_flag) {

#undef  SUBNAME
#define SUBNAME "remote_client_query_version"

  remote_client_query_version_args  args;
  remote_client_query_version_res   remoteresult;
  enum clnt_stat                    stat;
  char                              msg[MAX_MSG_LEN];
  char                              errmsg[MAX_MSG_LEN];
  int                               return_rc = E_OK;
  CLIENT *                          remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {   

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.agent_major_version  = daf_major_version;
    args.agent_minor_version  = daf_minor_version;
    args.agent_version_string = daf_version_string;
    args.set_zombie_reaper_debug_flag   = set_zombie_reaper_debug_flag; 
    args.reset_zombie_reaper_debug_flag = reset_zombie_reaper_debug_flag; 
    args.msglevel             = 0; 

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_QUERY_VERSION,
                          (xdrproc_t) xdr_remote_client_query_version_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_query_version_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_QUERY_VERSION, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

        snprintf(msg, sizeof(msg), "Local daf version %d.%d (%s), Remote service version %d.%d (%s)\n", 
                                   daf_major_version,
                                   daf_minor_version,
                                   daf_version_string,
                                   remoteresult.remote_client_query_version_res_u.outcome.agent_major_version,
                                   remoteresult.remote_client_query_version_res_u.outcome.agent_minor_version,
                                   remoteresult.remote_client_query_version_res_u.outcome.agent_version_string);
        print_msg_to_console(msg);

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_ping_agent                                                         */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client        structure described in the rmote_client_cntrl() routine above */
/*                                                                                  */
/*  quiet             set to TRUE to suppress error messages when remote clients    */
/*                    cannot be contacted                                           */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/*  Retrieves the version information for the daf service running on the remote    */
/*  client.                                                                         */
/* -------------------------------------------------------------------------------- */

int remote_client_ping_agent(char *remotehostname, bool_t quiet) { 

#undef  SUBNAME
#define SUBNAME "remote_client_ping_agent"

  remote_client_query_version_args  args;
  remote_client_query_version_res   remoteresult;
  enum clnt_stat                    stat;
  char                              msg[MAX_MSG_LEN];
  char                              errmsg[MAX_MSG_LEN];
  int                               return_rc = E_OK;
  CLIENT *                          remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    if (!quiet) {
       snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
       snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
       print_msg_to_console(msg);
    }
    return_rc = E_NOTOK;

  } else {   

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.agent_major_version  = daf_major_version;
    args.agent_minor_version  = daf_minor_version;
    args.agent_version_string = daf_version_string;
    args.msglevel       = 0; 

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_QUERY_VERSION,
                          (xdrproc_t) xdr_remote_client_query_version_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_query_version_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       if (!quiet) {
          clnt_stat_decode(stat, errmsg, sizeof(errmsg));
          snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                   SUBNAME, (long) CLIENT_REMOTE_CLIENT_QUERY_VERSION, stat, errmsg);
          print_msg_to_console(msg);
       }
       return_rc = E_NOTOK;

    } 

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 


/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_query_tag                                                          */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client        structure described in the rmote_client_cntrl() routine above */
/*  no_headers        set to TRUE to suppress the printing of column headers in the */
/*                    command log output - this mode is useful when parsing the     */
/*                    command log output in a script                                */
/*  use_delimiter     set to TRUE to supress any blanks between the columns in the  */
/*                    command log output and a delimiting string between the        */
/*                    columns - this mode is useful when parsing the command log    */
/*                    output in a script                                            */
/*  delimiter_string  the string used to separate columns in the output             */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/* Queries the status of the command associated with the specified tag on the       */
/* remote system.                                                                   */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_query_tag(char         *remotehostname,
                            Iu32         tag,
                            bool_t       no_headers,
                            bool_t       use_delimiter,
                            char         *delimiter_string) {

#undef  SUBNAME
#define SUBNAME "remote_client_query_tag"

  remote_client_query_tag_args  args;
  remote_client_query_tag_res   remoteresult;
  enum clnt_stat                stat;
  char                          state_string[MAX_MSG_LEN];
  char                          msg[MAX_MSG_LEN];
  char                          errmsg[MAX_MSG_LEN];
  int                           return_rc = E_OK;

  char                          *fmt_title = "%4s  %12s  %4s  %4s\n";
  char                          *fmt       = "%4d  %12s  %4d  %4d\n";
  char                          *fmt_title_delim = "%s%s%s%s%s%s%s\n";
  char                          *fmt_delim       = "%d%s%d%s%d%s%d\n";
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,      0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.tag      = tag;
    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_QUERY_TAG,
                          (xdrproc_t) xdr_remote_client_query_tag_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_query_tag_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_QUERY_TAG, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.remote_client_query_tag_res_u.outcome.valid) {

          if (! no_headers) {
             if (use_delimiter) {
                 snprintf(msg, sizeof(msg), fmt_title_delim, "tag", "state", "exitcode", "exit signal");
             } else {
                 snprintf(msg, sizeof(msg), fmt_title, "tag", "state", "exitcode", "exit signal");
             }
             print_string_to_console(msg);
          }

          cmd_log_state_decode(remoteresult.remote_client_query_tag_res_u.outcome.state, state_string,  sizeof(state_string));

          if (use_delimiter) {
             snprintf(msg, sizeof(msg), fmt_delim, tag,
                                        state_string,
                                        remoteresult.remote_client_query_tag_res_u.outcome.cmd_exit_code,
                                        remoteresult.remote_client_query_tag_res_u.outcome.cmd_exit_signal);
          } else {
             snprintf(msg, sizeof(msg), fmt, tag,
                                        state_string,
                                        remoteresult.remote_client_query_tag_res_u.outcome.cmd_exit_code,
                                        remoteresult.remote_client_query_tag_res_u.outcome.cmd_exit_signal);
          }
          print_string_to_console(msg);

       } 

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_get_unique_ident                                                          */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client        structure described in the rmote_client_cntrl() routine above */
/*  no_headers        set to TRUE to suppress the printing of column headers in the */
/*                    command log output - this mode is useful when parsing the     */
/*                    command log output in a script                                */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/* Gets a new and unique identifier string from the service on the remote host      */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_get_unique_ident(char         *remotehostname, 
                                   bool_t       no_headers) {

#undef  SUBNAME
#define SUBNAME "remote_get_unique_ident"

  remote_client_get_unique_ident_args  args;
  remote_client_get_unique_ident_res   remoteresult;
  enum clnt_stat                       stat;
  char                                 msg[MAX_MSG_LEN];
  char                                 errmsg[MAX_MSG_LEN];
  int                                  return_rc = E_OK;
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_GET_UNIQUE_IDENT,
                          (xdrproc_t) xdr_remote_client_get_unique_ident_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_get_unique_ident_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_GET_UNIQUE_IDENT, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.status == E_OK) {

          if (! no_headers) {
             print_string_to_console("Unique Ident\n");
          }

          print_string_to_console(remoteresult.remote_client_get_unique_ident_res_u.outcome.identstring);
          print_string_to_console("\n");
 
        } else {

          snprintf(msg, sizeof(msg), "problem getting unique ident from %s\n",remotehostname);
          print_msg_to_console(msg); 

        }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_query_ident                                                        */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client        structure described in the rmote_client_cntrl() routine above */
/*  no_headers        set to TRUE to suppress the printing of column headers in the */
/*                    command log output - this mode is useful when parsing the     */
/*                    command log output in a script                                */
/*  use_delimiter     set to TRUE to supress any blanks between the columns in the  */
/*                    command log output and a delimiting string between the        */
/*                    columns - this mode is useful when parsing the command log    */
/*                    output in a script                                            */
/*  delimiter_string  the string used to separate columns in the output             */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/* Queries the status of all of the commands associated with the specified ident    */
/* string on the remote system.                                                     */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_query_ident(char         *remotehostname, 
                              char         *identstring, 
                              bool_t       no_headers,
                              bool_t       use_delimiter,
                              char         *delimiter_string) {

#undef  SUBNAME
#define SUBNAME "remote_client_query_ident"

  remote_client_query_ident_args  args;
  remote_client_query_ident_res   remoteresult;
  enum clnt_stat                  stat;
  char                            msg[MAX_MSG_LEN];
  char                            errmsg[MAX_MSG_LEN];
  int                             return_rc = E_OK;
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.identstring  = identstring;
    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_QUERY_IDENT,
                          (xdrproc_t) xdr_remote_client_query_ident_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_query_ident_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_QUERY_IDENT, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.status == E_OK) {

           print_cmd_log_fragment(&remoteresult.remote_client_query_ident_res_u.outcome.cmd_log_fragment, no_headers, use_delimiter, delimiter_string);
 
           deallocate_cmd_log_fragment(&remoteresult.remote_client_query_ident_res_u.outcome.cmd_log_fragment);

        } else {

          snprintf(msg, sizeof(msg), "problem determinting commands with ident %s on %s\n", identstring, remotehostname);
          print_msg_to_console(msg); 

/*          snprintf(msg, sizeof(msg), "ident %s is not in use on %s\n", identstring, remotehostname);
          print_msg_to_console(msg); */

        }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_query_cmds                                                         */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client        structure described in the rmote_client_cntrl() routine above */
/*  no_headers        set to TRUE to suppress the printing of column headers in the */
/*                    command log output - this mode is useful when parsing the     */
/*                    command log output in a script                                */
/*  use_delimiter     set to TRUE to supress any blanks between the columns in the  */
/*                    command log output and a delimiting string between the        */
/*                    columns - this mode is useful when parsing the command log    */
/*                    output in a script                                            */
/*  delimiter_string  the string used to separate columns in the output             */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/* Queries the status of the commands that have been run on the remote system that  */
/* have not had a '-clear tag' operation run against them.  In effect this is all   */
/* the commands that are in the CMD_INITIALISED, CMD_RUNNING/CMD_BG_RUNNING or      */
/* CMD_COMPLETED states and have not been cleared out of the command log.   The     */
/* following totals are printed:                                                    */
/*                                                                                  */
/*  number of commands in the initialised state                                     */
/*  number of commands in the running state                                         */
/*  number of commands in the completed state                                       */
/*  number of commands that have completed with good status                         */
/*  number of commands that have completed with bad  status                         */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_query_cmds(char         *remotehostname, 
                             bool_t       no_headers,
                             bool_t       use_delimiter,
                             char         *delimiter_string) {

#undef  SUBNAME
#define SUBNAME "remote_client_query_cmds"

  remote_client_query_cmds_args  args;
  remote_client_query_cmds_res   remoteresult;
  enum clnt_stat                 stat;
  char                           msg[MAX_MSG_LEN];
  char                           errmsg[MAX_MSG_LEN];
  int                            return_rc = E_OK;

  char                          *fmt_title = "%12s  %12s  %12s  %18s  %18s\n";
  char                          *fmt       = "%12d  %12d  %12d  %18d  %18d\n";
  char                          *fmt_title_delim = "%s%s%s%s%s%s%s%s%s\n";
  char                          *fmt_delim       = "%d%s%d%s%d%s%d%s%d\n";
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_QUERY_CMDS,
                          (xdrproc_t) xdr_remote_client_query_cmds_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_query_cmds_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_QUERY_CMDS, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.status == E_OK) {     /* valid or result ?? <<<<<<<<<<< */

          if (! no_headers) {
             if (use_delimiter) {
                 snprintf(msg, sizeof(msg), fmt_title_delim, "initialised", "running", "completed", "completed_good", "completed_bad");
             } else {
                 snprintf(msg, sizeof(msg), fmt_title, "initialised", "running", "completed", "completed_good", "completed_bad");
             }
             print_string_to_console(msg);
          }
          if (use_delimiter) {
             snprintf(msg, sizeof(msg), fmt_delim, 
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_INITIALISED_commands,
                                        delimiter_string,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_RUNNING_commands,
                                        delimiter_string,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_commands,
                                        delimiter_string,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_good_status_commands,
                                        delimiter_string,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_bad_status_commands);
          } else {
             snprintf(msg, sizeof(msg), fmt,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_INITIALISED_commands,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_RUNNING_commands,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_commands,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_good_status_commands,
                                        remoteresult.remote_client_query_cmds_res_u.outcome.num_CMD_COMPLETED_bad_status_commands);
          }
          print_string_to_console(msg);


        } else {

          snprintf(msg, sizeof(msg), "problem running query cmds on %s\n", remotehostname);
          print_msg_to_console(msg); 

/*          snprintf(msg, sizeof(msg), "ident %s is not in use on %s\n", identstring, remotehostname);
          print_msg_to_console(msg); */

        }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_clear_tag                                                          */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  tag            the tag whose entry in the command log is to be cleared          */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/* Clears any record of the command associated with the specified tag on the        */
/* remote system.  It is only possible to clear a command that is in the            */
/* CMD_COMPLETED state.  Once the tag has been cleared the command will no longer   */
/* be reported in the "-query cmdlog" or "-query cmds" operations                   */
/* -------------------------------------------------------------------------------- */

int remote_client_clear_tag(char *remotehostname, Iu32 tag) { 

#undef  SUBNAME
#define SUBNAME "remote_client_clear_tag"

  remote_client_clear_tag_args  args;
  remote_client_clear_tag_res   remoteresult;
  enum clnt_stat                stat;
  char                          msg[MAX_MSG_LEN];
  char                          errmsg[MAX_MSG_LEN];
  int                           return_rc = E_OK;
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,      0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));
 
    args.tag      = tag;
    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_CLEAR_TAG,
                          (xdrproc_t) xdr_remote_client_clear_tag_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_clear_tag_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg), "%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_CLEAR_TAG, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.remote_client_clear_tag_res_u.outcome.valid_clear == REMOTE_CLEAR_TAG_OK) {

          snprintf(msg, sizeof(msg), "Tag %d: cleared ok\n", tag);
          print_msg_to_console(msg);

       } else {

          return_rc = E_NOTOK;

          if (remoteresult.remote_client_clear_tag_res_u.outcome.valid_clear == REMOTE_CLEAR_TAG_DOES_NOT_EXIST) {

             snprintf(msg, sizeof(msg), "Tag %d was not in use on %s\n", tag, remotehostname);
             print_msg_to_console(msg);
     
          } else if (remoteresult.remote_client_clear_tag_res_u.outcome.valid_clear == REMOTE_CLEAR_TAG_NOT_IN_COMPLETED_STATE) {

             snprintf(msg, sizeof(msg), "Tag %d was not in completed state on %s\n", tag, remotehostname);
             print_msg_to_console(msg);

          } else {

             snprintf(msg, sizeof(msg), "Tag %d was not cleared on %s\n", tag, remotehostname);
             print_msg_to_console(msg);

          }

       }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_clear_ident                                                        */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  identstring    the ident string that identifies the commands that are to be     */
/*                 cleared from the command log                                     */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description:                                                                     */
/*                                                                                  */
/* Clears any record of all commands associated with the specified ident string on  */
/* the remote system.  It is only possible to clear a command that is in the        */
/* CMD_COMPLETED state.  Once the tag has been cleared the command will no longer   */
/* be reported in the "-query cmdlog" or "-query cmds" operations                   */
/* -------------------------------------------------------------------------------- */

int remote_client_clear_ident(char *remotehostname, char *identstring) { 

#undef  SUBNAME
#define SUBNAME "remote_client_clear_ident"

  remote_client_clear_ident_args  args;
  remote_client_clear_ident_res   remoteresult;
  enum clnt_stat                  stat;
  char                            msg[MAX_MSG_LEN];
  char                            errmsg[MAX_MSG_LEN];
  int                             return_rc = E_OK;
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,      0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));
 
    args.identstring    = identstring;
    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_CLEAR_IDENT,
                          (xdrproc_t) xdr_remote_client_clear_ident_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_clear_ident_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg), "%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_CLEAR_IDENT, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.remote_client_clear_ident_res_u.outcome.valid_clear == REMOTE_CLEAR_IDENT_OK) {

          snprintf(msg, sizeof(msg), "ident %s: cleared ok\n", identstring);
          print_msg_to_console(msg);

       } else {

          return_rc = E_NOTOK;

          if (remoteresult.remote_client_clear_ident_res_u.outcome.valid_clear == REMOTE_CLEAR_IDENT_NOT_IN_COMPLETED_STATE) {

             snprintf(msg, sizeof(msg), "Some commands with ident %s were not in completed state on %s\n", identstring, remotehostname);
             print_msg_to_console(msg);

          } else {

             snprintf(msg, sizeof(msg), "ident %s was not cleared on %s\n", identstring, remotehostname);
             print_msg_to_console(msg);

          }

       }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_clear_alltags                                                      */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/* Clears any record of all completed commands on the remote system.                */
/* Once the tag has been cleared the command will no longer                         */
/* be reported in the "-query cmdlog" or "-query cmds" operations                   */
/* -------------------------------------------------------------------------------- */

int remote_client_clear_alltags(char *remotehostname) { 

#undef  SUBNAME
#define SUBNAME "remote_client_clear_alltags"

  remote_client_clear_alltags_args  args;
  remote_client_clear_alltags_res   remoteresult;
  enum clnt_stat                    stat;
  char                              msg[MAX_MSG_LEN];
  char                              errmsg[MAX_MSG_LEN];
  int                               return_rc = E_OK;
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,      0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.msglevel = 0;
 
    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_CLEAR_ALLTAGS,
                          (xdrproc_t) xdr_remote_client_clear_alltags_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_clear_alltags_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                SUBNAME, (long) CLIENT_REMOTE_CLIENT_CLEAR_ALLTAGS, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.remote_client_clear_alltags_res_u.outcome.valid_clear == REMOTE_CLEAR_TAG_OK) {

          snprintf(msg, sizeof(msg), "All tags cleared ok\n");
          print_msg_to_console(msg);

       } else {

          return_rc = E_NOTOK;

          snprintf(msg, sizeof(msg), "One more more tags were not cleared on %s\n", remotehostname);
          print_msg_to_console(msg);

       }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_query_alltags                                                      */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client        structure described in the rmote_client_cntrl() routine above */
/*  no_headers        set to TRUE to suppress the printing of column headers in the */
/*                    command log output - this mode is useful when parsing the     */
/*                    command log output in a script                                */
/*  use_delimiter     set to TRUE to supress any blanks between the columns in the  */
/*                    command log output and a delimiting string between the        */
/*                    columns - this mode is useful when parsing the command log    */
/*                    output in a script                                            */
/*  delimiter_string  the string used to separate columns in the output             */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/* Queries the status of all of the commands in the command log on the remote       */
/* system                                                                           */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_query_alltags(char *remotehostname,
                                bool_t        no_headers,
                                bool_t        use_delimiter,
                                char          *delimiter_string) {

#undef  SUBNAME
#define SUBNAME "remote_client_query_alltags"

  remote_client_query_alltags_args  args;
  remote_client_query_alltags_res   remoteresult;
  enum clnt_stat                    stat;
  char                              msg[MAX_MSG_LEN];
  char                              errmsg[MAX_MSG_LEN];
  int                               return_rc = E_OK;
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));
 
    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_QUERY_ALLTAGS,
                          (xdrproc_t) xdr_remote_client_query_alltags_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_query_alltags_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, (long) CLIENT_REMOTE_CLIENT_QUERY_ALLTAGS, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.status == E_OK) {

           print_cmd_log_fragment(&remoteresult.remote_client_query_alltags_res_u.outcome.cmd_log_fragment, no_headers, use_delimiter, delimiter_string);
           
           deallocate_cmd_log_fragment(&remoteresult.remote_client_query_alltags_res_u.outcome.cmd_log_fragment);

/* what if there are lots of tags and we exceed the UDP/TCP packet limit ? */

       } else {

          return_rc = E_NOTOK;

          snprintf(msg, sizeof(msg), "Problem with query alltags on %s\n", remotehostname);
          print_msg_to_console(msg);

       }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_cancel_tag                                                         */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  tag            the tag associated with the command that will be canceled        */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/*  Terminates the command running on the remote client that is associated with     */
/*  the specified tag.  The command is terminated by a kill -9 to its process       */
/*  group, so any sub processes for that command should also be terminated.         */
/*  The command will have CMD_COMPLETED state and will have a status indicating     */
/*  that a signal9 was used to terminate it.                                        */
/* -------------------------------------------------------------------------------- */

int remote_client_cancel_tag(char *remotehostname, Iu32 tag) { 

#undef  SUBNAME
#define SUBNAME "remote_client_cancel_tag"

  remote_client_cancel_tag_args  args;
  remote_client_cancel_tag_res   remoteresult;
  enum clnt_stat                 stat;
  char                           msg[MAX_MSG_LEN];
  char                           errmsg[MAX_MSG_LEN];
  int                            return_rc = E_OK;
  CLIENT *                       remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,      0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));
 
    args.tag      = tag;
    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_CANCEL_TAG,
                          (xdrproc_t) xdr_remote_client_cancel_tag_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_cancel_tag_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, (long) CLIENT_REMOTE_CLIENT_CANCEL_TAG, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.remote_client_cancel_tag_res_u.outcome.valid_cancel == REMOTE_CANCEL_TAG_OK) {

          snprintf(msg, sizeof(msg), "Tag %d: canceled ok\n", tag);
          print_msg_to_console(msg);

        } else {

          return_rc = E_NOTOK;

          if (remoteresult.remote_client_cancel_tag_res_u.outcome.valid_cancel == REMOTE_CANCEL_TAG_DOES_NOT_EXIST) {

             snprintf(msg, sizeof(msg), "Tag %d was not in use on %s\n", tag, remotehostname);
             print_msg_to_console(msg);
     
          } else if (remoteresult.remote_client_cancel_tag_res_u.outcome.valid_cancel == REMOTE_CANCEL_TAG_COMMAND_ALREADY_COMPLETED) {

             snprintf(msg, sizeof(msg), "Tag %d was already in completed state on %s\n", tag, remotehostname);
             print_msg_to_console(msg);

          } else {

             snprintf(msg, sizeof(msg), "Tag %d was not canceled on %s\n", tag, remotehostname);
             print_msg_to_console(msg);

          }

       }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_cancel_ident                                                       */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  identstring    the ident string associated with the commands that will be       */
/*                 canceled                                                         */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/*  Terminates all the commands running on the remote client that are associated    */
/*  with the specified ident string.  The commands are terminated by a kill -9 to   */
/*  their process group, so any sub processes for that command should also be       */
/*  terminated. The command will have CMD_COMPLETED state and will have a status    */
/*  indicating that a signal9 was used to terminate it.                             */
/* -------------------------------------------------------------------------------- */

int remote_client_cancel_ident(char *remotehostname, char *identstring) { 

#undef  SUBNAME
#define SUBNAME "remote_client_cancel_ident"

  remote_client_cancel_ident_args  args;
  remote_client_cancel_ident_res   remoteresult;
  enum clnt_stat                   stat;
  char                             msg[MAX_MSG_LEN];
  char                             errmsg[MAX_MSG_LEN];
  int                              return_rc = E_OK;
  CLIENT *                remote_client;

  /* Create a CLIENT data structure that reference the RPC
     procedure DAF_PROG, version DAF_VERSION running on the
     host specified by the 1st command line arg. */

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));
 
    args.identstring = identstring;
    args.msglevel = 0;

    if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_CANCEL_IDENT,
                          (xdrproc_t) xdr_remote_client_cancel_ident_args, (caddr_t) &args,
                          (xdrproc_t) xdr_remote_client_cancel_ident_res,  (caddr_t) &remoteresult,
                          TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(stat, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, (long) CLIENT_REMOTE_CLIENT_CANCEL_IDENT, stat, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.remote_client_cancel_ident_res_u.outcome.valid_cancel == REMOTE_CANCEL_IDENT_OK) {

          snprintf(msg, sizeof(msg), "ident %s: canceled ok\n", identstring);
          print_msg_to_console(msg);

        } else {

          return_rc = E_NOTOK;

          if (remoteresult.remote_client_cancel_ident_res_u.outcome.valid_cancel == REMOTE_CANCEL_IDENT_DOES_NOT_EXIST) {

             snprintf(msg, sizeof(msg), "ident %s was not in use on %s\n", identstring, remotehostname);
             print_msg_to_console(msg);
     
          } else if (remoteresult.remote_client_cancel_ident_res_u.outcome.valid_cancel == REMOTE_CANCEL_IDENT_COMMAND_ALREADY_COMPLETED) {

             snprintf(msg, sizeof(msg), "ident %s was already in completed state on %s\n", identstring, remotehostname);
             print_msg_to_console(msg);

          } else {

             snprintf(msg, sizeof(msg), "ident %s was not canceled on %s\n", identstring, remotehostname);
             print_msg_to_console(msg);

          }

       }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_get_dirlist                                                        */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  remotepathname the pathname of the directory on the remote host whose contents  */
/*                 is to be listed                                                  */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  dirlist        pointer to linked list of directory names - this must be freed   */
/*                 with deallocate_dirlist(dirlist) when the names are no longer    */
/*                 required                                                         */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/*  Gets the directory contents of a remote directory                               */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_get_dirlist(char *remotehostname, char *remotepathname, dirlist *ppdl) {

#undef  SUBNAME
#define SUBNAME "remote_client_get_dirlist"

  remote_client_dirlist_args     args;
  remote_client_dirlist_res      remoteresult;
  enum clnt_stat                 status;
  char                           msg[MAX_MSG_LEN];
  char                           errmsg[MAX_MSG_LEN];
  int                            return_rc = E_OK;
  CLIENT *                       remote_client;

  *ppdl = NULL;
  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

       memset(&args,         0, sizeof(args));
       memset(&remoteresult, 0, sizeof(remoteresult));

       args.remotepathname   = remotepathname;
       args.msglevel         = 0;

       if ((status = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_DIRLIST,
                               (xdrproc_t) xdr_remote_client_dirlist_args, (caddr_t) &args,
                               (xdrproc_t) xdr_remote_client_dirlist_res,  (caddr_t) &remoteresult,
                               TIMEOUT)) != RPC_SUCCESS) {
          clnt_stat_decode(status, errmsg, sizeof(errmsg));
          snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, (long) CLIENT_REMOTE_CLIENT_COPYFILE, status, errmsg);
          print_msg_to_console(msg);
          return_rc = E_NOTOK;

      } else {

          if (remoteresult.status == E_OK) {
   
             *ppdl = remoteresult.remote_client_dirlist_res_u.outcome.directory;

          } else {
     
             snprintf(msg, sizeof(msg), "%s: problem getting dirlist for %s from %s\n", SUBNAME, remotepathname, remotehostname);
             print_msg_to_console(msg);
             return_rc = E_NOTOK;

          }


       }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_dirlist                                                            */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  remotepathname the pathname of the directory on the remote host whose contents  */
/*                 is to be listed                                                  */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/*  Gets the directory contents of a remote directory                               */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_dirlist(char *remotehostname, char *remotepathname) {

#undef  SUBNAME
#define SUBNAME "remote_client_dirlist"

  char                           msg[MAX_MSG_LEN];
  char                           errmsg[MAX_MSG_LEN];
  int                            return_rc = E_OK;
  dirlist                        pdl;  
  dirlist                        pdl_save;  

  if (remote_client_get_dirlist(remotehostname, remotepathname, &pdl) != 0) {

    snprintf(errmsg, sizeof(errmsg), "%s:  dirlist failed", SUBNAME);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    pdl_save = pdl;

    while (pdl != NULL) {
       snprintf(msg, sizeof(msg), "%s\n", pdl->entry);
       print_msg_to_console(msg);
       pdl = pdl->next;
    }

    deallocate_dirlist(pdl_save);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_copyfile                                                           */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  localpathname  the pathname of the file on the local host                       */
/*  remotepathname the pathname of the file on the remote host                      */
/*  fromremote     FALSE if the copy direction is from the local to the remote      */
/*                 host, TRUE if the direction is from remote to local              */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/*  Copies a between the local system and the remote client.                        */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_copyfile(char *remotehostname, char *localpathname, char *remotepathname, bool_t fromremote) {

#undef  SUBNAME
#define SUBNAME "remote_client_copyfile"

  remote_client_copyfile_args    args;
  remote_client_copyfile_res     remoteresult;
  enum clnt_stat                 status;
  char                           msg[MAX_MSG_LEN];
  char                           errmsg[MAX_MSG_LEN];
  int                            return_rc = E_OK;
  Iu8                            *pbuffer = NULL;
  bool_t                         done = FALSE;
  FILE                           *fptr = NULL;
  int                            numbyteswrittentofile;
  int                            transfersize;
  struct stat                    statbuf;
  int                            rc;
  long                           offset;     /* Iu64 ???? <<<<<<<<<<<<< */
  CLIENT *                remote_client;


  transfersize = 4096; /* <<<<<<<<<<<<<<< */
  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    if ((pbuffer = malloc(sizeof(Iu8) * transfersize)) == NULL) {
       snprintf(errmsg, sizeof(errmsg), "%s: could not malloc %d bytes : ", SUBNAME, transfersize);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;
    }

    if (fromremote) {

       if ((fptr = fopen(localpathname, "wb")) == NULL ) {

          snprintf(msg, sizeof(msg), "%s: Could not open %s\n", SUBNAME, localpathname);
          print_msg_to_console(msg);
          return_rc = E_NOTOK;

       }

       offset = 0;

       while ((! done) && (return_rc == E_OK)) {

          memset(&args,         0, sizeof(args));
          memset(&remoteresult, 0, sizeof(remoteresult));

          args.remotepathname   = remotepathname;
          args.fromremote       = TRUE;
          args.requestedlength  = transfersize;
          args.offset           = offset;
          args.msglevel         = 0;

          args.data.data_len = 0;
          args.data.data_val = (char *) NULL;

          remoteresult.remote_client_copyfile_res_u.outcome.data.data_len = transfersize;
          remoteresult.remote_client_copyfile_res_u.outcome.data.data_val = (char *) pbuffer;

          if ((status = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_COPYFILE,
                                  (xdrproc_t) xdr_remote_client_copyfile_args, (caddr_t) &args,
                                  (xdrproc_t) xdr_remote_client_copyfile_res,  (caddr_t) &remoteresult,
                                  TIMEOUT)) != RPC_SUCCESS) {
             clnt_stat_decode(status, errmsg, sizeof(errmsg));
             snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, (long) CLIENT_REMOTE_CLIENT_COPYFILE, status, errmsg);
             print_msg_to_console(msg);
             return_rc = E_NOTOK;

          } else {

             if (remoteresult.status == E_OK) {

                if (remoteresult.remote_client_copyfile_res_u.outcome.valid_copyfile == REMOTE_COPYFILE_OK) {

                   numbyteswrittentofile = fwrite(pbuffer, sizeof(Iu8), remoteresult.remote_client_copyfile_res_u.outcome.num_bytes_in_transfer, fptr);
                   if (numbyteswrittentofile != remoteresult.remote_client_copyfile_res_u.outcome.num_bytes_in_transfer) {
                      snprintf(msg, sizeof(msg), "%s: only %u bytes written to %s when expecting to write %u bytes\n", 
                                                 SUBNAME, numbyteswrittentofile, localpathname, remoteresult.remote_client_copyfile_res_u.outcome.num_bytes_in_transfer);
                      print_msg_to_console(msg);
                      return_rc = E_NOTOK;
                   }
                   if (remoteresult.remote_client_copyfile_res_u.outcome.eof) {
                      done = TRUE;
                   }

                }  else if (remoteresult.remote_client_copyfile_res_u.outcome.valid_copyfile == REMOTE_COPYFILE_FILE_DOES_NOT_EXIST) {

                   snprintf(msg, sizeof(msg), "%s: remote file %s on remote host %s does not exist\n", SUBNAME, remotepathname, remotehostname);
                   print_msg_to_console(msg);
                   return_rc = E_NOTOK;

                }  else {

                   snprintf(msg, sizeof(msg), "%s: problem copying remote file %s from %s to local file %s\n", SUBNAME, remotepathname, remotehostname, localpathname);
                   print_msg_to_console(msg);
                   return_rc = E_NOTOK;

                }

             } else {
     
                snprintf(msg, sizeof(msg), "%s: problem copying remote file %s from %s to local file %s\n", SUBNAME, remotepathname, remotehostname, localpathname);
                print_msg_to_console(msg);
                return_rc = E_NOTOK;

             }

          }

          offset = offset + transfersize;

       }

    } else {

       if ((rc = stat(localpathname, &statbuf)) != 0) {
          snprintf(msg, sizeof(msg), "%s: local file %s does not exist\n", SUBNAME, localpathname);
          print_msg_to_console(msg);
          return_rc = E_NOTOK;
       }

       if ((fptr = fopen(localpathname, "r")) == NULL ) {

          snprintf(msg, sizeof(msg), "%s: Could not open local file %s\n", SUBNAME, localpathname);
          print_msg_to_console(msg);
          return_rc = E_NOTOK;

       }

       offset = 0;

       while ((! done) && (return_rc == E_OK)) {

          memset(&args,         0, sizeof(args));
          memset(&remoteresult, 0, sizeof(remoteresult));

          args.remotepathname   = remotepathname;
          args.fromremote       = FALSE;
          args.requestedlength  = transfersize;
          args.eof              = FALSE;

          fseek(fptr, offset, SEEK_SET);          /* <<<< check return code */
          args.requestedlength = fread(pbuffer, sizeof(Iu8), transfersize, fptr);

          args.data.data_len = args.requestedlength;
          args.data.data_val = (char *) pbuffer;

          if (feof(fptr)) {
             args.eof = TRUE;
             done = TRUE;
          }

          remoteresult.remote_client_copyfile_res_u.outcome.data.data_len = 0;
          remoteresult.remote_client_copyfile_res_u.outcome.data.data_val = (char *) NULL;

          if ((status = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_COPYFILE,
                                  (xdrproc_t) xdr_remote_client_copyfile_args, (caddr_t) &args,
                                  (xdrproc_t) xdr_remote_client_copyfile_res,  (caddr_t) &remoteresult,
                                  TIMEOUT)) != RPC_SUCCESS) {
             clnt_stat_decode(status, errmsg, sizeof(errmsg));
             snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, (long) CLIENT_REMOTE_CLIENT_COPYFILE, status, errmsg);
             print_msg_to_console(msg);
             return_rc = E_NOTOK;

          } else {

             if (remoteresult.status == E_OK) {

                if (remoteresult.remote_client_copyfile_res_u.outcome.valid_copyfile == REMOTE_COPYFILE_OK) {

                }  else {

                   snprintf(msg, sizeof(msg), "%s: valid_copyfile = %d, problem copying local file %s to remote file %s on %s\n", 
                                              SUBNAME, remoteresult.remote_client_copyfile_res_u.outcome.valid_copyfile, localpathname, remotepathname, remotehostname);
                   print_msg_to_console(msg);
                   return_rc = E_NOTOK;

                }

             } else {
     
                snprintf(msg, sizeof(msg), "%s: remoteresult.status != 0, problem copying local file %s to remote file %s on %s\n", SUBNAME, localpathname, remotepathname, remotehostname);
                print_msg_to_console(msg);
                return_rc = E_NOTOK;

             }

          }

          offset = offset + args.requestedlength;
       }

    }

    if (fptr != NULL) {
       fclose(fptr);
    }

    if (pbuffer != NULL) {
       free(pbuffer);
    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);
} 

/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_deletefile                                                         */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  remotepathname the pathname of the file on the remote host to be deleted        */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/*  Deletes a file on the remote client.                                             */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_deletefile(char *remotehostname, char *remotepathname) {

#undef  SUBNAME
#define SUBNAME "remote_client_deletefile"

  remote_client_deletefile_args    args;
  remote_client_deletefile_res     remoteresult;
  enum clnt_stat                   status;
  char                             msg[MAX_MSG_LEN];
  char                             errmsg[MAX_MSG_LEN];
  int                              return_rc = E_OK;
  CLIENT *                         remote_client;


  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.remotepathname   = remotepathname;
    args.msglevel         = 0;

    if ((status = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_DELETEFILE,
                            (xdrproc_t) xdr_remote_client_deletefile_args, (caddr_t) &args,
                            (xdrproc_t) xdr_remote_client_deletefile_res,  (caddr_t) &remoteresult,
                            TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(status, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, (long) CLIENT_REMOTE_CLIENT_DELETEFILE, status, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.status != E_OK) {

          snprintf(msg, sizeof(msg), "%s: problem deleting remote file %s on %s\n", SUBNAME, remotepathname, remotehostname);
          print_msg_to_console(msg);
          return_rc = E_NOTOK;

       }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);

} 


/* -------------------------------------------------------------------------------- */
/*                                                                                  */
/* remote_client_update_workqueue_status                                            */
/*                                                                                  */
/* Inputs:                                                                          */
/*                                                                                  */
/*  rpc_client     structure described in the rmote_client_cntrl() routine above    */
/*  etc                                                                             */
/*                                                                                  */
/* Outputs:                                                                         */
/*                                                                                  */
/*  None                                                                            */
/*                                                                                  */
/* Returns:                                                                         */
/*                                                                                  */
/*  E_OK           if the routine successfully ran the remote operation and it      */
/*                 completed without error                                          */
/*  E_NOTOK        An error occurred in trying to run this operation at the         */
/*                 remote client                                                    */
/*                                                                                  */
/* Description                                                                      */
/*                                                                                  */
/*  Sends a request to the remote client (which is expected to be the DAF server)   */
/*  to update a workqueue records in the DAF status database                        */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */

int remote_client_update_workqueue_status(char   *remotehostname, 
                                          int    workqueueID, 
                                          bool_t update_tag,
                                          int    tag,
                                          bool_t update_state,
                                          char   *state,
                                          bool_t update_pass,
                                          int    pass, 
                                          bool_t update_end) {

#undef  SUBNAME
#define SUBNAME "remote_client_update_workqueue_status"

  remote_client_update_workqueue_status_args    args;
  remote_client_update_workqueue_status_res     remoteresult;
  enum clnt_stat                                status;
  char                             msg[MAX_MSG_LEN];
  char                             errmsg[MAX_MSG_LEN];
  int                              return_rc = E_OK;
  CLIENT *                         remote_client;

  remote_client = clnt_create(remotehostname, DAF_PROG, DAF_VERSION, "tcp");  

  /* Make sure the create worked */
  if (remote_client == (CLIENT *) NULL) {

    snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, remotehostname);
    snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
    print_msg_to_console(msg);
    return_rc = E_NOTOK;

  } else {

    memset(&args,         0, sizeof(args));
    memset(&remoteresult, 0, sizeof(remoteresult));

    args.workqueueID      = workqueueID;
    args.update_tag       = update_tag;
    args.tag              = tag;
    args.update_state     = update_state;
    args.state            = state;
    args.update_pass      = update_pass;
    args.pass             = pass;
    args.update_end       = update_end;
    args.msglevel         = 0;

    if ((status = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_UPDATE_WORKQUEUE_STATUS,
                            (xdrproc_t) xdr_remote_client_update_workqueue_status_args, (caddr_t) &args,
                            (xdrproc_t) xdr_remote_client_update_workqueue_status_res,  (caddr_t) &remoteresult,
                            TIMEOUT)) != RPC_SUCCESS) {
       clnt_stat_decode(status, errmsg, sizeof(errmsg));
       snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", SUBNAME, 
                (long) CLIENT_REMOTE_CLIENT_UPDATE_WORKQUEUE_STATUS, status, errmsg);
       print_msg_to_console(msg);
       return_rc = E_NOTOK;

    } else {

       if (remoteresult.status != E_OK) {

          snprintf(msg, sizeof(msg), "%s: problem updating workqueue record %u on %s\n", SUBNAME, workqueueID, remotehostname);
          print_msg_to_console(msg);
          return_rc = E_NOTOK;

       }

    }

    if (remote_client->cl_auth != NULL) {
       auth_destroy(remote_client->cl_auth);
    }

    clnt_destroy(remote_client);

  }

  return(return_rc);

} 

