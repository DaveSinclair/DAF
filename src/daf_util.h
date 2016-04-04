#ifndef _DAF_UTIL_H
#define _DAF_UTIL__H

#include "OSstuff.h"
#include "daf_protocol.h"

#define MATCH(oper, string) (strcmp(oper, string) == 0)
#define MATCHN(oper, string, n) (memcmp(oper, string, n-1) == 0)
#define cknull(x) if((x)==NULL) {perror(""); exit(EXIT_FAILURE);}
#define cknltz(x) if((x)<0) {perror(""); exit(EXIT_FAILURE);}



void clnt_stat_decode(enum clnt_stat clnt_stat, char *msg, int msg_size);
Iu8 *duplicate_bytes(unsigned char *source, int length);
char *duplicate_string(char *source);
char *convert_epoch_time_to_string(time_t epoch_time, char *time_str, int time_str_length);
void timeval_sum (struct timeval start, struct timeval end, struct timeval *sum);
int timeval_diff(struct timeval start, struct timeval end, struct timeval *difference);
char *get_current_time_as_timestamp(char *timestamp, int max_timestamp_length, char separator);
char *time_msg(char *msg, int max_msg_len);
void print_string_to_console(char *text);
void print_msg_to_console(char *text);
void print_msg_to_console_log(char *text);
void print_errmsg_to_console(char *text);
int open_consolelog(char *console_log_filename);
int close_consolelog(void);
int log_msg_to_scenario_console_log(char *agent_log_pathname, char *prefix, char *msg, BOOL add_newline);


int ensure_directory_path_exists(char *pathname);
int does_file_exist(char *pathname);
int create_empty_file(char *pathname);
int copy_file(char *src, char *dest);
int chmod_file(char *src, char *permissions);
int delete_file(char *pathname);
char *safecpy(char *dest, char *src, unsigned int len);
char *safecat(char *dest, char *src, unsigned int len);

int run_system_cmd_and_print_all_output(char *cmd);
int run_system_cmd_and_capture_single_line_output(char *cmd,
        char *output,
        int max_output_line_length);
int run_system_cmd(char *cmd, BOOL quiet);
int run_system_cmd_in_background(char *cmd);


pid_t get_process_pid();
void chomp(char *mystring);
int set_ulimit_c_unlimited(char *errmsg, int max_errmsg_len);


#endif




