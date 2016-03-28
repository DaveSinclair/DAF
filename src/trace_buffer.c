
/****************************************************************************/
//
/*****************************************************************************/

#include "defines.h"
#include "DskXOSbase.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "rules.h"
#include "threads.h"
#include "trace_buffer.h"
#include "console_msg.h"
#include "print_console.h"

#define DEBUG 0

typedef struct trace_file_header
{
   Iu32 type;
   Iu32 version;
   Iu32 num_trace_entries;
   TID  thread_id;
   char device_name[MAX_BLOCKDEVICE_NAME_LEN];
   char rule_id[MAX_RULE_ID_LENGTH + 1];
} trace_file_header_t;


typedef struct trace_lookup_element
{
   int  trace_value;
   char trace_name[MAX_TRACE_NAME_LENGTH];
} trace_lookup_element_t;


static trace_lookup_element_t trace_lookup_table[MAX_TRACE_NUM] =
{

   { PROC_SEG_LOCK,   "seg_lock" },
   { PROC_SEG_UNLOCK, "seg_unlock" },
   { LOCK_MUTEX,      "lock_mutex" },
   { UNLOCK_MUTEX,    "unlock_mutex" },
   { PROC_COND_WAIT1, "proc_cond_wait1" },
   { WAIT_BEFORE,     "wait_before" },
   { WAIT_AFTER,      "wait_after" },
   { PROC_COND_BROADCAST1, "proc_cond_broadcast1" },
   { BROADCAST,       "broadcast" },
   { PROC_LOCK_MUTEX, "proc_lock_mutex" },
   { LOCK,            "lock" },
   { PROC_UNLOCK_MUTEX, "proc_unlock_mutex" },
   { UNLOCK,           "unlock" },
   { BUFFER_TIMESTAMP, "buffer_timestamp" },
   { BUFFER_CREATE,    "created" }

};


/************************************************************************/
/*                                                                      */
/* NAME:        print_trace_buffer                                      */
/*                                                                      */
/* FUNCTION:    Prints a decoded version of the thread trace buffer     */
/*                                                                      */
/* PARAMS IN:   pthread_workitem  thread workitem structure             */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     void                                              .     */
/************************************************************************/
void print_trace_buffer( u_thread_trace_element_t *pu_trace_table,
                         int                      num_entries_in_table)
{
   int i,j;
   Iu32 tracepoint;
   Iu32 tracetype;
   Iu32 timestamp;
   char *tracepoint_name;
   char *tracetype_name;
   char not_found[MAX_TRACE_NAME_LENGTH];
   char msg[MSG_TEXT_SIZE];
   thread_trace_element_t *ptrace_table;

   for (i=0; i< num_entries_in_table; i++) {

      ptrace_table = (thread_trace_element_t *) &pu_trace_table[i];
      tracepoint   = ptrace_table->tracepoint;
      tracetype    = ptrace_table->type;
      timestamp    = ptrace_table->timestamp,
      my_strncpy(not_found, "not found", MAX_TRACE_NAME_LENGTH);

      tracepoint_name = not_found;
      for (j = 0; j < MAX_TRACE_NUM; j++) {
         if (tracepoint == trace_lookup_table[j].trace_value)
         {
            tracepoint_name = trace_lookup_table[j].trace_name;
            break;
         }
      }

      tracetype_name = not_found;
      for (j = 0; j < MAX_TRACE_NUM; j++) {
         if (tracetype == trace_lookup_table[j].trace_value)
         {
            tracetype_name = trace_lookup_table[j].trace_name;
            break;
         }
      }

      sprintf(msg, "%08x %20s %20s %08x %08x %08x %08x\n",
             timestamp,
             tracepoint_name,
             tracetype_name,
             ptrace_table->p1,
             ptrace_table->p2,
             ptrace_table->p3,
             ptrace_table->p4);
      print_string_to_console(msg);

   }
   print_string_to_console("\n");
}

/************************************************************************/
/*                                                                      */
/* NAME:        save_trace_buffer                                       */
/*                                                                      */
/* FUNCTION:    Saves a binary version of the thread trace buffer       */
/*                                                                      */
/* PARAMS IN:   ptrace_table      pointer to a trace table              */
/*              pathname          pathname to file to save buffer in    */
/*              device_name       name of device being traced           */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                            */
/************************************************************************/
int save_trace_buffer( u_thread_trace_element_t *ptrace_table,
                       int                    num_trace_entries,
                       char                   *pathname,
                       char                   *device_name,
                       char                   *rule_id,
                       TID                    thread_id)
{

   static trace_file_header_t trace_header;

   FILE *stream;
   char msg[MSG_TEXT_SIZE];
   #define TRACE_BUFFER_VERSION 1

   trace_header.type              = 0;
   trace_header.version           = TRACE_BUFFER_VERSION;
   trace_header.num_trace_entries = num_trace_entries,
   trace_header.thread_id         = thread_id;

   my_strncpy(trace_header.device_name, device_name, sizeof(trace_header.device_name));
   my_strncpy(trace_header.rule_id,     rule_id,     sizeof(trace_header.rule_id));

   stream = fopen(pathname, "wb");   // open Write, Binary

   if (stream != NULL) {

      if (1 != fwrite( &trace_header, sizeof(trace_header), 1, stream)) {
        sprintf(msg, "Error:  save_trace_buffer: errno = %d, failed to write header to file %s\n", errno, pathname);
        fclose(stream);
        return(1);
      }

      if (num_trace_entries != fwrite( ptrace_table,
                                       sizeof(u_thread_trace_element_t),
                                       num_trace_entries,
                                       stream)) {
        sprintf(msg, "Error:  save_trace_buffer: errno = %d, failed to write to file %s\n",
                     errno, pathname);
        print_msg_to_console(msg);
        fclose(stream);
        return(1);
      }

      fclose(stream);
      return(0);
   }
   else
   {
      sprintf(msg, "Error:  save_trace_buffer: failed to write %s to file", pathname);
      print_error_msg_to_console(20000, "SYSTEM_FILEIO", msg);
      return(1);
   }
}


/************************************************************************/
/*                                                                      */
/* NAME:        read_trace_buffer                                       */
/*                                                                      */
/* FUNCTION:    Reads a binary version of the thread trace buffer       */
/*                                                                      */
/* PARAMS IN:   ptrace_tables[]        An array of trace buffers        */
/*              pnum_entries_in_table  The total number of entries      */
/*              pathname               pathname to read buffer from     */
/*              ptype                  returns the type value for this  */
/*                                     trace buffer                     */
/*              ptype                  returns the version number for   */
/*                                     this trace buffer                */
/*              pdevicename            returns the name of the device   */
/*                                     that this trace buffer applies to*/
/*              prule_id               returns the name of the rule     */
/*                                     that this trace buffer applies to*/
/*              pthread_id             returns the thread_ID that       */
/*                                     generated this trace buffer      */
/*                                                                      */
/*              print_header_data      shows header data for the trace  */
/*                                     file if TRUE                     */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                            */
/************************************************************************/
int read_trace_buffer( u_thread_trace_element_t **pu_trace_table,
                       int                    *num_entries_in_table,
                       char                   *pathname,
                       Iu32                   *type,
                       Iu32                   *version,
                       char                   *device_name,
                       char                   *rule_id,
                       TID                    *thread_id,
                       BOOL                   print_header_data)
{

   static trace_file_header_t trace_header;

   FILE *stream;
   char msg[MSG_TEXT_SIZE];
   thread_trace_element_t **ptrace_table = (thread_trace_element_t **) pu_trace_table;

   stream = fopen( pathname, "r" );
   if( stream != NULL )
   {

      if (1 != fread( &trace_header, sizeof(trace_header), 1, stream)) {
        sprintf(msg, "Error:  read_trace_buffer: errno = %d, failed to read %lu bytes in header from  %s file\n",
                      errno, (unsigned long)sizeof(trace_header), pathname);
        print_error_msg_to_console(20000, "SYSTEM_FILEIO", msg);
        fclose(stream);
        return( 1 );
      }

      *type                 = trace_header.type;
      *version              = trace_header.version;
      *num_entries_in_table = trace_header.num_trace_entries;
      *thread_id            = trace_header.thread_id;

      if (TRACE_BUFFER_VERSION !=  trace_header.version) {
        sprintf(msg, "Error:  read_trace_buffer: trace file %s uses header version %d but this version of the program uses version %d - cannot proceed\n",
                      pathname, trace_header.version, TRACE_BUFFER_VERSION);
        print_error_msg_to_console(20000, "TRACE_VERSION", msg);
        return(1);
      }


      my_strncpy(device_name, trace_header.device_name, sizeof(trace_header.device_name));
      my_strncpy(rule_id,     trace_header.rule_id,     sizeof(trace_header.rule_id));

      if ((DEBUG) || (print_header_data)) {
        printf("pathname %s, type %d, version %d, num_entries_in_table %d, TID %lu\n",
                pathname, *type, *version, *num_entries_in_table, printable_tid(*thread_id));
        printf("rule_id %s, device_name %s\n", rule_id, device_name);

      }

      if (*num_entries_in_table > 0) {

        *ptrace_table = (thread_trace_element_t *) my_malloc((*num_entries_in_table) * sizeof(thread_trace_element_t), "read_trace_buffer" );
        if (*ptrace_table == NULL) {
           sprintf(msg, "read_trace_buffer: Could not malloc space for trace table containing %d entries\n", *num_entries_in_table);
           print_error_msg_to_console(20000, "SYSTEM_FILEIO", msg);
           exit(1);
        }


        if (*num_entries_in_table != fread( *ptrace_table,
                                            sizeof(u_thread_trace_element_t),
                                            *num_entries_in_table,
                                            stream)) {
          sprintf(msg, "Error:  read_trace_buffer: errno = %d, failed to read the expected %d trace elements from %s\n", errno, *num_entries_in_table, pathname);
          print_error_msg_to_console(20000, "SYSTEM_FILEIO", msg);
          fclose(stream);
          return( 1 );
        }

      } else {

        sprintf(msg, "read_trace_buffer: num_entries in table = 0 for %s\n", pathname);
        print_info_msg_to_console(10000, "SYSTEM", msg);
        *ptrace_table = NULL;

      }

      fclose( stream );
   }
   else
   {
      sprintf(msg, "Error: read_trace_buffer: failed to open %s\n", pathname);
      print_error_msg_to_console(20000, "SYSTEM_FILEIO", msg);
      return(1);
   }
   sprintf(msg, "Read %s successfully\n", pathname);
   print_msg_to_console(msg);

   return (0);
}

/************************************************************************/
/*                                                                      */
/* NAME:        print_raw_trace_buffer                                  */
/*                                                                      */
/* FUNCTION:    Prints a raw version of the thread trace buffer         */
/*                                                                      */
/* PARAMS IN:   ptrace_table           pointer to the start of the trace*/
/*                                     table                            */
/*              num_entries_in_table   the number of entries in the     */
/*                                     trace table                      */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     void                                              .     */
/************************************************************************/
void print_raw_trace_buffer( u_thread_trace_element_t *pu_trace_table,
                             int                    num_entries_in_table)
{
   int i;
   Iu32 timestamp;
   Iu32 tracepoint;
   Iu32 tracetype;
   char not_found[MAX_TRACE_NAME_LENGTH];
   char msg[MSG_TEXT_SIZE];
   thread_trace_element_t *ptrace_table;

   for (i=0; i< num_entries_in_table; i++) {

      ptrace_table = (thread_trace_element_t *) &pu_trace_table[i];
      tracepoint = ptrace_table->tracepoint;
      tracetype  = ptrace_table->type;
      timestamp  = ptrace_table->timestamp,
      my_strncpy(not_found, "not found", MAX_TRACE_NAME_LENGTH);

      sprintf(msg, "%08x   %08x  %08x  %08x  %08x  %08x  %08x\n",
              timestamp,
              tracepoint,
              tracetype,
              ptrace_table->p1,
              ptrace_table->p2,
              ptrace_table->p3,
              ptrace_table->p4);
      print_string_to_console(msg);

   }
      print_string_to_console("\n");
}

/************************************************************************/
/*                                                                      */
/* NAME:        print_trace_buffers                                     */
/*                                                                      */
/* FUNCTION:    Prints a decoded version of the thread trace buffer     */
/*                                                                      */
/* PARAMS IN:   ptrace_table           pointer to an array of trace     */
/*                                     table addresses                  */
/*              num_entries_in_table   the number of entries in one     */
/*                                     trace table                      */
/*              num_tables             the number of trace tables       */
/*                                     in the array                     */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     void                                              .     */
/************************************************************************/
void print_trace_buffers(u_thread_trace_element_t *trace_table[MAX_NUM_THREAD_TRACE_TABLES],
                         int                    num_entries_in_tables[MAX_NUM_THREAD_TRACE_ENTRIES],
                         int                    num_tables,
                         char                   trace_names[][MAX_TRACEFILE_NAME_LEN],
                         char                   device_names[][MAX_BLOCKDEVICE_NAME_LEN+1],
                         char                   rule_ids[][MAX_RULE_ID_LENGTH+1],
                         TID                    *thread_ids)
{
   int i,j,t;
   int  next_entry[MAX_NUM_THREAD_TRACE_TABLES];
   BOOL all_entries_printed = FALSE;
   Iu32 tracepoint;
   Iu32 tracetype;
   Iu32 timestamp;
   Iu32 earliest_timestamp;
   Iu32 first_timestamp;
   Iu32 first_unix_timestamp;
   Iu32 timestamp_secs;
   Iu32 timestamp_ms;
   char *tracepoint_name;
   char *tracetype_name;
   char not_found[MAX_TRACE_NAME_LENGTH];
   char p1label[5];
   char p2label[5];
   char p3label[8];
   char p4label[5];
   u_thread_trace_element_t *ptrace_table;
   char msg[MSG_TEXT_SIZE];
   thread_trace_element_t * pthread_trace_element;
   thread_trace_element_open_disk_t * ptrace_open_disk;
   thread_trace_element_close_disk_t * ptrace_close_disk;
   thread_trace_element_read_disk_t * ptrace_read_disk;
   thread_trace_element_write_disk_t * ptrace_write_disk;
   char time_str[40];
   time_t first_time_in_secs;
   struct tm timestruct;
   struct tm *ptm = &timestruct;
   time_t time_then;

   /*------------------------------------------------------------------*/
   /* Sort the entries in each buffer into timestamp order             */
   /* and set up an index variable for each table that will point      */
   /* the next entry in that table that has not been printed - ie this */
   /* is the most recent entry in this table that has not yet been     */
   /* used                                                             */
   /*------------------------------------------------------------------*/

   for (i=0; i < num_tables; i++) {

     sort_trace_buffer( trace_table[i], num_entries_in_tables[i]);
     next_entry[i] = 0;
   }

   /*------------------------------------------------------------------*/
   /* The first line in each trace table is a dummy entry and instead  */
   /* and contains the unix time at which the                          */
   /* buffer was initialised in the trace_value field                  */
   /*------------------------------------------------------------------*/

   first_timestamp = trace_table[0][0].thread_trace_element.timestamp;
   first_unix_timestamp = trace_table[0][0].thread_trace_element.p1;

   for (i=0; i < num_tables; i++) {
     ptrace_table = trace_table[i];
     if (ptrace_table[0].thread_trace_element.timestamp < first_timestamp) {
        first_timestamp = (ptrace_table[0].thread_trace_element.timestamp);
        first_unix_timestamp = (ptrace_table[0].thread_trace_element.p1);
     }
   }

   first_time_in_secs = ((time_t) convert_Iu32_to_bigendian(first_unix_timestamp));
   localtime_r(&first_time_in_secs, ptm);
   strftime( time_str, 40, "%c %Z", ptm );

   sprintf(msg, "Earliest timestamp in all trace tables is 0x%08x milliseconds (%4d.%03d seconds) relative to start of DskX run\n",  first_timestamp, first_timestamp/1000, first_timestamp % 1000);
   print_string_to_console(msg);
   sprintf(msg, "Unix timestamp for start of run is 0x%08x (%s)\n",  first_unix_timestamp, time_str);
   print_string_to_console(msg);

   sprintf(msg, "%8s %8s %8s %13s parameters.....\n", "device", "rule", "time", "secs.millisec");
   print_string_to_console("\n");
   print_string_to_console(msg);
   print_string_to_console("\n");

   while (!all_entries_printed) {

      t = -1;
      ptrace_table = trace_table[0];
      earliest_timestamp = ptrace_table[next_entry[0]].thread_trace_element.timestamp;

      for (i=0; i < num_tables; i++) {
         ptrace_table = trace_table[i];
         if ((next_entry[i] < num_entries_in_tables[i])) {
           if (ptrace_table[next_entry[i]].thread_trace_element.timestamp != 0) {
             if (( ptrace_table[next_entry[i]].thread_trace_element.timestamp < earliest_timestamp ) || (t == -1)) {
                earliest_timestamp = ptrace_table[next_entry[i]].thread_trace_element.timestamp;
                t = i;
             }
           } else {
             next_entry[i]++;
           }
         }
      }

      if (t != -1) {

         // now we know that the earliest entry available is in table number t, and at
         // entry next_entry[t]

         ptrace_table = trace_table[t];
         timestamp  = ptrace_table[next_entry[t]].thread_trace_element.timestamp;

         if (timestamp != 0) {

            timestamp_secs = timestamp / 1000;
            timestamp_ms   = timestamp % 1000;
            time_then = first_time_in_secs + timestamp_secs;
            localtime_r(&time_then, ptm);
            sprintf(time_str, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
            tracepoint = ptrace_table[next_entry[t]].thread_trace_element.tracepoint;
            tracetype  = ptrace_table[next_entry[t]].thread_trace_element.type;

            my_strncpy(not_found, "not found", MAX_TRACE_NAME_LENGTH);

            tracepoint_name = not_found;
            for (j = 0; j < MAX_TRACE_NUM; j++) {
              if (tracepoint == trace_lookup_table[j].trace_value)
              {
                tracepoint_name = trace_lookup_table[j].trace_name;
                break;
              }
            }

            tracetype_name = not_found;
            for (j = 0; j < MAX_TRACE_NUM; j++) {
              if (tracetype == trace_lookup_table[j].trace_value)
              {
                 tracetype_name = trace_lookup_table[j].trace_name;
                 break;
              }
            }

            my_strncpy(p1label,"    ", sizeof(p1label));
            my_strncpy(p2label,"    ", sizeof(p2label));
            my_strncpy(p3label,"    ", sizeof(p3label));
            my_strncpy(p4label,"    ", sizeof(p4label));

            if (tracetype == OPEN_START) {
              ptrace_open_disk = ( thread_trace_element_open_disk_t * ) &trace_table[t][next_entry[t]];
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s mode=%-d\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            ptrace_open_disk->mode);
            } else if (tracetype == OPEN_COMPLETE) {
              ptrace_open_disk = ( thread_trace_element_open_disk_t * ) &trace_table[t][next_entry[t]];
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s mode=%-d handle=%-d rc=%-d\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            ptrace_open_disk->mode,
                            ptrace_open_disk->handle,
                            ptrace_open_disk->rc);
            } else if (tracetype == CLOSE_START) {
              ptrace_close_disk = ( thread_trace_element_close_disk_t * ) &trace_table[t][next_entry[t]];
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s handle=%-d\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            ptrace_close_disk->handle);
            } else if (tracetype == CLOSE_COMPLETE) {
              ptrace_close_disk = ( thread_trace_element_close_disk_t * ) &trace_table[t][next_entry[t]];
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s handle=%-d rc=%-d\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            ptrace_close_disk->handle,
                            ptrace_close_disk->rc);
            } else if (tracetype == READ_START) {
              ptrace_read_disk = ( thread_trace_element_read_disk_t * ) &trace_table[t][next_entry[t]];
              my_strncpy(p1label, "lba=", sizeof(p1label));
              my_strncpy(p2label, "len=", sizeof(p2label));
              my_strncpy(p3label, "handle=", sizeof(p3label));
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s %4s0x%016llx %4s0x%08x %7s%-d\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            p1label, ptrace_read_disk->lba,
                            p2label, ptrace_read_disk->numblks,
                            p3label, ptrace_read_disk->rc_or_handle);
            } else if (tracetype == WRITE_START) {
              ptrace_write_disk = ( thread_trace_element_write_disk_t * ) &trace_table[t][next_entry[t]];
              my_strncpy(p1label, "lba=", sizeof(p1label));
              my_strncpy(p2label, "len=", sizeof(p2label));
              my_strncpy(p3label, "handle=", sizeof(p3label));
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s %4s0x%016llx %4s0x%08x %7s%-d\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            p1label, ptrace_write_disk->lba,
                            p2label, ptrace_write_disk->numblks,
                            p3label, ptrace_write_disk->rc_or_handle);
            } else if (tracetype == READ_COMPLETE) {
              ptrace_read_disk = ( thread_trace_element_read_disk_t * ) &trace_table[t][next_entry[t]];
              my_strncpy(p1label, "lba=", sizeof(p1label));
              my_strncpy(p2label, "len=", sizeof(p2label));
              my_strncpy(p3label, " rc=", sizeof(p3label));
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s %4s0x%016llx %4s0x%08x %4s0x%08x\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            p1label, ptrace_read_disk->lba,
                            p2label, ptrace_read_disk->numblks,
                            p3label, ptrace_read_disk->rc_or_handle);
            } else if (tracetype == WRITE_COMPLETE) {
              ptrace_write_disk = ( thread_trace_element_write_disk_t * ) &trace_table[t][next_entry[t]];
              my_strncpy(p1label, "lba=", sizeof(p1label));
              my_strncpy(p2label, "len=", sizeof(p2label));
              my_strncpy(p3label, " rc=", sizeof(p3label));
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s %4s0x%016llx %4s0x%08x %4s0x%08x\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            p1label, ptrace_write_disk->lba,
                            p2label, ptrace_write_disk->numblks,
                            p3label, ptrace_write_disk->rc_or_handle);
            } else if (tracetype == BUFFER_CREATE) {
              pthread_trace_element = ( thread_trace_element_t * ) &trace_table[t][next_entry[t]];
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s timestamp=0x%08x\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            pthread_trace_element->p1);
            } else {
              pthread_trace_element = ( thread_trace_element_t * ) &trace_table[t][next_entry[t]];
              sprintf(msg, "%8s %8s %8s %4d.%03d  %20s %10s %4s0x%08x %4s0x%08x %4s0x%08x %4s0x%08x\n",
                            device_names[t], rule_ids[t], time_str, timestamp_secs, timestamp_ms,
                            tracepoint_name, tracetype_name,
                            p1label, pthread_trace_element->p1,
                            p2label, pthread_trace_element->p2,
                            p3label, pthread_trace_element->p3,
                            p4label, pthread_trace_element->p4);
            }

            print_string_to_console(msg);
         }


         // move the next entry index past this entry so we don't reconsider
         // it the next time round the loop
         if (next_entry[t] < num_entries_in_tables[t]) {
            next_entry[t]++;
         }
      }

      // maybe we are done - ie all next pointers are at the end of their
      // respective tables - if so, set the variable that allows us to
      // exit this loop

      all_entries_printed = TRUE;

      for (i=0; i < num_tables; i++) {

         if (next_entry[i] < num_entries_in_tables[i]) {
           all_entries_printed = FALSE;
           break;
         }

      }

   }

}

/************************************************************************/
/*                                                                      */
/* NAME:        sort_trace_buffer                                       */
/*                                                                      */
/* FUNCTION:    Sortts a trace buffer into time ascending order         */
/*                                                                      */
/* PARAMS IN:   ptrace_table           pointer to the start of the trace*/
/*                                     table                            */
/*              num_entries_in_table   the number of entries in the     */
/*                                     trace table                      */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     void                                              .     */
/************************************************************************/
void sort_trace_buffer( u_thread_trace_element_t *pu_trace_table,
                        int                    num_entries_in_table)
{
   int  i;
   Iu32 earliest_timestamp;
   int  index_of_earliest_timestamp;
   int  source_index;
   char msg[MSG_TEXT_SIZE];
   thread_trace_element_t *ptrace_table;
   u_thread_trace_element_t *pu_temp_table;
   thread_trace_element_t *ptemp_table;

   if (num_entries_in_table == 0) {
      return;
   }

   /*------------------------------------------------------------------*/
   /* Search the entire table to find the index of the entry that      */
   /* has the earliest time.  Entries with a timestamp = 0 will be     */
   /* ignored                                                          */
   /*------------------------------------------------------------------*/

   pu_temp_table = (u_thread_trace_element_t *) my_malloc(num_entries_in_table * sizeof(u_thread_trace_element_t), "sort_trace_buffer");
   if (pu_temp_table == NULL) {
      sprintf(msg, "sort_trace_buffer: Could not malloc space for temporary trace table containing %d entries\n", num_entries_in_table);
      print_error_msg_to_console(20000, "SYSTEM_MALLOC", msg);
      exit(1);
   }

   ptrace_table = (thread_trace_element_t *) &pu_trace_table[0];
   earliest_timestamp          = ptrace_table->timestamp;
   index_of_earliest_timestamp = 0;

   for (i=1; i< num_entries_in_table; i++) {

      ptrace_table = (thread_trace_element_t *) &pu_trace_table[i];
      if ( (ptrace_table->timestamp != 0) &&
           (ptrace_table->timestamp < earliest_timestamp ) ) {
         earliest_timestamp = ptrace_table->timestamp;
         index_of_earliest_timestamp = i;
      }
   }

   /*------------------------------------------------------------------*/
   /* Now move the earliest entry in the table to index 0, the         */
   /* next entry to index 1 and so on, remembering to wrap when we     */
   /* get to the end of the table.  Put the results of this move       */
   /* into a temporary table for the time being                        */
   /*------------------------------------------------------------------*/

   for (i=0; i< num_entries_in_table; i++) {

      source_index = (index_of_earliest_timestamp + i) % num_entries_in_table;

      ptrace_table = (thread_trace_element_t *) &pu_trace_table[source_index];
      ptemp_table =  (thread_trace_element_t *) &pu_temp_table[i];

      ptemp_table->timestamp    = ptrace_table->timestamp;
      ptemp_table->tracepoint   = ptrace_table->tracepoint;
      ptemp_table->type         = ptrace_table->type;
      ptemp_table->p1           = ptrace_table->p1;
      ptemp_table->p2           = ptrace_table->p2;
      ptemp_table->p3           = ptrace_table->p3;
      ptemp_table->p4           = ptrace_table->p4;

   }

   /*------------------------------------------------------------------*/
   /* Put the sorted table back into the original trace table          */
   /*------------------------------------------------------------------*/

   for (i=0; i< num_entries_in_table; i++) {

      ptrace_table = (thread_trace_element_t *) &pu_trace_table[i];
      ptemp_table =  (thread_trace_element_t *) &pu_temp_table[i];

      ptrace_table->timestamp   = ptemp_table->timestamp;
      ptrace_table->tracepoint  = ptemp_table->tracepoint;
      ptrace_table->type        = ptemp_table->type;
      ptrace_table->p1          = ptemp_table->p1;
      ptrace_table->p2          = ptemp_table->p2;
      ptrace_table->p3          = ptemp_table->p3;
      ptrace_table->p4          = ptemp_table->p4;

   }

}

/************************************************************************/
/*                                                                      */
/* NAME:        save_trace_buffer_to_disk                               */
/*                                                                      */
/* FUNCTION:    Saves a trace buffer to disk using the standard         */
/*              naming convention                                       */
/*                                                                      */
/* PARAMS IN:   pthread_workitem       pointer to the thread workitem   */
/*                                     that contains the trace table    */
/*                                     that is to be saved              */
/*              device_name            the name of the device being     */
/*                                     traced                           */
/*              rule_id                the rule name for the data that  */
/*                                     is being saved                   */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     int                                               .     */
/************************************************************************/
int save_trace_buffer_to_disk(thread_workitem_t *pthread_workitem,
                              char              *device_name,
                              char              *rule_id)
{
   char tracefile_pathname[MAX_TRACEFILE_NAME_LEN];
   int  return_rc = 0;
   TID  thread_id = thread_ID();
   char msg[MSG_TEXT_SIZE];

   determine_tracefile_pathname(tracefile_pathname,
                                sizeof(tracefile_pathname),
                                device_name,
                                rule_id,
                                thread_id);

   if ( save_trace_buffer(pthread_workitem->ptrace_table,
                          pthread_workitem->num_trace_entries,
                          tracefile_pathname,
                          device_name,
                          rule_id,
                          thread_id) != 0) {

      sprintf(msg, "Error: Could not write trace file %s to disk\n", tracefile_pathname);
      print_error_msg_to_console(20000, "SYSTEM_INTERNAL", msg);
      return_rc = 1;
   } else {
      sprintf(msg, "Trace file %s (device %s, entries %d, rule %s) successfully written to disk\n",
                   tracefile_pathname, device_name, pthread_workitem->num_trace_entries, rule_id);
      print_msg_to_console(msg);
   }
   return(return_rc);
}

/************************************************************************/
/*                                                                      */
/* NAME:        determine_tracefile_pathname                            */
/*                                                                      */
/* FUNCTION:    Works out the name of the tracefile that is to be used  */
/*              for this thread                                         */
/*                                                                      */
/* PARAMS IN    :  max_pathname_length   max length of pathname         */
/*                 device_name           name of device eg vpath3       */
/*                 rule_id               rule id for this thread        */
/*                                                                      */
/* PARAMS IN/OUT:  pathname        pointer to string in which the       */
/*                                pathname will be returned             */
/*                                                                      */
/* PARAMS OUT:  returns 0 for success, non zero for failure             */
/*                                                                      */
/* GLOBALS CHANGED: None.                                               */
/*                                                                      */
/* RETURNS:     0 for success, 1 for failure                            */
/************************************************************************/
void determine_tracefile_pathname(char *pathname,
                                  int max_pathname_length,
                                  char *device_name,
                                  char *rule_id,
                                  TID  thread_id)
{
   char temp_string[100];

   //# trace file name is of the form
   //# DskXvpath3_CMP10_4.trc
   //# where vpath3 is the device name
   //# where CMP10 is the rule id
   //# and 4 is the thread id

   my_strncpy(pathname, TRACE_PATH,    max_pathname_length);
   my_strncat(pathname, "DskX",        max_pathname_length);   // <<<<<<<<<<<<<
   my_strncat(pathname, device_name,   max_pathname_length);
   my_strncat(pathname, "_",           max_pathname_length);
   my_strncat(pathname, rule_id,       max_pathname_length);
   sprintf(temp_string, "_%-lu.trc", printable_tid(thread_id) );
   my_strncat(pathname, temp_string, max_pathname_length);

}
