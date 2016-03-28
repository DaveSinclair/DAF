/*****************************************************************************/
// Title: trace_buffer.h
//                                           .
/*****************************************************************************/

#ifndef _H_TRACE_BUFFER

#define _H_TRACE_BUFFER

#include "threads.h"
#include "rules.h"

void print_trace_buffer_version(void);
void print_raw_trace_buffer( u_thread_trace_element_t *ptrace_table,
                             int                    num_entries_in_table);

void print_trace_buffer( u_thread_trace_element_t *ptrace_table,
                         int                    num_entries_in_table);

void sort_trace_buffer( u_thread_trace_element_t *ptrace_table,
                        int                    num_entries_in_table);


void print_trace_buffers(u_thread_trace_element_t *trace_table[MAX_NUM_THREAD_TRACE_TABLES],
                         int                    num_entries_in_tables[MAX_NUM_THREAD_TRACE_ENTRIES],
                         int                    num_tables,
                         char                   trace_names[][MAX_TRACEFILE_NAME_LEN],
                         char                   device_names[][MAX_BLOCKDEVICE_NAME_LEN+1],
                         char                   rule_ids[][MAX_RULE_ID_LENGTH+1],
                         TID                    *thread_ids);

int save_trace_buffer( u_thread_trace_element_t *ptrace_table,
                       int                    num_trace_entries,
                       char                   *pathname,
                       char                   *device_name,
                       char                   *rule_id,
                       TID                    thread_id);

int read_trace_buffer( u_thread_trace_element_t **ptrace_table,
                       int                    *num_entries_in_table,
                       char                   *pathname,
                       Iu32                   *type,
                       Iu32                   *version,
                       char                   *device_name,
                       char                   *rule_id,
                       TID                    *thread_id,
                       BOOL                   print_header_data);


void determine_tracefile_pathname(char *pathname,
                                  int  max_pathname_length,
                                  char *device_name,
                                  char *rule_id,
                                  TID  thread_id);

int save_trace_buffer_to_disk(thread_workitem_t *pthread_workitem,
                              char              *device_name,
                              char              *rule_id);
#endif

