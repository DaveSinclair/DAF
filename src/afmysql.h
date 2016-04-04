
#ifndef _AFMYSQL_H
#define _AFMYSQL_H

#include <mysql.h>
#include "af_defns.h"
#include "afconstants.h"
#include "daf_shared_memory.h"

typedef struct af_daemon_thread_data
{
    char  sql_servername[MAX_SQLSERVERNAME_LEN];
    char  sql_username[MAX_SQLUSERNAME_LEN];
    char  sql_password[MAX_SQLPASSWORD_LEN];
    char  sql_databasename[MAX_SQLDATABASENAME_LEN];
    Iu16  sql_port;
    char  sql_socketname[MAX_SQLSOCKETNAME_LEN];
    bool_t simulator_mode;
} af_daemon_thread_data_t;

typedef struct af_collector_thread_data
{
    char  sql_servername[MAX_SQLSERVERNAME_LEN];
    char  sql_username[MAX_SQLUSERNAME_LEN];
    char  sql_password[MAX_SQLPASSWORD_LEN];
    char  sql_databasename[MAX_SQLDATABASENAME_LEN];
    Iu16  sql_port;
    char  sql_socketname[MAX_SQLSOCKETNAME_LEN];
    bool_t simulator_mode;
} af_collector_thread_data_t;

typedef struct af_agent_status_daemon_thread_data
{
    char  sql_servername[MAX_SQLSERVERNAME_LEN];
    char  sql_username[MAX_SQLUSERNAME_LEN];
    char  sql_password[MAX_SQLPASSWORD_LEN];
    char  sql_databasename[MAX_SQLDATABASENAME_LEN];
    Iu16  sql_port;
    char  sql_socketname[MAX_SQLSOCKETNAME_LEN];
    bool_t simulator_mode;
} af_agent_status_daemon_thread_data_t;

void lock_active_steps(active_steps_object_t *active_steps);
void unlock_active_steps(active_steps_object_t *active_steps);

bool_t lookup_collectortypeID(collectortype_t *collectortypes, int num_collectortype_records, char *name, int *ID);
void lookup_actiontype(actiontype_t *actiontypes, int num_actiontype_records, int ID, char *actiontype, int max_actiontype_len);
void lookup_testcase(testcase_t *testcases, int num_testcase_records, int ID, char *testcase, int max_testcase_len);

void get_all_actiontype_records(MYSQL         *sql_connection,
                                actiontype_t  *actiontype,
                                int           max_num_records,
                                int           *num_records );
void get_all_testcase_records(MYSQL       *sql_connection,
                              testcase_t  *testcase,
                              int          max_num_records,
                              int          *num_records );
void get_all_workqueue_records(MYSQL         *sql_connection,
                               workqueue_t   *workqueue,
                               int           max_num_records,
                               int           *num_records );
void get_action_records(MYSQL     *sql_connection,
                        action_t  *action,
                        int       max_num_records,
                        char      *where,
                        int       *num_records );
void get_host_records(MYSQL     *sql_connection,
                      host_t    *host,
                      int       max_num_records,
                      char      *where,
                      int       *num_records );
void get_collectortype_records(MYSQL            *sql_connection,
                               collectortype_t  *builtin_collectortype,
                               int              max_num_builtin_records,
                               collectortype_t  *user_collectortype,
                               int              max_num_user_records,
                               char             *where,
                               int              *num_builtin_types,
                               int              *num_user_types,
                               int              *num_records );
void get_collectorvalue_records(MYSQL     *sql_connection,
                                collectorvalue_t    *collectorvalue,
                                int       max_num_records,
                                char      *where,
                                int       *num_records );
void get_environmentalue_records(MYSQL               *sql_connection,
                                 environmentvalue_t  *environmentvalue,
                                 int                 max_num_records,
                                 char                *where,
                                 int                 *num_records );
void get_environmentvalue_assignments(MYSQL          *sql_connection,
                                      environment_assignment_t  *environment_assignment,
                                      int            max_num_records,
                                      char           *where,
                                      int            *num_records );


void get_testcase_record(MYSQL_RES   *res_set,
                         MYSQL_ROW   *row,
                         testcase_t  *testcase);
void get_actiontype_record(MYSQL_RES    *res_set,
                           MYSQL_ROW    *row,
                           actiontype_t *actiontype);
void get_action_record(MYSQL_RES  *res_set,
                       MYSQL_ROW  *row,
                       action_t   *action);
void get_actionresult_record(MYSQL_RES       *res_set,
                             MYSQL_ROW       *row,
                             actionresult_t  *actionresult);
void get_workqueue_record(MYSQL_RES    *res_set,
                          MYSQL_ROW    *row,
                          workqueue_t  *workqueue);
void get_workrequest_record(MYSQL_RES      *res_set,
                            MYSQL_ROW      *row,
                            workrequest_t  *workrequest);
void get_host_record(MYSQL_RES   *res_set,
                     MYSQL_ROW   *row,
                     host_t      *host);
void get_collectortype_record(MYSQL_RES        *res_set,
                              MYSQL_ROW        *row,
                              collectortype_t  *collectortype);
void get_collectorvalue_record(MYSQL_RES        *res_set,
                               MYSQL_ROW        *row,
                               collectorvalue_t *collectorvalue);
void get_collectorset_record(MYSQL_RES       *res_set,
                             MYSQL_ROW       *row,
                             collectorset_t  *collectorset);
void get_collectorsetmember_record(MYSQL_RES       *res_set,
                                   MYSQL_ROW       *row,
                                   collectorsetmember_t  *collectorsetmember);
void get_environment_record(MYSQL_RES      *res_set,
                            MYSQL_ROW      *row,
                            environment_t  *environment);
void get_environmentvalue_record(MYSQL_RES      *res_set,
                                 MYSQL_ROW      *row,
                                 environmentvalue_t  *environmentvalue);

void process_workqueue(MYSQL *sql_connection);
void process_new_workqueue_requests(MYSQL *sql_connection,
                                    char *sql_servername,
                                    char *sql_username,
                                    char *sql_password,
                                    char *sql_databasename,
                                    Iu16 sql_port,
                                    bool_t process_new_workqueue_requests);
void process_new_scenario_requests(MYSQL *sql_connection);

void *af_work_daemon(void *p);
void *af_collector_daemon(void *p);
void *af_agent_status_daemon(void *p);

#endif










