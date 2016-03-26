/* ------------------------------------------------------------------------------------------------- */
/*  af_mysql.h                                                                                       */
/*                                                                                                   */
/*  Automation Framework MYSQL client interface                                                      */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

#ifndef _AF_MYSQL_H
#define _AF_MYSQL_H

#include <mysql.h>
#include "daf_protocol.h"

void set_sql_debug_flag(bool_t sql_debug_flag);

int perform_query(char *caller, 
                  MYSQL *sql_connection, 
                  char *query);

MYSQL *do_connect(char *caller,
                   char *host_name, 
                   char *user_name, 
                   char *password, 
                   char *db_name,
                   unsigned int port_num, 
                   char *socket_name, 
                   unsigned int flags);
void do_disconnect(char *caller,
                        MYSQL *conn);

void print_query (MYSQL *conn, char *query);
void print_result_set (MYSQL_RES *res_set);

int perform_update(char *caller,
                   MYSQL *conn,
                   char  *table,
                   char  *fields[],
                   char  *values[],
                   char  *where,
                   char  *errmsg,
                   int   max_msg_len);

int perform_insert(char *caller,
                   MYSQL *conn,
                   char  *table,
                   char  *fields[],
                   char  *values[],
                   char  *errmsg,
                   int   max_msg_len);

int perform_drop(char *caller,
                 MYSQL *conn,
                 char  *table,
                 char  *where,
                 char  *errmsg,
                 int   max_msg_len);

MYSQL_RES *check_for_res_set(char *caller, 
                             MYSQL *sql_connection,
                             char *query);
#endif
