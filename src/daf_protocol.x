
/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf automation framework                                                                                             */
/*                                                                                                                      */
/* daf_protocol.x                                                                                                       */
/*                                                                                                                      */
/* This file is part of the daf automation agent                                                                        */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#ifndef _DAF_PROTOCOL_H
#define _DAF_PROTOCOL_H

const MAX_SQLSERVERNAME_LEN   = 64;
const MAX_SQLUSERNAME_LEN     = 64;
const MAX_SQLPASSWORD_LEN     = 64;
const MAX_SQLDATABASENAME_LEN = 64;
const MAX_SQLSOCKETNAME_LEN   = 64;

const MAX_MSG_LEN             = 512;
const MAX_HOSTNAME_LEN        = 64;
const MAX_PATHNAME_LEN        = 128;
const MAX_FILEHANDLE_LEN      = 64;
const MAX_SYSTEM_CMD_LEN      = 1024;
const E_OK                    = 0;
const E_NOTOK                 = 1;
const MAX_CMD_LEN             = 1024;
const MAX_CMD_OUTPUT_LINE_LEN = 1024;
const MAX_IDENT_LEN           = 32;

const DAF_DATABASE_NAME_LEN = 64;          /* max length of a database name in MYSQL */
const DAF_TABLE_NAME_LEN = 64;             /* max length of a database table name in MYSQL */
const DAF_PROJECT_LEN = 24;
const DAF_PROJECTDESCRIPTION_LEN = 256;
const DAF_PHASE_LEN = 24;
const DAF_REQUESTTYPE_LEN = 32;
const DAF_SCENARIO_LEN = 48;
const DAF_AUTHOR_LEN = 32;
const DAF_ACTIONTYPE_LEN  = 32;
const DAF_LOGFILENAME_LEN = 64;
const DAF_STATE_LEN      = 24;
const DAF_STATUS_LEN     = 32;
const DAF_STATEMODIFIER_LEN     = 32;
const DAF_TESTCASE_LEN   = 64;
const DAF_TESTCASE_DESCRIPTION_LEN   = 64;
const DAF_TESTSTAND_LEN  = 32;
const DAF_TESTSTAND_COMMENTS_LEN = 128;
const DAF_DESCRIPTOR1_LEN = 256;
const DAF_DESCRIPTOR1TYPE_LEN = 24;

const DAF_INVOCATION_LEN = 1024;
const DAF_DURATION_LEN = 24;
const DAF_LOGLOCATION_LEN = 128;
const DAF_LOGDIRECTORY_LEN = 128;
const DAF_COMMENTS_LEN = 256;
const DAF_PARAMETERS_LEN = 256;

const DAF_SCENARIORESULT_NAME_LEN = 32;
const DAF_SCENARIORESULT_PASS_LEN = 3;
const DAF_SCENARIORESULT_START_LEN = 24;
const DAF_SCENARIORESULT_END_LEN = 24;
const DAF_SCENARIORESULT_TESTSTAND_LEN= 32;
const DAF_SCENARIORESULT_TESTER_LEN = 32;
const DAF_SCENARIORESULT_LOGLOCATION_LEN = 128;
const DAF_SCENARIORESULT_COMMENTS_LEN = 256;

const DAF_ACTIONRESULT_NAME_LEN = 32;
const DAF_ACTIONRESULT_STATUS_LEN = 32;
const DAF_ACTIONRESULT_INVOCATION_LEN = 1024;
const DAF_ACTIONRESULT_PASS_LEN = 3;
const DAF_ACTIONRESULT_START_LEN = 24;
const DAF_ACTIONRESULT_END_LEN = 24;
const DAF_ACTIONRESULT_COMMENTS_LEN = 256;

const DAF_CLUSTER_NAME_LEN = 32;
const DAF_CLUSTER_COMMENTS_LEN = 128;

const DAF_NODE_NAME_LEN = 32;
const DAF_NODE_MODEL_LEN = 16;
const DAF_NODE_SERIAL_LEN = 16;
const DAF_NODE_COMMENTS_LEN = 128;

const DAF_JOBNAME_LEN = 48;
const DAF_TESTSTAND_LEN = 32;
const DAF_LEVEL_LEN = 32;
const DAF_TESTLEVEL_LEN = 32;
const DAF_TESTER_LEN = 32;

const DAF_MAX_NUM_PARAMS = 10;
const DAF_PARAMETER_NAME_LEN = 32;
const DAF_PARAMETER_DEFAULTVALUE_LEN = 64;
const DAF_PARAMETER_VALUE_LEN = 64;
const DAF_PARAMETER_DESCRIPTION_LEN = 64;

const DAF_HOSTSELECTORTYPE_NAME_LEN = 16;
const DAF_HOSTSELECTORVALUE_NAME_LEN = 32;
const DAF_ACTIONDESCRIPTION_LEN = 64;
const DAF_HOST_NAME_LEN = 32;
const DAF_HOST_TYPE_LEN = 16;
const DAF_HOST_MODEL_LEN = 16;
const DAF_HOST_SERIAL_LEN = 16;
const DAF_HOST_HOSTSELECTORVALUE_LEN = 32;
const DAF_HOST_COMMENTS_LEN = 128;
const DAF_HOST_AGENTSTATUS_LEN = 16;
const DAF_COLLECTORVALUE_LEN = 256;
const DAF_COMPARATOR_LEN = 3;
const DAF_COLLECTORTYPE_NAME_LEN = 32;
const DAF_COLLECTORTYPE_OSTYPE_LEN = 32;
const DAF_COLLECTORTYPE_INVOCATION_LEN = 256;
const DAF_ENVIRONMENT_NAME_LEN = 32;
const DAF_ENVIRONMENT_DESCRIPTION_LEN = 128;
const DAF_ENVIRONMENTVALUE_NAME_LEN = 32;
const DAF_ENVIRONMENTVALUE_VALUE_LEN = 255;
const DAF_PATHNAME_LEN = 128;
const DAF_FILENAME_LEN = 64;
const DAF_COLLECTORSET_NAME_LEN = 32;
const DAF_COLLECTORSET_DESCRIPTION_LEN = 128;
const DAF_MAX_NUM_COLLECTOR_TYPES = 20;
const DAF_OUTCOMEACTION_NAME_LEN = 32;
const DAF_ACTIONONPASS_LEN = 24;
const DAF_ACTIONONFAIL_LEN = 24;
const DAF_ERRORACTION_LEN = 24;
const DAF_NOTIFICATIONONPASS_LEN = 24;
const DAF_NOTIFICATIONONFAIL_LEN = 24;

const DAF_OUTCOMEACTION_LEN = 32;

const DAF_FIRSTNAME_LEN = 32;
const DAF_LASTNAME_LEN = 32;
const DAF_USERNAME_LEN = 32;
const DAF_AREA_LEN = 32;
const DAF_EMAIL_LEN = 64;
const DAF_PASSWORD_LEN = 32;

const DAF_MAILLIST_LEN = 32;
const DAF_MAILDESCRIPTION_LEN = 128;

const DAF_OBJECTTYPENAME_LEN = 32;
const DAF_OBJECTTYPEDESCRIPTION_LEN = 128;
const DAF_OBJECTNAME_LEN = 32;

const DAF_TESTLEVEL_DESCRIPTION_LEN = 128;

const DAF_LICENCE_LEN = 49;

typedef unsigned hyper Iu64;
typedef unsigned int Iu32;
typedef unsigned short Iu16;
typedef unsigned char Iu8;


#define DAF_VERSION_NUMBER 1

typedef string pathnamestring<MAX_PATHNAME_LEN>;   /*  neeeds a +1 ??????????? <<<<<<<<<<<<<<< */
typedef string hostnamestring<MAX_HOSTNAME_LEN>;

enum remote_client_error_types { REMOTE_CLIENT_OK,
                                 REMOTE_CLIENT_ERROR_NO_FREE_REMOTE_CLIENTS,
                                 REMOTE_CLIENT_COULD_NOT_MOUNT,
                                 REMOTE_CLIENT_COULD_NOT_UMOUNT,
                                 REMOTE_CLIENT_ERROR_CANNOT_DESTROY_RPC_CLIENT
};

enum remote_client_clear_tag_error_types { REMOTE_CLEAR_TAG_OK,  
                                           REMOTE_CLEAR_TAG_DOES_NOT_EXIST,
                                           REMOTE_CLEAR_TAG_NOT_IN_COMPLETED_STATE,
                                           REMOTE_CLEAR_TAG_FAILED
};

enum remote_client_clear_ident_error_types { REMOTE_CLEAR_IDENT_OK,  
                                           REMOTE_CLEAR_IDENT_NOT_IN_COMPLETED_STATE,
                                           REMOTE_CLEAR_IDENT_FAILED
};

enum remote_client_query_alltags_error_types { REMOTE_QUERY_ALLTAGS_OK,  
                                               REMOTE_QUERY_ALLTAGS_FAILED
};

enum remote_client_cancel_tag_error_types { REMOTE_CANCEL_TAG_OK,  
                                            REMOTE_CANCEL_TAG_DOES_NOT_EXIST,
                                            REMOTE_CANCEL_TAG_COMMAND_ALREADY_COMPLETED,
                                            REMOTE_CANCEL_TAG_KILLPG_FAILED
};

enum remote_client_cancel_ident_error_types { REMOTE_CANCEL_IDENT_OK,  
                                              REMOTE_CANCEL_IDENT_DOES_NOT_EXIST,
                                              REMOTE_CANCEL_IDENT_COMMAND_ALREADY_COMPLETED,
                                              REMOTE_CANCEL_IDENT_KILLPG_FAILED
};

enum remote_client_copyfile_error_types { REMOTE_COPYFILE_OK,
                                          REMOTE_COPYFILE_FILE_DOES_NOT_EXIST,
                                          REMOTE_COPYFILE_MALLOC_ERROR,
                                          REMOTE_COPYFILE_FAILED,
                                          REMOTE_COPYFILE_COULD_NOT_OPEN_FOR_WRITE,
                                          REMOTE_COPYFILE_COULD_NOT_WRITE_REQUESTED_LENGTH

};

enum remote_client_deletefile_error_types { REMOTE_DELETEFILE_OK,
                                            REMOTE_DELETEFILE_FILE_DOES_NOT_EXIST,
                                            REMOTE_DELETEFILE_COULD_NOT_DELETE_FILE

};

enum remote_client_cmd_error_types { REMOTE_CMD_OK, 
                                     REMOTE_CMD_COULD_NOT_ADD_TO_CMD_LOG
};

enum remote_client_dirlist_error_types { REMOTE_DIRLIST_OK,
                                         REMOTE_DIRLIST_DIRECTORY_DOES_NOT_EXIST,
                                         REMOTE_DIRLIST_COULD_NOT_OPEN_DIR
};

enum remote_client_start_scenario_error_types { REMOTE_START_SCENARIO_OK,
                                                REMOTE_START_SCENARIO_FAILED
};

struct filehandle {
    int len;
    unsigned char bytes[MAX_FILEHANDLE_LEN];
};

/* ------------ command log ------------------ */

enum cmd_log_state { CMD_FREE,
                     CMD_INITIALISED,
                     CMD_RUNNING, 
                     CMD_BG_RUNNING, 
                     CMD_COMPLETED,
                     CMD_CANCELED_TIMELIMIT,
                     CMD_CANCELED_TIMEDOUT, 
                     CMD_COMPLETED_TIMELIMIT,
                     CMD_COMPLETED_TIMEDOUT,
                     CMD_COMPLETION_LOST
};

struct cmd_log_entry {
   unsigned int     tag;
   unsigned int     pid;
   Iu32             workqueueID;
   unsigned int     state;
   int              status;
   unsigned int     start_time;
   unsigned int     end_time;
   unsigned int     timeout_at_time;
   bool             fail_on_timeout;
   char             cmdstring[MAX_CMD_LEN];
   char             identstring[MAX_IDENT_LEN];
   char             agent_log_pathname[MAX_PATHNAME_LEN];
   cmd_log_entry    *next;
};

struct cmd_log {
   cmd_log_entry  *first;
   cmd_log_entry  *last;
};

/* ------ cntrl ---------------------------------------------*/

struct remote_client_cntrl_args {
   bool               clear_shared_memory_flag;
   int                msglevel;
};

union remote_client_cntrl_res switch (int status) {
case 0:
   void;
default:
   void;
};

/* ------ run_cmd  ------------------------------------------------*/

struct remote_client_run_cmd_args {
   string cmdstring<MAX_CMD_LEN>;       
   int    msglevel;
};

struct remote_client_run_cmd_outcome {
   int    valid;   
   string outputline<MAX_CMD_OUTPUT_LINE_LEN>;       
};

union remote_client_run_cmd_res switch (int status) {
case 0:
   remote_client_run_cmd_outcome outcome;
default:
   void;
};

/* ------ prepare_cmd  ------------------------------------------------*/

struct stringlist {    
     string item<>;
     stringlist *next;
};

struct remote_client_prepare_cmd_args {
   Iu32               timeout;                 /* max time allowed for the command, in seconds, if 0 then timeout is infinite */
   string             cmdstring<MAX_CMD_LEN>;       /* only used when run_in_shell = 1 */
   string             identstring<MAX_IDENT_LEN>;
   bool               fail_on_timeout;
   Iu32               actionresultID;
   Iu32               workqueueID;
   int                msglevel;
};

struct remote_client_prepare_cmd_outcome {
   unsigned int tag;
};

union remote_client_prepare_cmd_res switch (int status) {
case 0:
   remote_client_prepare_cmd_outcome outcome;
default:
   void;
};

/* ------ execute_cmd  ------------------------------------------------*/

typedef string environmentvalue<DAF_ENVIRONMENTVALUE_VALUE_LEN>;
 
typedef struct envnode *envlist;
 
struct envnode {
        environmentvalue entry;    /* an enviroment setting */
        envlist          next;     /* next entry            */
};

struct remote_client_execute_cmd_args {
   string             cmdstring<MAX_CMD_LEN>;      
   envlist            environment_settings;         
   int                run_in_shell;
   int                tag;
   Iu32               actionresultID;
   Iu32               workqueueID;
   int                msglevel;
};

struct remote_client_execute_cmd_outcome {
   int    valid;   /* not used*/
};

union remote_client_execute_cmd_res switch (int status) {
case 0:
   remote_client_execute_cmd_outcome outcome;
default:
   void;
};

/* ------ cmd  ------------------------------------------------*/

struct remote_client_cmd_args {
   string             cmdstring<MAX_CMD_LEN>;       /* only used when run_in_shell = 1 */
   stringlist         environment_settings;             /* only used when run_in_shell = 0 */  
   string             identstring<MAX_IDENT_LEN>;
   int                run_in_background;
   int                run_in_shell;
   Iu32               timeout;                 /* max time allowed for the command, in seconds, if 0 then timeout is infinite */
   bool               fail_on_timeout;
   bool               workqueueID_flag;
   Iu32               workqueueID;
   Iu32               actionresultID;
   int                msglevel;
};

struct remote_client_cmd_outcome {
   unsigned int tag;
   int     cmd_exit_code;
   int     cmd_exit_signal;
};

union remote_client_cmd_res switch (int status) {
case 0:
   remote_client_cmd_outcome outcome;
default:
   void;
};

/* ------ start_scenario  ------------------------------------------------*/

typedef string parametername<DAF_PARAMETER_NAME_LEN>;
typedef string parametervalue<DAF_PARAMETER_VALUE_LEN>;
 
typedef struct parameternode *parameterlist;
 
struct parameternode {
   parametername  name;
   parametervalue value;    
   parameterlist  next;     /* next entry            */
};

struct remote_client_start_scenario_args {
   string             project<DAF_PROJECT_LEN>;    
   string             phase<DAF_PHASE_LEN>;      
   string             jobname<DAF_SCENARIO_LEN>; 
   string             loglocation<DAF_LOGDIRECTORY_LEN>;
   string             scenariologfile<DAF_LOGFILENAME_LEN>; 
   string             teststand<DAF_TESTSTAND_LEN>;   
   string             testlevel<DAF_TESTLEVEL_LEN>;  
   string             username<DAF_USERNAME_LEN>;        
   string             comments<DAF_SCENARIORESULT_COMMENTS_LEN>;
   parameterlist      parameters;           
   int                msglevel;
};

#define DAF_ERRMSG_LEN 256
struct remote_client_start_scenario_outcome {
   remote_client_start_scenario_error_types valid_start_scenario;
   string errmsg  <DAF_ERRMSG_LEN>;       
};

union remote_client_start_scenario_res switch (int status) {
case 0:
   remote_client_start_scenario_outcome outcome;
default:
   void;
};

/* ------ query_version  -----------------------------------------*/

#define MAX_VERSION_STRING_LEN 128
#define MAX_AGENT_BUILD_MACHINE_LEN 128
#define MAX_P_NAME_LEN 128
#define MAX_P_OS_TYPE_LEN 128
#define MAX_P_OS_VERSION_LEN 128
#define MAX_P_OS_DESCRIPTION_LEN 128
#define MAX_P_CPU_ARCHITECTURE_LEN 128

struct remote_client_query_version_args {
   unsigned int   agent_major_version;
   unsigned int   agent_minor_version;
   string         agent_version_string<MAX_VERSION_STRING_LEN>;
   bool           set_zombie_reaper_debug_flag;
   bool           reset_zombie_reaper_debug_flag;
   int            msglevel;
};

struct remote_client_query_version_outcome {
   unsigned int   agent_major_version;
   unsigned int   agent_minor_version;
   string         agent_version_string<MAX_VERSION_STRING_LEN>;
   string         agent_build_machine<MAX_AGENT_BUILD_MACHINE_LEN>;
   bool           p_osdata_is_valid;
   string         p_name<MAX_P_NAME_LEN>;
   string         p_os_type<MAX_P_OS_TYPE_LEN>;
   string         p_os_version<MAX_P_OS_VERSION_LEN>;
   string         p_os_description<MAX_P_OS_DESCRIPTION_LEN>;
   string         p_cpu_architecture<MAX_P_CPU_ARCHITECTURE_LEN>;
};

union remote_client_query_version_res switch (int status) {
case 0:
   remote_client_query_version_outcome outcome;
default:
   void;
};

/* ------ query_tag  -----------------------------------------*/

struct remote_client_query_tag_args {
   unsigned int tag;
   int          msglevel;
};

struct remote_client_query_tag_outcome {
   bool            valid;
   unsigned int    state;   
   int             cmd_exit_code;
   int             cmd_exit_signal;
};

union remote_client_query_tag_res switch (int status) {
case 0:
   remote_client_query_tag_outcome outcome;
default:
   void;
};

/* ------ query alltags -------------------------------------*/

struct remote_client_query_alltags_args {
   int msglevel;
};

struct remote_client_query_alltags_outcome {
   bool     last;
   cmd_log  cmd_log_fragment;   
};

union remote_client_query_alltags_res switch (int status) {
case 0:
   remote_client_query_alltags_outcome outcome;
default:
   void;
};

/* ------ query_cmdlog  --------------------------------------*/

struct remote_client_query_cmdlog_args {
   unsigned int index;
   int          msglevel;
};

struct remote_client_query_cmdlog_outcome {
   unsigned int  tag;
   unsigned int  pid;
   unsigned int  state;   
   unsigned int  status;
   unsigned int  start_time;
   unsigned int  end_time; 
   unsigned int  timeout_at_time; 
   bool          fail_on_timeout;
   bool          last; 
   string        cmdstring<MAX_CMD_LEN>; 
   string        identstring<MAX_IDENT_LEN>;
   unsigned int  workqueueID;

};

union remote_client_query_cmdlog_res switch (int status) {
case 0:
   remote_client_query_cmdlog_outcome outcome;
default:
   void;
};

/* ------ query_cmds  --------------------------------------*/

struct remote_client_query_cmds_args {
   int msglevel;
};

struct remote_client_query_cmds_outcome {
   unsigned int  num_CMD_INITIALISED_commands;   
   unsigned int  num_CMD_RUNNING_commands;
   unsigned int  num_CMD_COMPLETED_commands;
   unsigned int  num_CMD_COMPLETED_good_status_commands;
   unsigned int  num_CMD_COMPLETED_bad_status_commands;
};

union remote_client_query_cmds_res switch (int status) {
case 0:
   remote_client_query_cmds_outcome outcome;
default:
   void;
};

/* ------ get_unique_ident  ---------------------------------*/

struct remote_client_get_unique_ident_args {
   int      msglevel;
};

struct remote_client_get_unique_ident_outcome {
   string   identstring<MAX_IDENT_LEN>;
};

union remote_client_get_unique_ident_res switch (int status) {
case 0:
   remote_client_get_unique_ident_outcome outcome;
default:
   void;
};


/* ------ query_ident  -----------------------------------------*/

struct remote_client_query_ident_args {
   string   identstring<MAX_IDENT_LEN>;
   int      msglevel;
};

struct remote_client_query_ident_outcome {
   bool     last;
   cmd_log  cmd_log_fragment;   
};

union remote_client_query_ident_res switch (int status) {
case 0:
   remote_client_query_ident_outcome outcome;
default:
   void;
};

/* ------ clear tag  ----------------------------------------*/

struct remote_client_clear_tag_args {
   unsigned int tag;
   int          msglevel;
};

struct remote_client_clear_tag_outcome {
   remote_client_clear_tag_error_types valid_clear;
};

union remote_client_clear_tag_res switch (int status) {
case 0:
   remote_client_clear_tag_outcome outcome;
default:
   void;
};

/* ------ cancel tag  ----------------------------------------*/

struct remote_client_cancel_tag_args {
   unsigned int tag;
   int          msglevel;
};

struct remote_client_cancel_tag_outcome {
   remote_client_cancel_tag_error_types valid_cancel;
};

union remote_client_cancel_tag_res switch (int status) {
case 0:
   remote_client_cancel_tag_outcome outcome;
default:
   void;
};

/* ------ cancel ident ---------------------------------------*/

struct remote_client_cancel_ident_args {
   string       identstring<MAX_IDENT_LEN>;
   int          msglevel;
};

struct remote_client_cancel_ident_outcome {
   remote_client_cancel_ident_error_types valid_cancel;
};

union remote_client_cancel_ident_res switch (int status) {
case 0:
   remote_client_cancel_ident_outcome outcome;
default:
   void;
};

/* ------ clear alltags -------------------------------------*/

struct remote_client_clear_alltags_args {
   int  msglevel;
};

struct remote_client_clear_alltags_outcome {
   remote_client_clear_tag_error_types valid_clear;
};

union remote_client_clear_alltags_res switch (int status) {
case 0:
   remote_client_clear_alltags_outcome outcome;
default:
   void;
};

/* ------ clear_ident  -----------------------------------------*/

struct remote_client_clear_ident_args {
   string       identstring<MAX_IDENT_LEN>;
   int          msglevel;
};

struct remote_client_clear_ident_outcome {
   remote_client_clear_ident_error_types valid_clear;
};

union remote_client_clear_ident_res switch (int status) {
case 0:
   remote_client_clear_ident_outcome outcome;
default:
   void;
};

/* ------ copyfile -------------------------------------------*/

struct remote_client_copyfile_args {
   string  remotepathname<MAX_PATHNAME_LEN>;  /* <<<<<<<<< what should this max be? */
   bool    fromremote;
   int     requestedlength;
   opaque  data<>;
   int     offset;
   bool    eof;
   int     msglevel;
};

struct remote_client_copyfile_outcome {
   bool     valid_copyfile;
   int      num_bytes_in_transfer;                        
   opaque   data<>;
   bool     eof;
};

union remote_client_copyfile_res switch (int status) {
case 0:
   remote_client_copyfile_outcome outcome;
default:
   void;
};

/* ------ deletefile -------------------------------------------*/

struct remote_client_deletefile_args {
   string  remotepathname<MAX_PATHNAME_LEN>;  /* <<<<<<<<< what should this max be? */
   int     msglevel;
};

struct remote_client_deletefile_outcome {
   bool     valid_deletefile;
};

union remote_client_deletefile_res switch (int status) {
case 0:
   remote_client_deletefile_outcome outcome;
default:
   void;
};

/* ------ dirlist ------------------------------------------------*/

/* a directory entry */
typedef string direntrytype<MAX_PATHNAME_LEN>;
 
/* a link in the listing */
typedef struct dirnode *dirlist;
 
/*
 * A node in the directory listing
 */
struct dirnode {
        direntrytype entry;    /* name of directory entry */
        dirlist next;          /* next entry */
};

struct remote_client_dirlist_args {
   string  remotepathname<MAX_PATHNAME_LEN>;    
   int     msglevel;
};

struct remote_client_dirlist_outcome {
   bool     valid_dirlist;
   dirlist  directory;
};

union remote_client_dirlist_res switch (int status) {
case 0:
   remote_client_dirlist_outcome outcome;
default:
   void;
};

/* ------ update_workqueue_status  -------------------------------------------*/

typedef string command_state<DAF_STATE_LEN>;

struct remote_client_update_workqueue_status_args {
   Iu32            workqueueID;
   bool            update_state;
   command_state   state;
   bool            update_tag;
   int             tag; 
   bool            update_pass;
   int             pass; 
   bool            update_end;
   int             msglevel;
};

struct remote_client_update_workqueue_status_outcome {
   bool     valid_update;
};

union remote_client_update_workqueue_status_res switch (int status) {
case 0:
   remote_client_update_workqueue_status_outcome outcome;
default:
   void;
};


program DAF_PROG {
   version DAF_VERSION {
     remote_client_cntrl_res         CLIENT_REMOTE_CLIENT_CNTRL(remote_client_cntrl_args)                 = 41;
     remote_client_cmd_res           CLIENT_REMOTE_CLIENT_CMD(remote_client_cmd_args)                     = 1;
     remote_client_prepare_cmd_res   CLIENT_REMOTE_CLIENT_PREPARE_CMD(remote_client_prepare_cmd_args)     = 47;
     remote_client_execute_cmd_res   CLIENT_REMOTE_CLIENT_EXECUTE_CMD(remote_client_execute_cmd_args)     = 48;
     remote_client_run_cmd_res       CLIENT_REMOTE_CLIENT_RUN_CMD(remote_client_run_cmd_args)             = 49;
     remote_client_start_scenario_res   CLIENT_REMOTE_CLIENT_START_SCENARIO(remote_client_start_scenario_args)  = 50;
     remote_client_query_version_res CLIENT_REMOTE_CLIENT_QUERY_VERSION(remote_client_query_version_args) = 35;
     remote_client_query_tag_res     CLIENT_REMOTE_CLIENT_QUERY_TAG(remote_client_query_tag_args)         = 2;
     remote_client_query_alltags_res CLIENT_REMOTE_CLIENT_QUERY_ALLTAGS(remote_client_query_alltags_args) = 33;
     remote_client_query_cmdlog_res  CLIENT_REMOTE_CLIENT_QUERY_CMDLOG(remote_client_query_cmdlog_args)   = 3;
     remote_client_get_unique_ident_res   CLIENT_REMOTE_CLIENT_GET_UNIQUE_IDENT(remote_client_get_unique_ident_args)  = 43;
     remote_client_query_ident_res   CLIENT_REMOTE_CLIENT_QUERY_IDENT(remote_client_query_ident_args)     = 37;
     remote_client_query_cmds_res    CLIENT_REMOTE_CLIENT_QUERY_CMDS(remote_client_query_cmds_args)       = 40;
     remote_client_clear_tag_res     CLIENT_REMOTE_CLIENT_CLEAR_TAG(remote_client_clear_tag_args)         = 4;
     remote_client_clear_ident_res   CLIENT_REMOTE_CLIENT_CLEAR_IDENT(remote_client_clear_ident_args)     = 38;
     remote_client_clear_alltags_res CLIENT_REMOTE_CLIENT_CLEAR_ALLTAGS(remote_client_clear_alltags_args) = 5;
     remote_client_cancel_tag_res    CLIENT_REMOTE_CLIENT_CANCEL_TAG(remote_client_cancel_tag_args)       = 6;
     remote_client_cancel_ident_res  CLIENT_REMOTE_CLIENT_CANCEL_IDENT(remote_client_cancel_ident_args)    = 39;
     remote_client_copyfile_res      CLIENT_REMOTE_CLIENT_COPYFILE(remote_client_copyfile_args)           = 7;
     remote_client_deletefile_res    CLIENT_REMOTE_CLIENT_DELETEFILE(remote_client_deletefile_args)       = 45;
     remote_client_dirlist_res       CLIENT_REMOTE_CLIENT_DIRLIST(remote_client_dirlist_args)             = 44;
     remote_client_update_workqueue_status_res       CLIENT_REMOTE_CLIENT_UPDATE_WORKQUEUE_STATUS(remote_client_update_workqueue_status_args) = 46;

   } = DAF_VERSION_NUMBER;
} = 500000001;

                                                                                                                                      
#endif
