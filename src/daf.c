/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                        */
/*                                                                                                                      */
/* daf.c                                                                                                                */
/*                                                                                                                      */
/* This file is part of the daf test automation programme.                                                             */
/*                                                                                                                      */
/*   Compile with DAF_AGENT set to get the agent code only                                                              */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>

#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
/* #include <utmp.h> */
/* #include <rpcsvc/rusers.h>  */
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "daf_protocol.h"
#include "daf_util.h"
#include "dafclient.h"
#include "dafservice.h"
#include "daf_shared_memory.h"
#include "daf_service_environment.h"
#include "licence.h"
#include "daf_threads.h"

#include "afmysql.h"

#define TCP_TIMEOUT 30
#define MAX_MSG_LEN 512
#define MAX_HOSTNAME_LEN 64
#define E_OK  0
#define E_NOTOK 1

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif



typedef struct params
{

    bool_t                 quiet_flag;
    char                   remotehostname[MAX_HOSTNAME_LEN+1];
    bool_t                 remote_flag;
    bool_t                 start_scenario_flag;
#ifndef DAF_AGENT
    char                   cmdstring[MAX_SYSTEM_CMD_LEN];
    char                   identstring[MAX_IDENT_LEN];
    char                   localpathname[MAX_PATHNAME_LEN+1];
    char                   remotepathname[MAX_PATHNAME_LEN+1];
    char                   delimiter_string[16];
    bool_t                 no_headers_flag;
    bool_t                 delimiter_flag;
    bool_t                 get_flag;
    bool_t                 get_newident_flag;
    bool_t                 query_flag;
    bool_t                 fromremote_flag;
    bool_t                 query_version_flag;
    bool_t                 action_flag;
    bool_t                 execute_flag;
    bool_t                 tag_flag;
    bool_t                 version_flag;
    bool_t                 query_tag_flag;
    bool_t                 query_alltags_flag;
    bool_t                 query_ident_flag;
    bool_t                 query_cmds_flag;
    bool_t                 clear_flag;
    bool_t                 cancel_flag;
    bool_t                 clear_tag_flag;
    bool_t                 clear_alltags_flag;
    bool_t                 clear_ident_flag;
    bool_t                 clear_shared_memory_flag;
    bool_t                 cancel_tag_flag;
    bool_t                 cancel_ident_flag;
    bool_t                 dirlist_flag;
    bool_t                 getfile_flag;
    bool_t                 putfile_flag;
    bool_t                 report_flag;
    bool_t                 report_cmds_flag;
    bool_t                 run_in_background_flag;
    bool_t                 buffersize_flag;
    Iu32                   buffersize;
    Iu32                   tag;
    int                    timeout;
    bool_t                 set_sql_debug_flag;
    bool_t                 reset_sql_debug_flag;
    bool_t                 set_zombie_reaper_debug_flag;
    bool_t                 reset_zombie_reaper_debug_flag;
    bool_t                 validate_licence_flag;
    bool_t                 write_licence_flag;
    char                   licence_pathname[MAX_PATHNAME_LEN+1];
    char                   licence[DAF_LICENCE_LEN+1];
#endif
    char                   consolelog_pathname[MAX_PATHNAME_LEN+1];
    int                    msglevel;
    bool_t                 service_flag;
    bool_t                 service_start_flag;
    bool_t                 service_stop_flag;
    bool_t                 service_install_flag;
    bool_t                 service_delete_flag;
    bool_t                 service_run_flag;
    bool_t                 simulator_mode_flag;

    char                   sql_servername[MAX_SQLSERVERNAME_LEN];
    char                   sql_username[MAX_SQLUSERNAME_LEN];
    char                   sql_password[MAX_SQLPASSWORD_LEN];
    char                   sql_databasename[MAX_SQLDATABASENAME_LEN];
    Iu16                   sql_port;
    char                   sql_socketname[MAX_SQLSOCKETNAME_LEN];
    bool_t                 workqueueID_flag;
    Iu64                   workqueueID;
    char                   project[DAF_PROJECT_LEN];
    char                   phase[DAF_PHASE_LEN];
	char                   jobname[DAF_SCENARIO_LEN];
	char                   loglocation[DAF_LOGDIRECTORY_LEN];
	char                   scenariologfile[DAF_LOGFILENAME_LEN];
	char                   teststand[DAF_TESTSTAND_LEN];
	char                   testlevel[DAF_TESTLEVEL_LEN];
	char                   comments[DAF_SCENARIORESULT_COMMENTS_LEN];
	char                   username[DAF_USERNAME_LEN];
	parameterlist          parameters;

} parameters_t;

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* Globals                                                                                                */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

parameters_t       parameters;

struct
{
    char  sql_servername[MAX_SQLSERVERNAME_LEN];
    char  sql_username[MAX_SQLUSERNAME_LEN];
    char  sql_password[MAX_SQLPASSWORD_LEN];
    char  sql_databasename[MAX_SQLDATABASENAME_LEN];
    Iu16  sql_port;
    char  sql_socketname[MAX_SQLSOCKETNAME_LEN];
} sql_server;

int daf_major_version = 1;
int daf_minor_version = 1;
char *daf_version_string =  "daf: (compiled on " __DATE__ " at "  __TIME__ ")";
char buildmachine[MAX_MSG_LEN] = "";
char *daf_buildmachine = &buildmachine[0];


bool_t threaded_daf_service = FALSE;
bool_t multiprocess_daf_service = FALSE;

/* Declare flags that may be altered in another execution context (thread or
 * signal handler) as volatile. */
static volatile bool_t SIGINTReceived   = FALSE;   // signal 2
static volatile bool_t SIGTERMReceived  = FALSE;   // signal 15
static volatile bool_t SIGQUITReceived  = FALSE;

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* Private routines                                                                                       */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  print_compile_time_signature                                                               */
/*                                                                                                        */
/* PARAMS IN:      with_time_stamps         TRUE if timestamps are to be included in output messages      */
/*                                                                                                        */
/* PARAMS OUT:     none                                                                                   */
/*                                                                                                        */
/* RETURNS:        0 for success, 1 for failure                                                           */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int print_compile_time_signature(bool_t with_time_stamps)
{

    char msg[MAX_MSG_LEN];
    char uname[MAX_MSG_LEN] = "";

    sprintf(msg, "daf %d.%d: (compiled on %s at %s)\n",  daf_major_version, daf_minor_version, __DATE__, __TIME__);

    if (with_time_stamps)
    {
        print_msg_to_console(msg);
    }
    else
    {
        print_string_to_console(msg);
    }

#ifdef BUILDMACHINE
    safecpy(daf_buildmachine, BUILDMACHINE, sizeof(daf_buildmachine));
#endif

#ifdef UNAME
    safecpy(uname, UNAME, sizeof(uname));
#endif

    sprintf(msg, "daf: build machine: %s, uname: %s\n", daf_buildmachine, uname);

    if (with_time_stamps)
    {
        print_msg_to_console(msg);
    }
    else
    {
        print_string_to_console(msg);
    }

    return E_OK;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  print_usage                                                                                */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_usage()
{

    printf("daf - the Distributed Automation Framework\n\n");
#ifdef DAF_AGENT
    printf("   daf_agent  -service | multiprocess_service [-msglevel N]\n");
    printf("              -query version\n");
    printf("	          -remote hostname -start_scenario jobname -project project -phase phase [-loglocation pathtodir] [-scenariologfile filename] -teststand stand -testlevel level -comments string -username ussername\n");
#else
    printf("   daf  -help | -detailedhelp\n");
    printf("   or\n");
    printf("   daf  -remote hostname\n");
    printf("          [-msglevel N] [-timeout N] [-consolelog pathname] [-quiet]\n");
    printf("           -query version  -debug sql|nosql|zombie_reaper|nozombie_reaper\n");
    printf("           -execute cmdstring [-run_in_background] [-ident identstring [-workqueueID N]\n");
    printf("	       -start_scenario jobname -project project -phase phase -loglocation pathtodir -scenariologfile filename -teststand stand\n");
    printf("              -testlevel level -comments string -parameters name=value name=value...\n");
    printf("           -report cmds\n");
    printf("           -query  cmds [-nohdr] [-delim C]\n");
    printf("           -query  tag -tag N [-nohdr] [-delim C]\n");
    printf("           -clear  tag -tag N\n");
    printf("           -cancel tag -tag N\n");
    printf("           -query  alltags [-nohdr] [-delim C]\n");
    printf("           -clear  alltags\n");
    printf("           -clear  shared_memory\n");
    printf("           -query  ident -ident identstring [-nohdr] [-delim C]\n");
    printf("           -cancel ident -ident string\n");
    printf("           -clear  ident -ident string\n");
    printf("           -dirlist remotepathname\n");
    printf("           -getfile localpathname remotepathname\n");
    printf("           -putfile localname remotepathname\n");
    printf("           -validate_licence licencepathname\n");
    printf("           -write_licence licencepathname -licence NNNNN\n");
    printf("   or\n");
    printf("   daf  -service|-multiprocess_service|-threaded_service install|start|stop|delete|run\n");
    printf("		[-msglevel N]\n");
    printf("	    [-consolelog pathname]\n");
    printf("	    [-simulator_mode]\n");
    printf("        [-sqlserver hostname] [-sqluser username] [-sqlpassword password] [-sqldatabase databasename] [-sqlport N] [-sqlsocketname N]\n");
    printf("   or\n");
    printf("   daf -help | -detailedhelp\n");
#endif
    print_compile_time_signature(FALSE);
    printf("\n");
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  print_detailed_usage                                                                       */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/
void print_detailed_usage()
{

    printf("   The detailed help has not yet been written\n");

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  initialise_args                                                                            */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void initialise_args(parameters_t *parameters)
{


#ifndef DAF_AGENT
    safecpy(parameters->delimiter_string,  ":", sizeof(parameters->delimiter_string));
    parameters->quiet_flag         = FALSE;
    parameters->delimiter_flag     = FALSE;
    parameters->no_headers_flag    = FALSE;
    parameters->get_flag           = FALSE;
    parameters->get_newident_flag  = FALSE;
    parameters->query_flag         = FALSE;
    parameters->clear_flag         = FALSE;
    parameters->cancel_flag        = FALSE;
    parameters->fromremote_flag    = FALSE;
    parameters->query_version_flag = FALSE;
    parameters->action_flag        = FALSE;
    parameters->execute_flag       = FALSE;
    strncpy(parameters->cmdstring, "", sizeof(parameters->cmdstring));
    parameters->start_scenario_flag   = FALSE;
    strncpy(parameters->jobname,         "", sizeof(parameters->jobname));
    strncpy(parameters->project,         "", sizeof(parameters->project));
    strncpy(parameters->phase,           "", sizeof(parameters->phase));
    strncpy(parameters->loglocation,     "", sizeof(parameters->loglocation));
    strncpy(parameters->scenariologfile, "", sizeof(parameters->scenariologfile));
    strncpy(parameters->teststand,       "", sizeof(parameters->teststand));
    strncpy(parameters->testlevel,       "", sizeof(parameters->testlevel));
    strncpy(parameters->comments,        "", sizeof(parameters->comments));
    strncpy(parameters->username,        "", sizeof(parameters->username));
    parameters->validate_licence_flag = FALSE;
    parameters->write_licence_flag = FALSE;
    strncpy(parameters->licence_pathname, "", sizeof(parameters->licence_pathname));
    strncpy(parameters->licence, "", sizeof(parameters->licence));
    parameters->service_flag       = FALSE;
    parameters->service_install_flag  = FALSE;
    parameters->service_start_flag  = FALSE;
    parameters->service_stop_flag  = FALSE;
    parameters->service_delete_flag  = FALSE;
    parameters->service_run_flag  = FALSE;
    parameters->simulator_mode_flag= FALSE;
    parameters->remote_flag        = FALSE;
    parameters->tag_flag           = FALSE;
    parameters->version_flag       = FALSE;
    parameters->query_tag_flag     = FALSE;
    parameters->clear_tag_flag     = FALSE;
    parameters->clear_alltags_flag = FALSE;
    parameters->cancel_tag_flag    = FALSE;
    parameters->cancel_ident_flag  = FALSE;
    parameters->clear_shared_memory_flag    = FALSE;
    parameters->dirlist_flag       = FALSE;
    parameters->getfile_flag       = FALSE;
    parameters->putfile_flag       = FALSE;
    parameters->run_in_background_flag    = FALSE;
    parameters->report_flag        = FALSE;
    parameters->report_cmds_flag   = FALSE;
    parameters->workqueueID_flag   = FALSE;
    parameters->workqueueID        = 0;
    parameters->timeout            = TCP_TIMEOUT;
    parameters->buffersize         = 8192;
    parameters->set_sql_debug_flag     = FALSE;
    parameters->reset_sql_debug_flag   = FALSE;
    parameters->set_zombie_reaper_debug_flag     = FALSE;
    parameters->reset_zombie_reaper_debug_flag   = FALSE;
#endif

    safecpy(parameters->consolelog_pathname, "", sizeof(parameters->consolelog_pathname));
    parameters->msglevel              = 0;
    safecpy(parameters->sql_servername,   DEFAULT_SQLSERVERNAME,   sizeof(parameters->sql_servername));
    safecpy(parameters->sql_username,     DEFAULT_SQLUSERNAME,     sizeof(parameters->sql_username));
    safecpy(parameters->sql_password,     DEFAULT_SQLPASSWORD,     sizeof(parameters->sql_password));
    safecpy(parameters->sql_databasename, DEFAULT_SQLDATABASENAME, sizeof(parameters->sql_databasename));
    parameters->sql_port = DEFAULT_SQLSERVERPORT;
    safecpy(parameters->sql_socketname,   DEFAULT_SQLSOCKETNAME,   sizeof(parameters->sql_socketname));

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  print_parameters                                                                           */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_parameters(parameters_t *parameters)
{

    char          msg[MAX_MSG_LEN];
#ifndef DAF_AGENT
	parameterlist p = parameters->parameters;
#endif

    print_msg_to_console("\n");
    sprintf(msg, "daf:\n");

#ifndef DAF_AGENT
    print_msg_to_console("\n");
    print_msg_to_console(msg);
    sprintf(msg, "-remote:           %s\n", parameters->remotehostname);
    print_msg_to_console(msg);

    if (parameters->execute_flag)
    {
        sprintf(msg, "      -execute:         %s\n", parameters->cmdstring);
        print_msg_to_console(msg);

        if (parameters->run_in_background_flag)
        {
            print_msg_to_console("      -run_in_background:   TRUE\n");
        }
        else
        {
            print_msg_to_console("      -run_in_background:   FALSE\n");
        }
    }

    if (parameters->start_scenario_flag)
    {
        sprintf(msg, "-start_scenario:   %s\n", parameters->jobname);
        print_msg_to_console(msg);
        sprintf(msg, "-project:          %s\n", parameters->project);
        print_msg_to_console(msg);
        sprintf(msg, "-phase:            %s\n", parameters->phase);
        print_msg_to_console(msg);
        sprintf(msg, "-loglocation:      %s\n", parameters->loglocation);
        print_msg_to_console(msg);
        sprintf(msg, "-scenariologfile:  %s\n", parameters->scenariologfile);
        print_msg_to_console(msg);
        sprintf(msg, "-teststand:        %s\n", parameters->teststand);
        print_msg_to_console(msg);
        sprintf(msg, "-testlevel:        %s\n", parameters->testlevel);
        print_msg_to_console(msg);
        sprintf(msg, "-comments:         %s\n", parameters->comments);
        print_msg_to_console(msg);
        sprintf(msg, "-username:         %s\n", parameters->username);
        print_msg_to_console(msg);
	    while (p != NULL) {
	       sprintf(msg, "-parameter:        %s = %s\n", p->name, p->value);
	       print_msg_to_console(msg);
	       p = p->next;
	    }
    }

    sprintf(msg, "-buffersize:       %d\n", parameters->buffersize);
    print_msg_to_console(msg);
    sprintf(msg, "-consolelog:       %s\n", parameters->consolelog_pathname);
    print_msg_to_console(msg);
    sprintf(msg, "-quiet:            %d\n", parameters->quiet_flag);
    print_msg_to_console(msg);
    sprintf(msg, "-nohdr:            %d\n", parameters->no_headers_flag);
    print_msg_to_console(msg);
    if (parameters->delimiter_flag)
    {
       sprintf(msg, "-delim:            %s\n", parameters->delimiter_string);
       print_msg_to_console(msg);
    }
    sprintf(msg, "-msglevel:         %d\n", parameters->msglevel);
    print_msg_to_console(msg);
    sprintf(msg, "-timeout:          %d\n", parameters->timeout);
    print_msg_to_console(msg);

    if (parameters->get_flag)
    {
        sprintf(msg, "-get:\n");
        print_msg_to_console(msg);
    }

    if (parameters->get_newident_flag)
    {
        sprintf(msg, "   newident:\n");
        print_msg_to_console(msg);
    }

    if (parameters->query_flag)
    {
        sprintf(msg, "-query:\n");
        print_msg_to_console(msg);
    }

    if (parameters->dirlist_flag)
    {
        sprintf(msg, "-dirlist:\n");
        print_msg_to_console(msg);
        sprintf(msg, " -remotepath:       %s\n",    parameters->remotepathname);
        print_msg_to_console(msg);
    }

    if (parameters->getfile_flag)
    {
        sprintf(msg, "-getfile:\n");
        print_msg_to_console(msg);
        sprintf(msg, "   -localpath:      %s\n",    parameters->localpathname);
        print_msg_to_console(msg);
        sprintf(msg, "   -remotepath:     %s\n",    parameters->remotepathname);
        print_msg_to_console(msg);
    }

    if (parameters->putfile_flag)
    {
        sprintf(msg, "-putfile:\n");
        print_msg_to_console(msg);
        sprintf(msg, "   -localpath:       %s\n",    parameters->localpathname);
        print_msg_to_console(msg);
        sprintf(msg, "   -remotepath:      %s\n",    parameters->remotepathname);
        print_msg_to_console(msg);
    }

    sprintf(msg, "\n");
    print_msg_to_console(msg);
#endif

}



/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  parse_args                                                                                 */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int parse_args(int argc, char **argv, parameters_t *parameters )
{

    int i;
    char  thishostname[MAX_HOSTNAME_LEN];
    char  temp[64];
	bool_t end_of_params;
	int        num_daf_params;
	char       parameter_name[DAF_PARAMETER_NAME_LEN];
	char       parameter_value[DAF_PARAMETER_VALUE_LEN];
	parameterlist p;
	parameterlist *q = &(parameters->parameters);

    if (argc == 1)
    {
        print_usage();
        exit(0);
    }

    initialise_args(parameters);

    /* see if the user has specified a console log - because we want to open the console log as soon as possible */
    /* so that we can make sure that even error messages relating to parsing arguments are included in the log   */

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-consolelog") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->consolelog_pathname, argv[i], sizeof(parameters->consolelog_pathname));
            }
            else
            {
                printf("daf: missing value for -consolelog parameter\n");
                return E_NOTOK;
            }
        }
        else if ((strcmp(argv[i], "-quiet") == 0))
        {
            parameters->quiet_flag = TRUE;
        }
    }

    /* Open the console log */

    gethostname(thishostname, sizeof(thishostname));

    if (strlen(parameters->consolelog_pathname) == 0)
    {
        get_current_time_as_timestamp(temp, sizeof(temp), '.');
        safecpy(parameters->consolelog_pathname, DAF_SERVICE_LOG_DIR,  sizeof(parameters->consolelog_pathname));
        safecat(parameters->consolelog_pathname, "/",                  sizeof(parameters->consolelog_pathname));
        safecat(parameters->consolelog_pathname, thishostname,         sizeof(parameters->consolelog_pathname));
        /*      safecat(parameters->consolelog_pathname, "_",                  sizeof(parameters->consolelog_pathname));
                safecat(parameters->consolelog_pathname, temp,                 sizeof(parameters->consolelog_pathname)); */
        safecat(parameters->consolelog_pathname, "_console.log",       sizeof(parameters->consolelog_pathname));
    }

    ensure_directory_path_exists(DAF_SERVICE_LOG_DIR);
    open_consolelog(parameters->consolelog_pathname, parameters->quiet_flag);

    for (i = 1; i < argc; i++)
    {

        if ((strcmp(argv[i], "-?") == 0) || (strcmp(argv[i], "-h") ==0) || (strcmp(argv[i], "-help") ==0))
        {
            print_usage();
            exit(0);
        }
        else if ((strcmp(argv[i], "-detailedhelp") == 0))
        {
            print_detailed_usage();
            exit(0);
        }
        else if ((strcmp(argv[i], "-msglevel") == 0))
        {
            i++;

            if (i < argc)
            {
                parameters->msglevel = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("daf: missing value for -msglevel parameter\n");
                return E_NOTOK;
            }
        }
        else if ((strcmp(argv[i], "-service") == 0))
        {
            parameters->service_flag = TRUE;

            if ((i+1) >= argc)
            {
                break;
            }

            while (((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                i++;

                if (strcmp(argv[i], "install") == 0)
                {
                    parameters->service_install_flag = TRUE;
                }
                else if (strcmp(argv[i], "start") == 0)
                {
                    parameters->service_start_flag = TRUE;
                }
                else if (strcmp(argv[i], "stop") == 0)
                {
                    parameters->service_stop_flag = TRUE;
                }
                else if (strcmp(argv[i], "delete") == 0)
                {
                    parameters->service_delete_flag = TRUE;
                }
                else if (strcmp(argv[i], "run") == 0)
                {
                    parameters->service_run_flag = TRUE;
                }
                else
                {
                    printf("daf: %s is not a valid option for -service\n", argv[i]);
                    return E_NOTOK;
                }
            }
        }
        else if ((strcmp(argv[i], "-threaded_service") == 0))
        {
            parameters->service_flag = TRUE;
            threaded_daf_service = TRUE;
        }
        else if ((strcmp(argv[i], "-multiprocess_service") == 0))
        {
            parameters->service_flag = TRUE;
            multiprocess_daf_service = TRUE;
        }
        else if (strcmp(argv[i], "-consolelog") == 0)
        {
            i++;

            /* we have already dealt with the consolelog parameter so discard the parameter */
            if (i < argc)
            {
                i++;
            }
        }
		else if (strcmp(argv[i], "-remote") == 0)
		{
			i++;

			if (i < argc)
			{
				safecpy(parameters->remotehostname, argv[i], sizeof(parameters->remotehostname));
			}
			else
			{
				printf("daf: missing value for -remote parameter\n");
				return E_NOTOK;
			}

			parameters->remote_flag = TRUE;
		}
		else if (strcmp(argv[i], "-start_scenario") == 0)
		{
			parameters->start_scenario_flag = TRUE;
			i++;

			if (i < argc)
			{
				safecpy(parameters->jobname, argv[i], sizeof(parameters->jobname));
			}
			else
			{
				printf("daf: missing value for -start_scenario parameter\n");
				return E_NOTOK;
			}
		}
		else if (strcmp(argv[i], "-phase") == 0)
		{
			i++;

			if (i < argc)
			{
				safecpy(parameters->phase, argv[i], sizeof(parameters->phase));
			}
			else
			{
				printf("daf: missing value for -phase parameter\n");
				return E_NOTOK;
			}
		}
		else if (strcmp(argv[i], "-loglocation") == 0)
		{
			i++;

			if (i < argc)
			{
				safecpy(parameters->loglocation, argv[i], sizeof(parameters->loglocation));
			}
			else
			{
				printf("daf: missing value for -loglocation parameter\n");
				return E_NOTOK;
			}
		}
		else if (strcmp(argv[i], "-scenariologfile") == 0)
		{
			i++;

			if (i < argc)
			{
				safecpy(parameters->scenariologfile, argv[i], sizeof(parameters->scenariologfile));
			}
			else
			{
				printf("daf: missing value for -scenariologfile parameter\n");
			return E_NOTOK;
			}
		}
		else if (strcmp(argv[i], "-teststand") == 0)
		{
			i++;

			if (i < argc)
			{
				safecpy(parameters->teststand, argv[i], sizeof(parameters->teststand));
			}
			else
			{
				printf("daf: missing value for -teststand parameter\n");
				return E_NOTOK;
			}
		}
		else if (strcmp(argv[i], "-testlevel") == 0)
		{
			i++;

			if (i < argc)
			{
				safecpy(parameters->testlevel, argv[i], sizeof(parameters->testlevel));
			}
			else
			{
				printf("daf: missing value for -testlevel parameter\n");
				return E_NOTOK;
			}
		}
		else if (strcmp(argv[i], "-comments") == 0)
		{
			i++;

			if (i < argc)
			{
				safecpy(parameters->comments, argv[i], sizeof(parameters->comments));
			}
			else
			{
				printf("daf: missing value for -comments parameter\n");
				return E_NOTOK;
			}
		}
		else if (strcmp(argv[i], "-parameters") == 0)
		{

			end_of_params = FALSE;

			while (! end_of_params)
			{
				if ((i+1) >= argc)
				{
					break;
				}

				if (argv[i+1][0] != '-')
				{
					i++;
					if (num_daf_params >= DAF_MAX_NUM_PARAMS)
					{
						end_of_params = TRUE;
						printf("daf: Too many name=value parameters specified at -parameters parameter, %s\n", argv[i]);
						return E_NOTOK;
					}
					else
					{
						p = (parameterlist) malloc(sizeof(struct parameternode));
						split_namevalue_parameter(argv[i],
								                  parameter_name,
												  DAF_PARAMETER_NAME_LEN,
								                  parameter_value,
												  DAF_PARAMETER_VALUE_LEN);
						p->name = parameter_name;
						p->value = parameter_value;
						p->next = NULL;
						if (*q == NULL) {
							*q = p;
						} else {
							(*q)->next = p;
						}
						num_daf_params++;
					}
				}
				else
				{
					end_of_params = TRUE;
				}
			}

			if (num_daf_params == 0)
			{
				printf("daf: name=value(s) missing from -parameters parameter\n");
				return E_NOTOK;
			}
		}
		else if (strcmp(argv[i], "-username") == 0)
		{
			i++;

			if (i < argc)
			{
				safecpy(parameters->username, argv[i], sizeof(parameters->username));
			}
			else
			{
				printf("daf: missing value for -username parameter\n");
				return E_NOTOK;
			}

#ifndef DAF_AGENT
        }
        else if ((strcmp(argv[i], "-simulator_mode") == 0))
        {
            parameters->simulator_mode_flag = TRUE;
        }
        else if ((strcmp(argv[i], "-delim") == 0))
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->delimiter_string, argv[i], sizeof(parameters->delimiter_string));
            }
            else
            {
                printf("daf: missing value for -delim parameter\n");
                return E_NOTOK;
            }
        }
        else if ((strcmp(argv[i], "-nohdr") == 0))
        {
            parameters->no_headers_flag = TRUE;
        }
        else if ((strcmp(argv[i], "-timeout") == 0))
        {
            i++;

            if (i < argc)
            {
                parameters->timeout = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("daf: missing value for -timeout parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-execute") == 0)
        {
            parameters->execute_flag = TRUE;
            i++;

            if (i < argc)
            {
                safecpy(parameters->cmdstring, argv[i], sizeof(parameters->cmdstring));
            }
            else
            {
                printf("daf: missing value for -execute parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-validate_licence") == 0)
        {
            parameters->validate_licence_flag = TRUE;
            i++;

            if (i < argc)
            {
                safecpy(parameters->licence_pathname, argv[i], sizeof(parameters->licence_pathname));
            }
            else
            {
                printf("daf: missing value for -validate_licence parameter\n");
                return E_NOTOK;
            }

            /*  if -write_licence -anotherparam is entered, we do not catch it <<<<<<<<<<<<<< */
        }
        else if (strcmp(argv[i], "-write_licence") == 0)
        {
            parameters->write_licence_flag = TRUE;
            i++;

            if (i < argc)
            {
                safecpy(parameters->licence_pathname, argv[i], sizeof(parameters->licence_pathname));
            }
            else
            {
                printf("daf: missing value for -write_licence parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-licence") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->licence, argv[i], sizeof(parameters->licence));
            }
            else
            {
                printf("daf: missing value for -licence parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-project") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->project, argv[i], sizeof(parameters->project));
            }
            else
            {
                printf("daf: missing value for -project parameter\n");
                return E_NOTOK;
            }
        }

        else if (strcmp(argv[i], "-ident") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->identstring, argv[i], sizeof(parameters->identstring));
            }
            else
            {
                printf("daf: missing value for -ident parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-run_in_background") == 0)
        {
            parameters->run_in_background_flag = TRUE;
        }
        else if (strcmp(argv[i], "-tag") == 0)
        {
            i++;

            if (i < argc)
            {
                parameters->tag = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("daf: missing value for -tag parameter\n");
                return E_NOTOK;
            }

            parameters->tag_flag = TRUE;
        }
        else if (strcmp(argv[i], "-get") == 0)
        {
            parameters->get_flag = TRUE;

            if ((i+1) >= argc)
            {
                break;
            }

            if (argv[i+1][0] != '-')
            {
                i++;

                if (strcmp(argv[i], "newident") == 0)
                {
                    parameters->get_newident_flag = TRUE;
                }
                else
                {
                    printf("daf: %s is not a valid option for -get\n", argv[i]);
                    return E_NOTOK;
                }
            }
            else
            {
                printf("daf: missing option for -get\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-query") == 0)
        {
            parameters->query_flag = TRUE;

            if ((i+1) >= argc)
            {
                break;
            }

            if (argv[i+1][0] != '-')
            {
                i++;

                if (strcmp(argv[i], "tag") == 0)
                {
                    parameters->query_tag_flag = TRUE;
                }
                else if (strcmp(argv[i], "alltags") == 0)
                {
                    parameters->query_alltags_flag = TRUE;
                }
                else if (strcmp(argv[i], "ident") == 0)
                {
                    parameters->query_ident_flag = TRUE;
                }
                else if (strcmp(argv[i], "cmds") == 0)
                {
                    parameters->query_cmds_flag = TRUE;
                }
                else if (strcmp(argv[i], "version") == 0)
                {
                    parameters->query_version_flag = TRUE;
                }
                else
                {
                    printf("daf: %s is not a valid option for -query\n", argv[i]);
                    return E_NOTOK;
                }
            } else if ((strcmp(argv[i], "-quiet") == 0))
             {
                 // we have already dealt with this - so nothing to do
             }
            else
            {
                printf("daf: missing option for -query\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-report") == 0)
        {
            parameters->report_flag = TRUE;

            if ((i+1) >= argc)
            {
                printf("daf: missing option for -report\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;

                if (strcmp(argv[i], "cmds") == 0)
                {
                    parameters->report_cmds_flag = TRUE;
                }
                else
                {
                    printf("daf: %s is not a valid option for -report\n", argv[i]);
                    return E_NOTOK;
                }
            }
            else
            {
                printf("daf: missing option for -report\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-clear") == 0)
        {
            if ((i+1) >= argc)
            {
                printf("daf: missing option after -clear\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;

                if (strcmp(argv[i], "tag") == 0)
                {
                    parameters->clear_tag_flag = TRUE;
                }
                else if (strcmp(argv[i], "alltags") == 0)
                {
                    parameters->clear_alltags_flag = TRUE;
                }
                else if (strcmp(argv[i], "ident") == 0)
                {
                    parameters->clear_ident_flag = TRUE;
                }
                else if (strcmp(argv[i], "shared_memory") == 0)
                {
                    parameters->clear_shared_memory_flag = TRUE;
                }
                else
                {
                    printf("daf: %s is not a valid option for -clear\n", argv[i]);
                    return E_NOTOK;
                }
            }
            else
            {
                printf("daf: missing option for -clear\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-cancel") == 0)
        {
            if ((i+1) >= argc)
            {
                printf("daf: missing option after -cancel\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;

                if (strcmp(argv[i], "tag") == 0)
                {
                    parameters->cancel_tag_flag = TRUE;
                }
                else if (strcmp(argv[i], "ident") == 0)
                {
                    parameters->cancel_ident_flag = TRUE;
                }
                else
                {
                    printf("daf: %s is not a valid option for -cancel\n", argv[i]);
                    return E_NOTOK;
                }
            }
            else
            {
                printf("daf: missing option for -cancel\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-debug") == 0)
        {
            if ((i+1) >= argc)
            {
                printf("daf: missing option after -debug\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;

                if (strcmp(argv[i], "sql") == 0)
                {
                    parameters->set_sql_debug_flag = TRUE;
                }
                else if (strcmp(argv[i], "nosql") == 0)
                {
                    parameters->reset_sql_debug_flag = TRUE;
                }
                else if (strcmp(argv[i], "zombie_reaper") == 0)
                {
                    parameters->set_zombie_reaper_debug_flag = TRUE;
                }
                else if (strcmp(argv[i], "nozombie_reaper") == 0)
                {
                    parameters->reset_zombie_reaper_debug_flag = TRUE;
                }
                else
                {
                    printf("daf: %s is not a valid option for -debug\n", argv[i]);
                    return E_NOTOK;
                }
            }
            else
            {
                printf("daf: missing option for -debug\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-dirlist") == 0)
        {
            parameters->dirlist_flag = TRUE;
        }
        else if (strcmp(argv[i], "-dirlist") == 0)
        {
            parameters->dirlist_flag = TRUE;

            if ((i+1) >= argc)
            {
                printf("daf: missing option after -dirlist\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;
                safecpy(parameters->remotepathname, argv[i], sizeof(parameters->remotepathname));
            }
            else
            {
                printf("daf: missing option for -dirlist\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-getfile") == 0)
        {
            parameters->getfile_flag = TRUE;

            if ((i+1) >= argc)
            {
                printf("daf: missing option after -getfile\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;
                safecpy(parameters->localpathname, argv[i], sizeof(parameters->localpathname));
            }
            else
            {
                printf("daf: missing option for -getfile\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;
                safecpy(parameters->remotepathname, argv[i], sizeof(parameters->remotepathname));
            }
            else
            {
                printf("daf: missing option for -getfile\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-putfile") == 0)
        {
            parameters->putfile_flag = TRUE;

            if ((i+1) >= argc)
            {
                printf("daf: missing option after -putfile\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;
                safecpy(parameters->localpathname, argv[i], sizeof(parameters->localpathname));
            }
            else
            {
                printf("daf: missing option for -putfile\n");
                return E_NOTOK;
            }

            if (argv[i+1][0] != '-')
            {
                i++;
                safecpy(parameters->remotepathname, argv[i], sizeof(parameters->remotepathname));
            }
            else
            {
                printf("daf: missing option for -putfile\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-buffersize") == 0)
        {
            i++;

            if (i < argc)
            {
                parameters->buffersize = strtoul(argv[i], NULL, 0);
                parameters->buffersize_flag = TRUE;
            }
            else
            {
                printf("daf: missing value for -buffersize parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-sqlserver") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->sql_servername, argv[i], sizeof(parameters->sql_servername));
            }
            else
            {
                printf("daf: missing value for -sqlserver parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-sqluser") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->sql_username, argv[i], sizeof(parameters->sql_username));
            }
            else
            {
                printf("daf: missing value for -sqluser parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-sqlpassword") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->sql_password, argv[i], sizeof(parameters->sql_password));
            }
            else
            {
                printf("daf: missing value for -sqlpassword parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-sqlport") == 0)
        {
            i++;

            if (i < argc)
            {
                parameters->sql_port = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("daf: missing value for -sqlport parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-sqldatabase") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->sql_databasename, argv[i], sizeof(parameters->sql_databasename));
            }
            else
            {
                printf("daf: missing value for -sqldatabase parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-sqlsocketname") == 0)
        {
            i++;

            if (i < argc)
            {
                safecpy(parameters->sql_socketname, argv[i], sizeof(parameters->sql_socketname));
            }
            else
            {
                printf("daf: missing value for -sqlsocketname parameter\n");
                return E_NOTOK;
            }
        }
        else if (strcmp(argv[i], "-workqueueID") == 0)
        {
            parameters->workqueueID_flag = TRUE;
            i++;

            if (i < argc)
            {
                parameters->workqueueID = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("daf: missing value for -workqueueID parameter\n");
                return E_NOTOK;
            }

#endif
        }
        else
        {
            printf("daf: unrecognised argument: %s\n", argv[i]);
            return E_NOTOK;
        }

    }

    safecpy(sql_server.sql_servername,     parameters->sql_servername,     sizeof(sql_server.sql_servername));
    safecpy(sql_server.sql_username,       parameters->sql_username,       sizeof(sql_server.sql_username));
    safecpy(sql_server.sql_password,       parameters->sql_password,       sizeof(sql_server.sql_password));
    safecpy(sql_server.sql_databasename,   parameters->sql_databasename,   sizeof(sql_server.sql_databasename));
    sql_server.sql_port = parameters->sql_port;
    safecpy(sql_server.sql_socketname,     parameters->sql_socketname,     sizeof(sql_server.sql_socketname));

    if (MATCH(parameters->loglocation, "")) {
    	safecpy(parameters->loglocation, "/tmp/", sizeof(parameters->loglocation));
    	safecat(parameters->loglocation, parameters->project, sizeof(parameters->loglocation));
    }

    if (MATCH(parameters->scenariologfile, "")) {
    	safecpy(parameters->scenariologfile, "console_", sizeof(parameters->scenariologfile));
    	safecat(parameters->scenariologfile, parameters->jobname, sizeof(parameters->scenariologfile));
       	safecat(parameters->scenariologfile, ".out", sizeof(parameters->scenariologfile));
    }

    return E_OK;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  start_zombie_reaper                                                                        */
/*                                                                                                        */
/* PARAMS IN:  parameters                                                                                 */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void start_zombie_reaper(parameters_t *parameters)
{

    pthread_attr_t       attr;
    char                 msg[MAX_MSG_LEN];
    pthread_t            pthread;
    int                  rc;

    static af_daemon_thread_data_t  af_daemon_thread_data;

    safecpy(af_daemon_thread_data.sql_servername,    parameters->sql_servername,    sizeof(af_daemon_thread_data.sql_servername));
    safecpy(af_daemon_thread_data.sql_username,      parameters->sql_username,      sizeof(af_daemon_thread_data.sql_username));
    safecpy(af_daemon_thread_data.sql_password,      parameters->sql_password,      sizeof(af_daemon_thread_data.sql_password));
    safecpy(af_daemon_thread_data.sql_databasename,  parameters->sql_databasename,  sizeof(af_daemon_thread_data.sql_databasename));
    af_daemon_thread_data.sql_port = parameters->sql_port;
    safecpy(af_daemon_thread_data.sql_socketname,    parameters->sql_socketname,    sizeof(af_daemon_thread_data.sql_socketname));

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if ((rc = pthread_create( &pthread, &attr, zombie_reaper, (void *) &af_daemon_thread_data)) != 0)
    {
        sprintf(msg, "ERROR; start_zombie_reaper: return code from pthread_create() is %d\n", rc);
        print_msg_to_console(msg);
        exit(1);
    }

    /* Free attribute  - we don't expect this thread to terminate so we are done */
    pthread_attr_destroy(&attr);

}

#ifndef DAF_AGENT

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  start_af_daemon                                                                            */
/*                                                                                                        */
/* PARAMS IN:  parameters                                                                                 */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void start_af_work_daemon(parameters_t *parameters)
{

    pthread_attr_t       attr;
    char                 msg[MAX_MSG_LEN];
    pthread_t            pthread;
    int                  rc;
    static af_daemon_thread_data_t  af_daemon_thread_data;

    safecpy(af_daemon_thread_data.sql_servername, parameters->sql_servername, sizeof(af_daemon_thread_data.sql_servername));
    safecpy(af_daemon_thread_data.sql_username,   parameters->sql_username,   sizeof(af_daemon_thread_data.sql_username));
    safecpy(af_daemon_thread_data.sql_password,   parameters->sql_password,   sizeof(af_daemon_thread_data.sql_password));
    safecpy(af_daemon_thread_data.sql_databasename,  parameters->sql_databasename,  sizeof(af_daemon_thread_data.sql_databasename));
    af_daemon_thread_data.sql_port = parameters->sql_port;
    safecpy(af_daemon_thread_data.sql_socketname, parameters->sql_socketname,  sizeof(af_daemon_thread_data.sql_socketname));
    af_daemon_thread_data.simulator_mode = parameters->simulator_mode_flag;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if ((rc = pthread_create( &pthread, &attr, af_work_daemon, (void *) &af_daemon_thread_data)) != 0)
    {
        sprintf(msg, "ERROR; start_af_daemon_reaper: return code from pthread_create() is %d\n", rc);
        print_msg_to_console(msg);
        exit(1);
    }

    /* Free attribute  - we don't expect this thread to terminate so we are done */
    pthread_attr_destroy(&attr);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  start_af_collector_daemon                                                                         */
/*                                                                                                        */
/* PARAMS IN:  parameters                                                                                 */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void start_af_collector_daemon(parameters_t *parameters)
{

    pthread_attr_t       attr;
    char                 msg[MAX_MSG_LEN];
    pthread_t            pthread;
    int                  rc;
    static af_collector_thread_data_t  af_collector_thread_data;

    safecpy(af_collector_thread_data.sql_servername,    parameters->sql_servername,    sizeof(af_collector_thread_data.sql_servername));
    safecpy(af_collector_thread_data.sql_username,      parameters->sql_username,      sizeof(af_collector_thread_data.sql_username));
    safecpy(af_collector_thread_data.sql_password,      parameters->sql_password,      sizeof(af_collector_thread_data.sql_password));
    safecpy(af_collector_thread_data.sql_databasename,  parameters->sql_databasename,  sizeof(af_collector_thread_data.sql_databasename));
    af_collector_thread_data.sql_port       = parameters->sql_port;
    safecpy(af_collector_thread_data.sql_socketname,    parameters->sql_socketname,    sizeof(af_collector_thread_data.sql_socketname));
    af_collector_thread_data.simulator_mode = parameters->simulator_mode_flag;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if ((rc = pthread_create(&pthread, &attr, af_collector_daemon, (void *) &af_collector_thread_data)) != 0)
    {
        sprintf(msg, "ERROR; start_af_collector: return code from pthread_create() is %d\n", rc);
        print_msg_to_console(msg);
        exit(1);
    }

    /* Free attribute  - we don't expect this thread to terminate so we are done */
    pthread_attr_destroy(&attr);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  start_af_agent_status_daemon                                                               */
/*                                                                                                        */
/* PARAMS IN:  parameters                                                                                 */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void start_af_agent_status_daemon(parameters_t *parameters)
{

    pthread_attr_t       attr;
    char                 msg[MAX_MSG_LEN];
    pthread_t            pthread;
    int                  rc;
    static af_agent_status_daemon_thread_data_t  af_agent_status_daemon_thread_data;

    safecpy(af_agent_status_daemon_thread_data.sql_servername,    parameters->sql_servername,    sizeof(af_agent_status_daemon_thread_data.sql_servername));
    safecpy(af_agent_status_daemon_thread_data.sql_username,      parameters->sql_username,      sizeof(af_agent_status_daemon_thread_data.sql_username));
    safecpy(af_agent_status_daemon_thread_data.sql_password,      parameters->sql_password,      sizeof(af_agent_status_daemon_thread_data.sql_password));
    safecpy(af_agent_status_daemon_thread_data.sql_databasename,  parameters->sql_databasename,  sizeof(af_agent_status_daemon_thread_data.sql_databasename));
    af_agent_status_daemon_thread_data.sql_port       = parameters->sql_port;
    safecpy(af_agent_status_daemon_thread_data.sql_socketname,    parameters->sql_socketname,    sizeof(af_agent_status_daemon_thread_data.sql_socketname));
    af_agent_status_daemon_thread_data.simulator_mode = parameters->simulator_mode_flag;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if ((rc = pthread_create( &pthread, &attr, af_agent_status_daemon, (void *) &af_agent_status_daemon_thread_data)) != 0)
    {
        sprintf(msg, "ERROR; start_af_agent_status_daemon: return code from pthread_create() is %d\n", rc);
        print_msg_to_console(msg);
        exit(1);
    }

    /* Free attribute  - we don't expect this thread to terminate so we are done */
    pthread_attr_destroy(&attr);

}

#endif

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  daf_1_prog_freeresult                                                                     */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int daf_prog_1_freeresult(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{

    if (xdr_result == (xdrproc_t) xdr_remote_client_query_ident_res)
    {
        deallocate_cmd_log_fragment(&((remote_client_query_ident_res *) result)->remote_client_query_ident_res_u.outcome.cmd_log_fragment);
        return(1);
    }

    if (xdr_result == (xdrproc_t) xdr_remote_client_query_alltags_res)
    {
        deallocate_cmd_log_fragment(&((remote_client_query_alltags_res *) result)->remote_client_query_alltags_res_u.outcome.cmd_log_fragment);
        return(1);
    }

    xdr_free(xdr_result, result);
    return(1);

}

void print_transport ( SVCXPRT *transp)
{

    printf("transport address %p\n", transp);
    printf("xp_sock              %d\n", transp->xp_sock);
    printf("xp_port              %d\n", transp->xp_port);
    printf("xp_recv              %p\n", transp->xp_ops->xp_recv);
    printf("xp_getargs           %p\n", transp->xp_ops->xp_reply);
    printf("xp_freeargs          %p\n", transp->xp_ops->xp_freeargs);
    printf("xp_destroy           %p\n", transp->xp_ops->xp_destroy);
    printf("xp_addrlen           %d\n", transp->xp_addrlen);

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  daf_prog_dispatcher                                                                       */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void *daf_prog_dispatcher_1(void *data)
{

    struct thr_data
    {
        struct   svc_req *rqstp;
        SVCXPRT  *transp;
        int      thread_status;
    } *ptr_data;

    bool_t     retval;
    xdrproc_t  _xdr_argument;
    xdrproc_t  _xdr_result;
    bool_t     (*local)(char *, void *, struct svc_req *);
    char       request_string[MAX_MSG_LEN];

    ptr_data                 = (struct thr_data *) data;
    struct svc_req *rqstp    = ptr_data-> rqstp;
    SVCXPRT *transp          = ptr_data-> transp;
    int *pthread_status      = &(ptr_data->thread_status);

    *pthread_status = E_OK;

    int msglevel = 0;  /* <<<<<<<<<<<<< */

    union
    {
        remote_client_cntrl_args             client_remote_client_cntrl_1_arg;
        remote_client_cmd_args               client_remote_client_cmd_1_arg;
        remote_client_prepare_cmd_args       client_remote_client_prepare_cmd_1_arg;
        remote_client_execute_cmd_args       client_remote_client_execute_cmd_1_arg;
        remote_client_run_cmd_args           client_remote_client_run_cmd_1_arg;
        remote_client_start_scenario_args    client_remote_client_start_scenario_1_arg;
        remote_client_query_version_args     client_remote_client_query_version_1_arg;
        remote_client_query_tag_args         client_remote_client_query_tag_1_arg;
        remote_client_query_cmdlog_args      client_remote_client_query_cmdlog_1_arg;
        remote_client_query_ident_args       client_remote_client_query_ident_1_arg;
        remote_client_query_cmds_args        client_remote_client_query_cmds_1_arg;
        remote_client_get_unique_ident_args  client_remote_client_get_unique_ident_1_arg;
        remote_client_clear_tag_args         client_remote_client_clear_tag_1_arg;
        remote_client_clear_alltags_args     client_remote_client_clear_alltags_1_arg;
        remote_client_clear_ident_args       client_remote_client_clear_ident_1_arg;
        remote_client_cancel_tag_args        client_remote_client_cancel_tag_1_arg;
        remote_client_cancel_ident_args      client_remote_client_cancel_ident_1_arg;
        remote_client_copyfile_args          client_remote_client_copyfile_1_arg;
        remote_client_deletefile_args        client_remote_client_deletefile_1_arg;
        remote_client_dirlist_args           client_remote_client_dirlist_1_arg;
#ifndef DAF_AGENT
        remote_client_update_workqueue_status_args    client_remote_client_update_workqueue_status_1_arg;
#endif
    } argument;

    union
    {
        remote_client_cntrl_res            client_remote_client_cntrl_1_res;
        remote_client_cmd_res              client_remote_client_cmd_1_res;
        remote_client_prepare_cmd_res      client_remote_client_prepare_cmd_1_res;
        remote_client_execute_cmd_res      client_remote_client_execute_cmd_1_res;
        remote_client_run_cmd_res          client_remote_client_run_cmd_1_res;
        remote_client_start_scenario_res   client_remote_client_start_scenario_1_res;
        remote_client_query_version_res    client_remote_client_query_version_1_res;
        remote_client_query_tag_res        client_remote_client_query_tag_1_res;
        remote_client_query_alltags_res    client_remote_client_query_alltags_1_res;
        remote_client_query_cmdlog_res     client_remote_client_query_cmdlog_1_res;
        remote_client_query_ident_res      client_remote_client_query_ident_1_res;
        remote_client_query_cmds_res       client_remote_client_query_cmds_1_res;
        remote_client_get_unique_ident_res  client_remote_client_get_unique_ident_1_res;
        remote_client_clear_tag_res        client_remote_client_clear_tag_1_res;
        remote_client_clear_alltags_res    client_remote_client_clear_alltags_1_res;
        remote_client_clear_ident_res      client_remote_client_clear_ident_1_res;
        remote_client_cancel_tag_res       client_remote_client_cancel_tag_1_res;
        remote_client_cancel_ident_res     client_remote_client_cancel_ident_1_res;
        remote_client_copyfile_res         client_remote_client_copyfile_1_res;
        remote_client_deletefile_res       client_remote_client_deletefile_1_res;
        remote_client_dirlist_res          client_remote_client_dirlist_1_res;
#ifndef DAF_AGENT
        remote_client_update_workqueue_status_res          client_remote_client_update_workqueue_status_1_res;
#endif
    } result;

    remote_client_request_decode(rqstp->rq_proc, request_string, sizeof(request_string));

    msglevel = 0;

    if (msglevel > 2 )
    {


        printf(" ** dispatching: rqstp->rq_proc %d (%s) pid=%d  tid=%llu rqstp %p, &rqstp->rpq_proc %p, transp=%p\n",
               (Iu32) rqstp->rq_proc, request_string, (Iu32) getpid(), (long long unsigned) pthread_self(), rqstp, &rqstp->rq_proc, transp);
        print_transport(transp);
    }

    switch (rqstp->rq_proc)
    {

    case NULLPROC:
        (void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);

        if (threaded_daf_service)
        {
            pthread_exit( (void *) pthread_status);
        }
        else if (multiprocess_daf_service)
        {
            _exit(*pthread_status);
        }
        else
        {
            return ( (void *) pthread_status);
        }

    case CLIENT_REMOTE_CLIENT_CNTRL:
        _xdr_argument = (xdrproc_t) xdr_remote_client_cntrl_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_cntrl_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_cntrl_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_CMD:
        _xdr_argument = (xdrproc_t) xdr_remote_client_cmd_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_cmd_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_cmd_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_PREPARE_CMD:
        _xdr_argument = (xdrproc_t) xdr_remote_client_prepare_cmd_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_prepare_cmd_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_prepare_cmd_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_EXECUTE_CMD:
        _xdr_argument = (xdrproc_t) xdr_remote_client_execute_cmd_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_execute_cmd_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_execute_cmd_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_RUN_CMD:
        _xdr_argument = (xdrproc_t) xdr_remote_client_run_cmd_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_run_cmd_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_run_cmd_1_svc;
        break;

#ifndef DAF_AGENT

    case CLIENT_REMOTE_CLIENT_START_SCENARIO:
        _xdr_argument = (xdrproc_t) xdr_remote_client_start_scenario_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_start_scenario_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_start_scenario_1_svc;
        break;

#endif

    case CLIENT_REMOTE_CLIENT_QUERY_VERSION:
        _xdr_argument = (xdrproc_t) xdr_remote_client_query_version_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_query_version_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_query_version_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_GET_UNIQUE_IDENT:
        _xdr_argument = (xdrproc_t) xdr_remote_client_get_unique_ident_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_get_unique_ident_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_get_unique_ident_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_QUERY_TAG:
        _xdr_argument = (xdrproc_t) xdr_remote_client_query_tag_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_query_tag_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_query_tag_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_QUERY_ALLTAGS:
        _xdr_argument = (xdrproc_t) xdr_remote_client_query_alltags_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_query_alltags_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_query_alltags_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_QUERY_IDENT:
        _xdr_argument = (xdrproc_t) xdr_remote_client_query_ident_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_query_ident_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_query_ident_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_QUERY_CMDS:
        _xdr_argument = (xdrproc_t) xdr_remote_client_query_cmds_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_query_cmds_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_query_cmds_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_QUERY_CMDLOG:
        _xdr_argument = (xdrproc_t) xdr_remote_client_query_cmdlog_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_query_cmdlog_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_query_cmdlog_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_CLEAR_TAG:
        _xdr_argument = (xdrproc_t) xdr_remote_client_clear_tag_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_clear_tag_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_clear_tag_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_CLEAR_ALLTAGS:
        _xdr_argument = (xdrproc_t) xdr_remote_client_clear_alltags_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_clear_alltags_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_clear_alltags_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_CLEAR_IDENT:
        _xdr_argument = (xdrproc_t) xdr_remote_client_clear_ident_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_clear_ident_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_clear_ident_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_CANCEL_TAG:
        _xdr_argument = (xdrproc_t) xdr_remote_client_cancel_tag_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_cancel_tag_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_cancel_tag_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_CANCEL_IDENT:
        _xdr_argument = (xdrproc_t) xdr_remote_client_cancel_ident_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_cancel_ident_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_cancel_ident_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_COPYFILE:
        _xdr_argument = (xdrproc_t) xdr_remote_client_copyfile_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_copyfile_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_copyfile_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_DELETEFILE:
        _xdr_argument = (xdrproc_t) xdr_remote_client_deletefile_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_deletefile_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_deletefile_1_svc;
        break;

    case CLIENT_REMOTE_CLIENT_DIRLIST:
        _xdr_argument = (xdrproc_t) xdr_remote_client_dirlist_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_dirlist_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_dirlist_1_svc;
        break;

#ifndef DAF_AGENT

    case CLIENT_REMOTE_CLIENT_UPDATE_WORKQUEUE_STATUS:
        _xdr_argument = (xdrproc_t) xdr_remote_client_update_workqueue_status_args;
        _xdr_result = (xdrproc_t) xdr_remote_client_update_workqueue_status_res;
        local = (bool_t (*) (char *, void *,  struct svc_req *))client_remote_client_update_workqueue_status_1_svc;
        break;
#endif

    default:
        svcerr_noproc (transp);

        if (threaded_daf_service)
        {
            pthread_exit( (void *) pthread_status);
        }
        else if (multiprocess_daf_service)
        {
            _exit(*pthread_status);
        }
        else
        {
            return ( (void *) pthread_status);
        }
    }

    memset ((char *)&argument, 0, sizeof (argument));

    if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument))
    {
        svcerr_decode (transp);

        if (msglevel > 2)
        {
            printf(" ** argument decode error for %d (%s),  tid=%llu\n",
                   (Iu32) rqstp->rq_proc, request_string, (long long unsigned) pthread_self());
        }

        if (threaded_daf_service)
        {
            pthread_exit( (void *) pthread_status);
        }
        else if (multiprocess_daf_service)
        {
            _exit(*pthread_status);
        }
        else
        {
            return ( (void *) pthread_status);
        }
    }

    retval = (bool_t) (*local)((char *)&argument, (void *)&result, rqstp);


    if (msglevel > 2)
    {
        printf(" ** completed dispatch of %d (%s), retval = %d,  tid=%llu\n",
               (Iu32) rqstp->rq_proc, request_string, retval, (long long unsigned) pthread_self());
    }

    if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument))
    {
        printf("%s", " ** unable to free arguments\n");
        svcerr_systemerr (transp);
    }

    if (msglevel > 2)
    {
        printf(" ** completed freeargs of %d (%s), retval = %d,  tid=%llu\n",
               (Iu32) rqstp->rq_proc, request_string, retval, (long long unsigned) pthread_self());
    }

    /*  if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) { */
    if (!svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result))
    {
        printf("%s", " ** unable to run svc_sendreply\n");
        svcerr_systemerr (transp);
    }

    if (msglevel > 2)
    {
        printf(" ** completed sendreply of %d (%s), retval = %d,  tid=%llu\n",
               (Iu32) rqstp->rq_proc, request_string, retval, (long long unsigned) pthread_self());
    }

    if (!daf_prog_1_freeresult (transp, _xdr_result, (caddr_t) &result))
    {
        printf("%s", " ** unable to free results\n");
    }

    if (msglevel > 2)
    {
        printf(" ** completed %d (%s) rc=%d,  tid=%llu\n",
               (Iu32) rqstp->rq_proc, request_string, *pthread_status,  (long long unsigned) pthread_self());
    }

    if (threaded_daf_service)
    {
        pthread_exit( (void *) pthread_status);
    }
    else if (multiprocess_daf_service)
    {
        _exit(*pthread_status);
    }
    else
    {
        return ( (void *) pthread_status);
    }
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  threadme_1                                                                                 */
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

#if (0)
/* * Server side transport handle */
typedef struct SVCXPRT SVCXPRT;
struct SVCXPRT
{
    int xp_sock;
    u_short xp_port;                                                                         /* associated port number */
    const struct xp_ops
    {
        bool_t      (*xp_recv) (SVCXPRT *__xprt, struct rpc_msg *__msg);                       /* receive incoming requests */
        enum xprt_stat (*xp_stat) (SVCXPRT *__xprt);                                           /* get transport status */
        bool_t      (*xp_getargs) (SVCXPRT *__xprt, xdrproc_t __xdr_args, caddr_t args_ptr);   /* get arguments */
        bool_t      (*xp_reply) (SVCXPRT *__xprt, struct rpc_msg *__msg);                      /* send reply */
        bool_t      (*xp_freeargs) (SVCXPRT *__xprt, xdrproc_t __xdr_args, caddr_t args_ptr);  /* free mem allocated for args */
        void        (*xp_destroy) (SVCXPRT *__xprt);                                           /* destroy this struct */
    } *xp_ops;
    int           xp_addrlen;      /* length of remote address */
    struct sockaddr_in xp_raddr;   /* remote address */
    struct opaque_auth xp_verf;    /* raw response verifier */
    caddr_t               xp_p1;           /* private */
    caddr_t               xp_p2;           /* private */
    char          xp_pad [256];   /* padding, internal use */
};

/* * Service request */
struct svc_req
{
    rpcprog_t rq_prog;            /* service program number */
    rpcvers_t rq_vers;            /* service protocol version */
    rpcproc_t rq_proc;            /* the desired procedure */
    struct opaque_auth rq_cred;   /* raw creds from the wire */
    caddr_t rq_clntcred;          /* read only cooked cred */
    SVCXPRT *rq_xprt;             /* associated transport */
};

#endif



void threadme_1(struct svc_req *rqstp, SVCXPRT *transport_handle)
{

    struct data_str               /* <<<<<<<<< keep this in sync with the definition in daf_prog_dispatcher_1 */
    {
        struct    svc_req *rqstp;
        SVCXPRT   *transp;
        int       thread_status;
    } *data_ptr = (struct data_str *) malloc(sizeof(struct data_str));   /* <<<<<<<< memory leak ? */

    pthread_attr_t   attr;
    pthread_t        p_thread;
    int              pid;

    int msglevel = 0; /* <<<<<<<<<<<<<<<< */

    if (msglevel > 2)
    {
        printf(" XXXXXXXXXXXXXXX** threadme new entry tid=%llu, rqstp=%p, rqstp->rq_proc %d\n",
               (long long unsigned) pthread_self(), rqstp, (Iu32) rqstp->rq_proc);
        printf(" ** threadme new entry xp_sock=%d, xp_port=%d\n",
               transport_handle->xp_sock, transport_handle->xp_port);
    }

    data_ptr-> rqstp  = rqstp;
    data_ptr-> transp = transport_handle;

    if (threaded_daf_service)
    {

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&p_thread, &attr, daf_prog_dispatcher_1, (void *) data_ptr);

        if (msglevel > 2)
        {
            printf(" ** daf_prog_dispatcher_1 launched from  tid=%llu, rqstp->rq_proc %d\n",
                   (long long unsigned) pthread_self(), (Iu32) rqstp->rq_proc);
            print_transport(transport_handle);
        }

    }
    else if (multiprocess_daf_service)
    {

        /* fork - parent will return immediately so that svc_run(() can continue listening for new requests - child process */
        /* will continue and actually process the request */

        if ((pid = fork()))
        {
            return;
        }

        /* we are in the child here so call the dispatch routine to deal with the request */
        daf_prog_dispatcher_1( (void *) data_ptr);

        _exit(0);

    }
    else
    {

        daf_prog_dispatcher_1( (void *) data_ptr);

    }

    if (msglevel > 2)
    {
        printf(" ** threadme completd  tid=%llu, rqstp=%p, rqstp->rq_proc %d\n",
               (long long unsigned) pthread_self(), rqstp, (Iu32)rqstp->rq_proc);
    }

}

/* void * dealwithit(void *rfds) {

printf("rfds is %p\n", rfds);

  fd_set nfds  = * (fd_set *) rfds;
   svc_getreqset(&nfds);
   pthread_exit( (void *) NULL);

}


 void my_svc_run(void) {

    pthread_attr_t   attr;
    pthread_t        p_thread;
    int              rc;
  static fd_set     rfds;       <<<<<<<<<<<<<
  static int tsize = 0;

  if (!tsize) tsize = getdtablesize();

  while(1) {

     rfds = svc_fdset;

printf("trying \n");

     switch (select(tsize, &rfds, NULL,NULL,NULL)) {
        case -1:
        case 0 :
           break;
        default:
           // Handle RPC request on each file descriptor

printf("rfds address is %p\n", &rfds);
           pthread_attr_init(&attr);
           pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
       rc = pthread_create(&p_thread, &attr, dealwithit, (void *) &rfds);

           if (msglevel > 2) {
              printf(" ** dealwithit launched from  tid=%llu\n", (long long unsigned) pthread_self());
          }
      }
printf("done\n");
   }
} */



/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  run_service                                                                                */
/*                                                                                                        */
/* PARAMS IN:  parameters                                                                                 */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void run_service(parameters_t *parameters)
{

    char      msg[MAX_MSG_LEN];
    SVCXPRT   *transport;

    /*--------------------------------------------------------------------------------------------------------*/
    /* Initialise service environment                                                                         */
    /*--------------------------------------------------------------------------------------------------------*/

    init_timers();
    initialise_service_environment();
    start_zombie_reaper(parameters);
#ifndef DAF_AGENT
    start_af_work_daemon(parameters);
    start_af_collector_daemon(parameters);
    start_af_agent_status_daemon(parameters);
#endif
    pmap_unset(DAF_PROG, DAF_VERSION);

    /*--------------------------------------------------------------------------------------------------------*/
    /* Create an RPC based transport (using an abitrary free socket ) - this will be used for UDP access to   */
    /* the DAF service                                                                                       */
    /*--------------------------------------------------------------------------------------------------------*/

    transport = svcudp_create(RPC_ANYSOCK);

    if (transport == NULL)
    {
        snprintf(msg, sizeof(msg), "%s", "cannot create udp service\n");
        print_msg_to_console(msg);
        exit(1);
    }

    /*--------------------------------------------------------------------------------------------------------*/
    /* Register the DAF_PROG service so that it uses the RPC transport handle, and any UDP calls to this     */
    /* service get routed to the daf_prog_1 service routine                                                  */
    /*--------------------------------------------------------------------------------------------------------*/

    if (!svc_register(transport, DAF_PROG, DAF_VERSION, threadme_1, IPPROTO_UDP))
    {
        snprintf(msg, sizeof(msg), "%s", "unable to register (DAF_PROG, DAF_VERSION, udp)\n");
        print_msg_to_console(msg);
        exit(1);
    }

    /*--------------------------------------------------------------------------------------------------------*/
    /* Create an RPC based transport (using an abitrary free socket ) - this will be used for TCP access to   */
    /* the DAF service                                                                                       */
    /*--------------------------------------------------------------------------------------------------------*/

    transport = svctcp_create(RPC_ANYSOCK, 0, 0);

    if (transport == NULL)
    {
        snprintf(msg, sizeof(msg), "%s", "cannot create tcp service\n");
        print_msg_to_console(msg);
        exit(1);
    }

    /*--------------------------------------------------------------------------------------------------------*/
    /* Register the DAF_PROG service so that it uses the RPC transport handle, and any TCP calls to this     */
    /* service get routed to the daf_prog_1 service routine                                                  */
    /*--------------------------------------------------------------------------------------------------------*/

    if (!svc_register(transport, DAF_PROG, DAF_VERSION, threadme_1, IPPROTO_TCP))
    {
        snprintf(msg, sizeof(msg), "%s", "unable to register (DAF_PROG, DAF_VERSION, tcp)\n");
        print_msg_to_console(msg);
        exit(1);
    }

    /*--------------------------------------------------------------------------------------------------------*/
    /* Wait for an RPC request to arrive and call the appropriate service routine (assuming it exists) as     */
    /* set up by the calls above - the svc_run() routine never returns - it loops waiting for new requests    */
    /* - to achieve conncurrent servicing of incoming RPC requests it is necessary for the service routines   */
    /* (daf_prog_1 in this case) to return control to svc_run as soon as possible and process the actual     */
    /* RPC request in a parallel thread (or process)                                                          */
    /*--------------------------------------------------------------------------------------------------------*/

    svc_run ();
    /* my_svc_run(); */
    snprintf(msg, sizeof(msg), "%s", "main: svc_run returned - internal error - that should not happen ??\n");
    print_msg_to_console(msg);
    exit (1);

}

/************************************************************************/
/*                                                                      */
/* NAME:        sighandler                                              */
/*                                                                      */
/* FUNCTION:    Handles Unix signals                                    */
/*                                                                      */
/* PARAMS IN:   sig    the signal number that has caused this interrupt */
/*                                                                      */
/* PARAMS OUT:  none                                                    */
/*                                                                      */
/* GLOBALS CHANGED:   Exiting    - set if SIGINT or SIGTERM are         */
/*                                 signalled                            */
/*                                                                      */
/* RETURNS:     void                                              .     */
/*                                                                      */
/************************************************************************/
void sighandler( int sig )
{
    /* On Windows only SIGINT will ever actually be raised by the OS,
     * but some other signals are defined for ANSI compatibility:
     *   http://msdn.microsoft.com/en-us/library/xdkz3x12(VS.71).aspx
     * Unfortunately SIGQUIT is not one of these so it's conditionally
     * compiled here */
    switch( sig )
    {
    case SIGTERM :
        /* set_exiting_flag(TRUE); */
        SIGTERMReceived = TRUE;
        break;

    case SIGINT  :
        SIGINTReceived = TRUE;
        /* set_exiting_flag(TRUE); */
        break;
#ifndef WIN32

    case SIGQUIT :
        SIGQUITReceived = TRUE;
        /* set_exiting_flag(TRUE); */
        break;
#endif

    default      :
        break;
    }
}

//static void install_signal_handler(int signum, int flags, void (*handler)())
//{
//    int result;
//    /*
//     * On POSIX compatible OS's we use sigaction() to install signal handlers
//     * due to the inconsistent behaviour of signal() across platforms, and
//     * it's undefined behaviour in multi-threaded processes.
//     */
//    struct sigaction sa;
//    sigemptyset(&sa.sa_mask);
//    sa.sa_flags = flags;
//    sa.sa_handler = handler;
//    result = sigaction(signum, &sa, NULL);
//
//    if (result)
//    {
//        char msg[MAX_MSG_LEN];
//        snprintf(msg, MAX_MSG_LEN, "init_process_signal_handlers: sigaction(2) failed for signal: %d,  error is \"%s\"\n", signum, strerror(errno));
//        print_msg_to_console(msg);
//        exit(1);
//    }
//}

static void init_process_signal_handlers(void)
{
    /* Ignore SIGHUP rather than terminate */
    //   install_signal_handler(SIGHUP, SA_RESTART, SIG_IGN);

    /* SIGINT, SIGQUIT, SIGPIPE, SIGTERM are handled by sighandler() */
    //   install_signal_handler(SIGINT, 0, sighandler);
    //   install_signal_handler(SIGQUIT, 0, sighandler);
    //   install_signal_handler(SIGTERM, 0, sighandler);
    //   install_signal_handler(SIGCHLD, 0, sighandler);
    //  install_signal_handler(SIGPIPE, SA_RESTART, sighandler);

    /* SIGALRM is also handled as the behaviour of e.g. usleep() is
     * not defined when the signal is blocked or ignored. */
    //   install_signal_handler(SIGALRM, SA_RESTART, sighandler);
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  main                                                                                       */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    0 for success, 1 for failure                                                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int main(int argc, char **argv)
{

    char                 msg[MAX_MSG_LEN];
    char                 statusmsg[MAX_MSG_LEN];
    char                 errmsg[MAX_MSG_LEN];
    int                  exit_rc = 0;
    unsigned long long   service_pid;
    int                  rc;

    // we would like to always get a core file if the DAF server or agent crashes, so set ulimit -c unlimited in *nix
    if ((rc = set_ulimit_c_unlimited(errmsg, sizeof(errmsg))) != 0) {
        printf("main: set_ulimit_c_unlimited returned %d errno %d (%s)\n", rc, errno, strerror(errno));
        printf("      check that the SETUID bit is set on the %s executable\n", DAF_BINARY);
        exit(1);
    }

    if ((rc = setuid(0)) != 0)
    {
        printf("main: setuid returned %d errno %d (%s)\n", rc, errno, strerror(errno));
        printf("      check that the SETUID bit is set on the %s executable\n", DAF_BINARY);
        exit(1);
    }

    if (initialise_shared_segment(statusmsg, errmsg, sizeof(statusmsg)) != 0)
    {
        safecpy(msg, "main: problem with initialise_shared_segment: ", sizeof(msg));
        safecat(msg, statusmsg, sizeof(msg));
        safecat(msg, ": ", sizeof(msg));
        safecat(msg, errmsg, sizeof(msg));
        safecat(msg, "\n", sizeof(msg));
        print_msg_to_console(msg);
        exit(1);
    }

#ifndef DAF_AGENT
    mysql_library_init(0, NULL, NULL);           // TODO   we also need to de-initialise this on exit
#endif

    init_process_signal_handlers();

    /* ----------------------------------------------------------*/
    /* parse arguments and open console log                      */
    /* ----------------------------------------------------------*/
    if (parse_args(argc, argv, &parameters) != E_OK)
    {
        exit(1);
    }

    /* ----------------------------------------------------------*/
    /* deal with -service options                                */
    /* ----------------------------------------------------------*/
    if (parameters.service_flag)
    {

        if (parameters.service_stop_flag)
        {
            if (stop_daf_service_running() != 0)
            {
                close_consolelog();
                exit(1);
            }
        }

        if (parameters.service_delete_flag)
        {
            if (daf_service_delete() != 0)
            {
                close_consolelog();
                exit(1);
            }
        }

        if (parameters.service_install_flag)
        {
            if (daf_service_install(NULL, argv[0]) != 0)
            {
                close_consolelog();
                exit(1);
            }

            if (initialise_shared_segment_values(msg, sizeof(msg)) != 0)
            {
                print_msg_to_console(msg);
                close_consolelog();
                exit(1);
            }
        }

        if (parameters.service_start_flag)
        {
            if (start_daf_service_running() != 0)
            {
                close_consolelog();
                exit(1);
            }
        }

        if (parameters.service_run_flag)
        {
            if ((service_pid = is_daf_service_running()) == 0)
            {
                print_compile_time_signature(TRUE);
                run_service(&parameters);
            }
            else
            {
                snprintf(msg, sizeof(msg), "daf service cannnot be started as there is already an existing daf service (pid=%llu) running\n",
                         service_pid);
                print_msg_to_console(msg);
                close_consolelog();
                exit(1);
            }
        }

        close_consolelog();
        exit(0);
    }


    /* ----------------------------------------------------------*/
    /* deal with -remote options                                 */
    /* ----------------------------------------------------------*/

#ifndef DAF_AGENT

    if (! parameters.quiet_flag)
    {
        print_compile_time_signature(TRUE);
        print_parameters(&parameters);
    }

    if (parameters.execute_flag)
    {

        int run_in_shell = 0;

        exit_rc = remote_client_cmd(parameters.remotehostname, run_in_shell, parameters.cmdstring, parameters.identstring,  parameters.run_in_background_flag,
                                    parameters.workqueueID_flag, parameters.workqueueID, 0);

    } else if (parameters.start_scenario_flag)
    {

        exit_rc = remote_client_start_scenario(parameters.remotehostname, parameters.jobname, parameters.project, parameters.phase,
                                               parameters.loglocation, parameters.scenariologfile,
											   parameters.teststand, parameters.testlevel, parameters.comments, parameters.username, parameters.parameters);

    }
    else if (parameters.get_flag)
    {

        if (parameters.get_newident_flag)
        {

            exit_rc = remote_client_get_unique_ident(parameters.remotehostname, parameters.no_headers_flag);

        }

    }
    else if (parameters.query_flag)
    {

        if (parameters.query_tag_flag)
        {

            exit_rc = remote_client_query_tag(parameters.remotehostname, parameters.tag,
                                              parameters.no_headers_flag, parameters.delimiter_flag, parameters.delimiter_string);

        }
        else if (parameters.query_alltags_flag)
        {

            exit_rc = remote_client_query_alltags(parameters.remotehostname,
                                                  parameters.no_headers_flag, parameters.delimiter_flag, parameters.delimiter_string);

        }
        else if (parameters.query_ident_flag)
        {

            exit_rc = remote_client_query_ident(parameters.remotehostname, parameters.identstring,
                                                parameters.no_headers_flag, parameters.delimiter_flag, parameters.delimiter_string);

        }
        else if (parameters.query_cmds_flag)
        {

            exit_rc = remote_client_query_cmds(parameters.remotehostname,
                                               parameters.no_headers_flag, parameters.delimiter_flag, parameters.delimiter_string);

        }
        else if (parameters.query_version_flag)
        {

            exit_rc = remote_client_query_version(parameters.remotehostname, parameters.set_zombie_reaper_debug_flag,  parameters.reset_zombie_reaper_debug_flag);

        }
        else
        {


        }

    }
    else if (parameters.report_flag)
    {

        if (parameters.report_cmds_flag)
        {

            exit_rc = remote_client_query_cmdlog(parameters.remotehostname, parameters.no_headers_flag, parameters.delimiter_flag, parameters.delimiter_string);

        }

    }
    else if (parameters.clear_tag_flag)
    {

        exit_rc = remote_client_clear_tag(parameters.remotehostname, parameters.tag);

    }
    else if (parameters.clear_ident_flag)
    {

        exit_rc = remote_client_clear_ident(parameters.remotehostname, parameters.identstring);

    }
    else if (parameters.clear_alltags_flag)
    {

        exit_rc = remote_client_clear_alltags(parameters.remotehostname);

    }
    else if (parameters.clear_shared_memory_flag)
    {

        exit_rc = remote_client_cntrl(parameters.remotehostname, parameters.clear_shared_memory_flag);

    }
    else if (parameters.cancel_tag_flag)
    {

        exit_rc = remote_client_cancel_tag(parameters.remotehostname, parameters.tag);

    }
    else if (parameters.cancel_ident_flag)
    {

        exit_rc = remote_client_cancel_ident(parameters.remotehostname, parameters.identstring);

    }
    else if (parameters.dirlist_flag)
    {

        exit_rc = remote_client_dirlist(parameters.remotehostname, parameters.remotepathname);

    }
    else if (parameters.getfile_flag)
    {

        exit_rc = remote_client_copyfile(parameters.remotehostname, parameters.localpathname, parameters.remotepathname, TRUE);

    }
    else if (parameters.putfile_flag)
    {

        exit_rc = remote_client_copyfile(parameters.remotehostname, parameters.localpathname, parameters.remotepathname, FALSE);

    }
    else if (parameters.validate_licence_flag)
    {

        exit_rc = validate_licence(parameters.licence_pathname);

    }
    else if (parameters.write_licence_flag)
    {

        exit_rc = write_licence(parameters.licence_pathname, parameters.licence);

    }
    else
    {

        sprintf(msg, "no command specified\n");
        print_msg_to_console(msg);
        exit_rc = E_NOTOK;

    }

    if (! parameters.quiet_flag)
    {
        sprintf(msg, "Exit code for daf is %d\n", exit_rc);
        print_msg_to_console(msg);
    }

#endif

    if (strlen(parameters.consolelog_pathname) != 0)
    {
        close_consolelog();
    }

    exit(exit_rc);
}
