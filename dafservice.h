/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf automation framework                                                                                             */
/*                                                                                                                      */
/* dafservice.h                                                                                                         */
/*                                                                                                                      */
/* This file is part of the daf automation framework agent                                                              */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#ifndef _DAFSERVICE_H
#define _DAFSERVICE_H

#include "daf_shared_memory.h"

void set_zombie_reaper_debug_flag(bool_t zombie_reaper_debug_flag);
void remote_client_request_decode(int remote_client_request, char *msg, int msg_size);

void cmd_log_state_decode(enum cmd_log_state cmd_log_state, char *msg, int msg_size);
void lock_cmd_log(cmd_log_object_t *cmd_log, char *subname);
void unlock_cmd_log(cmd_log_object_t *cmd_log, char *subname);
int add_cmd_log_entry(cmd_log_object_t *cmd_log_object,
                      char  *cmdstring,
                      char  *identstring,
                      char  *agent_log_pathname,
                      Iu32  tag,
                      Iu32  actionrequestID,
                      Iu32  pid,
                      Iu32  state,
                      Iu32  status,
                      Iu32  start_time,
                      Iu32  end_time, 
                      Iu32  timeout_at_time, 
                      bool_t  fail_on_timeout,
                      char  *errmsg,
                      int   max_msg_len);
int add_cmd_log_fragment(cmd_log *cmd_log,
                         char  *cmdstring,
                         char  *identstring,
                         Iu32  tag,
                         Iu32  actionresultID,
                         Iu32  pid,
                         Iu32  state,
                         Iu32  status,
                         Iu32  start_time,
                         Iu32  end_time,
                         Iu32  timeout_at_time, 
                         bool_t  fail_on_timeout,
                         char  *errmsg,
                         int   max_msg_len);
bool_t remove_cmd_log_entry_by_tag(cmd_log_object_t *cmd_log_object, Iu32 tag);
bool_t remove_cmd_log_entry_by_ident(cmd_log_object_t *cmd_log_object, char *identstring);
int is_tag_in_cmd_log(cmd_log_object_t *cmd_log_object, Iu32 tag);
void deallocate_dirlist(dirlist pdl);
void deallocate_cmd_log(cmd_log_object_t  *cmd_log_object);
void deallocate_cmd_log_fragment(cmd_log *cmd_log);

int update_cmd_log_entry(cmd_log_object_t *cmd_log_object,
                         char *agent_log_pathname, 
                         Iu32 tag,
                         Iu32 state,
                         Iu32 status,
                         bool_t update_pid,
                         Iu32 pid);

void initialise_service_environment();

void print_cmd_log(cmd_log_object_t *cmd_log_object,
                   bool_t no_headers,
                   bool_t use_delimer,
                   char *delimter_string);

void print_cmd_log_fragment(cmd_log  *cmd_log_object,
                            bool_t   no_headers,
                            bool_t   use_delimiter,
                            char     *delimiter_string);

void *zombie_reaper(void *p);


#endif


