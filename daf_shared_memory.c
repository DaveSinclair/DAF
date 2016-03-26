/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                       */
/*                                                                                                                      */
/* daf.c                                                                                                               */
/*                                                                                                                      */
/* This file is part of the daf NFS test exerciser programme.                                                          */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* The daf shared memory area                                                                                          */
/*                                                                                                                      */
/* daf establishes a persistent shared area in memory when it first runs. This shared memory area does not persist    */
/* the daf terminates but is deleted when the host is rebooted.  It is used to store a record of which daf            */
/* processes have run, whether they are still running and/or what their exit status was.                                */
/* It is also used as a communication area between daf processes that are running on the same test host.               */
/*                                                                                                                      */
/* At present there is no locking/access mechanism so independent processes and threads                                 */
/* can read/write this area at will. Not sure if this is going to be a problem.  Probably.                              */
/*                                                                                                                      */
/* The daf communication area                                                                                          */
/*                                                                                                                      */
/*                                                                                                                      */
/* This contains the following information:                                                                             */
/*                                                                                                                      */
/*                                                                                                                      */
/*                                                                                                                      */
/*                                                                                                                      */
/* The daf process audit log                                                                                           */
/*                                                                                                                      */
/* If two or more more daf processes are accidentally run performing i/o to the same device, then it is easy to        */
/* generate a false miscompare, as each daf process will assume that files written by that process will remain on the  */
/* disk without change, unless the same proceess performs a subsequent write to that file. Eg, if process A running     */
/* daf writes data to /testfs/newdata/client0/level0/data1 and then process B, running a different instance of daf    */
/* writes new data to the same file, /testfs/newdata/client0/level0/data1, then a false miscompare can occur if process */
/* A then does a data validation operation on /testfs/newdata/client0/level0/data1.                                     */
/* This can happen if a user starts daf in a background process and then forgets or fails to notice that daf is still */
/* running, and then starts a second daf process.                                                                      */
/*                                                                                                                      */
/* To help debug such situations, daf provides an audit log of showing the last N instances of daf that were run on   */
/* that particular host, and the time they started and stopped.   This audit log is written to the console output       */
/* and captured in the consolelog file and looks like:                                                                  */
/*                                                                                                                      */
/* ???????????????                                                                                                      */
/*                                                                                                                      */
/* Here                                                                                                                 */
/*                                                                                                                      */
/*                                                                                                                      */
/* active               is 0 or 1 to indicate if that process is still actively running                                 */
/* pid                  is the process id associated with this instance of daf                                         */
/* status               is 0 and not currently used                                                                     */ 
/* start                is the start time of that daf run                                                              */
/* stop                 is the stop time of that daf run                                                               */
/* commandline          is the command line used to invoke that daf run                                                */
/*                                                                                                                      */
/* The audit log is held in a shared memory segment on the test host machine.  This shared memory segment is allocated  */
/* by the first daf instance to run on that machine and will persist until the machine is rebooted.   The last N       */
/* instances of daf to run on the host are recorded in the audit log, where N is typically 200. The current state of   */
/* the audit log is printed out by each daf process just before it terminates, so if there is a miscompare that could  */
/* have been caused by multiple daf sessions running on the same host exercising the same devices, then this can be    */
/* spotted by examining the active daf processes in the audit log and their command lines and seeing if they are       */
/* running to the same file names and filesystem names.                                                                 */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "daf_shared_memory.h"
#include "daf_util.h"
#include "daf_service_environment.h"

void *p_shm    = NULL;

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* active_state                                                           */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

void active_state(int active, char *active_string, int max_msg_length) {

   if (active == DAF_PROCESS_FREE) {
      safecpy(active_string, "Free", max_msg_length);
   } else if (active == DAF_PROCESS_ACTIVE) {
      safecpy(active_string, "Running", max_msg_length);
   } else if (active == DAF_PROCESS_COMPLETED) {
      safecpy(active_string, "Complete", max_msg_length);
   } else if (active == DAF_PROCESS_STALE) {
      safecpy(active_string, "Stale   ", max_msg_length);
   } else if (active == DAF_PROCESS_NEVER_RUN) {
      safecpy(active_string, "Neverrun", max_msg_length);
   } else {
      safecpy(active_string, "Unknown", max_msg_length);
   }
}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* get_shared_segment_process_pointer                                     */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

bool_t get_shared_segment_process_pointer( shm_daf_t **p, char *errmsg, int max_msg_length) {

#undef  SUBNAME
#define SUBNAME "get_shared_segment_process_pointer"

  char msg[MAX_MSG_LEN];

  if ((*p = query_shared_segment_process_pointer()) == NULL) {
     sprintf(msg, "%s: query_shared_segement_process_pointer() returned p = NULL - continuing", SUBNAME);
     safecpy(errmsg, msg, max_msg_length);
     return(FALSE);
   }

   // -------------------------------------------------------------
   /* Check that the shared memory version number matches the    */
   /* the version in use by this program                         */
   // -------------------------------------------------------------

   if ((*p)->shm_version != DAF_SHM_VERSION) {
     sprintf(msg, "remove_process_to_shared_segment: Wrong value (%d) for shm_version in shared memory at %p - should be %d",
                  (*p)->shm_version, *p, DAF_SHM_VERSION);
     safecpy(errmsg, msg, max_msg_length);
     return(FALSE);
   }

   return(TRUE);

}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* find_cmd_log                                                           */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

bool_t find_cmd_log(cmd_log_object_t  **p_service_cmd_log_object,
                    char              *errmsg, 
                    int               max_msg_length) {

#undef  SUBNAME
#define SUBNAME "find_cmd_log"

   char               msg[MAX_MSG_LEN];
   shm_daf_t *p;

   if (errmsg != NULL) {
     errmsg[0] = 0;
   }

   if ((p = query_shared_segment_process_pointer()) == NULL) {
     sprintf(msg, "%s: query_shared_segement_process_pointer() returned p = NULL - continuing", SUBNAME);
     if (errmsg != NULL) {
        safecpy(errmsg, msg, max_msg_length);
        safecpy(errmsg, "\n", max_msg_length);
     }
     return(FALSE);
   }

   *p_service_cmd_log_object    = &(p->service_cmd_log_object);

   return(TRUE);

}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* find_active_steps                                                      */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

bool_t find_active_steps(active_steps_object_t  **p_service_active_steps_object,
                         char                   *errmsg, 
                         int                    max_msg_length) {

#undef  SUBNAME
#define SUBNAME "find_active_steps"

   char        msg[MAX_MSG_LEN];
   shm_daf_t  *p;

   if (errmsg != NULL) {
     errmsg[0] = 0;
   }

   if ((p = query_shared_segment_process_pointer()) == NULL) {
     sprintf(msg, "%s: query_shared_segement_process_pointer() returned p = NULL - continuing", SUBNAME);
     if (errmsg != NULL) {
        safecpy(errmsg, msg, max_msg_length);
        safecpy(errmsg, "\n", max_msg_length);
     }
     return(FALSE);
   }

   *p_service_active_steps_object = &(p->active_steps_object);

   return(TRUE);

}


/**************************************************************************/
/*                                                                        */
/* initialise_shared_segment                                              */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*   none                                                                 */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*  statusmsg       char string to accept an status message               */
/*                  If the return code is zero, this string contains      */
/*                  an informational message                              */
/*                  If the return code is non zero, this string is returned */
/*                  as ""                                                 */
/*                                                                        */
/*  errmsg          char string to accept an error message                */
/*                  if the return code from this routine is non zero.     */
/*                  If the return code is zero, this string is returned   */
/*                  as ""                                                 */
/*  max_msg_length  max length of string in errmsg                        */
/*                                                                        */
/*  returns 0 for success, 1 for error                                    */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/* This routine either creates a shared memory segment for daf usage if  */
/* one does not exist or connects to an existing daf shared memory       */
/*                                                                        */
/* daf might be run from a number of different shells or scripts on the  */
/* the same host - the idea here is that all these difference instances   */
/* of daf (that are running in different processes) can then interact    */
/* using this single, shared memory area.  Once created, the shared       */
/* memory area will persist until the machine is rebooted                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

int initialise_shared_segment(char *statusmsg, 
                              char *errmsg, 
                              int  max_msg_length) {

#undef  SUBNAME
#define SUBNAME "initialise_shared_segment"

   key_t              key;
   int                shmid;
   struct             shmid_ds shm_buf;
   size_t             current_size_shm;
   size_t             size_shm;
   size_t             full_size_shm;
   int                just_created = 0;
   char               msg[MAX_MSG_LEN]    = "";
   shm_daf_t *p;

   memset(&shm_buf, 0, sizeof(shm_buf));
   errmsg[0]    = 0;
   statusmsg[0] = 0;

   /* ------------------------------------------------------------- */
   /* make the key:                                                 */
   /* ------------------------------------------------------------- */

   if ((key = ftok("/tmp", 'S' )) == -1) {
      sprintf(msg, "%s: Could not create key for shared memory acess error=%d %s", SUBNAME, errno, strerror(errno));
      safecpy(errmsg, msg, max_msg_length);
      return(1);
   }

   /* --------------------------------------------------------------------------------- */
   /* we are actually going to create the memory size somewhat larger than needed       */
   // to allow for future expansion.
   /* --------------------------------------------------------------------------------- */

   full_size_shm = SIZE_shm_daf_BYTES;
   size_shm      = sizeof(shm_daf_t);
   if (size_shm > full_size_shm) {
      sprintf(msg, "%s: internal error size_shm > full_size_shm (%d > %d)\n",
                    SUBNAME, (int)size_shm, (int)full_size_shm);
      safecpy(errmsg, msg, max_msg_length);
      return(1);

   }

   /* --------------------------------------------------------------------------------- */
   /* See if we can connect to an existing shared memory segment                        */
   /* we try to connect to a segment and use a size of zero                             */
   /* - this should work if any size segment with the right                             */
   /* key exists, and if we can connect we can find out the real                        */
   /* size from shmctl                                                                  */
   /* --------------------------------------------------------------------------------- */

   if ((shmid = shmget(key, 0, 0644 )) == -1) {

      /* ------------------------------------------------------------------------------ */
      /* cannot connect - no segment present , so create a new one                      */
      /* ------------------------------------------------------------------------------ */

      if ((shmid = shmget(key, full_size_shm, 0644 | IPC_CREAT)) == -1) {
        sprintf(msg, "%s: errno = %d (%s): Could not create a shared memory block of size " SIZE_TSPEC,
                      SUBNAME, errno, strerror(errno), (int)full_size_shm);
         safecpy(errmsg, msg, max_msg_length);
         return(1);
      } else {
         just_created = TRUE;
         sprintf(msg, "%s: created shared memory block - id %d, size " SIZE_TSPEC,
                      SUBNAME, shmid, (int)full_size_shm);
         safecpy(errmsg, msg, max_msg_length);
      }

   } else {

      /* ------------------------------------------------------------------------------ */
      /* date structure exists already - is it the right size?                          */
      /* ------------------------------------------------------------------------------ */

      if (shmctl(shmid, IPC_STAT, &shm_buf)) {

         sprintf(msg, "%s: INTERNAL_ERROR: could not run shmctl with IPC_STAT errno=%d (%s)",
                      SUBNAME, errno, strerror(errno));
         safecpy(errmsg, msg, max_msg_length);
         return(1);

      } else {

         current_size_shm = shm_buf.shm_segsz;

         if (current_size_shm != full_size_shm) {

            /* ------------------------------------------------------------------------------ */
            /* data structure exists but is wrong size, I thought it would                    */
            /* be possible to dynamicallyo resize it and                                      */
            /* treat it as a newly initialised data structure however that                    */
            /* does not seem to work so treat this is an error for the time                   */
            /* being                                                                          */
            /* ------------------------------------------------------------------------------ */

            if (shmctl(shmid, IPC_STAT, &shm_buf)) {
               shm_buf.shm_segsz = full_size_shm;
               sprintf(msg, "%s: INTERNAL_ERROR: could not run shmctl with SHM_SIZE (currrent_shm_size = "
                            SIZE_TSPEC ", full_size_shm = " SIZE_TSPEC ",  errno=%d (%s)",
                            SUBNAME, (int) current_size_shm, (int)full_size_shm, errno, strerror(errno));
               safecpy(errmsg, msg, max_msg_length);
               return(1);
            }
            sprintf(msg, "%s: INTERNAL_ERROR: shared memory size not as expected - currrent_shm_size = "
                          SIZE_TSPEC ", desired full_size_shm = " SIZE_TSPEC "\n",
                          SUBNAME, (int) current_size_shm, (int) full_size_shm);
            safecpy(errmsg, msg, max_msg_length);
            return(1);
         }

      }
   }

   /* ------------------------------------------------------------------------------ */
   /* attach to the segment to get a pointer to it and if it is                      */
   /* a newly created segment, then initialise it to all 0s.  If                     */
   /* it is an existing segment, then check the shm_version                          */
   /* number                                                                         */
   /* ------------------------------------------------------------------------------ */

   p_shm = shmat(shmid, (void *)0, 0);

   if (p_shm == (char *)(-1)) {
      sprintf(msg, "%s: Could not attach to shared memory block id=%d, error= %d %s", SUBNAME, shmid, errno, strerror(errno));
      safecpy(errmsg, msg, max_msg_length);
      return(1);
   } else {
      p = (shm_daf_t *)(p_shm);
      if (just_created) {
         initialise_shared_segment_values(NULL, 0);
      } else {
         if (p->shm_version != DAF_SHM_VERSION) {
            sprintf(msg, "%s: Wrong value (%d) for shm_version in shared memory at %p- should be %d - ",
                         SUBNAME, p->shm_version, p, DAF_SHM_VERSION);
 /* <<<<<<<< what to do here ??? */
            safecpy(errmsg, msg, max_msg_length);
            return(1);
         }
      }
   }

   if (just_created) {
     sprintf(statusmsg, "%s: A new shared memory segment was created at %p", SUBNAME, p_shm);
   } else {
     sprintf(statusmsg, "%s: An existing shared memory segment was used at %p", SUBNAME, p_shm);
   }

   return(0);

}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* initialise_shared_sgement_mutexes                                      */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*   p              pointer to the shared memory area                     */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*  errmsg          char string to accept an error message                */
/*                  if the return code from this routine is non zero.     */
/*                  If the return code is zero, this string is returned   */
/*                  as ""                                                 */
/*                                                                        */
/*  max_msg_length  max length of string in errmsg                        */
/*                                                                        */
/*  returns 0 for success, 1 for error                                    */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int allocate_shared_segment_mutexes(shm_daf_t *p, char *errmsg, int max_msg_length) {

#undef  SUBNAME
#define SUBNAME "allocate_shared_segment_mutexes"

   char msg[MAX_MSG_LEN];
   pthread_mutexattr_t psharedm;

   pthread_mutexattr_init(&psharedm);
   pthread_mutexattr_setpshared(&psharedm, PTHREAD_PROCESS_SHARED);

   if (pthread_mutex_init(&(p->service_cmd_log_object.mp), &psharedm) != 0) {
      sprintf(msg, "%s: problem initialising service cmd log mutex, errno = %d - exiting", SUBNAME, errno);
      exit(1);
   }

   if (pthread_mutex_init(&(p->active_steps_object.mp), NULL) != 0) {
      sprintf(msg, "%s: problem initialising active_steps_object mutex, errno = %d - exiting", SUBNAME, errno);
      exit(1);
   }

   return 0;

}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* initialise_shared_segment_values                                       */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*   none                                                                 */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*  errmsg          char string to accept an error message                */
/*                  if the return code from this routine is non zero.     */
/*                  If the return code is zero, this string is returned   */
/*                  as ""                                                 */
/*                                                                        */
/*  max_msg_length  max length of string in errmsg                        */
/*                                                                        */
/*  returns 0 for success, 1 for error                                    */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/*  <<<< what happens if this is called with one (or more)                */ 
/* of the mutex in the alreaqdy locked state?                             */
/* ---------------------------------------------------------------------- */

int initialise_shared_segment_values(char *errmsg, int max_msg_length) {

#undef  SUBNAME
#define SUBNAME "initialise_shared_segment_values"

   char               msg[MAX_MSG_LEN];
   shm_daf_t *p;
   int rc;
   
   if (errmsg != NULL) {
     errmsg[0] = 0;
   }

   if ((p = query_shared_segment_process_pointer()) == NULL) {
     sprintf(msg, "%s: query_shared_segement_process_pointer() returned p = NULL - continuing", SUBNAME);
     if (errmsg != NULL) {
        safecpy(errmsg, msg, max_msg_length);
        safecpy(errmsg, "\n", max_msg_length);
     }
     return(1);
   }

   // -------------------------------------------------------------
   // -------------------------------------------------------------

   memset(p, 0, sizeof(shm_daf_t));

   p->shm_version                                            = DAF_SHM_VERSION;
   p->size_shm_shared_bytes                                  = (Iu32) sizeof(shm_daf_t);
   p->daemon_pid                                             = 0;
   p->daemon_tick                                            = 0;
   p->max_num_slots                                          = DAF_MAX_PROCESSES; 
   p->next_slot_to_use                                       = 0;
   p->current_cmd_tag                                        = 1;
   p->epoch_time_last_cleared                                = (Iu32) time(0);

   if (clear_cmd_array(errmsg, max_msg_length) != 0) {
     sprintf(msg, "%s: problem running clear_cmd_array()", SUBNAME);
     if (errmsg != NULL) {
        safecpy(errmsg, msg, max_msg_length);
        safecpy(errmsg, "\n", max_msg_length);
     }
     return(1);
   }

   rc = allocate_shared_segment_mutexes(p, errmsg, max_msg_length);

   return(rc);

}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* clear_cmd_array                                                        */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*   none                                                                 */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*  errmsg          char string to accept an error message                */
/*                  if the return code from this routine is non zero.     */
/*                  If the return code is zero, this string is returned   */
/*                  as ""                                                 */
/*                                                                        */
/*  max_msg_length  max length of string in errmsg                        */
/*                                                                        */
/*  returns 0 for success, 1 for error                                    */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int clear_cmd_array(char *errmsg, int max_msg_length) {

#undef  SUBNAME
#define SUBNAME "clear_cmd_array"

   char                  msg[MAX_MSG_LEN];
   char                  agent_log_pathname[MAX_PATHNAME_LEN];
   shm_daf_t *  p;
   int                   i;
   char                  thishostname[MAX_HOSTNAME_LEN];

   if (errmsg != NULL) {
     errmsg[0] = 0;
   }

   if ((p = query_shared_segment_process_pointer()) == NULL) {
     sprintf(msg, "%s: query_shared_segement_process_pointer() returned p = NULL - continuing", SUBNAME);
     if (errmsg != NULL) {
        safecpy(errmsg, msg, max_msg_length);
     }
     return(1);
   }

   /* what about stale processes see above <<<<<<<<<<<<<<<<<<< */

   gethostname(thishostname, sizeof(thishostname));



   // -------------------------------------------------------------
   // -------------------------------------------------------------

   for (i=0; i<DAF_MAX_COMMANDS; i++) {

      if (p->service_cmd_log_object.cmd_array[i].state == CMD_COMPLETED ) {
         safecpy(p->service_cmd_log_object.cmd_array[i].cmdstring,   "", sizeof(p->service_cmd_log_object.cmd_array[i].cmdstring));
         safecpy(p->service_cmd_log_object.cmd_array[i].identstring, "", sizeof(p->service_cmd_log_object.cmd_array[i].identstring));
         safecpy(p->service_cmd_log_object.cmd_array[i].agent_log_pathname, agent_log_pathname, 
                 sizeof(p->service_cmd_log_object.cmd_array[i].agent_log_pathname));
         p->service_cmd_log_object.cmd_array[i].state       = CMD_FREE;
         p->service_cmd_log_object.cmd_array[i].status      = 0;
         p->service_cmd_log_object.cmd_array[i].start_time  = 0;
         p->service_cmd_log_object.cmd_array[i].end_time    = 0;
         p->service_cmd_log_object.cmd_array[i].tag         = 0;
         p->service_cmd_log_object.cmd_array[i].pid         = 0;
         p->service_cmd_log_object.cmd_array[i].workqueueID = 0;
      }

   }

   return(0);

}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* delete_shared_segment                                                  */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*   none                                                                 */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*  errmsg          char string to accept an error message                */
/*                  if the return code from this routine is non zero.     */
/*                  If the return code is zero, this string is returned   */
/*                  as ""                                                 */
/*                                                                        */
/*  max_msg_length  max length of string in errmsg                        */
/*                                                                        */
/*  returns 0 for success, 1 for error                                    */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int delete_shared_segment(char *errmsg, int max_msg_length) {

#undef  SUBNAME
#define SUBNAME "delete_shared_segment"

   key_t           key;
   int             shmid;
   char            msg[MAX_MSG_LEN];
   struct shmid_ds shm_buf;

   memset(&shm_buf, 0, sizeof(shm_buf));

   if (errmsg != NULL) {
     errmsg[0] = 0;
   }

   errmsg[0] = 0;

   // -------------------------------------------------------------
   /* make the key: */
   // -------------------------------------------------------------

   if ((key = ftok("/tmp", 'N' )) == -1) {
      sprintf(msg, "%s: Could not create key for shared memory acess error=%d %s", SUBNAME, errno, strerror(errno));
      safecpy(errmsg, msg, max_msg_length);
      return(1);
   }

   // -------------------------------------------------------------
   /* See if we can connect to an existing shared memory segment */
   // -------------------------------------------------------------

   if ((shmid = shmget(key, 0, 0644 )) == -1) {
       sprintf(msg, "%s: errno = %d (%s): Could not find a shared memory block to delete",
                    SUBNAME, errno, strerror(errno));
       safecpy(errmsg, msg, max_msg_length);
       return(1);
   }

   if (shmctl(shmid, IPC_RMID, &shm_buf) == -1) {
      sprintf(msg, "%s: errno = %d (%s): Could not delete shared memory block",
                   SUBNAME, errno, strerror(errno));
      safecpy(errmsg, msg, max_msg_length);
      return(1);
   }


   // -------------------------------------------------------------
   // -------------------------------------------------------------


   return(0);

}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* terminate_use_of_shared_segment                                        */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*   none                                                                 */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*  errmsg          char string to accept an error message                */
/*                  if the return code from this routine is non zero.     */
/*                  If the return code is zero, this string is returned   */
/*                  as ""                                                 */
/*                                                                        */
/*  max_msg_length  max length of string in errmsg                        */
/*                                                                        */
/*  returns 0 for success, 1 for error                                    */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int terminate_use_of_shared_segment(char *errmsg, int max_msg_length) {

#undef  SUBNAME
#define SUBNAME "terminate_use_of_shared_segment"

   char               msg[MAX_MSG_LEN];
   shm_daf_t *p;

   errmsg[0] = 0;

   if (! get_shared_segment_process_pointer(&p, errmsg, max_msg_length)) {
     return E_NOTOK;
   }

   // -------------------------------------------------------------
   /* detach from the segment:                                   */
   // -------------------------------------------------------------

   if (shmdt((void *) p) == -1) {
      sprintf(msg, "%s: errno = %d (%s): Could not detach from shared memory block",
                   SUBNAME, errno, strerror(errno));
      safecpy(errmsg, msg, max_msg_length);
      return(1);
   }

   return 0;

}

/* ---------------------------------------------------------------------- */
/*                                                                        */
/* query_shared_segment_process_pointer                                   */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*   none                                                                 */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*  returns either NULL if the shared segment process pointer is not      */
/*                      defined or the address of the shared segment.     */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/* ---------------------------------------------------------------------- */
shm_daf_t *query_shared_segment_process_pointer(void) {

#undef  SUBNAME
#define SUBNAME "query_shared_segment_process_pointer"

   return (shm_daf_t *) p_shm;

}


/* ---------------------------------------------------------------------- */
/*                                                                        */
/* print_shared_segment_details                                           */
/*                                                                        */
/* Inputs:                                                                */
/*                                                                        */
/*   none                                                                 */
/*                                                                        */
/* Outputs:                                                               */
/*                                                                        */
/*  returns 0 for success, 1 for error                                    */
/*                                                                        */
/* Description                                                            */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int print_shared_segment_details(char *errmsg, int max_msg_length) {

#undef  SUBNAME
#define SUBNAME "print_shared_segment_details"

   shm_daf_t *p;

   if (! get_shared_segment_process_pointer(&p, errmsg, max_msg_length)) {
     return E_NOTOK;
   }

   // -------------------------------------------------------------
   // -------------------------------------------------------------

   printf("daf shared segment ------------------------------------------------------------------------\n");
   printf("shm_version                                                 %d\n",       p->shm_version);
   printf("size_shm_shared_bytes                                       %d\n",       p->size_shm_shared_bytes);
   printf("size_shm_shared_bytes address                               %p\n",       p);
   printf("daemon_pid                                                  %llu\n",     (unsigned long long) p->daemon_pid);
   printf("daemon_tick                                                 0x%08x\n",   p->daemon_tick);
   printf("epoch_time_last_cleared                                     0x%08x\n",   p->epoch_time_last_cleared);

   return 0;

}



