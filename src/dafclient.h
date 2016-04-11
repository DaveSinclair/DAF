
/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf automation framework                                                                                             */
/*                                                                                                                      */
/* dafclient.h                                                                                                          */
/*                                                                                                                      */
/* This file is part of the daf automation framework agent                                                              */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */


#ifndef _DAFCLIENT_H
#define _DAFCLIENT_H

#include "daf_protocol.h"

int remote_client_cntrl(char *remotehostname, bool_t clear_shared_memory_flag);
int remote_client_cmd(char *remotehostname, int run_in_shell, char *cmdstring, char *identstring,
                      bool_t run_in_background, bool_t workqueueID_flag, Iu32 workqueueID, Iu32 actionresultID);
int remote_client_start_scenario(char *remotehostname, char *jobname, char *project, char *phase, char*loglocation, char *scenariologfile,
		                         char *teststand, char *testlevel, char *comments, char *username, parameterlist parameters);
int remote_client_query_cmdlog(char *remotehostname, bool_t no_headers, bool_t use_delimiter, char *delimter_string);
int remote_client_query_auditlog(char *remotehostname, bool_t no_headers, bool_t use_delimiter, char *delimiter_string);

int remote_client_query_version(char *remotehostname,
                                bool_t set_zombie_reaper_debug_flag,
                                bool_t reset_zombie_reaper_debug_flag);

int remote_client_ping_agent(char *remotehostname, bool_t quiet);
int remote_client_query_tag(char *remotehostname, Iu32 tag, bool_t no_headers, bool_t use_delimiter, char *delimiter_string);
int remote_client_query_alltags(char *remotehostname, bool_t no_headers, bool_t use_delimiter, char *delimiter_string);
int remote_client_get_unique_ident(char *remotehostname, bool_t no_headers);
int remote_client_query_ident(char *remotehostname, char *identstring, bool_t no_headers, bool_t use_delimer, char *delimter_string);
int remote_client_query_cmds(char *remotehostname, bool_t no_headers, bool_t use_delimiter, char *delimiter_string);
int remote_client_clear_tag(char *remotehostname, Iu32 tag);
int remote_client_clear_alltags(char *remotehostname);
int remote_client_clear_ident(char *remotehostname, char *ident);
int remote_client_cancel_tag(char *remotehostname, Iu32 tag);
int remote_client_cancel_ident(char *remotehostname, char *ident);
int remote_client_copyfile(char *remotehostname, char *localpathname, char *remotepathname, bool_t fromremote);
int remote_client_dirlist(char *remotehostname, char *remotepathname);
int remote_client_get_dirlist(char *remotehostname, char *remotepathname, dirlist *ppdl);
int remote_client_deletefile(char *remotehostname, char *remotepathname);
int remote_client_update_workqueue_status(char *remotehostname, int workqueueID, bool_t update_tag, int tag, bool_t update_state, char *state, bool_t update_pass, int pass, bool_t update_end);

int query_slave_tag(char         *remotehostname,
                    Iu32         tag,
					int          *exit_code,
					int          *exit_signal,
					cmd_log_state          *exit_state);


#endif
