/*-------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                     */
/* daf exerciser                                                                                                       */
/*                                                                                                                     */
/* afmysql.c                                                                                                           */
/*                                                                                                                     */
/* This file is part of the daf automation package.                                                                    */
/*                                                                                                                     */
/*                                                                                                                     */
/* ------------------------------------------------------------------------------------------------------------------- */


/* <<<<<< most (or all) of this file should be excluded if this is the DAF_AGENT */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <mysql.h>
#include "af_defns.h"
#include "af_mysql.h"
#include "afmysql.h"
#include "daf_protocol.h"
#include "daf_util.h"
#include "daf_shared_memory.h"
#include "dafservice.h"
#include "daf_service_environment.h"
#include "dafclient.h"
#include "email.h"
#include "licence.h"

extern int   daf_major_version;
extern int   daf_minor_version;
extern char  *daf_version_string;

typedef struct suitable_hosts {
   char Hostname[MAX_HOSTNAME_LEN];
   char Hostselectorvalue[DAF_HOST_HOSTSELECTORVALUE_LEN];
   int Num_actions_allocated_in_this_scenario_step;
} suitable_hosts_t;

typedef struct hosts_in_use {
   char Hostname[DAF_HOST_NAME_LEN];
   int Num_actions_allocated_in_this_scenario_step;
} hosts_in_use_t;



#define COLLECTORSET_REQUIRED_VALUES_FIELDS "ID, Name, Description"
   typedef struct collectorset_required_values {
      char        Collectorsetname[DAF_COLLECTORSET_NAME_LEN];
      int         CollectortypeID;
      char        Collectortypename[DAF_COLLECTORTYPE_NAME_LEN];
      char        Requiredvalue[DAF_COLLECTORVALUE_LEN];
      char        Comparator[DAF_COMPARATOR_LEN];
  } collectorset_required_values_t;

typedef struct host_collectorvalues {
   int         HostID;
   char        Hostname[DAF_HOST_NAME_LEN];
   char        Hostselectorvalue[DAF_HOSTSELECTORVALUE_NAME_LEN];
   int         num_collectortypes_on_this_host;
   int         CollectortypeIDs[DAF_MAX_NUM_COLLECTOR_TYPES];
   char        Collectornames[DAF_MAX_NUM_COLLECTOR_TYPES][DAF_COLLECTORTYPE_NAME_LEN];
   char        Collectorvalues[DAF_MAX_NUM_COLLECTOR_TYPES][DAF_COLLECTORVALUE_LEN];
} host_collectorvalues_t;

objecttype_t *allocate_and_get_all_objecttype_records(MYSQL  *sql_connection, 
                                                      int    *num_records );

FILE *scenario_console_log_fptr = NULL;

typedef struct object_name_chain {
   char                      name[DAF_OBJECTNAME_LEN];
   struct object_name_chain  *next;
} object_name_chain_t;

int add_to_start_of_object_name_chain(object_name_chain_t **chain, char *objectname) {

   object_name_chain_t *p;

   if ((p = (object_name_chain_t *) malloc(sizeof(object_name_chain_t))) != NULL) {
      safecpy(p->name, objectname, sizeof(p->name));
      p->next = *chain; 
      *chain = p;
   } else {
      return E_NOTOK;
   }

   return E_OK;

}

char *get_next_object_name_from_chain(object_name_chain_t **chain) {

   char *p;

   p = (*chain)->name;
   (*chain) = (*chain)->next;

   return p;

}


typedef struct parent_child_chain {
   char                       name[DAF_OBJECTTYPENAME_LEN];
   struct parent_child_chain  *next;
} parent_child_chain_t;

int add_to_start_of_parent_child_chain(parent_child_chain_t **chain, char *objecttype) {

   parent_child_chain_t *p;

   if ((p = (parent_child_chain_t *) malloc(sizeof(parent_child_chain_t))) != NULL) {
      safecpy(p->name, objecttype, sizeof(p->name));
      p->next = *chain; 
      *chain = p;
   } else {
      return E_NOTOK;
   }

   return E_OK;

}

char *get_next_objecttype_from_chain(parent_child_chain_t **chain) {

   char *p;
   
   p = (*chain)->name;
   (*chain) = (*chain)->next;

   return p;

}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  determine_parent_chain                                                                     */
/*                                                                                                        */
/* PARAMS IN:  sql_connection                                                                             */
/*                                                                                                        */
/*             objecttype                                                                                 */
/*                                                                                                        */
/* PARAMS OUT: num_objecttypes_in_chain                                                                   */
/*                                                                                                        */
/* RETURNS:    a pointer to the chain of parent/child objects for this objectype                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*  eg determine_parent_chain("powersupply") might return ("teststand", "cluster", "node")                */
/*                                                                                                        */
/* Note: the parent_child_chain list is in malloc'd memory, so this must be freed outside this routine    */
/* when the list is no longer being used                                                                  */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

parent_child_chain_t *determine_parent_chain(MYSQL *sql_connection, char *objecttype, int *num_objecttypes_in_chain) {

#undef  SUBNAME
#define SUBNAME "determine_parent_chain"

   objecttype_t *objecttypes;
   int          num_objecttypes;
   int          i,j;
   int          current_row = -1;
   parent_child_chain_t *parent_child_chain = NULL;
   
   *num_objecttypes_in_chain = 0;

   objecttypes = allocate_and_get_all_objecttype_records(sql_connection, &num_objecttypes);
   
   j = 0;
   
   for (i=0; i<num_objecttypes; i++) {
      if (MATCH(objecttypes[i].Name, objecttype)) {
         current_row = i;
      }
   }  

   if (current_row == -1) {
      /* objecttype not found in daf.objcettype table */
      return parent_child_chain;
   } else {
   
      add_to_start_of_parent_child_chain(&parent_child_chain, objecttype);
      (*num_objecttypes_in_chain)++;
      while (objecttypes[current_row].ParentID != 0) {   /* do we need a check to prevent this being infinite loop ? <<<< */
         for (i=0; i<num_objecttypes; i++) {
            if (objecttypes[i].ID == objecttypes[current_row].ParentID) {
               current_row = i;
               (*num_objecttypes_in_chain)++;
               add_to_start_of_parent_child_chain(&parent_child_chain, objecttypes[current_row].Name);
               j++;
               break;
            }
         }
      }      
   }
   
   (*num_objecttypes_in_chain)++;
   add_to_start_of_parent_child_chain(&parent_child_chain, "teststand");
   
   if (objecttypes != NULL) {
     free(objecttypes);
   }

   return parent_child_chain;

}
   
/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  find_objects_in_teststand                                                                  */
/*                                                                                                        */
/* PARAMS IN:  sql_connection                                                                             */
/*                                                                                                        */
/*             teststandID                                                                                */
/*                                                                                                        */
/*             objecttype                                                                                 */
/*                                                                                                        */
/* PARAMS OUT: num_objects                                                                                */
/*                                                                                                        */
/* RETURNS:    a pointer to the chain of names of the the objects in the teststand for this objectype     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/* Note: the name_list is in malloc'd memory, so this must be freed outside this routine                  */
/* when the list is no longer being used                                                                  */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

object_name_chain_t *find_objects_in_teststand(MYSQL *sql_connection, int teststandID, char *objecttype, int *num_objects) { 

#undef  SUBNAME
#define SUBNAME "find_objects_in_teststand"

   MYSQL_RES             *res_set;
   MYSQL_ROW             row;
   parent_child_chain_t  *p, *parent_child_chain = NULL;
   int                   num_objecttypes_in_chain;
   char                  query[1024] = "";
   char                  query1[1024];
   object_name_chain_t   *object_name_chain = NULL;
   int                   i;

   *num_objects = 0;

   parent_child_chain = p = determine_parent_chain(sql_connection, objecttype, &num_objecttypes_in_chain);
   
   if (num_objecttypes_in_chain < 2) {
     /* internal error */
   } else if (num_objecttypes_in_chain == 2) {
   
      snprintf(query, sizeof(query), "SELECT Name FROM daf.%s WHERE parentID = '%u'", objecttype, teststandID);
   
   } else {

      get_next_objecttype_from_chain(&p);
      snprintf(query, sizeof(query), "SELECT ID FROM daf.%s WHERE parentID = '%u'", get_next_objecttype_from_chain(&p), teststandID);
   
      for (i=2; i < (num_objecttypes_in_chain-1); i++) {   
         snprintf(query1, sizeof(query1), "SELECT ID FROM daf.%s WHERE ID IN (%s)", get_next_objecttype_from_chain(&p), query);
         safecpy(query, query1, sizeof(query));
       }
   
      snprintf(query, sizeof(query), "SELECT Name FROM daf.%s WHERE ID IN (%s)", get_next_objecttype_from_chain(&p), query1);
   
   } 

   if (parent_child_chain != NULL) {
      free(parent_child_chain);
   }
   
   if (perform_query(SUBNAME, sql_connection, query) == 0) {
      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {
         while ((row = mysql_fetch_row(res_set)) != NULL) {
            add_to_start_of_object_name_chain(&object_name_chain, row[0]);
            (*num_objects)++;
         }
         mysql_free_result(res_set);
      }
   }
   
   return object_name_chain;  
   
}


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    open_scenario_console_log                                                                */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void open_scenario_console_log(char *Scenariologdirectory, char *Scenariologfilename) {

   char msg[MAX_MSG_LEN];
   char Scenariologpathname[MAX_PATHNAME_LEN];
   
   safecpy(Scenariologpathname, Scenariologdirectory, sizeof(Scenariologpathname));
   safecat(Scenariologpathname, "/", sizeof(Scenariologpathname));
   safecat(Scenariologpathname, Scenariologfilename, sizeof(Scenariologpathname));

   if (scenario_console_log_fptr == NULL) { 

      if ((scenario_console_log_fptr = fopen(Scenariologpathname, "a")) == NULL) {
         sprintf(msg, "Error %d (%s) opening scenario console log filename: %s\n", errno, strerror(errno), Scenariologpathname);
         print_msg_to_console(msg);
      } else {
         // sprintf(msg, "Opening scenario console log filename: %s\n", Scenariologpathname);
         // print_msg_to_console(msg);
      }

   } else {
      sprintf(msg, "Scenario console log filename: %s already open\n", Scenariologpathname);
      print_msg_to_console(msg);
   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    print_scenario_console_log_doublerule                                                    */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/


void print_scenario_console_log_doublerule() {
    
   if (scenario_console_log_fptr != NULL) {
      fprintf(scenario_console_log_fptr, "==============================================================================================\n");  
   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    print_scenario_console_log_rule                                                          */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_scenario_console_log_rule() {
    
   if (scenario_console_log_fptr != NULL) {
      fprintf(scenario_console_log_fptr, "----------------------------------------------------------------------------------------------\n");  
   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    print_scenario_console_log                                                               */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/


void print_scenario_console_log(char *msg) {

  char time_string[16];

  time_msg(time_string, sizeof(time_string));
    
   if (scenario_console_log_fptr != NULL) {
      fprintf(scenario_console_log_fptr, "%s: %s", time_string, msg );
   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    print_scenario_console_log_string                                                        */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_scenario_console_log_string(char *msg) {

   if (scenario_console_log_fptr != NULL) {
      fprintf(scenario_console_log_fptr, "%s", msg);
   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    print_scenario_console_log_blank_timestamp                                               */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_scenario_console_log_blank_timestamp(char *msg) {

   char time_string[16] = "               ";
   if (scenario_console_log_fptr != NULL) {
      fprintf(scenario_console_log_fptr, "%s: %s", time_string, msg);
   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    close_scenario_console_log                                                               */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void close_scenario_console_log() {
    
   // char msg[MAX_MSG_LEN];
   if (scenario_console_log_fptr != NULL) {
      fclose(scenario_console_log_fptr);  
      scenario_console_log_fptr = NULL;
   }
   // sprintf(msg, "Closing scenario console log filename\n");
   // print_msg_to_console(msg);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    determine_status_database_name                                                           */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void determine_status_database_name(char *Project, char *Phase, char *status_database_name, int max_status_database_name_len) {

  safecpy(status_database_name, Project,   max_status_database_name_len);
  safecat(status_database_name, "_",       max_status_database_name_len);
  safecat(status_database_name, Phase,     max_status_database_name_len);
  safecat(status_database_name, "_results", max_status_database_name_len);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    determine_status_table_name                                                              */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void determine_status_table_name(char *status_database_name, char *tablename, char *status_table_name, int max_status_table_name_len) {

   safecpy(status_table_name, status_database_name, max_status_table_name_len);
   safecat(status_table_name, ".",                  max_status_table_name_len);
   safecat(status_table_name, tablename,            max_status_table_name_len);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    make_legal_filename                                                                      */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void make_legal_filename(char *filename) {

   int i;

   for (i=0; i<strlen(filename); i++) {
      if (filename[i] == '/') {
         filename[i] = '_';
      }
   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  lock_active_steps                                                                          */
/*                                                                                                        */
/* PARAMS IN:  active_steps object                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void lock_active_steps(active_steps_object_t *active_steps) {

   char msg[MAX_MSG_LEN];

   if (pthread_mutex_lock(&(active_steps->mp)) != 0) {
      snprintf(msg, sizeof(msg), "problem locking mutex in active_steps, errno = %d - exiting\n", errno);
      exit(1);
   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  unlock_active_steps                                                                        */
/*                                                                                                        */
/* PARAMS IN:  active_steps object                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void unlock_active_steps(active_steps_object_t *active_steps) {

   char msg[MAX_MSG_LEN];

   if (pthread_mutex_unlock(&(active_steps->mp)) != 0) {
      snprintf(msg, sizeof(msg), "problem unlocking mutex in active_steps, errno =  %d - exiting\n", errno);
      exit(1);
   }

}


/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   lookup_collectortypeID                                                                */
/*                                                                                                   */
/* Inputs:     collectortypes      Array containing all the collectorypes that are to be searched    */
/*                                                                                                   */
/*             num_collectortype_records  The number of elements in the collectortypes array         */
/*                                                                                                   */
/*             name                The Name of the collectortype record that we are looking for      */
/*                                                                                                   */
/* Outputs:    ID                  The ID field of the collectortype record whose Name is name       */
/*                                                                                                   */
/* Returns:    TRUE if the record is found, FALSE if not                                             */
/*                                                                                                   */
/* Function:   Searches the collectortypes for a record with a given Name and returns the ID         */
/*             field value in that record.                                                           */
/* ------------------------------------------------------------------------------------------------- */

bool_t lookup_collectortypeID(collectortype_t *collectortypes, 
                              int num_collectortype_records, 
                              char *name, 
                              int *ID) {

  int i;
  bool_t found = FALSE;

  for (i=0; i<num_collectortype_records; i++) {

    if (MATCH((collectortypes+i)->Name, name)) {
      *ID = (collectortypes+i)->ID;
      found = TRUE;
      break;
    }
  }
  
  return found;

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   lookup_actiontype                                                                     */
/*                                                                                                   */
/* Inputs:     actiontypes         Array containing all the actiontypes that are to be searched      */
/*                                                                                                   */
/*             num_action_types    The number of elements in the actiontypes array                   */
/*                                                                                                   */
/*             ID                  The ID of the actiontype record that we are looking for           */
/*                                                                                                   */
/*             max_actiontype_len  The max size of the string (including the terminating \0) that    */
/*                                 may be returned in the actiontype parameter - if the actiontype   */
/*                                 field is longer than this, the value returned will be truncated   */
/*                                                                                                   */
/* Outputs:    actiontype          The type field of the actiontype record whose id is ID            */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Searches the actiontypes for a record with a given ID and returns the Actiontype      */
/*             field value in that record.                                                           */
/* ------------------------------------------------------------------------------------------------- */

void lookup_actiontype(actiontype_t *actiontypes, 
                       int num_actiontype_records, 
                       int ID, 
                       char *actiontype, 
                       int max_actiontype_len) {

  int i;

  safecpy(actiontype, "not found", max_actiontype_len); 

  for (i=0; i<num_actiontype_records; i++) {
    if ((actiontypes+i)->ID == ID) {
      safecpy(actiontype, (actiontypes+i)->Type, max_actiontype_len);
      break;
    }
  }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   lookup_testcase                                                                       */
/*                                                                                                   */
/* Inputs:     testcases           Array containing all the testcases that are to be searched        */
/*                                                                                                   */
/*             num_testcase_records  The number of elements in the testcases array                   */
/*                                                                                                   */
/*             ID                  The ID of the testcase record that we are looking for             */
/*                                                                                                   */
/*             max_testcase_len    The max size of the string (including the terminating \0) that    */
/*                                 may be returned in the testcase parameter - if the testcase       */
/*                                 field is longer than this, the value returned will be truncated   */
/*                                                                                                   */
/* Outputs:    testcase            The name field of the testcase record whose id is ID              */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Searches the testcases table for a record with a given ID and returns the Name        */
/*             field value in that record.                                                           */
/* ------------------------------------------------------------------------------------------------- */

void lookup_testcase(testcase_t *testcases, 
                     int num_testcase_records, 
                     int ID, 
                     char *testcase,
                     int max_testcase_len) {

  int i;

  safecpy(testcase, "not found", max_testcase_len); 

  for (i=0; i<num_testcase_records; i++) {
    if ((testcases+i)->ID == ID) {
      safecpy(testcase, (testcases+i)->Name, max_testcase_len);
      break;
    }
  }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_name_from_ID                                                                      */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             tablename             A DAF table name such as "scenario"                             */
/*                                                                                                   */
/*             ID                    the ID of the record whose name is to be looked up              */
/*                                                                                                   */
/*             max_scenario_name_length    the max number of chars (including the terminating 0)     */
/*                                   allowed in the string scenario_name                             */
/*                                                                                                   */
/* Outputs:    scenario_name         the name of the scenario with the specified ID, or an empty     */
/*                                   string if there is no scenario with the given ID                */
/*                                                                                                   */
/* Returns:    0 if successful, 1 if an error occurred                                               */
/*                                                                                                   */
/* Function:   Looks up the row in the specified table with the specified ID and determines          */
/*             the value of the Name field in that row                                               */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

int get_name_from_ID(MYSQL *sql_connection, 
                     char     *tablename,
                     int      ID,
                     char     *scenario_name,
                     int      max_scenario_name_length) {

#undef  SUBNAME
#define SUBNAME "get_name_from_ID"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char query[256];
   int rc = 1;

   safecpy(scenario_name, "", max_scenario_name_length); 
   snprintf(query, sizeof(query), "SELECT Name FROM %s where ID='%u'", tablename, ID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {

            safecpy(scenario_name, row[0], max_scenario_name_length); 
            rc = 0;

         }
     
         mysql_free_result(res_set);

      }

   }

   return(rc);

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_all_actiontype_records                                                            */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             actiontype            Pointer to the first element in an array that will hold         */
/*                                   the records read from the actiontype table                      */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   actiontype array                                                */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   actiontype array                                                */
/*                                                                                                   */
/* Outputs:    actiontype            see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the actiontype table from the MYSQL database and puts its contents in an array. */
/*             The array must be big enough to hold the table, if not, then only the first part of   */
/*             of the table will be read into the array                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_all_actiontype_records(MYSQL         *sql_connection, 
                                actiontype_t  *actiontype, 
                                int           max_num_records,
                                int           *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_all_actiontype_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char *query =  "SELECT " ACTIONTYPE_FIELDS " FROM daf.actiontype";

   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) && (*num_records < max_num_records)) {

            get_actiontype_record(res_set, &row, actiontype);
            (*num_records)++;
            actiontype++;

         }
     
         mysql_free_result(res_set);

      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   allocate_and_get_all_actiontype_records                                               */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   workqueue array                                                 */
/*                                                                                                   */
/* Outputs:    actiontype            see above                                                       */
/*                                                                                                   */
/* Returns:    actiontype_t*         Pointer to the first element in the memory area that holds the  */
/*                                   actiontype array                                                */
/*                                                                                                   */
/* Function:   Reads the actiontype table from the MYSQL database and mallocs a sufficiently large   */
/*             memory area to hold the table - the memory must be free()d after use outside this     */
/*             routine.                                                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

actiontype_t *allocate_and_get_all_actiontype_records(MYSQL  *sql_connection, 
                                                     int    *num_records ) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_all_actiontype_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   actiontype_t *p, *q = NULL;
   int num_rows = 0;
   char *query = "SELECT " ACTIONTYPE_FIELDS " FROM daf.actiontype";

   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) { 

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(actiontype_t) );

         while (((row = mysql_fetch_row(res_set)) != NULL) && ((*num_records) < num_rows)) {

            get_actiontype_record(res_set, &row, q);
            (*num_records)++;
            q++;
         }
     
         mysql_free_result(res_set);

      }

   }

   return(p);

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_all_testcase_records                                                              */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             testcase              Pointer to the first element in an array that will hold         */
/*                                   the records read from the testcase table                        */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   testcase array                                                  */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   testcase array                                                  */
/*                                                                                                   */
/* Outputs:    testcase              see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the testcase table from the MYSQL database and puts its contents in an array.   */
/*             The array must be big enough to hold the table, if not, then only the first part of   */
/*             of the table will be read into the array                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_all_testcase_records(MYSQL       *sql_connection, 
                              testcase_t  *testcase,
                              int          max_num_records,
                              int          *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_all_testcase_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char *query = "SELECT " TESTCASE_FIELDS " FROM daf.testcase";

   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) && (*num_records < max_num_records)) {

            get_testcase_record(res_set, &row, testcase);
            (*num_records)++;
            testcase++;

         }

         mysql_free_result(res_set);
     
      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   allocate_and_get_host_records                                                         */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             where                 A where clause that selects the desired hosts                   */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   workqueue array                                                 */
/*                                                                                                   */
/* Outputs:    testcase              see above                                                       */
/*                                                                                                   */
/* Returns:    host_t*               Pointer to the first element in the memory area that holds the  */
/*                                   host array                                                      */
/*                                                                                                   */
/* Function:   Reads the host table from the MYSQL database and mallocs a sufficiently large         */
/*             memory area to hold the selected hosts - this memory must be free()d after use        */
/*             outside this routine.                                                                 */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

host_t *allocate_and_get_host_records(MYSQL  *sql_connection, 
                                      char   *where,
                                      int    *num_records ) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_all_host_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   host_t *p, *q = NULL;
   int num_rows = 0;
   char *fmt = "SELECT " HOST_FIELDS " FROM daf.host WHERE %s";
   char query[1024];

   snprintf(query, sizeof(query), fmt, where);
   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) { 

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(host_t) );

         while (((row = mysql_fetch_row(res_set)) != NULL) && ((*num_records) < num_rows)) {

            get_host_record(res_set, &row, q);
            (*num_records)++;
            q++;
         }
     
         mysql_free_result(res_set);

      }

   }

   return(p);

}

void print_hosts_in_use(hosts_in_use_t *h, int num_records) {

  int i;
  char msg[MAX_MSG_LEN];

  snprintf(msg, sizeof(msg), "Hosts in use = %d\n", num_records);
  print_scenario_console_log(msg);
  for (i=0; i<num_records; i++) {
     snprintf(msg, sizeof(msg), "Hostname=%s, Num_actions_allocated_in_this_scenario_step=%u\n",
              h[i].Hostname, h[i].Num_actions_allocated_in_this_scenario_step);
     print_scenario_console_log(msg);
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:    allocate_and_get_host_in_use_records_in_this_scenario_step                           */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             ScenarioresultID                                                                      */
/*                                                                                                   */
/*             Stepnumber                                                                            */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                                                                                   */
/*                                                                                                   */
/* Outputs:    nothing                                                                               */
/*                                                                                                   */
/* Returns:    hosts_in_use_t*       Pointer to the first element in the memory area that holds the  */
/*                                   hosts_in_use array                                              */
/*                                                                                                   */
/* Function:   Reads a table in malloc'd memory that contains an array describing the hosts that     */
/*             currently have actions allocated to them in a particular Scenario step and how many   */
/*             actions are allocated to each host.   The memory arrea must be freed()d after use     */ 
/*             outside this routine.                                                                 */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

hosts_in_use_t * allocate_and_get_host_in_use_records_in_this_scenario_step(MYSQL *sql_connection, 
                                                                           int    ScenarioresultID,
                                                                           int    Stepnumber,    
                                                                           int    *num_records ) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_host_in_use_records_in_this_scenario_step"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   hosts_in_use_t *p, *q = NULL;
   int num_rows = 0;
   int i;
   char *fmt = "SELECT Hostname, Count(Hostname) FROM daf.workqueue WHERE ScenarioresultID = '%u' AND " 
               "Stepnumber = '%u' AND Hostname != 'NOHOSTSELECTED' GROUP BY Hostname";
   char query[1024];
   char msg[MAX_MSG_LEN];

/*   char *fmt1 = "SELECT * FROM daf.workqueue WHERE ScenarioresultID = '%u' AND " 

               "Stepnumber = '%u' AND Hostname != 'NOHOSTSELECTED' GROUP BY Hostname";

   snprintf(query, sizeof(query), fmt1, ScenarioresultID, Stepnumber);

   if (perform_query(SUBNAME, sql_connection, query) == 0) { 
      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {
         num_rows = mysql_num_rows(res_set);
         while ((row = mysql_fetch_row(res_set)) != NULL) {
            for (i = 0; i < mysql_num_fields(res_set); i++) {
               print_scenario_console_log_string(row[i]);
               print_scenario_console_log_string(",");
            }
            print_scenario_console_log_string("\n");
         }
      }          
   } */

   snprintf(query, sizeof(query), fmt, ScenarioresultID, Stepnumber);
   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) { 

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(hosts_in_use_t) );

         while ((row = mysql_fetch_row(res_set)) != NULL) {

            for (i = 0; i < mysql_num_fields(res_set); i++) {
            switch(i) {
               case 0:
                  safecpy(q->Hostname, row[i], sizeof(q->Hostname)); 
                  break;
               case 1:
                  q->Num_actions_allocated_in_this_scenario_step = (Iu32) strtoull(row[i], NULL, 0); 
                  break;
               default:
                  /* problem */
                  snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n",
                           SUBNAME, i);
                  print_msg_to_console(msg);
                  break;
               }
            }

            (*num_records)++;
            q++;
         }
     
         mysql_free_result(res_set);

      }

   }

   return(p);

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   allocate_and_get_all_testcase_records                                                 */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   workqueue array                                                 */
/*                                                                                                   */
/* Outputs:    testcase              see above                                                       */
/*                                                                                                   */
/* Returns:    testcase_t*           Pointer to the first element in the memory area that holds the  */
/*                                   testcase array                                                  */
/*                                                                                                   */
/* Function:   Reads the testcase table from the MYSQL database and mallocs a sufficiently large     */
/*             memory area to hold the table - the memory must be free()d after use outside this     */
/*             routine.                                                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

testcase_t *allocate_and_get_all_testcase_records(MYSQL  *sql_connection, 
                                                  int    *num_records ) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_all_testcase_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   testcase_t *p, *q = NULL;
   int num_rows = 0;
   char *query = "SELECT " TESTCASE_FIELDS " FROM daf.testcase";

   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) { 

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(testcase_t) );

         while (((row = mysql_fetch_row(res_set)) != NULL) && ((*num_records) < num_rows)) {

            get_testcase_record(res_set, &row, q);
            (*num_records)++;
            q++;
         }
     
         mysql_free_result(res_set);

      }

   }

   return(p);

}


/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_all_workqueue_records                                                             */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             workqueue             Pointer to the first element in an array that will hold         */
/*                                   the records read from the workqueue table                       */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   workqueue array                                                 */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   workqueue array                                                 */
/*                                                                                                   */
/* Outputs:    workqueue             see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the workqueue table from the MYSQL database and puts its contents in an array.  */
/*             The array must be big enough to hold the table, if not, then only the first part of   */
/*             of the table will be read into the array                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_all_workqueue_records(MYSQL         *sql_connection, 
                               workqueue_t   *workqueue, 
                               int           max_num_records,
                               int           *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_all_workqueue_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char *query = "SELECT " WORKQUEUE_FIELDS " FROM daf.workqueue";

   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) { 

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) && (*num_records < max_num_records)) {

            get_workqueue_record(res_set, &row, workqueue);
            (*num_records)++;
            workqueue++;

         }
     
         mysql_free_result(res_set);

      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   allocate_and_get_all_workqueue_records                                                */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   workqueue array                                                 */
/*                                                                                                   */
/* Outputs:    workqueue             see above                                                       */
/*                                                                                                   */
/* Returns:    workqueue_t*          Pointer to the first element in the memory area that holds the  */
/*                                   workqueue array                                                 */
/*                                                                                                   */
/* Function:   Reads the workqueue table from the MYSQL database and mallocs a sufficiently large .  */
/*             memory area to hold the table - the memory must be free()d after use outside this     */
/*             routine.                                                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

workqueue_t *allocate_and_get_all_workqueue_records(MYSQL  *sql_connection, 
                                                    int    *num_records ) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_all_workqueue_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   workqueue_t *p, *q = NULL;
   int num_rows = 0;
   char *query = "SELECT " WORKQUEUE_FIELDS " FROM daf.workqueue";

   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) { 

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(workqueue_t) );

         while (((row = mysql_fetch_row(res_set)) != NULL) && ((*num_records) < num_rows)) {

            get_workqueue_record(res_set, &row, q);
            (*num_records)++;
            q++;
         }
     
         mysql_free_result(res_set);

      }

   }

   return(p);

}


/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_collectortype_records                                                             */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             collectortype          Pointer to the first element in an array that will hold        */
/*                                   the selection of records that are read from the collectortype   */
/*                                   table                                                           */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   collectortype array                                             */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23".  If no where clause  */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/* Outputs:    collectortype         see above                                                       */
/*                                                                                                   */
/*             num_built_in_types                                                                    */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   collectortype array                                             */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the collectortype table from the MYSQL database and puts its contents in an    */
/*             array. The array must be big enough to hold the table, if not, then only the first    */
/*             part of the table will be read into the array                                         */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_collectortype_records(MYSQL            *sql_connection, 
                               collectortype_t  *builtin_collectortype, 
                               int              max_num_builtin_records,
                               collectortype_t  *user_collectortype, 
                               int              max_num_user_records,
                               char             *where,
                               int              *num_builtin_types,
                               int              *num_user_types,
                               int              *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_collectortype_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char       query[256];
   collectortype_t  collectortype;

   *num_records = 0;
   *num_builtin_types = 0;
   *num_user_types = 0;

   char msg[MAX_MSG_LEN];
  
   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " COLLECTORTYPE_FIELDS " FROM daf.collectortype");

   } else {

      snprintf(query, sizeof(query), "SELECT " COLLECTORTYPE_FIELDS " FROM daf.collectortype where %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while ((row = mysql_fetch_row(res_set)) != NULL) {

            get_collectortype_record(res_set, &row, &collectortype);
 
            if ((collectortype.Builtin == 1) && (*num_builtin_types < max_num_builtin_records)) {

               builtin_collectortype->ID = collectortype.ID;
               safecpy(builtin_collectortype->Name, collectortype.Name, sizeof(builtin_collectortype->Name));
               builtin_collectortype->Builtin = collectortype.Builtin;
               safecpy(builtin_collectortype->OStype, collectortype.OStype, sizeof(builtin_collectortype->OStype));
               safecpy(builtin_collectortype->Invocation, collectortype.Invocation, sizeof(builtin_collectortype->Invocation));
               (*num_builtin_types)++;
               builtin_collectortype++;
            
            } else if ((collectortype.Builtin == 0) && (*num_user_types < max_num_user_records)) {
  
               user_collectortype->ID = collectortype.ID;
               safecpy(user_collectortype->Name, collectortype.Name, sizeof(user_collectortype->Name));
               user_collectortype->Builtin = collectortype.Builtin;
               safecpy(user_collectortype->OStype, collectortype.OStype, sizeof(user_collectortype->OStype));
               safecpy(user_collectortype->Invocation, collectortype.Invocation, sizeof(user_collectortype->Invocation));
               (*num_user_types)++;
               user_collectortype++;
            
            } else {

               /* problem */
               snprintf(msg, sizeof(msg), "%s: internal error - not enough space to store collector records\n", SUBNAME);
               print_msg_to_console(msg);
               snprintf(msg, sizeof(msg), "%s: user_collectortype->ID=%d, num_builtin_types=%d, max_num_builtin_records=%d, "
                                          "num_user_types=%d, max_num_user_records=%d\n",
                                          SUBNAME, user_collectortype->ID, *num_builtin_types, max_num_builtin_records,
                                          *num_user_types, max_num_user_records);
               print_msg_to_console(msg);

            }

            (*num_records)++;

         }
     
         mysql_free_result(res_set);

      }

   }

}


/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_collectorvalue_records                                                            */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             collectorvalue        Pointer to the first element in an array that will hold         */
/*                                   the selection of records that are read from the collectorvalue  */
/*                                   table                                                           */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   collectorvalue array                                            */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23".  If no where clause  */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   collectorvalue array                                            */
/*                                                                                                   */
/* Outputs:    workqueue             see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the collectorvalue table from the MYSQL database and puts its contents in an    */
/*             array. The array must be big enough to hold the table, if not, then only the first    */
/*             part of the table will be read into the array                                         */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_collectorvalue_records(MYSQL             *sql_connection, 
                                collectorvalue_t  *collectorvalue, 
                                int               max_num_records,
                                char              *where,
                                int               *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_collectorvalue_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char       query[256];

   *num_records = 0;
  
   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " COLLECTORVALUE_FIELDS " FROM daf.collectorvalue");

   } else {

      snprintf(query, sizeof(query), "SELECT " COLLECTORVALUE_FIELDS " FROM daf.collectorvalue where %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) &&
                 (*num_records < max_num_records)) {

            get_collectorvalue_record(res_set, &row, collectorvalue);
            (*num_records)++;
            collectorvalue++;

         }
     
         mysql_free_result(res_set);

      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_environmentvalue_records                                                          */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             environmentvalue      Pointer to the first element in an array that will hold         */
/*                                   the selection of records that are read from the                 */
/*                                   environmentvalue table                                          */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   environmentvalue array                                          */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23".  If no where clause  */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   environmentvalue array                                          */
/*                                                                                                   */
/* Outputs:    environmentvalue      see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the environmentvaluei tabl from the MYSQL database and puts its contents in an  */
/*             array.  The array must be big enough to hold the table, if not, then only the first   */
/*             part of the table will be read into the array                                         */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_environmentvalue_records(MYSQL               *sql_connection, 
                                  environmentvalue_t  *environmentvalue, 
                                  int                 max_num_records,
                                  char                *where,
                                  int                 *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_environmentvalue_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char       query[256];

   *num_records = 0;
  
   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " ENVIRONMENTVALUE_FIELDS " FROM daf.environmentvalue");

   } else {

      snprintf(query, sizeof(query), "SELECT " ENVIRONMENTVALUE_FIELDS " FROM daf.environmentvalue where %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) &&
                 (*num_records < max_num_records)) {

            get_environmentvalue_record(res_set, &row, environmentvalue);
            (*num_records)++;
            environmentvalue++;

         }
     
         mysql_free_result(res_set);

      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_environment_assignments                                                           */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             environment_assignment    Pointer to the first element in an array that will hold     */
/*                                   the selection of records that are read from the                 */
/*                                   environmentvalue table expressed as Name=Value strings          */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   assigment                                                       */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23".  If no where clause  */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   assignment array                                                */
/*                                                                                                   */
/* Outputs:    environment_assignment        see above                                               */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the environmentvalue table from the MYSQL database and puts its contents in an  */
/*             array, where each element is a string of the form "name=value".  The array must be    */
/*             must be big enough to hold the table, if not, then only the first                     */
/*             part of the table will be read into the array                                         */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_environmentvalue_assignments(MYSQL          *sql_connection, 
                                      environment_assignment_t  *environment_assignment,
                                      int            max_num_records,
                                      char           *where,
                                      int            *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_environment_assignments"

   MYSQL_RES           *res_set;
   MYSQL_ROW           row;
   char                query[256];
   environmentvalue_t  environmentvalue;

   *num_records = 0;
  
   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " ENVIRONMENTVALUE_FIELDS " FROM daf.environmentvalue");

   } else {

      snprintf(query, sizeof(query), "SELECT " ENVIRONMENTVALUE_FIELDS " FROM daf.environmentvalue where %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) &&
                 (*num_records < max_num_records)) {

            get_environmentvalue_record(res_set, &row, &environmentvalue);
            safecpy(environment_assignment->Value, environmentvalue.Name,  sizeof(environment_assignment->Value));
            safecat(environment_assignment->Value, "=",                    sizeof(environment_assignment->Value));
            safecat(environment_assignment->Value, environmentvalue.Value, sizeof(environment_assignment->Value));
            (*num_records)++;
            environment_assignment++;

         }
     
         mysql_free_result(res_set);

      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_action_records                                                                    */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             action                Pointer to the first element in an array that will hold         */
/*                                   the selection of records that are read from the action table    */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   action array                                                    */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23".  If no where clause  */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   action array                                                    */
/*                                                                                                   */
/* Outputs:    workqueue             see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the workqueue table from the MYSQL database and puts its contents in an array.  */
/*             The array must be big enough to hold the table, if not, then only the first part of   */
/*             of the table will be read into the array                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_action_records(MYSQL     *sql_connection, 
                        action_t  *action, 
                        int       max_num_records,
                        char      *where,
                        int       *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_action_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;

   char       query[256];
   *num_records = 0;
  
   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " ACTION_FIELDS " FROM daf.action");

   } else {

      snprintf(query, sizeof(query), "SELECT " ACTION_FIELDS " FROM daf.action where %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) &&
                 (*num_records < max_num_records)) {

            get_action_record(res_set, &row, action);
            (*num_records)++;
            action++;

         }
     
         mysql_free_result(res_set);

      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   allocate_and_get_action_records                                                       */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23".  If no where clause  */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   action array                                                    */
/*                                                                                                   */
/* Outputs:    workqueue             see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the workqueue table from the MYSQL database and puts its contents in an area of */
/*             malloc'd memory - this area must be free'd after use outside this routine             */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

action_t *allocate_and_get_action_records(MYSQL     *sql_connection, 
                                          char      *where,
                                          int       *num_records ) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_action_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char       query[256];
   action_t   *p, *q = NULL;
   int        num_rows;

   *num_records = 0;
  
   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " ACTION_FIELDS " FROM daf.action");

   } else {

      snprintf(query, sizeof(query), "SELECT " ACTION_FIELDS " FROM daf.action where %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(action_t) );

         while (((row = mysql_fetch_row(res_set)) != NULL) &&
                 ((*num_records) < num_rows)) {

            get_action_record(res_set, &row, q);
            (*num_records)++;
            q++;

         }
     
         mysql_free_result(res_set);

      }

   }

   return(p);

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_host_records                                                                      */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             host                  Pointer to the first element in an array that will hold         */
/*                                   the selection of records that are read from the host table      */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   action array                                                    */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23". If no where clause   */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   host array                                                      */
/*                                                                                                   */
/* Outputs:    host                  see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the host table from the MYSQL database and puts its contents in an array.       */
/*             The array must be big enough to hold the table, if not, then only the first part of   */
/*             of the table will be read into the array                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_host_records(MYSQL     *sql_connection, 
                      host_t    *host, 
                      int       max_num_records,
                      char      *where,
                      int       *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_host_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;

   char       query[256];
   *num_records = 0;

   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " HOST_FIELDS " FROM daf.host");

   } else {

      snprintf(query, sizeof(query), "SELECT " HOST_FIELDS " FROM daf.host WHERE %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) &&
                 (*num_records < max_num_records)) {

            get_host_record(res_set, &row, host);
            (*num_records)++;
            host++;

         }
     
         mysql_free_result(res_set);

      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_collectorset_records                                                              */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             collectorset          Pointer to the first element in an array that will hold         */
/*                                   the selection of records that are read from the                 */
/*                                   collectorset table                                              */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   collectorset array                                              */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23". If no where clause   */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   collectorset array                                              */
/*                                                                                                   */
/* Outputs:    host                  see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the collectorset table from the MYSQL database and puts its contents in         */
/*             an array.  The array must be big enough to hold the table, if not, then only the      */
/*             first part of the table will be read into the array                                   */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_collectorset_records(MYSQL           *sql_connection, 
                              collectorset_t  *collectorset, 
                              int             max_num_records,
                              char            *where,
                              int             *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_collectorset_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;

   char       query[256];
   *num_records = 0;

   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " COLLECTORSET_FIELDS " FROM daf.collectorset");

   } else {

      snprintf(query, sizeof(query), "SELECT " COLLECTORSET_FIELDS " FROM daf.collectorset where %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) &&
                 (*num_records < max_num_records)) {

            get_collectorset_record(res_set, &row, collectorset);
            (*num_records)++;
            collectorset++;

         }
     
         mysql_free_result(res_set);
      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_collectorsetmember_records                                                        */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/*             collectorsetmember    Pointer to the first element in an array that will hold         */
/*                                   the selection of records that are read from the                 */
/*                                   collectorsetmember table                                        */
/*                                                                                                   */
/*             max_num_records       the maximum number of records that may be returned in the       */
/*                                   collectorsetmember array                                        */
/*                                                                                                   */
/*             where                 a MYSQL where clause that selects which particular records      */
/*                                   are to be read from the table, eg "ID=23". If no where clause   */
/*                                   is required, set where to the NULL pointer                      */
/*                                                                                                   */
/*             num_records           the number of records that were actually returned in the        */
/*                                   collectorsetmember array                                        */
/*                                                                                                   */
/* Outputs:    host                  see above                                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Reads the collectorsetmember table from the MYSQL database and puts its contents in   */
/*             an array.  The array must be big enough to hold the table, if not, then only the      */
/*             first part of the table will be read into the array                                   */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_collectorsetmembers_records(MYSQL                 *sql_connection, 
                                     collectorsetmember_t  *collectorsetmember, 
                                     int                   max_num_records,
                                     char                  *where,
                                     int                   *num_records ) {

#undef  SUBNAME
#define SUBNAME "get_collectorsetmembers_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;

   char       query[256];
   *num_records = 0;

   if (where == NULL) {
  
      snprintf(query, sizeof(query), "SELECT " COLLECTORSETMEMBER_FIELDS " FROM daf.collectorsetmember");

   } else {

      snprintf(query, sizeof(query), "SELECT " COLLECTORSETMEMBER_FIELDS " FROM daf.collectorsetmember where %s", where);

   }

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) &&
                 (*num_records < max_num_records)) {

            get_collectorsetmember_record(res_set, &row, collectorsetmember);
            (*num_records)++;
            collectorsetmember++;

         }
     
         mysql_free_result(res_set);

      }

   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_testcase_record                                                                   */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the testcase table.                                          */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the testcase    */
/*                                   table - this structure is filled in with values taken from      */
/*                                   row parameter.                                                  */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the testcase table and places it in a structure.     */
/*             The query that set up the MYSQL row structure must have been such that the            */
/*             row is populated in the order specified by the TESTCASE_FIELDS definition in the      */
/*             af_defns.h file                                                                       */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_testcase_record(MYSQL_RES   *res_set,
                         MYSQL_ROW   *row,
                         testcase_t  *testcase) {

#undef  SUBNAME
#define SUBNAME "get_testcase_record"

   int          i;
   char         msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {
      switch(i) {
         case 0:
            testcase->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(testcase->Name, (*row)[i], sizeof(testcase->Name)); break; 
         case 2:
            safecpy(testcase->Description, (*row)[i], sizeof(testcase->Description)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_actiontype_record                                                                 */
/*                                                                                                   */
/*             Same as get_testcase_record, but for the actiontype field                             */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_actiontype_record(MYSQL_RES     *res_set,
                           MYSQL_ROW     *row,
                           actiontype_t  *actiontype) {

#undef  SUBNAME
#define SUBNAME "get_actiontype_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {
      switch(i) {
         case 0:
            actiontype->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(actiontype->Type, (*row)[i], sizeof(actiontype->Type)); break; 
         case 2:
            safecpy(actiontype->Description, (*row)[i], sizeof(actiontype->Description)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_action_record                                                                     */
/*                                                                                                   */
/*             Same as get_testase_record, but for the action field                                  */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_action_record(MYSQL_RES *res_set,
                       MYSQL_ROW *row,
                       action_t  *action) {

#undef  SUBNAME
#define SUBNAME "get_action_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {
      switch(i) {
         case 0:
            action->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            action->ScenarioID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 2:
            action->ActiontypeID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 3:
            safecpy(action->Description, (*row)[i], sizeof(action->Description)); break; 
         case 4:
            action->Stepnumber = strtoull((*row)[i], NULL, 0); break;
         case 5:
            safecpy(action->Hostselectortype, (*row)[i], sizeof(action->Hostselectortype)); break;
         case 6:
            safecpy(action->Hostselectorvalue, (*row)[i], sizeof(action->Hostselectorvalue)); break;
         case 7:
            action->CollectorsetID = strtoull((*row)[i], NULL, 0); break;
         case 8:
            action->TestcaseID = strtoull((*row)[i], NULL, 0); break;
         case 9: 
            safecpy(action->Parameters, (*row)[i], sizeof(action->Parameters)); break;
         case 10:
            action->EnvironmentID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 11:
            safecpy(action->Duration, (*row)[i], sizeof(action->Duration)); break;
         case 12:
            action->Maxduration = (Iu32) strtoull((*row)[i], NULL, 0); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_actionresult_record                                                               */
/*                                                                                                   */
/*             Same as get_testase_record, but for the action field                                  */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_actionresult_record(MYSQL_RES       *res_set,
                             MYSQL_ROW       *row,
                             actionresult_t  *actionresult) {

#undef  SUBNAME
#define SUBNAME "get_actionresult_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {
      switch(i) {
         case 0:
            actionresult->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            actionresult->ScenarioresultID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 2:
            safecpy(actionresult->Actiontype, (*row)[i], sizeof(actionresult->Actiontype)); break;
         case 3:
            actionresult->Stepnumber = strtoull((*row)[i], NULL, 0); break;
         case 4:
            safecpy(actionresult->Status, (*row)[i], sizeof(actionresult->Status)); break;
         case 5:
            safecpy(actionresult->Hostname, (*row)[i], sizeof(actionresult->Hostname)); break; 
         case 6:
            safecpy(actionresult->Testcase, (*row)[i], sizeof(actionresult->Testcase)); break; 
         case 7:
            safecpy(actionresult->Invocation, (*row)[i], sizeof(actionresult->Invocation)); break;
         case 8:
            actionresult->Pass = strtoull((*row)[i], NULL, 0); break;
         case 9:
            safecpy(actionresult->Start, (*row)[i], sizeof(actionresult->Start)); break;
         case 10:
            safecpy(actionresult->End, (*row)[i], sizeof(actionresult->End)); break;
         case 11:
            safecpy(actionresult->Comments, (*row)[i], sizeof(actionresult->Comments)); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_workqueue_record                                                                  */
/*                                                                                                   */
/*             Same as get_testase_record, but for the action field                                  */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

#undef  SUBNAME
#define SUBNAME "get_workqueue_record"

void get_workqueue_record(MYSQL_RES    *res_set,
                          MYSQL_ROW    *row,
                          workqueue_t  *workqueue) {

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {
      switch(i) {
         case 0:
            workqueue->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(workqueue->Project, (*row)[i], sizeof(workqueue->Project)); break;
         case 2:
            safecpy(workqueue->Phase, (*row)[i], sizeof(workqueue->Phase)); break;
         case 3:
            workqueue->ScenarioID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 4:
            workqueue->TestlevelID = (Iu32) strtoull((*row)[i], NULL, 0); break;   /* what if this field is NULL etc <<<<<<<<<< */
         case 5:
            workqueue->TeststandID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 6:
            workqueue->ScenarioresultID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 7:
            workqueue->ActionresultID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 8:
            workqueue->EnvironmentID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 9:
            workqueue->OutcomeactionID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 10:
            workqueue->TesterID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 11:
            workqueue->MaillistID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 12:
            safecpy(workqueue->Actiontype, (*row)[i], sizeof(workqueue->Actiontype)); break;
         case 13:
            workqueue->Stepnumber = strtoull((*row)[i], NULL, 0); break;
         case 14:
            safecpy(workqueue->Hostname, (*row)[i], sizeof(workqueue->Hostname)); break;
         case 15:
            safecpy(workqueue->State, (*row)[i],sizeof(workqueue->State)); break;
         case 16:
            safecpy(workqueue->Statemodifier, (*row)[i],sizeof(workqueue->Statemodifier)); break;
         case 17:
            safecpy(workqueue->Testcase, (*row)[i], sizeof(workqueue->Testcase)); break; 
         case 18:
            safecpy(workqueue->Invocation, (*row)[i], sizeof(workqueue->Invocation)); break;
         case 19:
            safecpy(workqueue->Duration, (*row)[i], sizeof(workqueue->Duration)); break;
         case 20:
            workqueue->Maxduration = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 21:
            workqueue->Pass = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 22:
            safecpy(workqueue->Start, (*row)[i], sizeof(workqueue->Start)); break;
         case 23:
            safecpy(workqueue->End, (*row)[i], sizeof(workqueue->End)); break;
         case 24:
            workqueue->Tag = strtoull((*row)[i], NULL, 0); break;
         case 25:
            safecpy(workqueue->Loglocation, (*row)[i], sizeof(workqueue->Loglocation)); break;
         case 26:
            safecpy(workqueue->Scenariologfilename, (*row)[i], sizeof(workqueue->Scenariologfilename)); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_workqrequest_record                                                               */
/*                                                                                                   */
/*             Same as get_testase_record, but for the action field                                  */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_workrequest_record(MYSQL_RES      *res_set,
                            MYSQL_ROW      *row,
                            workrequest_t  *workrequest) {

#undef  SUBNAME
#define SUBNAME "get_workqueuerequest_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {
      switch(i) {
         case 0:
            workrequest->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(workrequest->Project, (*row)[i], sizeof(workrequest->Project)); break;
         case 2:
            safecpy(workrequest->Phase, (*row)[i], sizeof(workrequest->Phase)); break;
         case 3:
            safecpy(workrequest->RequestType, (*row)[i], sizeof(workrequest->RequestType)); break;
         case 4:
            workrequest->TesterID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 5:
            safecpy(workrequest->State, (*row)[i], sizeof(workrequest->State)); break;
         case 6:
            safecpy(workrequest->Tablename1, (*row)[i], sizeof(workrequest->Tablename1)); break;
         case 7:
            workrequest->ObjectID1 = strtoull((*row)[i], NULL, 0); break;
         case 8:
            safecpy(workrequest->Tablename2, (*row)[i], sizeof(workrequest->Tablename2)); break;
         case 9: 
            workrequest->ObjectID2 = strtoull((*row)[i], NULL, 0); break;
         case 10:
            safecpy(workrequest->Tablename3, (*row)[i], sizeof(workrequest->Tablename3)); break;
         case 11:
            workrequest->ObjectID3 = strtoull((*row)[i], NULL, 0); break;
         case 12:
            safecpy(workrequest->Tablename4, (*row)[i], sizeof(workrequest->Tablename4)); break;
         case 13:
            workrequest->ObjectID4 = strtoull((*row)[i], NULL, 0); break;
         case 14:
            safecpy(workrequest->Tablename5, (*row)[i], sizeof(workrequest->Tablename5)); break;
         case 15:
            workrequest->ObjectID5 = strtoull((*row)[i], NULL, 0); break;
         case 16:
            safecpy(workrequest->Logdirectory, (*row)[i], sizeof(workrequest->Logdirectory)); break;
         case 17:
            safecpy(workrequest->Scenariologfilename, (*row)[i], sizeof(workrequest->Scenariologfilename)); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }    
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_scenario_record                                                                   */
/*                                                                                                   */
/*             Same as get_testase_record, but for the action field                                  */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_scenario_record(MYSQL_RES      *res_set,
                         MYSQL_ROW      *row,
                         scenario_t     *scenario) {

#undef  SUBNAME
#define SUBNAME "get_scenario_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {
      switch(i) {
         case 0:
            scenario->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(scenario->Name, (*row)[i], sizeof(scenario->Name)); break;
         case 2:
            safecpy(scenario->Author, (*row)[i], sizeof(scenario->Author)); break;
         case 3:
            scenario->OutcomeactionID = strtoull((*row)[i], NULL, 0); break;
         case 4:
            safecpy(scenario->Createdon, (*row)[i], sizeof(scenario->Createdon)); break;
         case 5:
            safecpy(scenario->Lastchanged, (*row)[i], sizeof(scenario->Lastchanged)); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }    
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_scenarioresult_record                                                             */
/*                                                                                                   */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_scenarioresult_record(MYSQL_RES            *res_set,
                               MYSQL_ROW            *row,
                               scenarioresult_t     *scenarioresult) {

#undef  SUBNAME
#define SUBNAME "get_scenarioresult_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {
      switch(i) {
         case 0:
            scenarioresult->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(scenarioresult->Project, (*row)[i], sizeof(scenarioresult->Project)); break;
         case 2:
            safecpy(scenarioresult->Phase, (*row)[i], sizeof(scenarioresult->Phase)); break;
         case 3:
            safecpy(scenarioresult->Scenarioname, (*row)[i], sizeof(scenarioresult->Scenarioname)); break;
         case 4:
            safecpy(scenarioresult->Jobname, (*row)[i], sizeof(scenarioresult->Jobname)); break;
         case 5:
            safecpy(scenarioresult->State, (*row)[i], sizeof(scenarioresult->State)); break;
         case 6:
            scenarioresult->Actionsinscenario = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 7:
            scenarioresult->Actionsattempted = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 8:
            scenarioresult->Actionscompleted = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 9:
            scenarioresult->Actionspassed = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 10:
            scenarioresult->Actionsfailed = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 11:
            scenarioresult->Pass = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 12:
            safecpy(scenarioresult->Start, (*row)[i], sizeof(scenarioresult->Start)); break;
         case 13:
            safecpy(scenarioresult->End, (*row)[i], sizeof(scenarioresult->End)); break;
         case 14:
            safecpy(scenarioresult->Teststand, (*row)[i], sizeof(scenarioresult->Teststand)); break;
         case 15:
            scenarioresult->TeststandrecordID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 16:
            safecpy(scenarioresult->Testlevel, (*row)[i], sizeof(scenarioresult->Testlevel)); break;
         case 17:
            scenarioresult->TestlevelrecordID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 18:
            safecpy(scenarioresult->Tester, (*row)[i], sizeof(scenarioresult->Tester)); break;
         case 19:
            safecpy(scenarioresult->Loglocation, (*row)[i], sizeof(scenarioresult->Loglocation)); break;
         case 20:
            safecpy(scenarioresult->Scenariologfilename, (*row)[i], sizeof(scenarioresult->Scenariologfilename)); break;
         case 21:
            safecpy(scenarioresult->Comments, (*row)[i], sizeof(scenarioresult->Comments)); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }    
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_host_record                                                                       */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the host table.                                              */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the host        */
/*                                   table - this structure is filled in with values taken from      */
/*                                   row parameter.                                                  */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the host table and places it in a structure.         */
/*             The query that set up the MYSQL row structure must have been such that the            */
/*             row is populated in the order specified by the HOST_FIELDS definition in the          */
/*             af_defns.h file                                                                       */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_host_record(MYSQL_RES   *res_set,
                     MYSQL_ROW   *row,
                     host_t      *host) {

#undef  SUBNAME
#define SUBNAME "get_host_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            host->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            host->TeststandID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 2:
            safecpy(host->Name, (*row)[i], sizeof(host->Name)); break; 
         case 3:
            safecpy(host->Type, (*row)[i], sizeof(host->Type)); break; 
         case 4:
            safecpy(host->Model, (*row)[i], sizeof(host->Model)); break; 
         case 5:
            safecpy(host->Serial, (*row)[i], sizeof(host->Serial)); break; 
         case 6:
            safecpy(host->Hostselectorvalue, (*row)[i], sizeof(host->Hostselectorvalue)); break; 
         case 7:
            host->Teststandprimary = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 8:
            safecpy(host->Comments, (*row)[i], sizeof(host->Comments)); break; 
         case 9:
            safecpy(host->Agentstatus, (*row)[i], sizeof(host->Agentstatus)); break; 
         case 10:
            safecpy(host->Agentstatusdate, (*row)[i], sizeof(host->Agentstatusdate)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_collectorvalue_record                                                             */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the collectorvalue table.                                    */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the             */
/*                                   collectorvlaue table - this structure is filled in with values  */
/*                                   taken from row parameter.                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the collectorvalue table and places it in a          */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the COLLECTORVALUE_FIELDS definition   */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_collectorvalue_record(MYSQL_RES         *res_set,
                               MYSQL_ROW         *row,
                               collectorvalue_t  *collectorvalue) {

#undef  SUBNAME
#define SUBNAME "get_collectorvalue_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            collectorvalue->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            collectorvalue->CollectortypeID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 2:
            collectorvalue->HostID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 3:
            safecpy(collectorvalue->Value, (*row)[i], sizeof(collectorvalue->Value)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_collectortype_record                                                              */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the collectortype table.                                     */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the             */
/*                                   collectortype table - this structure is filled in with values   */
/*                                   taken from row parameter.                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the collectortype table and places it in a           */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the COLLECTORTYPE_FIELDS definition    */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_collectortype_record(MYSQL_RES         *res_set,
                              MYSQL_ROW         *row,
                              collectortype_t   *collectortype) {

#undef  SUBNAME
#define SUBNAME "get_collectortype_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            collectortype->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(collectortype->Name, (*row)[i], sizeof(collectortype->Name)); break; 
         case 2:
            collectortype->Builtin = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 3:
            safecpy(collectortype->OStype, (*row)[i], sizeof(collectortype->OStype)); break; 
         case 4:
            safecpy(collectortype->Invocation, (*row)[i], sizeof(collectortype->Invocation)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_collectorset_record                                                               */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the collectorset table.                                      */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the             */
/*                                   collectorset table - this structure is filled in with values    */
/*                                   taken from row parameter.                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the collectorset table and places it in a            */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the COLLECTORTYPE_FIELDS definition    */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_collectorset_record(MYSQL_RES       *res_set,
                             MYSQL_ROW       *row,
                             collectorset_t  *collectorset) {

#undef  SUBNAME
#define SUBNAME "get_collectorset_record"

   int          i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            collectorset->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(collectorset->Name, (*row)[i], sizeof(collectorset->Name)); break; 
         case 2:
            safecpy(collectorset->Description, (*row)[i], sizeof(collectorset->Description)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_collectorsetmember_record                                                         */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the collectorsetmember table.                                */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the             */
/*                                   collectorsetmember table - this structure is filled in with     */
/*                                   values taken from row parameter.                                */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the collectorsetmember table and places it in a      */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the COLLECTORTYPE_FIELDS definition    */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_collectorsetmember_record(MYSQL_RES             *res_set,
                                   MYSQL_ROW             *row,
                                   collectorsetmember_t  *collectorsetmember) {

#undef  SUBNAME
#define SUBNAME "get_collectorsetmember_record"

   int          i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            collectorsetmember->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            collectorsetmember->CollectortypeID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 2:
            safecpy(collectorsetmember->Requiredvalue, (*row)[i], sizeof(collectorsetmember->Requiredvalue)); break; 
         case 3:
            collectorsetmember->CollectorsetID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_environment_record                                                                */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the environment table.                                       */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the             */
/*                                   environment table - this structure is filled in with values     */
/*                                   taken from row parameter.                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the environment table and places it in a             */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the ENVIRONMENT_FIELDS definition      */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_environment_record(MYSQL_RES      *res_set,
                            MYSQL_ROW      *row,
                            environment_t  *environment) {

#undef  SUBNAME
#define SUBNAME "get_environment_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            environment->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(environment->Name, (*row)[i], sizeof(environment->Name)); break; 
         case 2:
            safecpy(environment->Description, (*row)[i], sizeof(environment->Description)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_environmentvalue_record                                                           */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the environmentvalue table.                                  */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the             */
/*                                   environmentvalue table - this structure is filled in with       */
/*                                   values taken from row parameter.                                */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the environmentvalue table and places it in a        */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the ENVIRONMENTVALUE_FIELDS definition */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_environmentvalue_record(MYSQL_RES           *res_set,
                                 MYSQL_ROW           *row,
                                 environmentvalue_t  *environmentvalue) {
#undef  SUBNAME
#define SUBNAME "get_environmentvalue_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            environmentvalue->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(environmentvalue->Name, (*row)[i], sizeof(environmentvalue->Name)); break; 
         case 2:
            safecpy(environmentvalue->Value, (*row)[i], sizeof(environmentvalue->Value)); break; 
         case 3:
            environmentvalue->EnvironmentID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_outcomeaction_record                                                              */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the outcomeaction table.                                     */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     testcase              A structure representing the fields in a row in the             */
/*                                   outcomeaction table - this structure is filled in with          */
/*                                   values taken from row parameter.                                */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the outcomeaction table and places it in a           */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the OUTCOMEACTION_FIELDS definition    */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_outcomeaction_record(MYSQL_RES           *res_set,
                              MYSQL_ROW           *row,
                              outcomeaction_t     *outcomeaction) {

#undef  SUBNAME
#define SUBNAME "get_outcomeaction_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            outcomeaction->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(outcomeaction->Name, (*row)[i], sizeof(outcomeaction->Name)); break; 
         case 2:
            safecpy(outcomeaction->Actiononpass, (*row)[i], sizeof(outcomeaction->Actiononpass)); break; 
         case 3:
            safecpy(outcomeaction->Actiononfail, (*row)[i], sizeof(outcomeaction->Actiononfail)); break; 
         case 4:
            outcomeaction->Scenariotoberunonpass = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 5:
            outcomeaction->Scenariotoberunonfail = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 6:
            safecpy(outcomeaction->Erroraction, (*row)[i], sizeof(outcomeaction->Erroraction)); break; 
         case 7:
            safecpy(outcomeaction->Notificationonpass, (*row)[i], sizeof(outcomeaction->Notificationonpass)); break; 
         case 8:
            safecpy(outcomeaction->Notificationonfail, (*row)[i], sizeof(outcomeaction->Notificationonfail)); break; 
         case 9:
            outcomeaction->TesterID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 10:
            outcomeaction->MaillistID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_user_record                                                                       */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the user table.                                              */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     user                  A structure representing the fields in a row in the             */
/*                                   user table - this structure is filled in with values            */
/*                                   taken from row parameter.                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the user table and places it in a                    */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the USER_FIELDS definition             */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_user_record(MYSQL_RES         *res_set,
                     MYSQL_ROW         *row,
                     user_t            *user) {

#undef  SUBNAME
#define SUBNAME "get_user_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            user->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            safecpy(user->Firstname, (*row)[i], sizeof(user->Firstname)); break; 
         case 2:
            safecpy(user->Lastname, (*row)[i], sizeof(user->Lastname)); break; 
         case 3:
            safecpy(user->Area, (*row)[i], sizeof(user->Area)); break; 
         case 4:
            safecpy(user->Email, (*row)[i], sizeof(user->Email)); break; 
         case 5:
            user->Admin = (bool_t) strtoull((*row)[i], NULL, 0); break;
         case 6:
            safecpy(user->Password, (*row)[i], sizeof(user->Password)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   get_objecttype_record                                                                 */
/*                                                                                                   */
/* Inputs:     res_set               A MYSQL result set that contains the result of a query          */
/*                                   on the objecttype table.                                        */
/*                                                                                                   */
/*             row                   A MYSQL row structure obtained from the res_set above           */
/*                                                                                                   */
/* Outputs     user                  A structure representing the fields in a row in the             */
/*                                   objecttype table - this structure is filled in with values      */
/*                                   taken from row parameter.                                       */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   Decodes the contents of a row of the objecttype table and places it in a              */
/*             structure.  The query that set up the MYSQL row structure must have been such that    */
/*             the row is populated in the order specified by the OBJECTTYPE_FIELDS definition       */
/*             in the af_defns.h file                                                                */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void get_objecttype_record(MYSQL_RES         *res_set,
                           MYSQL_ROW         *row,
                           objecttype_t      *objecttype) {

#undef  SUBNAME
#define SUBNAME "get_objecttype_record"

   int i;
   char msg[MAX_MSG_LEN];

   for (i = 0; i < mysql_num_fields(res_set); i++) {

      switch(i) {
         case 0:
            objecttype->ID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 1:
            objecttype->ParentID = (Iu32) strtoull((*row)[i], NULL, 0); break;
         case 2:
            safecpy(objecttype->Name, (*row)[i], sizeof(objecttype->Name)); break; 
         case 3:
            safecpy(objecttype->Description, (*row)[i], sizeof(objecttype->Description)); break; 
         default:
            /* problem */
            snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                     SUBNAME, i);
            print_msg_to_console(msg);
            break;
      }
   }
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_workrequest                                                                          */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_workrequest(MYSQL *sql_connection, int workrequestID, char * tablename, workrequest_t *workrequest) {

   char         query[1024];
   MYSQL_RES    *res_set;
   MYSQL_ROW    row;
   int          rc = 0;

#undef  SUBNAME
#define SUBNAME "get_workrequest"

   memset(workrequest, 0, sizeof(*workrequest)); 
   snprintf(query, sizeof(query), "SELECT " WORKREQUEST_FIELDS " FROM %s WHERE ID = '%u'", tablename, workrequestID);

   if (perform_query(SUBNAME, sql_connection, query) != 0) {
  
      rc = 1;

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {
            get_workrequest_record(res_set, &row, workrequest);
         } else {
            rc = 1;                    
         }
         mysql_free_result(res_set);

      }
   }
  
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_scenario                                                                             */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_scenario(MYSQL *sql_connection, int scenarioID, scenario_t *scenario) {

   char         query[1024];
   MYSQL_RES    *res_set;
   MYSQL_ROW    row;
   int          rc = 0;

   memset(scenario, 0, sizeof(*scenario)); 
   snprintf(query, sizeof(query), "SELECT " SCENARIO_FIELDS " FROM daf.scenario WHERE ID = '%u'", scenarioID);

   if (perform_query(SUBNAME, sql_connection, query) != 0) {

      rc = 1;

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {
            get_scenario_record(res_set, &row, scenario);
         } else {
            rc = 1;                 
         }
         mysql_free_result(res_set);

      }
   }
  
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_scenario                                                                             */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_scenarioresult(MYSQL *sql_connection, int scenarioresultID, char *tablename, scenarioresult_t *scenarioresult) {

   char         query[1024];
   MYSQL_RES    *res_set;
   MYSQL_ROW    row;
   int          rc = 0;

   memset(scenarioresult, 0, sizeof(*scenarioresult)); 
   snprintf(query, sizeof(query), "SELECT " SCENARIORESULT_FIELDS " FROM %s WHERE ID = '%u'", tablename, scenarioresultID);

   if (perform_query(SUBNAME, sql_connection, query) != 0) {

      rc = 1;

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {
            get_scenarioresult_record(res_set, &row, scenarioresult);
         } else {
            rc = 1;               
         }
         mysql_free_result(res_set);

      }
   }
   
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_scenario                                                                             */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_outcomeaction(MYSQL *sql_connection, int OutcomeactionID, outcomeaction_t *outcomeaction) {

   char       query[1024];
   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   int        rc = 0;

#undef  SUBNAME
#define SUBNAME "get_outcomeaction"

   memset(outcomeaction, 0, sizeof(*outcomeaction));
   snprintf(query, sizeof(query), "SELECT " OUTCOMEACTION_FIELDS " FROM outcomeaction WHERE ID = '%u'", OutcomeactionID);

   if (perform_query(SUBNAME, sql_connection, query) != 0) {  

      rc = 1;

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {
            get_outcomeaction_record(res_set, &row, outcomeaction);
         } else {
            rc = 1;   /* res_set was empty */
         }
         mysql_free_result(res_set);

      }
   }
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_user                                                                                 */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* <<<<<<<< <<<<<<<<<<<<<<<<   need to ensure that user is returned initialised even if query fails       */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_user(MYSQL *sql_connection, int userID, user_t *user) {

   char         query[1024];
   MYSQL_RES    *res_set;
   MYSQL_ROW    row;
   int          rc = 0;

   memset(user, 0, sizeof(*user));
   snprintf(query, sizeof(query), "SELECT " USER_FIELDS " FROM daf.user WHERE ID = '%u'", userID);

   if (perform_query(SUBNAME, sql_connection, query) != 0) {

      rc = 1;                   

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {
            get_user_record(res_set, &row, user);
         } else {
            rc = 1;                
         }
         mysql_free_result(res_set);

      }
   }
  
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    count_actions_in_scenario                                                                */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int count_actions_in_scenario(MYSQL *sql_connection, int scenarioID) {

   char       query[1024];
   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   int        rc = 0;

#undef  SUBNAME
#define SUBNAME "count_actions_in_scenario"

   snprintf(query, sizeof(query), "SELECT COUNT(*) FROM daf.action WHERE scenarioID = '%u'", scenarioID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {
            rc = (Iu32) strtoull(row[0], NULL, 0);
         } else {
            rc = 0;   /* res_set was empty */
         }
         mysql_free_result(res_set);

      }
   }
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    count_entries_in_workqueue                                                               */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int count_entries_in_workqueue(MYSQL *sql_connection) {

   char       query[1024];
   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   int        rc = 0;

#undef  SUBNAME
#define SUBNAME "count_entries_in_workqueue"

   snprintf(query, sizeof(query), "SELECT COUNT(*) FROM daf.workqueue");

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {
            rc = (Iu32) strtoull(row[0], NULL, 0);
         } else {
            rc = 0;   /* res_set was empty */
         }
         mysql_free_result(res_set);

      }
   }
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_required_collectorset_values                                                         */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_required_collectorset_values(MYSQL                          *sql_connection, 
                                     int                            collectorsetID, 
                                     collectorset_required_values_t *required_values, 
                                     int                            max_num_records, 
                                     int                            *num_records) {

#undef  SUBNAME
#define SUBNAME "get_required_collectors_values"

   char       query[1024];
   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char       msg[MAX_MSG_LEN];

   *num_records = 0;
   int rc = 0;

/* what to do if there is no collectorset defined for that action ? <<<<<<<< */

   snprintf(query, sizeof(query), "SELECT collectorset.Name, collectorsetmember.CollectortypeID, collectortype.Name, collectorsetmember.Requiredvalue, " 
                                  "collectorsetmember.Comparator FROM " 
                                  "daf.collectorsetmember INNER JOIN daf.collectorset ON collectorsetmember.CollectorsetID = collectorset.ID " 
                                  "INNER JOIN daf.collectortype ON collectorsetmember.CollectortypeID = collectortype.ID " 
                                  "WHERE CollectorsetID = '%u'",
                                  collectorsetID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) && (*num_records < max_num_records)) {

            int i;

            for (i = 0; i < mysql_num_fields(res_set); i++) {

               switch(i) {
                  case 0:
                     safecpy(required_values->Collectorsetname, row[i] , sizeof(required_values->Collectorsetname)); break; 
                  case 1:
                     required_values->CollectortypeID = (Iu32) strtoull(row[i], NULL, 0);
                  case 2:
                     safecpy(required_values->Collectortypename, row[i] , sizeof(required_values->Collectortypename)); break; 
                  case 3: 
                     safecpy(required_values->Requiredvalue, row[i], sizeof(required_values->Requiredvalue)); break; 
                  case 4: 
                     safecpy(required_values->Comparator, row[i], sizeof(required_values->Comparator)); break; 
                  default:
                     /* problem */
                     snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                              SUBNAME, i);
                     print_msg_to_console(msg);
                     break;
               }
            }
            required_values++;
            (*num_records)++;

         }
         mysql_free_result(res_set);

      }
   }
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    allocate_and_get_required_collectorset_values                                            */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

collectorset_required_values_t *allocate_and_get_required_collectorset_values(MYSQL  *sql_connection, 
                                                                              int    collectorsetID, 
                                                                              int    *num_records) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_required_collectors_values"

   char       query[1024];
   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char       msg[MAX_MSG_LEN];
   collectorset_required_values_t *p=NULL, *q;
   *num_records = 0;
   int num_rows;

/* what to do if there is no collectorset defined for that action ? <<<<<<<< */

   snprintf(query, sizeof(query), "SELECT collectorset.Name, collectorsetmember.CollectortypeID, collectortype.Name, collectorsetmember.Requiredvalue, " 
                                  "collectorsetmember.Comparator FROM " 
                                  "daf.collectorsetmember INNER JOIN daf.collectorset ON collectorsetmember.CollectorsetID = collectorset.ID " 
                                  "INNER JOIN daf.collectortype ON collectorsetmember.CollectortypeID = collectortype.ID " 
                                  "WHERE CollectorsetID = '%u'",
                                  collectorsetID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(collectorset_required_values_t) );

         while ((row = mysql_fetch_row(res_set)) != NULL) {

            int i;

            for (i = 0; i < mysql_num_fields(res_set); i++) {

               switch(i) {
                  case 0:
                     safecpy(q->Collectorsetname, row[i] , sizeof(q->Collectorsetname)); break; 
                  case 1:
                     q->CollectortypeID = (Iu32) strtoull(row[i], NULL, 0);
                  case 2:
                     safecpy(q->Collectortypename, row[i] , sizeof(q->Collectortypename)); break; 
                  case 3: 
                     safecpy(q->Requiredvalue, row[i], sizeof(q->Requiredvalue)); break; 
                  case 4: 
                     safecpy(q->Comparator, row[i], sizeof(q->Comparator)); 
                     break;
                  default:
                     /* problem */
                     snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                              SUBNAME, i);
                     print_msg_to_console(msg);
                     break;
               }
            }
            q++;
            (*num_records)++;

         }
         mysql_free_result(res_set);

      }
   }
   return p;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    log_required_collectorset_values                                                         */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void log_required_collectorset_values(char                           *title, 
                                      collectorset_required_values_t *required_values, 
                                      int                            num_records) {

    int i;
    char msg[MAX_MSG_LEN];
    char *title_fmt = "      %-20s   %-32s   %-10s   %-s\n";               
    char *fmt =       "      %-20s   %-32s   %-10s   %-s\n";               

    if (num_records > 0) {

       snprintf(msg, sizeof(msg), "%s\n", title);
       print_scenario_console_log(msg);

       snprintf(msg, sizeof(msg), title_fmt, "Collectorset", "Name", "Comparator", "Required Value");
       print_scenario_console_log(msg);
       for (i=0; i<num_records; i++) {
          snprintf(msg, sizeof(msg), fmt, 
                                     (required_values+i)->Collectorsetname, 
                                     /* (required_values+i)->CollectortypeID,  */
                                     (required_values+i)->Collectortypename, 
                                     (required_values+i)->Comparator, 
                                     (required_values+i)->Requiredvalue);
          print_scenario_console_log(msg);
       }

    } else {

       snprintf(msg, sizeof(msg), "There are no required collectorvalues\n");
       print_scenario_console_log(msg);

    }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_host_collectorvalues                                                                 */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_host_collectorvalues(MYSQL                   *sql_connection, 
                             int                     TeststandID,
                             host_collectorvalues_t  *host_collectorvalues, 
                             int                     max_num_hosts, 
                             int                     *num_hosts) {

#undef  SUBNAME
#define SUBNAME "get_host_collectorvalues"

   char       query[1024];
   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   int rc = 0;
   int HostID;
   bool_t known_host;

   *num_hosts = 0;

   memset(host_collectorvalues, 0, sizeof(*host_collectorvalues) * max_num_hosts);

   snprintf(query, sizeof(query), "SELECT host.ID, host.Name, collectorvalue.CollectortypeID, collectortype.Name, collectorvalue.Value, host.Hostselectorvalue "
                                  "FROM daf.collectorvalue INNER JOIN daf.host ON collectorvalue.HostID = host.ID "
                                  "INNER JOIN collectortype ON daf.collectorvalue.CollectortypeID = collectortype.ID "
                                  "WHERE host.TeststandID = %u\n", TeststandID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while (((row = mysql_fetch_row(res_set)) != NULL) && (*num_hosts < max_num_hosts)) {

            int i;

            HostID = (Iu32) strtoull(row[0], NULL, 0);
   
            known_host = FALSE;  i = 0;
            while ( i<(*num_hosts) ) {
               if (host_collectorvalues[i].HostID == HostID) {
                  known_host = TRUE; 
                  break;
               }
               i++;
            }

            if (! known_host) {
               i = *num_hosts;
               host_collectorvalues[i].HostID = HostID;
               safecpy(host_collectorvalues[i].Hostname, row[1], sizeof(host_collectorvalues[i].Hostname));
               safecpy(host_collectorvalues[i].Hostselectorvalue, row[5], sizeof(host_collectorvalues[i].Hostselectorvalue));
               (*num_hosts)++;
            }
            host_collectorvalues[i].CollectortypeIDs[host_collectorvalues[i].num_collectortypes_on_this_host] = (Iu32) strtoull(row[2], NULL, 0);
            safecpy(host_collectorvalues[i].Collectornames[host_collectorvalues[i].num_collectortypes_on_this_host],
                    row[3],
                    sizeof(host_collectorvalues[i].Collectornames[0]));
            safecpy(host_collectorvalues[i].Collectorvalues[host_collectorvalues[i].num_collectortypes_on_this_host],
                    row[4],
                    sizeof(host_collectorvalues[i].Collectorvalues[0]));
            host_collectorvalues[i].num_collectortypes_on_this_host++;

         }

         mysql_free_result(res_set);

      }
   }
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    allocate_and_get_host_collectorvalues                                                    */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

host_collectorvalues_t *allocate_and_get_host_collectorvalues(MYSQL *sql_connection, 
                                                              int   TeststandID,
                                                              int   *num_hosts) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_host_collectorvalues"

   MYSQL_RES              *res_set;
   MYSQL_ROW              row;
   char                   query[1024];
   int                    HostID;
   bool_t                 known_host;
   host_collectorvalues_t *p = NULL;
   int                    num_rows;

   *num_hosts = 0;

   snprintf(query, sizeof(query), "SELECT host.ID, host.Name, collectorvalue.CollectortypeID, collectortype.Name, collectorvalue.Value, host.Hostselectorvalue "
                                  "FROM daf.collectorvalue INNER JOIN daf.host ON collectorvalue.HostID = host.ID "
                                  "INNER JOIN collectortype ON daf.collectorvalue.CollectortypeID = collectortype.ID "
                                  "WHERE host.TeststandID = %u\n", TeststandID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {  

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = malloc( num_rows * sizeof(host_collectorvalues_t) );

         while ((row = mysql_fetch_row(res_set)) != NULL) {

            int i;

            HostID = (Iu32) strtoull(row[0], NULL, 0);
   
            known_host = FALSE;  i = 0;
            while (i < (*num_hosts) ) {
               if (p[i].HostID == HostID) {
                  known_host = TRUE; 
                  break;
               }
               i++;
            }

            if (! known_host) {
               i = *num_hosts;
               p[i].HostID = HostID;
               p[i].num_collectortypes_on_this_host = 0;
               safecpy(p[i].Hostname, row[1], sizeof(p[i].Hostname));
               safecpy(p[i].Hostselectorvalue, row[5], sizeof(p[i].Hostselectorvalue));
               (*num_hosts)++;
            }
            p[i].CollectortypeIDs[p[i].num_collectortypes_on_this_host] = (Iu32) strtoull(row[2], NULL, 0);
            safecpy(p[i].Collectornames[p[i].num_collectortypes_on_this_host],
                    row[3],
                    sizeof(p[i].Collectornames[0]));
            safecpy(p[i].Collectorvalues[p[i].num_collectortypes_on_this_host],
                    row[4],
                    sizeof(p[i].Collectorvalues[0]));
            p[i].num_collectortypes_on_this_host++;

         }

         mysql_free_result(res_set);

      }
   }
   return p;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    log_host_collectorvalues                                                                 */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void log_host_collectorvalues(host_collectorvalues_t *host_collectorvalues, 
                              int                    num_records) {

   int  i, j;
   char msg[MAX_MSG_LEN];

   snprintf(msg, sizeof(msg), "Host collectorvalues are:\n");
   print_scenario_console_log(msg);

   for (i=0; i<num_records; i++) {
      snprintf(msg, sizeof(msg), "   HostID: %8u  Hostname: %s\n", 
                                 (host_collectorvalues+i)->HostID, 
                                 (host_collectorvalues+i)->Hostname);
      print_scenario_console_log(msg);
      for (j=0; j<(host_collectorvalues+i)->num_collectortypes_on_this_host; j++) {
         snprintf(msg, sizeof(msg), "      CollectorID: %8u  Collectorname: %32s  Collectorvalue: %s\n", 
                                    (host_collectorvalues+i)->CollectortypeIDs[j], 
                                    &((host_collectorvalues+i)->Collectornames[j][0]),
                                    &((host_collectorvalues+i)->Collectorvalues[j][0]));
         print_scenario_console_log(msg);
      }
   }
}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/* Function:   allocate_and_get_all_objecttype_records                                               */
/*                                                                                                   */
/* Inputs:     sql_connection        An established connection to the MYSQL database                 */
/*                                                                                                   */
/* Outputs:    num_records           the number of records that were actually returned in the        */
/*                                   objecttype array                                                */
/*                                                                                                   */
/* Returns:    objecttype_t*         Pointer to the first element in the memory area that holds the  */
/*                                   objecttype array                                                */
/*                                                                                                   */
/* Function:   Reads the objecttype table from the MYSQL database and mallocs a sufficiently large   */
/*             memory area to hold the table - the memory must be free()d after use outside this     */
/*             routine.                                                                              */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

objecttype_t *allocate_and_get_all_objecttype_records(MYSQL  *sql_connection, 
                                                      int    *num_records ) {

#undef  SUBNAME
#define SUBNAME "allocate_and_get_all_objecttype_records"

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   objecttype_t *p, *q = NULL;
   int num_rows = 0;
   char *query = "SELECT " OBJECTTYPE_FIELDS " FROM daf.objecttype";

   *num_records = 0;

   if (perform_query(SUBNAME, sql_connection, query) == 0) { 

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(objecttype_t) );

         while (((row = mysql_fetch_row(res_set)) != NULL) && ((*num_records) < num_rows)) {

            get_objecttype_record(res_set, &row, q);
            (*num_records)++;
            q++;
         }
     
         mysql_free_result(res_set);

      }

   }

   return p;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_scenario_pass_rate                                                                   */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_scenario_pass_rate(MYSQL *sql_connection, 
                           char *status_database_name, 
                           int scenarioresultID, 
                           int *num_actions, 
                           int *num_completed,
                           int *num_pass, 
                           int *num_failed, 
                           int *pass) {

   MYSQL_RES  *res_set;
   MYSQL_ROW  row;
   char       query[1024];
   int        pass_for_this_action;
   char       status_for_this_action[32];
   int        accumulated_passes = 0;
   int        rc = 0;
   int        num_actions_in_this_scenario = 0;

   *num_actions   = 0;
   *num_pass      = 0;
   *num_failed    = 0;
   *num_completed = 0;

   snprintf(query, sizeof(query), "SELECT Actionsinscenario FROM %s.scenarioresult WHERE "
            "ID = '%u'", status_database_name, scenarioresultID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         if ((row = mysql_fetch_row(res_set)) != NULL) {
            num_actions_in_this_scenario =  (Iu32) strtoull(row[0], NULL, 0);
         } else {
            /* problem */
         }

         mysql_free_result(res_set);
      }

   }

   snprintf(query, sizeof(query), "SELECT Status, Pass FROM %s.actionresult WHERE "
            "actionresult.scenarioresultID = '%u'", status_database_name, scenarioresultID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while ((row = mysql_fetch_row(res_set)) != NULL) {

            safecpy(status_for_this_action, row[0], sizeof(status_for_this_action));
            if (MATCH(status_for_this_action, "Completed")) {
               (*num_completed)++;
               pass_for_this_action = (Iu32) strtoull(row[1], NULL, 0); 
               if (pass_for_this_action == 100) {
                  (*num_pass)++;
               } else {
                  (*num_failed)++;
               }
               accumulated_passes = accumulated_passes + pass_for_this_action;
            } 

            (*num_actions)++;
         }
         
         if (num_actions_in_this_scenario > 0) {
            *pass = accumulated_passes / num_actions_in_this_scenario;
         } else {
            *pass = 0;
         }

      }
         mysql_free_result(res_set);
   }
   return rc;
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    get_maillistID_from_outcomeaction                                                        */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_maillistID_from_outcomeaction(MYSQL         *sql_connection, 
                                      int           outcomeactionID,
                                      int           *maillistID) {

#undef  SUBNAME
#define SUBNAME "get_maillistID_from_outcomeaction"

   char             query[1024];
   MYSQL_RES        *res_set;
   MYSQL_ROW        row;
   int rc = 0;

   snprintf(query, sizeof(query), "SELECT MaillistID FROM outcomeaction where ID = %u", outcomeactionID);

   if (perform_query(SUBNAME, sql_connection, query) == 0) {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {
   
         row = mysql_fetch_row(res_set);   /* there should only be one row */

         if (row != NULL) {

            *maillistID = (Iu32) strtoull(row[0], NULL, 0);

         } else {

            *maillistID = 0;
            /* problem */
            rc = 1;
         }
      
         mysql_free_result(res_set);

      }

   }
   return rc;

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/*                                                                                                   */
/*                                                                                                   */
/*                                                                                                   */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

void print_suitable_hosts(suitable_hosts_t *s, int num_records) {

  int i;
  char msg[MAX_MSG_LEN];

  snprintf(msg, sizeof(msg), "Suitable hosts = %d\n", num_records);
  print_scenario_console_log(msg);
  for (i=0; i<num_records; i++) {
     snprintf(msg, sizeof(msg), "Hostname=%s, Hostselectorvalue=%s, Num_actions_allocated_in_this_scenario_step=%u\n",
              s[i].Hostname, s[i].Hostselectorvalue, s[i].Num_actions_allocated_in_this_scenario_step);
     print_scenario_console_log(msg);
   }
}


int select_host_for_action(MYSQL *sql_connection, int ScenarioID, int ScenarioresultID, int Stepnumber, int TeststandID, 
                           action_t *action, char *hostname, int max_hostname_len) {

#undef  SUBNAME
#define SUBNAME "select_host_for_action"

   host_t                          *hosts = NULL;
   suitable_hosts_t                *suitable_hosts = NULL;
   hosts_in_use_t                  *hosts_in_use = NULL;
   collectorset_required_values_t  *required_values = NULL;
   host_collectorvalues_t          *host_collectorvalues;
   int                             num_required_values_records;
   char                            where[512];
   int                             num_host_records;
   int                             num_hosts_in_use_records = 0;
   int                             num_host_collectorvalues_records;
   int                             i,j,k,l;
   char                            msg[MAX_MSG_LEN];
   int                             rc = 0;
   bool_t                          host_is_suitable;
   bool_t                          required_value_collector_found;
   int                             num_suitable_hosts = 0;
   char                            primary_hostname[MAX_HOSTNAME_LEN];

   /* ---------------------------------------------------------------------------------------- */
   /* Determine which host to use - first get a list of all the hosts that are allocated to    */
   /* the teststand that is being used in this scenario.                                       */
   /* Then examine the collectorset for this step - and eliminate any hosts that do not have   */
   /* the desired collectorvalues                                                              */
   /* Then look at the hostselectortype:                                                       */
   /*    Any       - make a random choice of any host that is on the test stand                */
   /*    Primary   - select the host that has the hostselectorvalue of 'Primary'               */
   /*    Group X   - select the host that has the hostselectorvalue  of 'X'                    */
   /*    OSspecific X  - select any host that has the hostselectorqualifier of 'X'             */
   /*    hmmm, this is going to need some refinement <<<<< TODO                                */
   /*    also how do we load balance ?                                                         */
   /* ---------------------------------------------------------------------------------------- */

   snprintf(where, sizeof(where), "TeststandID = '%d'", TeststandID);
   hosts = allocate_and_get_host_records(sql_connection, where, &num_host_records);

   if (num_host_records > 0) {
      suitable_hosts = (suitable_hosts_t *) malloc (num_host_records * sizeof(suitable_hosts_t));
   }

   /* snprintf(msg, sizeof(msg), "%s: The following %d host(s) are candidates for this action:\n", SUBNAME, num_host_records);
   print_scenario_console_log(msg); 
   
   print_scenario_console_log_blank_timestamp("  ");
   for (i=0; i < num_host_records; i++) {
      print_scenario_console_log_string(hosts[i].Name);
      if (i < (num_host_records-1)) {
         print_scenario_console_log_string(", ");
      } 
      if ( ((i != 0) && ((i % 16) == 0)) || (i == (num_host_records-1))) {
         print_scenario_console_log_string("\n");
         if ((i != (num_host_records-1))) {
            print_scenario_console_log_blank_timestamp("  ");
         }
      }
   } */

   safecpy(primary_hostname, "NOHOSTSELECTED", sizeof(primary_hostname));
   for (l=0; l<num_host_records; l++) {
      if (hosts[l].Teststandprimary) {
         safecpy(primary_hostname, hosts[l].Name, max_hostname_len);
         break;
      }
   }
   hosts_in_use = allocate_and_get_host_in_use_records_in_this_scenario_step(sql_connection, ScenarioresultID, Stepnumber, &num_hosts_in_use_records);

   /* snprintf(msg, sizeof(msg), "%s: The following %d host(s) are already in use in this scenario:\n", SUBNAME, num_hosts_in_use_records);
   print_scenario_console_log(msg);

   print_scenario_console_log_blank_timestamp("  ");
   for (i=0; i < num_hosts_in_use_records; i++) {
      snprintf(msg, sizeof(msg), "%s(%d)", hosts_in_use[i].Hostname, hosts_in_use[i].Num_actions_allocated_in_this_scenario_step);
      print_scenario_console_log_string(msg);
      if (i < (num_host_records-1)) {
         print_scenario_console_log_string(", ");
      } 
      if ( ((i != 0) && ((i % 16) == 0)) || (i == (num_host_records-1))) {
         print_scenario_console_log_string("\n");
         if ((i != (num_hosts_in_use_records-1))) {
            print_scenario_console_log_blank_timestamp("  ");
         }
      }
   } */

   required_values = allocate_and_get_required_collectorset_values(sql_connection, action->CollectorsetID, &num_required_values_records);  
   log_required_collectorset_values("   Collectorset values required for action host selection are:", 
                                    &required_values[0], num_required_values_records);

   if (num_required_values_records == 0) {

      snprintf(msg, sizeof(msg), "   There are no required collector values for hosts in this action\n");
      print_scenario_console_log(msg);

      for (i=0; i < num_host_records; i++) {
         safecpy(suitable_hosts[i].Hostname, hosts[i].Name, sizeof(suitable_hosts[i].Hostname));
         safecpy(suitable_hosts[i].Hostselectorvalue, hosts[i].Hostselectorvalue, sizeof(suitable_hosts[i].Hostselectorvalue));
      }
      num_suitable_hosts = num_host_records;

   } else {

      host_collectorvalues = allocate_and_get_host_collectorvalues(sql_connection, TeststandID, &num_host_collectorvalues_records);   
      /* log_host_collectorvalues(&host_collectorvalues[0], num_host_collectorvalues_records); */

      for (i=0; i < num_host_collectorvalues_records; i++) {

         /* The primary host in the test stand is excluded from the suitable hosts */
         if (MATCH(host_collectorvalues[i].Hostname, primary_hostname)) {
             continue;
         }

         host_is_suitable = TRUE;

         for (j=0; j < num_required_values_records; j++) {

            required_value_collector_found = FALSE;
            for (k=0; k < host_collectorvalues[i].num_collectortypes_on_this_host; k++) {

               if (host_collectorvalues[i].CollectortypeIDs[k] == required_values[j].CollectortypeID) {

                  required_value_collector_found = TRUE;

                  if (MATCH(required_values[j].Comparator, "EQ")) {

                     if (! MATCH( host_collectorvalues[i].Collectorvalues[k], required_values[j].Requiredvalue)) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "NE")) {
                     if (MATCH( host_collectorvalues[i].Collectorvalues[k], required_values[j].Requiredvalue)) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "==")) {
                     if (! (strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) ==
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "!=")) {
                     if (! ( strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) ==
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "<")) {
                     if (! (strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) < 
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "<=")) {
                     if (!( strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) <=
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, ">")) {
                     if (! (strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) >
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                    }
                  } else if (MATCH(required_values[j].Comparator, ">=")) {
                     if (! (strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) >=
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                       host_is_suitable = FALSE;
                       break;
                     }
                  } else {
                     /* problem */  
                     snprintf(msg, sizeof(msg), "%s: internal error - unexpected comparator (%s)\n", 
                              SUBNAME, required_values[j].Comparator);
                     print_scenario_console_log(msg);
                     break;
                  }
               }
            }
            if (! required_value_collector_found) {
               host_is_suitable = FALSE;
            }
         }

         /* -----------------------------------------------------------------------------------*/
         /* Now we know if the required collector values are present on this particular host   */
         /* so, if the host is suitable, add it to the list of candidate hosts for this action */
         /* -----------------------------------------------------------------------------------*/

         if (host_is_suitable) {
            safecpy(suitable_hosts[num_suitable_hosts].Hostname, host_collectorvalues[i].Hostname, 
                    sizeof(suitable_hosts[num_suitable_hosts].Hostname));
            safecpy(suitable_hosts[num_suitable_hosts].Hostselectorvalue, host_collectorvalues[i].Hostselectorvalue, 
                    sizeof(suitable_hosts[num_suitable_hosts].Hostselectorvalue));
            num_suitable_hosts++;   /* need a check for an array overflow ??? <<<<<<<<< */
         }

      }

      if (num_suitable_hosts == 0) {

         snprintf(msg, sizeof(msg), "   There are no candidate hosts that meet the collector requirements for this action\n");
         print_scenario_console_log(msg);

      } else {

       /*  snprintf(msg, sizeof(msg), "%s: The following candidate hosts meet the collector requirements for this action:\n", SUBNAME);
         print_scenario_console_log(msg);
   
         print_scenario_console_log_blank_timestamp("  ");
         for (i=0; i < num_suitable_hosts; i++) {
            print_scenario_console_log_string(suitable_hosts[i].Hostname);
            if (i < (num_suitable_hosts-1)) {
               print_scenario_console_log_string(", ");
            } 
            if ( ((i != 0) && ((i % 16) == 0)) || (i == (num_suitable_hosts-1))) {
               print_scenario_console_log_string("\n");
               if ((i != (num_suitable_hosts-1))) {
                  print_scenario_console_log_blank_timestamp("  ");
               }
            }
         } */
      }

   }

   if (required_values != NULL) free(required_values);

   /* We now have the list of suitable hosts which does not take into account the number of actions allocated to each host */
   /* If the action specifies that the primary host is to be used, then that is the host we will use (provided it is in    */
   /* the suitable host list                                                                                               */

   if (num_suitable_hosts == 0) {

     /* problem */  /* <<<<<<<<<<<<<<<<<<<< */
     snprintf(msg, sizeof(msg), "%s: number of suitable hosts = 0 - cannot proceed - Host scelector %s, Scenario ID %u, Stepnumber %u\n",
                   SUBNAME, action->Hostselectorvalue, ScenarioID, Stepnumber);
     print_scenario_console_log(msg);
     rc = 1;

   } else {

      /* <<<<<<<<<<< What stops us using the same host twice within the same step ?   */

      safecpy(hostname, "NOHOSTSELECTED", max_hostname_len);

      if (MATCH(action->Hostselectortype, "UsePrimaryHost")) {

         safecpy(hostname, primary_hostname, max_hostname_len);

         if (MATCH(hostname, "NOHOSTSELECTED")) {
            snprintf(msg, sizeof(msg), "%s: could not find primary host in hosts where %s in Scenario ID %u, Stepnumber %u\n",
                     SUBNAME, where, ScenarioID, Stepnumber);
            print_scenario_console_log(msg);
            rc = 1;
         
            /* <<<<<<<< need a better way to report this error */
         } else {
            snprintf(msg, sizeof(msg), "   Primary host %s selected for Scenario ID %u, Stepnumber %u\n",
                     hostname, ScenarioID, Stepnumber);
            print_scenario_console_log(msg);
         }

      } else { 

         int min_num_actions_allocated_in_this_scenario_step;
         int num_suitable_hosts_at_this_min = 0;
         int hi = -1;
         int sk = 0;
         int m,n;

         if (MATCH(action->Hostselectortype, "BySelectorValue")) sk = 1;

         for (m=0; m < num_suitable_hosts; m++) {
            if (sk) {
               if (!MATCH(suitable_hosts[m].Hostselectorvalue, action->Hostselectorvalue)) {
                  continue;
               }
            }
            if (m==0) hi = 0;
            int found = 0;
            suitable_hosts[m].Num_actions_allocated_in_this_scenario_step = 0;                   
            for (n=0; n < num_hosts_in_use_records; n++) {
               if (MATCH(suitable_hosts[m].Hostname, hosts_in_use[n].Hostname)) {
                  suitable_hosts[m].Num_actions_allocated_in_this_scenario_step = hosts_in_use[n].Num_actions_allocated_in_this_scenario_step;
                  found = 1;
                  break;
               }
            } 
            if (!found) {
               suitable_hosts[m].Num_actions_allocated_in_this_scenario_step = 0;
            }
            if (m == 0) {
               min_num_actions_allocated_in_this_scenario_step = suitable_hosts[m].Num_actions_allocated_in_this_scenario_step;
               num_suitable_hosts_at_this_min = 1;
               hi = m;
            } else {
               if (suitable_hosts[m].Num_actions_allocated_in_this_scenario_step < min_num_actions_allocated_in_this_scenario_step) {
                  min_num_actions_allocated_in_this_scenario_step = suitable_hosts[m].Num_actions_allocated_in_this_scenario_step;
                  num_suitable_hosts_at_this_min = 1;
                  hi = m;
               } else if (suitable_hosts[m].Num_actions_allocated_in_this_scenario_step == min_num_actions_allocated_in_this_scenario_step) {
                  num_suitable_hosts_at_this_min++;
               }
            }

         } 

               /* print_suitable_hosts(suitable_hosts, num_suitable_hosts);

               print_hosts_in_use(hosts_in_use, num_hosts_in_use_records);
               snprintf(msg, sizeof(msg), "min_num_actions_allocated_in_this_scenario_step = %d, num_suitable_hosts_at_this_min = %d\n",
                           min_num_actions_allocated_in_this_scenario_step, num_suitable_hosts_at_this_min);
               print_scenario_console_log(msg); */


         /* Now we have     num_suitable_hosts   the number of hosts we can choose from for this action  */
         /*                 min_num_actions_allocated_in_this_scenario_step   the smallest number of actions allocated to any host in this step */
         /*                 num_suitable_hosts_at_this_min    the number of hosts already allocated this minumum number of actions */
         /*      if   num_suitable_hosts_at_this_min <  num_suitable_hosts      */
         /*             then we must choose a host that currently has min_num_actions_allocated_in_this_scenario_step actions allocated */
         /*             if not, we can choose any suitable (non primary) host as all hosts are currently load balanced */

         if (MATCH(action->Hostselectortype, "Any")) {
            
            if (hi != -1) {
               safecpy(hostname, suitable_hosts[hi].Hostname, max_hostname_len);
               snprintf(msg, sizeof(msg), "   Host %s selected for Scenario ID %u, Stepnumber %u\n",
                        hostname, ScenarioID, Stepnumber);
               print_scenario_console_log(msg);
            }

         }  else if (MATCH(action->Hostselectortype, "BySelectorValue")) {

          /*  for (l=0; l<num_suitable_hosts; l++) {
               if (num_suitable_hosts_at_this_min < num_suitable_hosts) {
                  if (suitable_hosts[l].Num_actions_allocated_in_this_scenario_step == min_num_actions_allocated_in_this_scenario_step) {
                     if (MATCH(suitable_hosts[l].Hostselectorvalue, action->Hostselectorvalue)) {
                        safecpy(hostname, suitable_hosts[l].Hostname, max_hostname_len);
                        break;
                     }
                  }
               } else {
                  if (MATCH(suitable_hosts[l].Hostselectorvalue, action->Hostselectorvalue)) {
                     safecpy(hostname, suitable_hosts[l].Hostname, max_hostname_len);
                     break;
                  }
               }  
            }  */

            if (hi != -1) {
               safecpy(hostname, suitable_hosts[hi].Hostname, max_hostname_len);
               snprintf(msg, sizeof(msg), "   Host %s selected for Scenario ID %u, Stepnumber %u\n",
                        hostname, ScenarioID, Stepnumber);
               print_scenario_console_log(msg);
            }

            if (MATCH(hostname, "NOHOSTSELECTED")) {
               snprintf(msg, sizeof(msg), "%s: could not find group host where %s (selectorvalue %s) in Scenario ID %u, Stepnumber %u\n",
                        SUBNAME, where, action->Hostselectorvalue, ScenarioID, Stepnumber);
               print_scenario_console_log(msg);
               rc = 1;
            }

         } else {

            snprintf(msg, sizeof(msg), "%s: could not allocate host for hostselectorytype %s in Scenario ID %u, Stepnumber %u\n",
                     SUBNAME, action->Hostselectortype, ScenarioID, Stepnumber);
            print_scenario_console_log(msg);
            /* <<<<<<<< need a better way to report this error */
            rc = 1;

         }

      }

   }

   if (hosts_in_use   != NULL) free(hosts_in_use);
   if (suitable_hosts != NULL) free(suitable_hosts);
   if (hosts          != NULL) free(hosts);

   return(rc);

}

/* ------------------------------------------------------------------------------------------------- */
/*                                                                                                   */
/*                                                                                                   */
/*                                                                                                   */
/*                                                                                                   */
/*                                                                                                   */
/* ------------------------------------------------------------------------------------------------- */

int select_host_for_codelevel(MYSQL *sql_connection, int TeststandID, int CollectorsetID,
                              char *hostname, int max_hostname_len) {

#undef  SUBNAME
#define SUBNAME "select_host_for_codelevel"

   host_t                          *hosts = NULL;
   collectorset_required_values_t  *required_values = NULL;
   host_collectorvalues_t          *host_collectorvalues = NULL;
   int                             num_required_values_records;
   char                            where[512];
   int                             num_host_records;
   int                             num_host_collectorvalues_records;
   int                             i,j,k;
   char                            msg[MAX_MSG_LEN];
   int                             rc = 0;
   bool_t                          host_is_suitable;
   bool_t                          required_value_collector_found;
   int                             num_suitable_hosts = 0;

   safecpy(hostname, "", max_hostname_len);

   /* ---------------------------------------------------------------------------------------- */
   /* Determine which host to use - first get a list of all the hosts that are allocated to    */
   /* the teststand that is being used in this scenario.                                       */
   /* Then examine the collectorset for this step - and eliminate any hosts that do not have   */
   /* the desired collectorvalues                                                              */
   /* ---------------------------------------------------------------------------------------- */

   snprintf(where, sizeof(where), "TeststandID = %d", TeststandID);
   hosts = allocate_and_get_host_records(sql_connection, where, &num_host_records);

   /* snprintf(msg, sizeof(msg), "%s: The following %d host(s) are candidates for codelevel determination:\n", SUBNAME, num_host_records);
   print_scenario_console_log(msg); 
   
   print_scenario_console_log_blank_timestamp("  ");
   for (i=0; i < num_host_records; i++) {
      print_scenario_console_log_string(hosts[i].Name);
      if (i < (num_host_records-1)) {
         print_scenario_console_log_string(", ");
      } 
      if ( ((i != 0) && ((i % 16) == 0)) || (i == (num_host_records-1))) {
         print_scenario_console_log_string("\n");
         if ((i != (num_host_records-1))) {
            print_scenario_console_log_blank_timestamp("  ");
         }
      }
   } */

   required_values = allocate_and_get_required_collectorset_values(sql_connection, CollectorsetID, &num_required_values_records); 

   if (num_required_values_records == 0) {

      snprintf(msg, sizeof(msg), "   There are no required collector values for hosts in this action\n");
      print_scenario_console_log(msg);

      /* whathappens if there are no hosts in the test stand <<<< */
      safecpy(hostname, hosts[0].Name, max_hostname_len);

   } else {
   
      log_required_collectorset_values("   Required collectorvalues for testlevelmember host proxy are:", 
                                       required_values, num_required_values_records);

      host_collectorvalues = allocate_and_get_host_collectorvalues(sql_connection, TeststandID,
                                                                   &num_host_collectorvalues_records);   
 
      /* log_host_collectorvalues(&host_collectorvalues[0], num_host_collectorvalues_records); */

      for (i=0; i < num_host_collectorvalues_records; i++) {

         host_is_suitable = TRUE;

         for (j=0; j < num_required_values_records; j++) {

            required_value_collector_found = FALSE;
            for (k=0; k < host_collectorvalues[i].num_collectortypes_on_this_host; k++) {

               if (host_collectorvalues[i].CollectortypeIDs[k] == required_values[j].CollectortypeID) {

                  required_value_collector_found = TRUE;

                  if (MATCH(required_values[j].Comparator, "EQ")) {

                     if (! MATCH( host_collectorvalues[i].Collectorvalues[k], required_values[j].Requiredvalue)) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "NE")) {
                     if (MATCH( host_collectorvalues[i].Collectorvalues[k], required_values[j].Requiredvalue)) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "==")) {
                     if (! (strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) ==
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "!=")) {
                     if (! ( strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) ==
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "<")) {
                     if (! (strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) < 
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, "<=")) {
                     if (!( strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) <=
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                     }
                  } else if (MATCH(required_values[j].Comparator, ">")) {
                     if (! (strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) >
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                        host_is_suitable = FALSE;
                        break;
                    }
                  } else if (MATCH(required_values[j].Comparator, ">=")) {
                     if (! (strtoull(host_collectorvalues[i].Collectorvalues[k], NULL, 0) >=
                         strtoull(required_values[j].Requiredvalue, NULL, 0))) {
                       host_is_suitable = FALSE;
                       break;
                     }
                  } else {
                     /* problem */  
                     snprintf(msg, sizeof(msg), "%s: internal error - unexpected comparator (%s)\n", 
                              SUBNAME, required_values[j].Comparator);
                     print_scenario_console_log(msg);
                     break;
                  }
               }
            }
            if (! required_value_collector_found) {
               host_is_suitable = FALSE;
            }
         }

         /* -----------------------------------------------------------------------------------*/
         /* Now we know if the required collector values are present on this particular host   */
         /* so, if the host is suitable, add it to the list of candidate hosts for this action */
         /* -----------------------------------------------------------------------------------*/

         if (host_is_suitable) {
            safecpy(hostname, host_collectorvalues[i].Hostname, max_hostname_len);
            num_suitable_hosts++;   /* need a check for an array overflow ??? <<<<<<<<< */
            break;  /* we only need one host */
         }

      }

      if (num_suitable_hosts == 0) {

         snprintf(msg, sizeof(msg), "      There are no candidate hosts that meet the collector requirements for this object\n");
         print_scenario_console_log(msg);

      } else {

         snprintf(msg, sizeof(msg), "      %s met the host proxy collector requirements for this object\n", hostname);
         print_scenario_console_log(msg);

      }
   
   }

   if (hosts != NULL) free(hosts);
   if (required_values != NULL) free(required_values);
   if (host_collectorvalues != NULL) free(host_collectorvalues);

   if (num_suitable_hosts == 0) {

     rc = 1;
     /* problem */  /* <<<<<<<<<<<<<<<<<<<< */

   }

   return(rc);

}

/*---------------------------------------------------------------------------------------------------*/
/*                                                                                                   */
/* Function:   levelstuff                                                                            */
/*                                                                                                   */
/* Inputs:     sql_connection      An established connection to the status database                  */
/*                                                                                                   */
/* Outputs:    none                                                                                  */
/*                                                                                                   */
/* Returns:    ???                                                                                   */
/*                                                                                                   */
/* Function:                                                                                         */
/*                                                                                                   */
/*---------------------------------------------------------------------------------------------------*/

typedef struct levelstuff {
   char levelname[32]; /* <<< */
   char collectortypename[32]; /* <<< */
   int  proxycollectorsetID;
   char desired_level_value[256];
   char objecttype[32];  /* <<<< */
   char invocation[256];
   char OStype[32];
} levelstuff_t;

void print_levelstuff(levelstuff_t *l, int num_records) {
  int i;
  char msg[MAX_MSG_LEN];

  for (i=0; i<num_records; i++) {
     snprintf(msg, sizeof(msg), "Objectype %s, levelname %s:, desired value %s\n", l[i].objecttype, l[i].levelname, l[i].desired_level_value);
     print_scenario_console_log(msg);
     snprintf(msg, sizeof(msg), "   collectortypename %s, proxycollectorset %u, OStype %s, invocation %s\n",
                                l[i].collectortypename, l[i].proxycollectorsetID, l[i].OStype, l[i].invocation);
     print_scenario_console_log(msg);
   }
}

levelstuff_t *getlevelstuff(MYSQL *sql_connection, 
                         int testlevelID, 
                         int *num_records) {

#undef  SUBNAME
#define SUBNAME "levelstuff"

   char         query[1024];
   char         msg[MAX_MSG_LEN];
   MYSQL_RES    *res_set;
   MYSQL_ROW    row;
   levelstuff_t *p, *q = NULL;
   int num_rows;

   *num_records = 0;
   snprintf(query, sizeof(query), 
            "SELECT level.Name as 'Levelname', collectortype.Name as 'Collectortypename', level.ProxycollectorsetID, " 
            "levelinstance.Value, objecttype.Name as 'Objecttype', collectortype.Invocation, collectortype.OStype " 
            "FROM testlevelmember "
            "INNER JOIN levelinstance ON testlevelmember.LevelinstanceID = levelinstance.ID "
            "INNER JOIN level ON levelinstance.levelID = level.ID "
            "INNER JOIN collectortype ON level.collectortypeID = collectortype.ID "
            "INNER JOIN objecttype ON level.objecttypeID = objecttype.ID "
            "WHERE TestlevelID ='%u'", testlevelID);


   if (perform_query(SUBNAME, sql_connection, query) != 0) {

     printf("%s: perform_query failed on %s\n", SUBNAME, query);
     /* problem <<< */

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         num_rows = mysql_num_rows(res_set);

         p = q = malloc( num_rows * sizeof(levelstuff_t) );

         while (((row = mysql_fetch_row(res_set)) != NULL) && (*num_records < num_rows)) {

            int i;

            for (i = 0; i < mysql_num_fields(res_set); i++) {

               switch(i) {
                  case 0:
                     safecpy(p->levelname, row[i] , sizeof(p->levelname)); break; 
                  case 1:
                     safecpy(p->collectortypename, row[i] , sizeof(p->collectortypename)); break; 
                  case 2:
                     p->proxycollectorsetID = (Iu32) strtoull(row[i], NULL, 0);
                  case 3:
                     safecpy(p->desired_level_value, row[i] , sizeof(p->desired_level_value)); break; 
                  case 4:
                     safecpy(p->objecttype, row[i] , sizeof(p->objecttype)); break; 
                  case 5:
                     safecpy(p->invocation, row[i] , sizeof(p->invocation)); break; 
                  case 6:
                     safecpy(p->OStype, row[i] , sizeof(p->OStype)); break; 
                  default:
                     /* problem */
                     snprintf(msg, sizeof(msg), "%s: internal error - unexpected switch index value i=%d (more fields than expected in mysql row\n", 
                              SUBNAME, i);
                     print_msg_to_console(msg);
                     break;
               }
            }
            p++;
            (*num_records)++;

         }
         mysql_free_result(res_set);

      }
   }
   return q;

}


/*---------------------------------------------------------------------------------------------------*/
/*                                                                                                   */
/* Function:   validate_object_code_levels                                                           */
/*                                                                                                   */
/* Inputs:     sql_connection      An established connection to the status database                  */
/*                                                                                                   */
/*             TestlevelID                                                                           */
/*                                                                                                   */
/*             testlevel_name                                                                        */
/*                                                                                                   */
/*             TeststandID                                                                           */
/*                                                                                                   */
/*             teststand_name                                                                        */
/*                                                                                                   */
/*             Project                                                                               */
/*                                                                                                   */
/*             Phase                                                                                 */
/*                                                                                                   */
/*             TestlevelrecordID                                                                     */
/*                                                                                                   */
/*                                                                                                   */
/* Outputs:    none                                                                                  */
/*                                                                                                   */
/* Returns:    E_OK for success, 1 if the code levels do not validate or a failure occurred          */
/*                                                                                                   */
/* Function:                                                                                         */
/*                                                                                                   */
/*---------------------------------------------------------------------------------------------------*/

int validate_object_code_levels(MYSQL *sql_connection, int TestlevelID, char *testlevel_name, 
                                int TeststandID, char *teststand_name, char *Project, char *Phase, int TestlevelrecordID) {

#undef  SUBNAME
#define SUBNAME "validate_object_code_levels"

   CLIENT                            *remote_client;
   remote_client_run_cmd_args        run_cmd_args;
   remote_client_run_cmd_res         run_cmd_result;
   enum clnt_stat                    stat;
   static struct timeval             TIMEOUT = { 25, 0 };

   levelstuff_t        *levelstuff;
   int                 num_levelstuff_records;
   int                 num_object_records;
   int                 i, j;
   int                 rc = 0;
   int                 return_code = 0;
   char                msg[MAX_MSG_LEN];
   char                errmsg[MAX_MSG_LEN];
   object_name_chain_t *object_name_chain;
   char                *objectname;
   char                hostname[DAF_HOST_NAME_LEN];  
   char                testlevelrecordmember_state[32];
   char                tablename[64];
   char                status_database_name[DAF_DATABASE_NAME_LEN]; 

   char *fields[] = {"TestlevelrecordID", "Levelname", "Objecttype", "Levelinstancevalue", "State", NULL};
   char *values[6];

   char S_TestlevelrecordID[16];
   char S_Levelname[32];
   char S_Objecttype[32];
   char S_Levelinstancevalue[256];
   char S_State[32];

   values[0] = &S_TestlevelrecordID[0];
   values[1] = &S_Levelname[0];
   values[2] = &S_Objecttype[0];
   values[3] = &S_Levelinstancevalue[0];
   values[4] = &S_State[0];
   values[5] = NULL;

   snprintf(msg, sizeof(msg), "Validating object code levels in TeststandID %d (%s) against Testlevel %d (%s)\n", 
                              TeststandID, teststand_name, TestlevelID, testlevel_name);
   print_scenario_console_log(msg);

   levelstuff = getlevelstuff(sql_connection, TestlevelID, &num_levelstuff_records);
   /* print_levelstuff(levelstuff, num_levelstuff_records); */

   if (levelstuff != NULL) { 
      for (i=0; i< num_levelstuff_records; i++) {
          snprintf(msg, sizeof(msg), "   Testlevelmember: levelname = %s, objecttype = %s, desiredvalue = %s\n",
                                     levelstuff[i].levelname, 
                                     levelstuff[i].objecttype, 
                                     levelstuff[i].desired_level_value);
          print_scenario_console_log(msg);
          safecpy(testlevelrecordmember_state, "unknown", sizeof(testlevelrecordmember_state));

          object_name_chain = find_objects_in_teststand(sql_connection, TeststandID, levelstuff[i].objecttype, &num_object_records);

          if (object_name_chain != NULL) {

             for (j=0; j<num_object_records; j++) {
                objectname = get_next_object_name_from_chain(&object_name_chain);
                /* snprintf(msg, sizeof(msg), "objectname = %s, required level value = %s, proxy collector set ID = %u, " 
                                           "OStype = %s, collector type = %s, collector invocation = %s\n", 
                                           objectname,
                                           levelstuff[i].desired_level_value,
                                           levelstuff[i].proxycollectorsetID,
                                           levelstuff[i].OStype,
                                           levelstuff[i].collectortypename,
                                           levelstuff[i].invocation); 
                print_scenario_console_log(msg); */
             }

             if (select_host_for_codelevel(sql_connection, TeststandID, levelstuff[i].proxycollectorsetID, hostname, 
                                           sizeof(hostname)) == E_OK) {

                /* ------------------------------------------------------------------------------------------------------ */
                /* Send a request to the remote agent to run the collector and get the value back                         */
                /* ------------------------------------------------------------------------------------------------------ */

                remote_client = clnt_create(hostname, DAF_PROG, DAF_VERSION, "tcp");

                /* ------------------------------------------------------------------------------------------------------ */
                /* Connect to the host - if the connection cannot be established, skip thist host and go onto the next    */
                /* one immediately                                                                                        */
                /* ------------------------------------------------------------------------------------------------------ */

                if (remote_client == (CLIENT *) NULL) {

                   snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, hostname);
                   snprintf(msg,    sizeof(msg),    "%s\n", clnt_spcreateerror(errmsg));
                   print_scenario_console_log(msg);
                   return_code = 1;
                   safecpy(testlevelrecordmember_state, "proxy_collectorhost_no_connect", sizeof(testlevelrecordmember_state));

                } else {

                   memset(&run_cmd_args,   0, sizeof(run_cmd_args));
                   memset(&run_cmd_result, 0, sizeof(run_cmd_result));

                   run_cmd_args.cmdstring = levelstuff[i].invocation;
                   run_cmd_args.msglevel = 0;

                   if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_RUN_CMD,
                                         (xdrproc_t) xdr_remote_client_run_cmd_args, (caddr_t) &run_cmd_args,
                                         (xdrproc_t) xdr_remote_client_run_cmd_res,  (caddr_t) &run_cmd_result,
                                         TIMEOUT)) != RPC_SUCCESS) {
                      clnt_stat_decode(stat, errmsg, sizeof(errmsg));
                      snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n",
                               SUBNAME, (long) CLIENT_REMOTE_CLIENT_RUN_CMD, stat, errmsg);
                      print_scenario_console_log(msg);
                      return_code = 1;
                      safecpy(testlevelrecordmember_state, "proxy_collectorhost_fail", sizeof(testlevelrecordmember_state));

                   } else {

                      if (run_cmd_result.remote_client_run_cmd_res_u.outcome.valid) {
                         if (MATCH( levelstuff[i].desired_level_value, 
                                    run_cmd_result.remote_client_run_cmd_res_u.outcome.outputline )) {
                                    snprintf(msg, sizeof(msg), "      Collectortype (%s) collectorvalue (%s) from object %s matches required value %s\n",
                                             levelstuff[i].collectortypename, run_cmd_result.remote_client_run_cmd_res_u.outcome.outputline, objectname,
                                             levelstuff[i].desired_level_value);
                            print_scenario_console_log(msg);
                            safecpy(testlevelrecordmember_state, "match", sizeof(testlevelrecordmember_state));
                            /* good */
                         } else {
                            snprintf(msg, sizeof(msg), "      Collectortype (%s) collectorvalue (%s) from object %s does not match required value %s\n",
                                     levelstuff[i].collectortypename, run_cmd_result.remote_client_run_cmd_res_u.outcome.outputline, objectname,
                                     levelstuff[i].desired_level_value);
                            print_scenario_console_log(msg);
                            /* bad */
                            return_code = 1;
                            safecpy(testlevelrecordmember_state, "no_match", sizeof(testlevelrecordmember_state));
                         }
                      } else {
                            snprintf(msg, sizeof(msg), "%s: Could not obtain collectorvalue for collectortype (%s) from object %s of type %s\n",
                                         SUBNAME, levelstuff[i].collectortypename, objectname, levelstuff[i].objecttype);
                            print_scenario_console_log(msg);
                            snprintf(msg, sizeof(msg), "%s: Collector invocation on host %s was %s\n",
                                         SUBNAME, hostname, levelstuff[i].invocation);
                            print_scenario_console_log(msg);
                            /* bad  - fail */
                           return_code = 1;
                           safecpy(testlevelrecordmember_state, "proxy_collectorhost_fail", sizeof(testlevelrecordmember_state));
                      }
                   }

                   if (remote_client->cl_auth != NULL) {
                      auth_destroy(remote_client->cl_auth);
                   }

                   clnt_destroy(remote_client);

                }
             }

             free(object_name_chain);

          } else {
             snprintf(msg, sizeof(msg), "   There are no objects of type %s in teststandID %u\n", levelstuff[i].objecttype, TeststandID);
             print_scenario_console_log(msg);
             safecpy(testlevelrecordmember_state, "no_objects", sizeof(testlevelrecordmember_state));
          }

          /* ------------------------------------------------------------------------------------------------------ */
          /* Now create a testlevelrecordmember for this level containing the following fields                      */
          /*    ID                                                                                                  */
          /*    TestlevelrecordID                                                                                   */
          /*    Levelname                 levelstuff[i].levelname                                                   */
          /*    Objecttype                levelstuff[i].objecttype                                                  */
          /*    Levelinstancevalue        levelstuff[i].desired_level_value                                         */
          /*    State                     'unknown'                                                                 */
          /*                              'proxy_collectorhost_no_connect'                                          */
          /*                              'proxy_collectorhost_fail'                                                */
          /*                              'match'     The desired level was found on the object                     */
          /*                              'no_match'  The desired level was not found on the object                 */
          /*                                                                                                        */
          /* ------------------------------------------------------------------------------------------------------ */

          snprintf(S_TestlevelrecordID, sizeof(S_TestlevelrecordID), "%d",  TestlevelrecordID);
          safecpy(S_Levelname, levelstuff[i].levelname, sizeof(S_Levelname));
          safecpy(S_Objecttype, levelstuff[i].objecttype, sizeof(S_Objecttype));
          safecpy(S_Levelinstancevalue, levelstuff[i].desired_level_value, sizeof(S_Levelinstancevalue));
          safecpy(S_State, testlevelrecordmember_state, sizeof(S_State));

          determine_status_database_name(Project, Phase, status_database_name, sizeof(status_database_name));
          determine_status_table_name(status_database_name, "testlevelrecordmember", tablename, sizeof(tablename));

          rc = perform_insert(SUBNAME, sql_connection,
                              tablename,
                              fields,
                              values,
                              errmsg,
                              sizeof(errmsg));

           if (rc != 0) {
              snprintf(msg, sizeof(msg), "%s: testlevelrecordmember - problem with sql insert - %s\n", SUBNAME, errmsg);
              print_scenario_console_log(msg);
              return_code = 1;
           } 
       }

    } else {
       snprintf(msg, sizeof(msg), "levelstuff() returned NULL pointer\n");
       print_scenario_console_log(msg);
       return_code = 1;
    }

    if (levelstuff != NULL) {
       free(levelstuff);
    }

    return return_code;

}
/*---------------------------------------------------------------------------------------------------*/
/*                                                                                                   */
/* Function:   spawn_new_workrequest_record                                                          */
/*                                                                                                   */
/* Inputs:     sql_connection      An established connection to the status database                  */
/*                                                                                                   */
/* Outputs:    none                                                                                  */
/*                                                                                                   */
/* Returns:         rc                                                                               */
/*                                                                                                   */
/* Function:                                                                                         */
/*                                                                                                   */
/*---------------------------------------------------------------------------------------------------*/

int spawn_new_workrequest(MYSQL *sql_connection, 
                          int   archive_workrequestID,
                          int   new_scenarioID,
                          char  *Project,
                          char  *Phase) {

#undef  SUBNAME
#define SUBNAME "spawn_new_workqueue_record"

   int         rc = 0;
   char        msg[MAX_MSG_LEN];
   char        errmsg[MAX_MSG_LEN];
   char        query[1024];
   char        status_database_name[DAF_DATABASE_NAME_LEN]; 
   char        tablename[64];
   int         scenarioresultID;
   char        loglocation[128];
   int         num_actions_in_scenario;
   char        temp[256];

   workrequest_t    original_workrequest;
   scenarioresult_t original_scenarioresult;
   scenario_t       new_scenario;

   char        S_Project[64];
   char        S_Phase[64];
   char        S_Scenarioname[32];
   char        S_Jobname[48];
   char        S_RequestType[16];
   char        S_TesterID[16];
   char        S_State[24];
   char        S_Teststand[32];
   char        S_TeststandrecordID[16];
   char        S_Testlevel[32];
   char        S_TestlevelrecordID[16];
   char        S_Tester[32];
   char        S_Comments[128];
   char        S_Tablename1[32];
   char        S_ObjectID1[16];
   char        S_Tablename2[32];
   char        S_ObjectID2[16];
   char        S_Tablename3[32];
   char        S_ObjectID3[16];
   char        S_Tablename4[32];
   char        S_ObjectID4[16];
   char        S_Start[16];
   char        S_End[16];
   char        S_Logdirectory[128];
   char        S_Scenariologfilename[64];

   /* scenarioresult */
   char        *fields[]  = {"Project", "Phase", "Scenarioname", "Jobname", "Teststand", "TeststandrecordID", "Testlevel", "TestlevelrecordID", 
                             "Tester", "Comments", "State", NULL};
   char        *values[12];

   values[0] = &S_Project[0];
   values[1] = &S_Phase[0];
   values[2] = &S_Scenarioname[0];
   values[3] = &S_Jobname[0];
   values[4] = &S_Teststand[0];
   values[5] = &S_TeststandrecordID[0];
   values[6] = &S_Testlevel[0];
   values[7] = &S_TestlevelrecordID[0];
   values[8] = &S_Tester[0];
   values[9] = &S_Comments[0];
   values[10]= &S_State[0];
   values[11] = NULL;

   char        *fields1[] = {"Project", "Phase", "RequestType", "TesterID", "State", "Tablename1", "ObjectID1",
                             "Tablename2", "ObjectID2", "Tablename3", "ObjectID3",
                             "Tablename4", "ObjectID4", "Start", "End",  
                             "Logdirectory", "Scenariologfilename", NULL};
   char        *values1[19];

   values1[0]  = &S_Project[0];
   values1[1]  = &S_Phase[0];
   values1[2]  = &S_RequestType[0];
   values1[3]  = &S_TesterID[0];
   values1[4]  = &S_State[0];
   values1[6]  = &S_Tablename1[0];
   values1[7]  = &S_ObjectID1[0];
   values1[8]  = &S_Tablename2[0];
   values1[9]  = &S_ObjectID2[0];
   values1[10] = &S_Tablename3[0];
   values1[11] = &S_ObjectID3[0];
   values1[12] = &S_Tablename4[0];
   values1[13] = &S_ObjectID4[0];
   values1[14] = &S_Start[0];
   values1[15] = &S_End[0];
   values1[16] = &S_Logdirectory[0];
   values1[17] = &S_Scenariologfilename[0];
   values1[18] = NULL;

   print_scenario_console_log_rule();
   snprintf(msg, sizeof(msg), "Creating workrequest item for Scenario ID %u\n", new_scenarioID);
   print_scenario_console_log(msg);   /* how do we know this is open for the correct console scenario log file ?? <<<<< */

   determine_status_database_name(Project, Phase, status_database_name, sizeof(status_database_name));   
   determine_status_table_name(status_database_name, "archive_workrequest", tablename, sizeof(tablename));   

   /* too many SQL calls here <<<< */
   get_scenario(sql_connection, new_scenarioID, &new_scenario);
   get_workrequest(sql_connection, archive_workrequestID, tablename, &original_workrequest);

   /* --------------------------------------------------------------------------------------------- */
   /* create the scenarioresult record for this new job                                             */
   /* --------------------------------------------------------------------------------------------- */

   determine_status_table_name(status_database_name, "scenarioresult", tablename, sizeof(tablename));   
   get_scenarioresult(sql_connection, original_workrequest.ObjectID2, tablename, &original_scenarioresult);

   /* scenariologfilename  is   scenarioname_YYYYMMDD.hhmmss   . "_" . date("Ymd.his");    <<< make this into a routine and in the php */

   safecpy(S_Project,             original_workrequest.Project,              sizeof(S_Project));
   safecpy(S_Phase,               original_workrequest.Phase,                sizeof(S_Phase));
   safecpy(S_Scenarioname,        new_scenario.Name,                         sizeof(S_Scenarioname));    
   safecpy(S_Jobname, new_scenario.Name, sizeof(S_Jobname));    
   make_legal_filename(S_Jobname);
   safecpy(S_Teststand,           original_scenarioresult.Teststand,         sizeof(S_Teststand));  
   snprintf(S_TeststandrecordID,  sizeof(S_TeststandrecordID), "%u", original_scenarioresult.TeststandrecordID);  
   safecpy(S_Testlevel,           original_scenarioresult.Testlevel,         sizeof(S_Teststand));  
   snprintf(S_TestlevelrecordID,  sizeof(S_TestlevelrecordID), "%u", original_scenarioresult.TestlevelrecordID);  
   safecpy(S_Tester,              original_scenarioresult.Tester,            sizeof(S_Tester));  
   snprintf(temp, sizeof(temp),   "chained from scenario %s (ID=%u)", 
            original_scenarioresult.Scenarioname, original_workrequest.ObjectID1 );
   safecpy(S_Comments,            temp,                                      sizeof(S_Comments));  
   safecpy(S_State,               "Initialised",                             sizeof(S_State));  

   rc = perform_insert(SUBNAME, sql_connection, tablename, fields, values, errmsg, sizeof(errmsg));
   if (rc != 0) {
      print_scenario_console_log(errmsg);   /* how do we know this is open for the correct console scenario log file ?? <<<<< */
      return rc;
   }

   scenarioresultID = (Iu32) mysql_insert_id(sql_connection); 

   num_actions_in_scenario = count_actions_in_scenario(sql_connection, new_scenarioID);

   safecpy(S_Scenariologfilename, "console", sizeof(S_Scenariologfilename));
   safecat(S_Scenariologfilename, "_", sizeof(S_Scenariologfilename));
   safecpy(S_Scenariologfilename, S_Jobname, sizeof(S_Scenariologfilename));
   safecat(S_Scenariologfilename, "_", sizeof(S_Scenariologfilename));
   snprintf(temp, sizeof(temp), "%u", scenarioresultID);
   safecat(S_Scenariologfilename, temp, sizeof(S_Scenariologfilename)); 

/* safecat(S_Scenariologfilename, "_", sizeof(S_Scenariologfilename));
   get_current_time_as_timestamp(timestamp, sizeof(timestamp), '.');
   safecat(S_Scenariologfilename, timestamp, sizeof(S_Scenariologfilename)); */
   safecat(S_Scenariologfilename, ".out", sizeof(S_Scenariologfilename)); 

   /* loglocation = $gp['loglocation'] . '/' . 'scenario_' . $scenarioresultID . '_' . $sanitised_name . "_" . date("Ymd");  <<<< */
   safecpy(loglocation, original_workrequest.Logdirectory, sizeof(loglocation));   
   safecat(loglocation, "/scenario_", sizeof(loglocation));
   snprintf(temp, sizeof(temp), "%u", scenarioresultID);
   safecat(loglocation, temp, sizeof(loglocation));
   safecat(loglocation, "_", sizeof(loglocation));
   safecat(loglocation, S_Jobname, sizeof(loglocation));

   snprintf(query, sizeof(query), "UPDATE %s SET Loglocation = '%s', Scenariologfilename = '%s', Actionsinscenario = '%u' WHERE ID='%u'",
            tablename, loglocation, S_Scenariologfilename, num_actions_in_scenario, scenarioresultID);
              
   perform_query(SUBNAME, sql_connection, query);   /* what if this query fails? <<<<<< */

   /* --------------------------------------------------------------------------------------------- */
   /* create the workrequest record for this new job                                                */
   /* --------------------------------------------------------------------------------------------- */

   safecpy(S_RequestType,   original_workrequest.RequestType,    sizeof(S_RequestType));
   snprintf(S_TesterID, sizeof(S_TesterID), "%u", original_workrequest.TesterID);
   safecpy(S_State,         "Idle",                              sizeof(S_State));    /* why does not the GUI set this <<<< */
   safecpy(S_Tablename1,    original_workrequest.Tablename1,     sizeof(S_Tablename1));
   snprintf(S_ObjectID1, sizeof(S_ObjectID1),   "%d", new_scenarioID);                      /* scenario */
   safecpy(S_Tablename2,    original_workrequest.Tablename2,     sizeof(S_Tablename2));
   snprintf(S_ObjectID2, sizeof(S_ObjectID2), "%u", scenarioresultID);                      /* scenarioresult */
   safecpy(S_Tablename3,    original_workrequest.Tablename3,     sizeof(S_Tablename3));
   snprintf(S_ObjectID3, sizeof(S_ObjectID3), "%u", original_workrequest.ObjectID3);        /* testlevel */
   safecpy(S_Tablename4,    original_workrequest.Tablename4,     sizeof(S_Tablename4));
   snprintf(S_ObjectID4, sizeof(S_ObjectID4), "%u", original_workrequest.ObjectID4);        /* teststand */
   safecpy(S_Start,         "NOW()",                             sizeof(S_Start));
   safecpy(S_Logdirectory,  original_workrequest.Logdirectory,   sizeof(S_Logdirectory)); 

   rc = perform_insert(SUBNAME, 
                       sql_connection,
                       "daf.workrequest",
                       fields1,
                       values1,
                       errmsg,
                       sizeof(errmsg));

   if (rc != 0) {
      print_scenario_console_log(msg);   /* how do we know this is open for the correct console scenario log file ?? <<<<< */
   }

   return rc;

}


/*---------------------------------------------------------------------------------------------------*/
/*                                                                                                   */
/* Function:   create_new_workqueue_record                                                           */
/*                                                                                                   */
/* Inputs:     sql_connection      An established connection to the status database                  */
/*                                                                                                   */
/* Outputs:    none                                                                                  */
/*                                                                                                   */
/* Returns:         rc                                                                               */
/*                                                                                                   */
/* Function:                                                                                         */
/*                                                                                                   */
/*---------------------------------------------------------------------------------------------------*/

int create_new_workqueue_record(MYSQL          *sql_connection, 
                                char           *Project,
                                char           *Phase,
                                action_t       *actions, 
                                int            num_action_records,
                                actiontype_t   *actiontypes,
                                int            num_actiontype_records,
                                testcase_t     *testcases, 
                                int            num_testcase_records,
                                int            index,
                                int            ScenarioID, 
                                int            Stepnumber, 
                                int            ScenarioresultID,
                                int            TestlevelID,
                                int            TeststandID,
                                int            OutcomeactionID,
                                int            TesterID,
                                int            MaillistID,
                                char           *Loglocation,
                                char           *Scenariologfilename) {

#undef  SUBNAME
#define SUBNAME "create_new_workqueue_record"

   int         rc = 0;
   char        msg[MAX_MSG_LEN];
   char        errmsg[MAX_MSG_LEN];
   char        actiontype[32];
   char        testcase[32];

   char        S_Project[64];
   char        S_Phase[64];
   char        S_ScenarioID[16];
   char        S_TestlevelID[16];
   char        S_TeststandID[16];
   char        S_ScenarioresultID[16];
   char        S_EnvironmentID[16];
   char        S_OutcomeactionID[16];
   char        S_TesterID[16];
   char        S_MaillistID[16];
   char        S_Actiontype[32];
   char        S_Stepnumber[16];
   char        S_Hostname[16];
   char        S_State[24];
   char        S_Statemodifier[32];
   char        S_Testcase[32];
   char        S_Invocation[1024];
   char        S_Duration[24];
   char        S_Maxduration[16];
   char        S_Loglocation[128];
   char        S_Scenariologfilename[128];
   char        S_Start[24];
   char        *fields1[] = {"Project", "Phase", "ScenarioID", "TestlevelID", "TeststandID", "ScenarioresultID", 
                             "EnvironmentID", "OutcomeactionID", "TesterID", "MaillistID", "Actiontype",
                             "Stepnumber", "Hostname", "State", "Statemodifier", "Testcase", "Invocation", "Duration", 
                             "Maxduration", "Loglocation", "Scenariologfilename", "Start", NULL};
   char        *values1[23];

   values1[0] = &S_Project[0];
   values1[1] = &S_Phase[0];
   values1[2] = &S_ScenarioID[0];
   values1[3] = &S_TestlevelID[0];
   values1[4] = &S_TeststandID[0];
   values1[5] = &S_ScenarioresultID[0];
   values1[6] = &S_EnvironmentID[0];
   values1[7] = &S_OutcomeactionID[0];
   values1[8] = &S_TesterID[0];
   values1[9] = &S_MaillistID[0];
   values1[10] = &S_Actiontype[0];
   values1[11] = &S_Stepnumber[0];
   values1[12] = &S_Hostname[0];
   values1[13] = &S_State[0];
   values1[14] = &S_Statemodifier[0];
   values1[15] = &S_Testcase[0];
   values1[16] = &S_Invocation[0];
   values1[17] = &S_Duration[0];
   values1[18] = &S_Maxduration[0];
   values1[19] = &S_Loglocation[0];
   values1[20] = &S_Scenariologfilename[0];
   values1[21] = &S_Start[0];
   values1[22] = NULL;

   lookup_actiontype(actiontypes,  num_actiontype_records, (actions+index)->ActiontypeID, actiontype, sizeof(actiontype));
   lookup_testcase(testcases,      num_testcase_records,   (actions+index)->TestcaseID,   testcase,   sizeof(testcase));

   print_scenario_console_log_rule();
   snprintf(msg, sizeof(msg), "Creating workqueue item for Scenario ID %u, step %u, action %s, testcase %s\n", 
            ScenarioID, Stepnumber, actiontype, testcase);
   print_scenario_console_log(msg);   /* how do we know this is open for the correct console scenario log file ?? <<<<< */

   if (select_host_for_action(sql_connection, ScenarioID, ScenarioresultID, Stepnumber, TeststandID, (actions+index), S_Hostname, sizeof(S_Hostname)) == 0) {

      safecpy(S_State,           "Notstarted", sizeof(S_State));
      safecpy(S_Statemodifier,   "None",       sizeof(S_Statemodifier));

   } else {

      snprintf(msg, sizeof(msg), "%s: Could not select host for Scenario ID %u step %u\n", SUBNAME, ScenarioID, Stepnumber);
      print_scenario_console_log(msg);   /* how do we know this is open for the correct console scenario log file ?? <<<<< */

      safecpy(S_State,         "Notstarted",           sizeof(S_State));
      safecpy(S_Statemodifier, "No_available_host",    sizeof(S_Statemodifier));

   }

   safecpy(S_Project, Project, sizeof(S_Project));
   safecpy(S_Phase,   Phase,   sizeof(S_Phase));
   snprintf(S_ScenarioID,       sizeof(S_ScenarioID),       "%d", ScenarioID);
   snprintf(S_TestlevelID,      sizeof(S_TestlevelID),      "%d", TestlevelID);
   snprintf(S_TeststandID,      sizeof(S_TeststandID),      "%d", TeststandID);
   snprintf(S_ScenarioresultID, sizeof(S_ScenarioresultID), "%d", ScenarioresultID);
   snprintf(S_OutcomeactionID,  sizeof(S_OutcomeactionID),  "%d", OutcomeactionID);
   snprintf(S_TesterID,         sizeof(S_MaillistID),       "%d", TesterID);   
   snprintf(S_MaillistID,       sizeof(S_MaillistID),       "%d", MaillistID);
   snprintf(S_Stepnumber,       sizeof(S_Stepnumber),       "%d", (actions+index)->Stepnumber);
   snprintf(S_EnvironmentID,    sizeof(S_EnvironmentID),    "%d", (actions+index)->EnvironmentID);
   snprintf(S_Maxduration,      sizeof(S_Maxduration),      "%d", (actions+index)->Maxduration);

   safecpy(S_Duration,   (actions+index)->Duration, sizeof(S_Duration));
   safecpy(S_Actiontype,  actiontype,               sizeof(S_Actiontype));
   safecpy(S_Testcase,    testcase,                 sizeof(S_Testcase));
   safecpy(S_Invocation,  testcase,                 sizeof(S_Invocation));
   safecat(S_Invocation,  " ",                      sizeof(S_Invocation));
   safecat(S_Invocation,  (actions+index)->Parameters,  sizeof(S_Invocation));
   safecpy(S_Loglocation, Loglocation,                  sizeof(S_Loglocation));  
   safecpy(S_Scenariologfilename, Scenariologfilename,  sizeof(S_Scenariologfilename));  
   safecpy(S_Start,       "XXXX_CURRENT_TIME_XXXX", sizeof(S_Start));

   rc = perform_insert(SUBNAME,
                       sql_connection,
                       "daf.workqueue",
                       fields1,
                       values1,
                       errmsg,
                       sizeof(errmsg));

   if (rc != 0) {
      print_scenario_console_log(errmsg);   /* how do we know this is open for the correct console scenario log file ?? <<<<< */
   }

   return rc;

}

/*---------------------------------------------------------------------------------------------------*/
/*                                                                                                   */
/* Function:   create_actions_for_step                                                               */
/*                                                                                                   */
/* Inputs:     sql_connection      An established connection to the status database                  */
/*                                                                                                   */
/* Outputs:    none                                                                                  */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:                                                                                         */
/*                                                                                                   */
/*---------------------------------------------------------------------------------------------------*/

int create_actions_for_step(MYSQL           *sql_connection,
                            char            *Project,
                            char            *Phase, 
                            action_t        *actions,
                            int             num_action_records,
                            actiontype_t    *actiontypes,
                            int             num_actiontype_records,
                            testcase_t      *testcases,
                            int             num_testcase_records,
                            int             ScenarioID,
                            int             Stepnumber,
                            int             ScenarioresultID,
                            int             TestlevelID,
                            int             TeststandID,
                            int             OutcomeactionID,
                            int             TesterID,   
                            int             MaillistID,
                            char            *Loglocation,
                            char            *Scenariologfilename) {

#undef  SUBNAME
#define SUBNAME "create_actions_for_step"

   int k;
   int rc = 0;

   for (k=0; k<num_action_records; k++) {

      rc = rc |  create_new_workqueue_record(sql_connection,
                                             Project,
                                             Phase,
                                             actions,
                                             num_action_records,
                                             actiontypes,
                                             num_actiontype_records,
                                             testcases,
                                             num_testcase_records,
                                             k,
                                             ScenarioID,
                                             Stepnumber,
                                             ScenarioresultID,
                                             TestlevelID,
                                             TeststandID,
                                             OutcomeactionID,
                                             TesterID,
                                             MaillistID,
                                             Loglocation,
                                             Scenariologfilename);

   }

   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    email_scenario_completion                                                                */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int email_scenario_completion(user_t *user, char *status_table_name, 
                              scenarioresult_t *scenarioresult, 
                              outcomeaction_t *outcomeaction, char *errmsg, int max_errmsg_len) {
   
#undef  SUBNAME
#define SUBNAME "email_scenario_completion"

   char subject[128];
   char reportline[22][64];
   char passfail[16];
   char *userlist[] = {user->Email, NULL};
   char *lines[] = {reportline[0], reportline[1], reportline[2], reportline[3], reportline[4], reportline[5],
                    reportline[6], reportline[7], reportline[8], reportline[9], reportline[10], reportline[11],
                    reportline[12], reportline[13], reportline[14], reportline[15], reportline[16], 
                    reportline[17], reportline[18], reportline[19], reportline[20], reportline[21], NULL};
   int rc = 0;

   errmsg[0] = 0;

   if (scenarioresult->Pass == 100) {
      safecpy(passfail, "PASSED", sizeof(passfail));
   } else {
      safecpy(passfail, "FAILED", sizeof(passfail));
   }

   snprintf(subject, sizeof(subject), "Scenario %s, Scenarioresult %u has %s\n", 
                                      scenarioresult->Scenarioname, scenarioresult->ID, passfail);
   snprintf(reportline[0],  sizeof(reportline[0]),  "Project:              %s\n",    scenarioresult->Project);
   snprintf(reportline[1],  sizeof(reportline[1]),  "Phase:                %s\n",    scenarioresult->Phase);
   snprintf(reportline[2],  sizeof(reportline[2]),  "Scenario:             %s\n",    scenarioresult->Scenarioname);
   snprintf(reportline[3],  sizeof(reportline[3]),  "Jobname:              %s\n",    scenarioresult->Jobname);
   snprintf(reportline[4],  sizeof(reportline[4]),  "Teststand:            %s\n",    scenarioresult->Teststand);
   snprintf(reportline[5],  sizeof(reportline[5]),  "Testlevel:            %s\n",    scenarioresult->Testlevel);

   snprintf(reportline[6],  sizeof(reportline[6]),  "Tester:               %s %s\n", user->Firstname, user->Lastname);

   snprintf(reportline[7],  sizeof(reportline[7]),  "Actiononpass:         %s\n",    outcomeaction->Actiononpass);
   snprintf(reportline[8],  sizeof(reportline[8]),  "Actiononfail:         %s\n",    outcomeaction->Actiononfail);
   snprintf(reportline[9],  sizeof(reportline[9]),  "Scenariotoberunonpass %u\n",    outcomeaction->Scenariotoberunonpass);
   snprintf(reportline[10], sizeof(reportline[10]), "Scenariotoberunonfail %u\n",    outcomeaction->Scenariotoberunonfail);
   snprintf(reportline[11], sizeof(reportline[11]), "Erroraction           %s\n",    outcomeaction->Erroraction);
   snprintf(reportline[12], sizeof(reportline[12]), "Notificationonpass    %s\n",    outcomeaction->Notificationonpass);
   snprintf(reportline[13], sizeof(reportline[13]), "Notificationononfail  %s\n",    outcomeaction->Notificationonfail);
   snprintf(reportline[14], sizeof(reportline[14]), "TesterID:             %u\n",    outcomeaction->TesterID);
   snprintf(reportline[15], sizeof(reportline[15]), "MaillistID:           %u\n",    outcomeaction->MaillistID);

   snprintf(reportline[16], sizeof(reportline[16]),  "Total     actions:   %d\n",    scenarioresult->Actionsinscenario);
   snprintf(reportline[17], sizeof(reportline[17]),  "Attempted actions:   %d\n",    scenarioresult->Actionsattempted);
   snprintf(reportline[18], sizeof(reportline[18]),  "Passed    actions:   %d\n",    scenarioresult->Actionspassed);
   snprintf(reportline[19], sizeof(reportline[19]),  "Failed    actions:   %d\n",    scenarioresult->Actionsfailed);
   snprintf(reportline[20], sizeof(reportline[20]),  "Pass Rate:           %d%%\n",  scenarioresult->Pass);
   snprintf(reportline[21], sizeof(reportline[21]),  "Pass/Fail:           %s\n",    passfail);
                    
   if (send_email_message(userlist, subject, lines, errmsg, sizeof(errmsg)) != 0) {
      snprintf(errmsg, max_errmsg_len, "%s: problem sending status email for scenario %s to user %s %s (email=%s)\n", 
               SUBNAME, scenarioresult->Scenarioname, user->Firstname, user->Lastname, user->Email);
      rc = 1;
   } 

   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    email_scenario_completion_to_maillist                                                    */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int email_scenario_completion_to_maillist(MYSQL *sql_connection, int MaillistID, char *status_table_name, 
                                          scenarioresult_t *scenarioresult, outcomeaction_t *outcomeaction, 
                                          char *errmsg, int max_errmsg_len) {

#undef  SUBNAME
#define SUBNAME "email_scenario_completion_to_maillist"

   MYSQL_RES   *res_set;
   MYSQL_ROW   row;
   int         rc = 0;
   char        query[1024];
   char        msg[MAX_MSG_LEN];
   user_t      user;

   errmsg[0] = 0;
    
   snprintf(query, sizeof(query), "SELECT " USER_FIELDS " FROM user INNER JOIN maillistmember ON user.ID = maillistmember.UserID " 
                                  "WHERE maillistmember.MaillistID = '%u'", MaillistID);
   if (perform_query(SUBNAME, sql_connection, query) != 0) {

      snprintf(errmsg, max_errmsg_len, "%s: problem sending status email for scenario %s to maillist (MaillistID=%d): %s\n", 
               SUBNAME, scenarioresult->Scenarioname, MaillistID, msg);
      rc = 1;                   

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while ((row = mysql_fetch_row(res_set)) != NULL) {

             get_user_record(res_set, &row, &user);
             if ((rc = email_scenario_completion(&user, status_table_name, scenarioresult, 
                                            outcomeaction, errmsg, max_errmsg_len)) != 0) {

                snprintf(errmsg, max_errmsg_len, "%s: problem sending status email for scenario %s to user %s %s (email=%s)\n", 
                         SUBNAME, scenarioresult->Scenarioname, user.Firstname, user.Lastname, user.Email);
             } 

         } 

         mysql_free_result(res_set);

      }
   }
  
   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    archive_work_request                                                                     */
/*                                                                                                        */
/* PARAMS IN:    status_database_name                                                                     */
/*                                                                                                        */
/*               ScenarioresultID                                                                         */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      ID of the archived scenario result record                                                */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int archive_workrequest(MYSQL *sql_connection, char *status_database_name, int ScenarioresultID) {

#undef  SUBNAME
#define SUBNAME "archive_workrequest"

   char query[1024];
   int  archived_scenarioresultID;

   snprintf(query, sizeof(query), "INSERT INTO %s.archive_workrequest SELECT * FROM daf.workrequest where "
                                  "Tablename2='scenarioresult' AND ObjectID2='%d'", status_database_name, ScenarioresultID);
   perform_query(SUBNAME, sql_connection, query);

   archived_scenarioresultID = (Iu32) mysql_insert_id(sql_connection);

   snprintf(query, sizeof(query), "DELETE FROM daf.workrequest WHERE Tablename2='scenarioresult' AND ObjectID2='%d'",
                                  ScenarioresultID);
   perform_query(SUBNAME, sql_connection, query);

   return archived_scenarioresultID;

}



/*---------------------------------------------------------------------------------------------------*/
/*                                                                                                   */
/* Function:   process_workqueue                                                                     */
/*                                                                                                   */
/* Inputs:     sql_connection      An established connection to the status database                  */
/*                                                                                                   */
/* Outputs:    none                                                                                  */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   It is intended that this routine is called from the Automation Framework status       */
/*             daemon.  The routine examines the contents of the workqueue table and determines, for */
/*             each active scenario, whether all the actions in the current step of that scenario    */
/*             have been completed.  If they have, the routine looks at the scenario to see if there */
/*             is a subsequent step in the scenario.  If there is, new workqueue items are created   */
/*             for each action in the step and the routine returns.  If there all the steps in the   */
/*             current scenario were found to have been completed and there is no subsequent step,   */
/*             then the scenario has been completed and the appropriate Scenarioresult record is     */
/*             updated and the Workrequest record for that scenario is removed from the Workqueue    */
/*             table                                                                                 */
/*             The routine returns once all the items in the workqueue have been examined - any      */
/*             new items that are added to the workqueue table during the invocation of the routine  */
/*             are not examined during this invocation of the routine.  (They will be records that   */
/*             are in the idle state and will be processed by the process_new_workqueue_requests()   */
/*             routine                                                                               */
/* ------------------------------------------------------------------------------------------------- */

void process_workqueue(MYSQL *sql_connection) {

#undef  SUBNAME
#define SUBNAME "process_workqueue"

   int known_scenarios[500];

   typedef struct step {
      char Project[DAF_PROJECT_LEN];
      char Phase[DAF_PHASE_LEN];
      int ScenarioID;
      int ScenarioresultID;
      int TestlevelID;
      int TeststandID;
      int OutcomeactionID;
      int TesterID;
      int MaillistID;
      int Stepnumber;
      char Loglocation[128];
      char Scenariologfilename[128];
   } step_t;

   step_t *steps;

   workqueue_t *p_workqueues;
   int         num_workqueue_records;
   bool_t      known_scenario;
   int         num_known_scenario;
   int         i,j;
   bool_t      known_step;
   int         num_known_step;
   int         num_records_in_this_step;
   bool_t      allcomplete;
   int         ScenarioID;           
   int         ScenarioresultID;
   int         TestlevelID;          
   int         TeststandID;          
   int         OutcomeactionID;
   int         TesterID;
   int         MaillistID;          
   int         Stepnumber;
   char        Loglocation[128];

   int              num_actiontype_records;
   int              num_testcase_records;
   int              num_action_records_in_next_step;
   actiontype_t     *p_actiontypes = NULL;
   testcase_t       *p_testcases = NULL;
   action_t         *p_actions = NULL;
   user_t           user;
   scenarioresult_t scenarioresult;

   char   query[1024];
   char   *fields1[] = {"State", "End", NULL};   
   char   *values1[] = {"Completed", "XXXX_CURRENT_TIME_XXXX", NULL};

   char   *fields2[] = {"State", NULL};
   char   *values2[] = {"Tobecanceled", NULL};

   char    where[128];
   int     rc;
   char    msg[MAX_MSG_LEN];
   char    errmsg[MAX_MSG_LEN];
   char    Scenariologfilename[128];

   bool_t  actions_in_step_have_failed;
   bool_t  terminate_scenario;

   int     num_actions;
   int     num_completed;
   int     num_pass;
   int     num_failed;
   int     pass;
   int     archived_scenarioresultID;
   char    status_database_name[DAF_DATABASE_NAME_LEN];   
   char    status_table_name[DAF_TABLE_NAME_LEN];

   int num_current_actions;
   int max_allowed_actions;
   

   if ((steps = (step_t *) malloc(500 * sizeof(step_t))) == NULL) {  /* 5000 ? <<<<<<<<<<<<<<<<<<<<<< */
       /* problem */
       return;
   }

   /* --------------------------------------------------------------------------------------------- */
   /* Get the records that are currently in the workqueue                                           */
   /* These can be in Idle, Running or Completed states                                             */
   /* --------------------------------------------------------------------------------------------- */

   p_workqueues  = allocate_and_get_all_workqueue_records(sql_connection, &num_workqueue_records);
   p_actiontypes = allocate_and_get_all_actiontype_records(sql_connection, &num_actiontype_records);
   p_testcases   = allocate_and_get_all_testcase_records(sql_connection, &num_testcase_records);

   if (num_workqueue_records > 0) {

      /* --------------------------------------------------------------------------------------------- */
      /* Examine the workqueue records and work out which Scenarios are being run and which step       */
      /* in each scenario is active                                                                    */
      /* --------------------------------------------------------------------------------------------- */

      num_known_scenario = 0;
      num_known_step = 0;

      for (i=0; i<num_workqueue_records; i++) {

         known_scenario = FALSE;
         for (j=0; j<num_known_scenario; j++) {
            if (p_workqueues[i].ScenarioresultID == known_scenarios[j]) {
               known_scenario = TRUE;
            }
         }
         if (! known_scenario) {
            known_scenarios[num_known_scenario] = p_workqueues[i].ScenarioresultID;
            num_known_scenario++;
         }

         known_step = FALSE;
         for (j=0; j<num_known_step; j++) {
            if ((p_workqueues[i].ScenarioresultID == steps[j].ScenarioresultID) && 
                (p_workqueues[i].Stepnumber       == steps[j].Stepnumber)) {
               known_step = TRUE;
            }
         }
         if (! known_step) {
            steps[num_known_step].ScenarioID       = p_workqueues[i].ScenarioID;
            steps[num_known_step].ScenarioresultID = p_workqueues[i].ScenarioresultID;
            steps[num_known_step].TestlevelID      = p_workqueues[i].TestlevelID;
            steps[num_known_step].TeststandID      = p_workqueues[i].TeststandID;
            steps[num_known_step].OutcomeactionID  = p_workqueues[i].OutcomeactionID,
            steps[num_known_step].MaillistID       = p_workqueues[i].MaillistID,     
            steps[num_known_step].TesterID         = p_workqueues[i].TesterID,       
            steps[num_known_step].Stepnumber       = p_workqueues[i].Stepnumber;
            safecpy(steps[num_known_step].Scenariologfilename, p_workqueues[i].Scenariologfilename, sizeof(steps[num_known_step].Scenariologfilename));
            safecpy(steps[num_known_step].Loglocation,         p_workqueues[i].Loglocation,         sizeof(steps[num_known_step].Loglocation));
            safecpy(steps[num_known_step].Project,             p_workqueues[i].Project,             sizeof(steps[num_known_step].Project));
            safecpy(steps[num_known_step].Phase,               p_workqueues[i].Phase,               sizeof(steps[num_known_step].Phase));
            num_known_step++;
         }

      }

      /* for (i=0; i<num_known_step; i++) {

         open_scenario_console_log(steps[i].Loglocation, steps[i].Scenariologfilename); 
         print_scenario_console_log("------------\n");
         print_scenario_console_log(msg);
         snprintf(msg, sizeof(msg), "Active Step....\n");
         snprintf(msg, sizeof(msg), "ScenarioID %u ScenarioresultID %u Stepnumber %u\n", 
                 steps[i].ScenarioID, steps[i].ScenarioresultID, steps[i].Stepnumber);
         print_scenario_console_log(msg);

         snprintf(msg, sizeof(msg), "Active Workqueue items for this step....\n");
         print_scenario_console_log(msg);
         for (j=0; j<num_workqueue_records; j++) {
            if (p_workqueues[j].Stepnumber == steps[i].Stepnumber) {
               snprintf(msg, sizeof(msg), "ID=%u, host=%s, j=%u, ScenarioresultID = %u, Stepnumber = %d, " 
                                          "State = %s, Statemodifier = %s, Tag = %d, Pass = %d\n", 
                        p_workqueues[j].ID, p_workqueues[j].Hostname, j, p_workqueues[j].ScenarioresultID,
                        p_workqueues[j].Stepnumber, p_workqueues[j].State, 
                        p_workqueues[j].Statemodifier, p_workqueues[j].Tag, p_workqueues[j].Pass);
               print_scenario_console_log(msg);

            }
         }
         print_scenario_console_log("------------\n");
         close_scenario_console_log(); 
      } */

      /* --------------------------------------------------------------------------------------------- */
      /* known_scenarios[] is a now list of all ScenarioresultIDs that are in the workqueue            */
      /* --------------------------------------------------------------------------------------------- */
      /* steps[] is a list of the unique duples for (ScenarioResultID, Stepnumber)                     */
      /* ie, step[j] represents an action within a particular step (whose stepnumber is                */
      /* step[j].Stepnumber) and that is associated with a run of the scenario that has                */
      /* ID step[j].ScenarioID.   The scenarioresult record that is being used to collect the results  */
      /* of the run of this scenario is in step[j].ScenarioResultID and the state of this action is    */
      /* in step[j].State.  Because there can be more than one action in a given Scenario step, there  */
      /* can be more than element in the step[] that has the same step[].Stepnumber, step[].ScenarioID */
      /* and step[].ScenarioresultID value.                                                            */
      /* To determine if a step has completed, we need to find all the step[] elements that do have    */
      /* the same StepNumber and ScenarioID fields - and then check if the State of all those elements */
      /* is in the Completed state - if they are, then the step is complete and a new set of workqueue */
      /* elements should be created in the Idle state for the next Scenario step                       */
      /* --------------------------------------------------------------------------------------------- */

      for (i=0; i<num_known_step; i++) {

         outcomeaction_t outcomeaction;

         ScenarioID       = steps[i].ScenarioID;
         ScenarioresultID = steps[i].ScenarioresultID;
         TeststandID      = steps[i].TeststandID;
         TestlevelID      = steps[i].TestlevelID;
         OutcomeactionID  = steps[i].OutcomeactionID;
         MaillistID       = steps[i].MaillistID;
         TesterID         = steps[i].TesterID;
         Stepnumber       = steps[i].Stepnumber;
         determine_status_database_name(steps[i].Project, steps[i].Phase, status_database_name, sizeof(status_database_name));
         safecpy(Scenariologfilename, steps[i].Scenariologfilename, sizeof(Scenariologfilename));
         safecpy(Loglocation,         steps[i].Loglocation,         sizeof(Loglocation));

         open_scenario_console_log(Loglocation, Scenariologfilename); 

         allcomplete                 = TRUE;
         actions_in_step_have_failed = FALSE;
         terminate_scenario          = FALSE;
         num_records_in_this_step    = 0;

         for (j=0; j<num_workqueue_records; j++) {

            if ((p_workqueues[j].ScenarioresultID == ScenarioresultID) && (p_workqueues[j].Stepnumber == Stepnumber)) {

               if (! MATCH(p_workqueues[j].State, "Completed")) {
                  allcomplete = FALSE;
               } else {
                  if (p_workqueues[j].Pass != 100) {
                     actions_in_step_have_failed = TRUE;
                  }
               }
               num_records_in_this_step++;
            }

sprintf(msg, "i %d j %d step %d ScenarioresultID %d state %s %d\n", 
             i, j, p_workqueues[j].Stepnumber, p_workqueues[j].ScenarioresultID, p_workqueues[j].State, allcomplete);
print_msg_to_console(msg);

         }

         get_outcomeaction(sql_connection, OutcomeactionID, &outcomeaction); /* too many calls?  <<<< */

         /* ----------------------------------------------------------------------------------*/
         /* Take the error action if the step has failed                                      */         
         /* cancel other non complete actions in this step                                    */
         /* what happens if an action completes just as it is being canceled ??? <<<<<<<<<<<  */
         /* ----------------------------------------------------------------------------------*/

         if (actions_in_step_have_failed) {

            /* open_scenario_console_log(Loglocation, Scenariologfilename); <<<<<<<<<< restore */
            snprintf(msg, sizeof(msg), "One or more actions in step: %u  ScenarioresultID: %u have failed\n", 
                     Stepnumber, ScenarioresultID) ;
            print_scenario_console_log(msg);

            if (MATCH(outcomeaction.Erroraction, "Terminatescenario")) {

               snprintf(msg, sizeof(msg), "Cancelling scenario as Outcomeaction (ID: %u) has Erroraction set to %s\n", 
                                           OutcomeactionID, outcomeaction.Erroraction) ;
               print_scenario_console_log(msg);

               for (j=0; j<num_workqueue_records; j++) {
                  if ((p_workqueues[j].ScenarioresultID == ScenarioresultID) && (p_workqueues[j].Stepnumber == Stepnumber)) {
                     if ((!MATCH(p_workqueues[j].State, "Completed")) && (!MATCH(p_workqueues[j].State, "Pendingcompletion"))) {

                        snprintf(msg, sizeof(msg), "Cancelling workqueue.ID: %u  in step: %u  hostname: %s ScenarioresultID: %u\n", 
                                 p_workqueues[j].ID, Stepnumber, p_workqueues[j].Hostname, ScenarioresultID) ;
                        print_scenario_console_log(msg);

                        snprintf(where, sizeof(where), "ID = '%u'",  p_workqueues[j].ID);
                        rc = perform_update(SUBNAME, sql_connection, "workqueue", fields2, values2, where, errmsg, sizeof(errmsg));
                        if (rc != 0) {
                           print_scenario_console_log(errmsg);
                        }
                        safecpy(p_workqueues[j].State, "Tobecanceled", sizeof(p_workqueues[j].State)); /* <<<<<<<<< is this needed ??? */

                     }
                  }
               }

               terminate_scenario = TRUE;


            } else {

               snprintf(msg, sizeof(msg), "Scenario will continue with errors as Outcomeaction (ID: %u) has Erroraction set to %s\n", 
                                           p_workqueues[j].OutcomeactionID, outcomeaction.Erroraction) ;
               print_scenario_console_log(msg);

            }
         } 

sprintf(msg, "allcomplete %d num_records_in_this_step %d\n", allcomplete, num_records_in_this_step);
print_msg_to_console(msg);

         if ((allcomplete) && ((num_records_in_this_step>0) || terminate_scenario)) {

            p_actions = NULL;

            /* --------------------------------------------------------------------------------------------- */
            /* ScenarioresultID, Stepnumber is all complete, so move to the next step if there is one,       */
            /* or end the Sceneario if there are no more steps or the scenario is being terminated           */
            /* --------------------------------------------------------------------------------------------- */
  
            if (! terminate_scenario) {

               snprintf(msg, sizeof(msg), "Completed all actions in step: %u  ScenarioresultID: %u  terminate_scenario: %d\n", 
                        Stepnumber, ScenarioresultID, terminate_scenario) ;
               print_scenario_console_log(msg);

               snprintf(where, sizeof(where), "ScenarioID = '%d' and Stepnumber = '%d'", ScenarioID, (Stepnumber+1));
               p_actions = allocate_and_get_action_records(sql_connection, where, &num_action_records_in_next_step);

            } else {
              
               num_action_records_in_next_step = 0;

            }

sprintf(msg, "num_action_records_in_next_step %d terminate_scenario %d\n", num_action_records_in_next_step, terminate_scenario);
print_msg_to_console(msg);

            if (num_action_records_in_next_step == 0) {  

               /* ------------------------------------------------------------------------------------------- */
               /* there are no more steps to be done in this scenario - so the original Workrequest record    */
               /* that initiated this run is moved to the 'Completed' state before it is archived, unless the */
               /* workrequest has been canceled at some point in the past by the user in which case we leave  */
               /* the state of the workrequest set to canceled                                                */
               /* ------------------------------------------------------------------------------------------- */

               snprintf(where, sizeof(where), "Tablename2 = 'scenarioresult' AND ObjectID2 = '%u' AND State != 'Canceled'", ScenarioresultID);
               rc = perform_update(SUBNAME, sql_connection, "workrequest", fields1, values1, where, errmsg, sizeof(errmsg));
               if (rc != 0) {
                  print_scenario_console_log(errmsg);
               }

               /* ------------------------------------------------------------------------------------------- */
               /* there are no more steps in this scenario - so the scenarioresult is complete - move the     */
               /* state of the Scenarioresult record to the same state as the original Workrequest - this     */
               /* means the scenarioresult State will either be 'Canceled' or 'Completed'                     */
               /* ------------------------------------------------------------------------------------------- */

               determine_status_database_name(steps[i].Project, steps[i].Phase, status_database_name, sizeof(status_database_name));
               determine_status_table_name(status_database_name, "scenarioresult", status_table_name, sizeof(status_table_name));
               snprintf(query, sizeof(query),
                        "UPDATE %s INNER JOIN daf.workrequest ON workrequest.ObjectID2 = scenarioresult.ID "
                        "SET scenarioresult.State = workrequest.State WHERE scenarioresult.ID = %u\n",
                        status_table_name, ScenarioresultID);
               perform_query(SUBNAME, sql_connection, query);

               /* ------------------------------------------------------------------------------------------- */
               /* Now we must compute if the Scenario passed or not - this means examining all the            */
               /* action request records that have this scenarioresultID and adding up their Pass fields.     */
               /* Scenario Pass =  Sum(actionresult.Pass) / Count(actionrecordsinscenario)                    */
               /* ------------------------------------------------------------------------------------------- */

               if (get_scenario_pass_rate(sql_connection, status_database_name, ScenarioresultID, &num_actions, 
                                          &num_completed, &num_pass, &num_failed, &pass) == 0) {
                  snprintf(query, sizeof(query),
                           "UPDATE %s.scenarioresult SET Pass = '%u', Actionsattempted = '%u', Actionscompleted = '%u', Actionspassed = '%u', " 
                           "Actionsfailed = '%u', End = NOW() WHERE scenarioresult.ID = %u\n",
                            status_database_name, pass, num_actions, num_completed, num_pass, num_failed, ScenarioresultID);
                  perform_query(SUBNAME, sql_connection, query);

                  snprintf(msg, sizeof(msg), "Pass rate = %u for Scenarioresult ID %u\n", pass, ScenarioresultID);
                  print_scenario_console_log(msg);
                  snprintf(msg, sizeof(msg), "Total actions = %u, Number of completed actions = %u, Number of passed actions = %u, Number of failed actions = %u\n", 
                           num_actions, num_completed, num_pass, num_failed);
                  print_scenario_console_log(msg);
               } else {
                  snprintf(msg, sizeof(msg), "Error: Could not update pass rates in Scenarioresult.ID %u\n", 
                          ScenarioresultID);
                  print_scenario_console_log(msg);
               }


               /* --------------------------------------------------------------------------------------------- */
               /* all the steps in the Workrequest for this Scenario run have been completed so we do not need  */
               /* the Workrequest anymore, but to provide an audit trail we first copy it to the                */
               /* archive_workrequest table before we delete it from the original workrequest table             */
               /* --------------------------------------------------------------------------------------------- */

               archived_scenarioresultID = archive_workrequest(sql_connection, status_database_name, ScenarioresultID);

               snprintf(msg, sizeof(msg), "Scenario is completed - All steps in Scenarioresult ID %u have been completed\n", 
                        ScenarioresultID) ;
               print_scenario_console_log(msg);
               print_scenario_console_log_doublerule();

               /* --------------------------------------------------------------------------------------------- */
               /* The scenario has completed - now look at the outcomeaction record to see if the user has      */
               /* requested that a new scenario is run by setting the Actionpass or Actiononfail field in the   */
               /* outcomeaction associated with the current scenario - allowed values for Actiononpass and      */
               /* Actiononfail are "None" and "Runscenario"                                                     */
               /* Also, see if the user has asked for Notificationonpass or Notificationonfail - the valid      */
               /* settings here are "None", "Tester" or "Maillist"                                              */
               /* --------------------------------------------------------------------------------------------- */

               if (pass == 100) {
                   if (MATCH(outcomeaction.Actiononpass, "Runscenario")) {
                       snprintf(msg, sizeof(msg), "Chaining on success to Scenario ID %u\n", outcomeaction.Scenariotoberunonpass);
                       print_scenario_console_log(msg);
                       spawn_new_workrequest(sql_connection, archived_scenarioresultID,
                                             outcomeaction.Scenariotoberunonpass, steps[i].Project, steps[i].Phase);
                   }
               } else {
                   if (MATCH(outcomeaction.Actiononfail, "Runscenario")) {
                       snprintf(msg, sizeof(msg), "Chaining on failure to Scenario ID %u\n", outcomeaction.Scenariotoberunonpass);
                       print_scenario_console_log(msg);
                       spawn_new_workrequest(sql_connection, archived_scenarioresultID,
                                             outcomeaction.Scenariotoberunonfail, steps[i].Project, steps[i].Phase);
                   }
               }
               if ( (MATCH(outcomeaction.Notificationonpass, "Tester") && (pass == 100)) ||
                    (MATCH(outcomeaction.Notificationonfail, "Tester") && (pass != 100)) ) {
                   /* TesterID */   /* <<< check rc for the two routines below */

                  get_user(sql_connection, TesterID, &user);
                  get_scenarioresult(sql_connection, ScenarioresultID, status_table_name, &scenarioresult);

                  if (email_scenario_completion(&user, status_table_name, &scenarioresult, &outcomeaction, errmsg, sizeof(errmsg)) != 0) {
                     print_scenario_console_log(errmsg);
                  } 

               } else if ( (MATCH(outcomeaction.Notificationonpass, "Maillist") && (pass == 100)) ||
                           (MATCH(outcomeaction.Notificationonfail, "Maillist") && (pass != 100)) ) {
                    /* MaillistID */

                  get_scenarioresult(sql_connection, ScenarioresultID, status_table_name, &scenarioresult);

                  if (email_scenario_completion_to_maillist(sql_connection, MaillistID, status_table_name, 
                                                            &scenarioresult, &outcomeaction, errmsg, sizeof(errmsg)) != 0) {
                     print_scenario_console_log(errmsg);
                  } 

               }

              /* close_scenario_console_log(); */

            } else {

                     /* --------------------------------------------------------------------------------------------- */
                     /* all the actions in this step have been completed, so we don't need the workqueue items        */
                     /* associated with them anymore, but to provide an audit trail we first copy them to the         */
                     /* archive_workqueue table, and then we delete them from the original workqueue table            */
                     /* --------------------------------------------------------------------------------------------- */

                     snprintf(query, sizeof(query), "INSERT INTO %s.archive_workqueue SELECT * FROM daf.workqueue WHERE " 
                                                    "ScenarioResultID='%d' AND Stepnumber='%d' and " 
                                                    "(State = 'Completed' OR State = 'Canceled')", status_database_name, ScenarioresultID, Stepnumber);

                     perform_query(SUBNAME, sql_connection, query);
   
                     snprintf(query, sizeof(query), "DELETE FROM daf.workqueue WHERE ScenarioResultID='%d' AND Stepnumber='%d' AND " 
                                                    "(State = 'Completed' OR State = 'Canceled')", ScenarioresultID, Stepnumber);

                     perform_query(SUBNAME, sql_connection, query); 

               /* --------------------------------------------------------------------------------------------- */
               /* There are more steps to be done in the Scenario - so create a new workqueue item for each     */
               /* action in the next step in the Scenario - create the workqueue item in the 'Notstarted' state */
               /* --------------------------------------------------------------------------------------------- */

print_msg_to_console("more steps\n");

               /* open_scenario_console_log(Loglocation, Scenariologfilename); */
               print_scenario_console_log_doublerule();

               /* --------------------------------------------------------------------------------------------- */
               /* Get all the action records that apply to the next step in this Scenario                       */
               /* <<<< this means we have to do one query for every ScenarioResultID/Step - is this too many?   */
               /* --------------------------------------------------------------------------------------------- */

               snprintf(msg, sizeof(msg), "Scenario ID %u step %u contains %u action records\n", ScenarioID, Stepnumber+1, num_action_records_in_next_step);
               print_scenario_console_log(msg);

               /* ----------------------------------------------------------------------------------------------*/
               /* Check to see if we are going to exceed the number of actions allowed in the licence           */
               /* ----------------------------------------------------------------------------------------------*/

               num_current_actions = count_entries_in_workqueue(sql_connection);
               max_allowed_actions = get_licence_max_actions("/opt/daf/daf_licence_file", errmsg, sizeof(errmsg)); /* <<<< hardcoded */

               if (max_allowed_actions == 0) {

                  snprintf(msg, sizeof(msg), "Cannot limit actions to licence limit as a problem was encountered with the licence file: %s\n", errmsg);
                  print_scenario_console_log(msg);

               } 

sprintf(msg, "num_current_actions %d  num_action_records_in_next_step %d  max_allowed_actions %d", num_current_actions, num_action_records_in_next_step,  max_allowed_actions);
print_msg_to_console(msg);

                  if (((num_current_actions + num_action_records_in_next_step) <= max_allowed_actions) || (max_allowed_actions == 0)) { 

                     rc =  create_actions_for_step(sql_connection,
                                                steps[i].Project,
                                                steps[i].Phase,
                                                p_actions,
                                                num_action_records_in_next_step,
                                                p_actiontypes,
                                                num_actiontype_records,
                                                p_testcases,
                                                num_testcase_records,
                                                ScenarioID,
                                                (Stepnumber+1),
                                                ScenarioresultID,
                                                TestlevelID,
                                                TeststandID, 
                                                OutcomeactionID,
                                                TesterID, 
                                                MaillistID,
                                                Loglocation,
                                                Scenariologfilename);

                     if (p_actions != NULL) {
                        free(p_actions);
                     }

                  } else {

                     /* tell the user that the licence does not allow this many actions and that the scenario has not been */
                     /* started yet     <<<<<<<                                                                            */
                  }

            }

         }

         close_scenario_console_log(); 

      }

   }

   if (p_actiontypes != NULL) {
     free(p_actiontypes);
   }
   if (p_testcases != NULL) {
     free(p_testcases);
   }
   if (p_workqueues != NULL) {
     free(p_workqueues);
   }
   if (steps != NULL) {
     free(steps);
   }
}


/*---------------------------------------------------------------------------------------------------*/
/*                                                                                                   */
/* Function:   process_new_workqueue_requests     ?///  <<<< processes non new requests too!         */
/*                                                                                                   */
/* Inputs:     sql_connection      An established connection to the status database                  */
/*                                                                                                   */
/* Outputs:    none                                                                                  */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   It is intended that this routine is called from the Automation Framework status       */
/*             daemon.  The routine examines the contents of the workqueue table and selects any     */
/*             workqueue item that is in 'Notstarted' state.   These are workitems for which a       */
/*             testcase has not yet been started.  This routine creates a new actionresult record    */
/*             for the scenario/testcase in the workqueue item and then launches the testcase on the */
/*             appropriate remote host, passing the ID of the current workqueue record - this is so  */
/*             when the testcase completes, the automation agent on the remote host can update this  */
/*             workqueue record with the outcome of the testcase                                     */
/*             The routine also processes workqueue items that are in the 'Tobecanceled' stae - here the  */
/*             user has requested that any testcase associated with the action should be terminated  */
/*             and the action removed from the active workqueue list                                 */
/*---------------------------------------------------------------------------------------------------*/

void process_new_workqueue_requests(MYSQL *sql_connection,
                                    char *sql_servername,
                                    char *sql_username,
                                    char *sql_password,
                                    char *sql_databasename,
                                    Iu16 sql_port,
                                    bool_t simulator_mode) {

#undef  SUBNAME
#define SUBNAME "process_new_workqueue_requests"

   MYSQL_RES   *res_set;
   MYSQL_ROW   row;

   workqueue_t workqueue;
   char        cmdstring[1024];
   int         i, rc = 0;
   char        msg[MAX_MSG_LEN];
   char        errmsg[MAX_MSG_LEN];
   char        where[128];
   int         actionresultID;
   char        query[1024];

   char        S_ActionresultID[16];   /* <<<<<<<< hostname too ? */
   char        S_ScenarioresultID[16];   /* <<<<<<<< hostname too ? */
   char        S_Actiontype[32];
   char        S_Stepnumber[16];
   char        S_State[24];
   char        S_Status[32];
   char        S_Statemodifier[32];
   char        S_Hostname[32];
   char        S_Testcase[32];
   char        S_Invocation[1024];
   char        S_Pass[8];
   char        S_Start[24];
   char        S_End[24];

   char        *fields[] = {"State", "ActionresultID", NULL};  
   char        *values[3];

   char        *fields1[] = {"ScenarioresultID", "Actiontype", "Stepnumber", "Status", "Hostname", "Testcase", "Invocation", "Pass", 
                             "Start", NULL};
   char        *values1[10];

   char        *fields2[] = {"State", "ActionresultID", "Statemodifier", NULL};
   char        *values2[4];

   char        *fields3[] = {"Pass", "Status", "End", NULL};
   char        *values3[4];

   char        *fields4[] = {"State", NULL};
   char        *values4[] = {"Completed"};

  remote_client_prepare_cmd_args  prepare_args;
  remote_client_prepare_cmd_res   prepare_result;
  remote_client_execute_cmd_args  execute_args;
  remote_client_execute_cmd_res   execute_result;
  enum clnt_stat          stat;
  char identstring[10] = "";
  CLIENT *  remote_client;            /* the RPC handle to reach the remote test host                 */
  static struct timeval TIMEOUT = { 25, 0 };

#define MAX_NUM_ENVIRONMENT_VALUES 100 
/* <<<<< duplicate of value elsewhere */
  environment_assignment_t  environment_assignments[MAX_NUM_ENVIRONMENT_VALUES];
  int                       num_environment_assignments;

   values[0]  = &S_State[0];
   values[1]  = &S_ActionresultID[0];
   values[2]  = NULL;                  

   values1[0]  = &S_ScenarioresultID[0];
   values1[1]  = &S_Actiontype[0];
   values1[2]  = &S_Stepnumber[0];
   values1[3]  = &S_Status[0];
   values1[4]  = &S_Hostname[0];
   values1[5]  = &S_Testcase[0];
   values1[6]  = &S_Invocation[0];
   values1[7]  = &S_Pass[0];
   values1[8]  = &S_Start[0];
   values1[9] = NULL;

   values2[0]  = &S_State[0];
   values2[1]  = &S_ActionresultID[0];
   values2[2]  = &S_Statemodifier[0];
   values2[3]  = NULL;

   values3[0]  = &S_Pass[0];
   values3[1]  = &S_Status[0];
   values3[2]  = &S_End[0];
   values3[3]  = NULL;
  
   dirlist pdl_base;
   dirlist pdl;
   char    local_pathname[MAX_PATHNAME_LEN];
   char    remote_pathname[MAX_PATHNAME_LEN];
   char    status_database_name[DAF_DATABASE_NAME_LEN];   
   char    table_name[DAF_TABLE_NAME_LEN];   

   if (perform_query(SUBNAME, sql_connection, "SELECT " WORKQUEUE_FIELDS " FROM daf.workqueue " 
                     "WHERE State = 'Notstarted' OR State = 'Pendingcompletion' OR State = 'Tobecanceled'") != 0) { 
      snprintf(msg, sizeof(msg), "%s: query (%s) failed, errono=%u - %s\n", 
                                 SUBNAME, query, mysql_errno(sql_connection), mysql_error(sql_connection));
      print_msg_to_console(msg);

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         while ((row = mysql_fetch_row(res_set)) != NULL) {

            get_workqueue_record(res_set, &row, &workqueue);
            determine_status_database_name(workqueue.Project, workqueue.Phase, status_database_name, sizeof(status_database_name));

            open_scenario_console_log(workqueue.Loglocation, workqueue.Scenariologfilename);

            /* snprintf(msg, sizeof(msg), "Processing workqueue ID=%u,  state=%s,  statemodifer=%s\n",  
                                       workqueue.ID, workqueue.State, workqueue.Statemodifier);
            print_scenario_console_log(msg); */

            if (MATCH(workqueue.State, "Tobecanceled")) {

               /* --------------------------------------------------------------------------------------------- */
               /* User has asked us to cancel the testcase associated with this workqueue item - if we have a   */
               /* tag for this testcase we can instruct the remote automation agent to cancel the testcase, if  */
               /* not we cannot take any action to cancel the testcase.  In either case we move the workqueue   */
               /* record to the archive_workqueue table.  We also move the corresponding workrequest record     */
               /* to the canceled state so that we eventually record a canceled state in the scenarioresult     */
               /* record                                                                                        */
               /* --------------------------------------------------------------------------------------------- */

               if (workqueue.Tag != 0) {

                  rc = remote_client_cancel_tag(workqueue.Hostname, workqueue.Tag);
                  snprintf(msg, sizeof(msg), "WorkqueueID %u - canceling tag %u on %s at user request - rc=%d\n", 
                           workqueue.ID, workqueue.Tag, workqueue.Hostname, rc);
                  print_scenario_console_log(msg);

               }

               snprintf(msg, sizeof(msg), "WorkqueueID %u on host %s - has been canceled at user request\n", workqueue.ID, workqueue.Hostname);
               print_scenario_console_log(msg);
               
               snprintf(query, sizeof(query), "UPDATE daf.workqueue SET State='Pendingcompletion', StateModifier='Canceled_due_to_error_in_step', End=NOW() WHERE " 
                                              "ID='%u'", workqueue.ID); 

printf("%s\n", query);
               perform_query(SUBNAME, sql_connection, query);

/*               snprintf(query, sizeof(query), "UPDATE daf.workrequest SET State='Canceled', End=NOW() WHERE "
                                              "Tablename2='scenarioresult' AND ObjectID2='%u'", workqueue.ScenarioresultID); 
               perform_query(SUBNAME, sql_connection, query);

               snprintf(query, sizeof(query), "UPDATE daf.workqueue SET State='Canceled', End=NOW() WHERE " 
                                              "ID='%u'", workqueue.ID); 
               perform_query(SUBNAME, sql_connection, query);


               snprintf(query, sizeof(query), "INSERT INTO %s.archive_workqueue SELECT * FROM daf.workqueue where " 
                                              "ScenarioResultID='%d' and Stepnumber='%d' and " 
                                              "State = 'Canceled'", status_database_name, workqueue.ScenarioresultID, workqueue.Stepnumber);

               perform_query(SUBNAME, sql_connection, query);

               snprintf(query, sizeof(query), "DELETE FROM daf.workqueue where ScenarioResultID='%d' and Stepnumber='%d' and " 
                                              "State = 'Canceled'", workqueue.ScenarioresultID, workqueue.Stepnumber);

               perform_query(SUBNAME, sql_connection, query);  */

            } else if (MATCH(workqueue.State, "Notstarted")) {

               /* --------------------------------------------------------------------------------------------- */
               /* Create a new actionresult row for this paricular workrequest                                  */
               /* --------------------------------------------------------------------------------------------- */

               snprintf(S_ScenarioresultID, sizeof(S_ScenarioresultID), "%d", workqueue.ScenarioresultID);
               snprintf(S_Stepnumber,       sizeof(S_Stepnumber),       "%d", workqueue.Stepnumber);
               snprintf(S_Pass,             sizeof(S_Pass),             "%d", 0);
               safecpy(S_Actiontype,        workqueue.Actiontype,     sizeof(S_Actiontype));
               safecpy(S_Status,            "None",                   sizeof(S_Status));
               safecpy(S_Testcase,          workqueue.Testcase,       sizeof(S_Testcase));
               safecpy(S_Invocation,        workqueue.Invocation,     sizeof(S_Invocation));
               safecpy(S_Start,             "XXXX_CURRENT_TIME_XXXX", sizeof(S_Start));
               safecpy(S_Hostname,          workqueue.Hostname,       sizeof(S_Hostname));

/* <<<<< how do we record the Environment ? */

               snprintf(table_name, sizeof(table_name), "%s.actionresult", status_database_name);

               rc = perform_insert(SUBNAME, sql_connection,
                                   table_name,
                                   fields1,
                                   values1,
                                   errmsg,
                                   sizeof(errmsg));

               if (rc != 0) {
                  snprintf(msg, sizeof(msg), "workqueueID %u - problem with sql update - %s\n", workqueue.ID, errmsg);
                  print_scenario_console_log(msg);
               } else {
                  actionresultID = (Iu32) mysql_insert_id(sql_connection);        /* <<<< 64 bit ??  */
                  //snprintf(msg, sizeof(msg), "workqueueID %u - created actionresultID %u for scenarioresultID %u\n", 
                  //         workqueue.ID, actionresultID, workqueue.ScenarioresultID);
                  //print_scenario_console_log(msg);
               }

               if (MATCH(workqueue.Statemodifier, "No_available_host")) {

                  snprintf(msg, sizeof(msg), "%s: no host available for workqueue.ID %u - action abandoned\n", SUBNAME, workqueue.ID);
                  print_scenario_console_log(msg);

                  snprintf(where, sizeof(where), "ID = %u", workqueue.ID);
                  snprintf(S_ActionresultID, sizeof(S_ActionresultID), "%d", actionresultID);
                  safecpy(S_State,          "Pendingcompletion",       sizeof(S_State));
                  safecpy(S_Statemodifier,  "No_available_host", sizeof(S_Statemodifier));
                  rc = perform_update(SUBNAME "xxx1", sql_connection, "daf.workqueue", fields2, values2, where, errmsg, sizeof(errmsg)); 
                  if (rc != 0) {
                      print_scenario_console_log("xxx1");
                      print_scenario_console_log(errmsg);
                  }

               } else {

                  /* --------------------------------------------------------------------------------------------- */
                  /* Start the action ....                                                                         */
                  /* --------------------------------------------------------------------------------------------- */

                     /* what path and parameters ? <<<<<<<<<<<<<<<< */
                     /* could there be " etc in the parameters ?? <<<<<<<< */

                     snprintf(where, sizeof(where), "EnvironmentID = '%d'", workqueue.EnvironmentID); 
                     get_environmentvalue_assignments(sql_connection, environment_assignments,
                                                      MAX_NUM_ENVIRONMENT_VALUES, where, &num_environment_assignments);

                     /* snprintf(msg, sizeof(msg), "EnvironmentID: %u  there are %u environment assigments to be applied\n", 
                              workqueue.EnvironmentID, num_environment_assignments);
                     print_scenario_console_log(msg); */

                     remote_client = clnt_create(workqueue.Hostname, DAF_PROG, DAF_VERSION, "tcp");

                     if (remote_client == (CLIENT *) NULL) {

                        /* ------------------------------------------------------------------------------------------- */
                        /* We could not contact the remote test host so we could not run the testcase                  */
                        /* Set the workqueue record State field to 'Pendingcompletion', the Statemodifier field to     */
                        /* 'No_remote_host_connect' and the Start and End and fields to NOW()                          */
                        /* ------------------------------------------------------------------------------------------- */

                        snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, workqueue.Hostname);
                        snprintf(msg, sizeof(msg), "%s\n", clnt_spcreateerror(errmsg));
                        print_scenario_console_log(msg);

                        snprintf(where, sizeof(where), "ID = %u", workqueue.ID);
                        snprintf(S_ActionresultID, sizeof(S_ActionresultID), "%d", actionresultID);
                        safecpy(S_State,          "Pendingcompletion",       sizeof(S_State));
                        safecpy(S_Statemodifier,  "No_remote_host_connect", sizeof(S_Statemodifier));
                        rc = perform_update(SUBNAME "xxx5", sql_connection, "daf.workqueue", fields2, values2, where, errmsg, sizeof(errmsg)); 
                        if (rc != 0) {
                           print_scenario_console_log("xxx5");
                           print_scenario_console_log(errmsg);
                        }

                     } else {

                        memset(&prepare_args,   0, sizeof(prepare_args));
                        memset(&prepare_result, 0, sizeof(prepare_result));

                        snprintf(msg, sizeof(msg), "Preparing action in step: %u  hostname: %s  workqueue.ID: %u  Maxduration: %u secs  " 
                                 "Duration: %s  Identstring: %s\n", 
                                 workqueue.Stepnumber, workqueue.Hostname, workqueue.ID, workqueue.Maxduration, workqueue.Duration, identstring);
                        print_scenario_console_log(msg);

                        prepare_args.timeout           = workqueue.Maxduration;
                        if (MATCH(workqueue.Duration, "TimeoutafterNminutes")) {
                           prepare_args.fail_on_timeout = TRUE;
                        } else {
                           prepare_args.fail_on_timeout = FALSE;
                        }
                        prepare_args.identstring       = identstring;
                        prepare_args.workqueueID       = workqueue.ID;
                        prepare_args.actionresultID    = actionresultID;

                        safecpy(cmdstring, workqueue.Invocation, sizeof(cmdstring));   
                        prepare_args.cmdstring         = cmdstring;     
 
                        if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_PREPARE_CMD,
                                              (xdrproc_t) xdr_remote_client_prepare_cmd_args, (caddr_t) &prepare_args,
                                              (xdrproc_t) xdr_remote_client_prepare_cmd_res,  (caddr_t) &prepare_result,
                                              TIMEOUT)) != RPC_SUCCESS) {

                           clnt_stat_decode(stat, errmsg, sizeof(errmsg));
                           snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s) - testcase did not run\n", 
                                    SUBNAME, (long) CLIENT_REMOTE_CLIENT_PREPARE_CMD, stat, errmsg);
                           print_scenario_console_log(msg);

                           snprintf(where, sizeof(where), "ID = %u", workqueue.ID);
                           snprintf(S_ActionresultID, sizeof(S_ActionresultID), "%d", actionresultID);
                           safecpy(S_State,          "Pendingcompletion",      sizeof(S_State));
                           safecpy(S_Statemodifier,  "Clnt_call_failed",       sizeof(S_Statemodifier));
                           rc = perform_update(SUBNAME "xxx6", sql_connection, "daf.workqueue", fields2, values2, where, errmsg, sizeof(errmsg));
                           if (rc != 0) {
                              print_scenario_console_log("xxx6");
                              print_scenario_console_log(errmsg);
                           }

                        } else {

                           if (prepare_result.status != REMOTE_CMD_OK) {
     
                              /* prepare failed */
                              snprintf(msg, sizeof(msg) ,"%s: prepare_cmd() failed - status = %d - testcase did not run\n", 
                                       SUBNAME, prepare_result.status);
                              print_scenario_console_log(msg);
                              snprintf(where, sizeof(where), "ID = %u", workqueue.ID);
                              snprintf(S_ActionresultID, sizeof(S_ActionresultID), "%d", actionresultID);
                              safecpy(S_State,          "Pendingcompletion",      sizeof(S_State));
                              safecpy(S_Statemodifier,  "Prepare_cmd_failed",     sizeof(S_Statemodifier));
                              rc = perform_update(SUBNAME "xxx7", sql_connection, "daf.workqueue", fields2, values2, where, errmsg, sizeof(errmsg));
                              if (rc != 0) {
                                 print_scenario_console_log("xxx7");
                                 print_scenario_console_log(errmsg);
                              }
                        
                           } else {
                    
                              int tag = prepare_result.remote_client_prepare_cmd_res_u.outcome.tag;

                              snprintf(query, sizeof(query), "UPDATE daf.workqueue SET Tag='%u', State='Preparing' WHERE ID='%u'", tag, workqueue.ID); 
                              perform_query(SUBNAME, sql_connection, query);

                              snprintf(msg, sizeof(msg), "Executing action in step: %u  hostname: %s  workqueue.ID: %u  tag: %u  " 
                                                      "Maxduration: %u secs  Duration: %s  Identstring: %s\n", 
                                       workqueue.Stepnumber, workqueue.Hostname, workqueue.ID, tag, workqueue.Maxduration, workqueue.Duration, identstring);
                              print_scenario_console_log(msg);

                              memset(&execute_args,   0, sizeof(execute_args));
                              memset(&execute_result, 0, sizeof(execute_result));

                              safecpy(cmdstring, workqueue.Invocation, sizeof(cmdstring));   

                              execute_args.cmdstring         = cmdstring;     
                              execute_args.tag               = tag;
                              execute_args.run_in_shell      = 0; /* <<<<<<<<< */
                              execute_args.workqueueID       = workqueue.ID;
                              execute_args.actionresultID    = actionresultID;

                              if (MATCH(workqueue.Actiontype, "RunTestcaseInEnvironment")) {
                                 execute_args.run_in_shell   = FALSE;
                              } else if (MATCH(workqueue.Actiontype, "RunTestcaseInShell")) {
                                 execute_args.run_in_shell   = TRUE;
                              } else {
                                 /* problem */
                                 snprintf(msg, sizeof(msg), "%s: Internal error: unexpected value of workqueue.Actiontype (%s)\n",
                                          SUBNAME, workqueue.Actiontype);
                                 print_scenario_console_log(msg);
                                 /* <<<<<<<<<< what to do now */
                              }

                              execute_args.msglevel             = 0;
                              execute_args.environment_settings = NULL;

                              envlist p;    /* struct envnode *p */
                              envlist *q = &(execute_args.environment_settings);    /* struct envnode **q */

                              q = &(execute_args.environment_settings);
                              i = 0;
                              while (i < num_environment_assignments) {
                                 p = (envlist) malloc(sizeof(envnode ));
                                 p->entry = &(environment_assignments[i].Value[0]);
                                 p->next = NULL;
                                 (*q) = p;
                                 q = &(p->next);
                                 snprintf(msg, sizeof(msg), "   Environment[%d]: %s\n", i, environment_assignments[i].Value);
                                 print_scenario_console_log(msg);
                                 i++;
                              } 
 
                              snprintf(msg, sizeof(msg), "   Invocation: %s\n", workqueue.Invocation);
                              print_scenario_console_log(msg);

                              if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_EXECUTE_CMD,
                                                    (xdrproc_t) xdr_remote_client_execute_cmd_args, (caddr_t) &execute_args,
                                                    (xdrproc_t) xdr_remote_client_execute_cmd_res,  (caddr_t) &execute_result,
                                                    TIMEOUT)) != RPC_SUCCESS) {

                                 clnt_stat_decode(stat, errmsg, sizeof(errmsg));
                                 snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n", 
                                          SUBNAME, (long) CLIENT_REMOTE_CLIENT_EXECUTE_CMD, stat, errmsg);
                                 print_scenario_console_log(msg);

                                 snprintf(where, sizeof(where), "ID = %u", workqueue.ID);
                                 snprintf(S_ActionresultID, sizeof(S_ActionresultID), "%d", actionresultID);
                                 safecpy(S_State,          "Pendingcompletion",      sizeof(S_State));
                                 safecpy(S_Statemodifier,  "Clnt_call_failed",       sizeof(S_Statemodifier));
                                 rc = perform_update(SUBNAME "xxx8", sql_connection, "daf.workqueue", fields2, values2, where, errmsg, sizeof(errmsg));
                                 if (rc != 0) {
                                    print_scenario_console_log("xxx8");
                                    print_scenario_console_log(errmsg);
                                 }

                              } else {
    
                                 if (execute_result.status != REMOTE_CMD_OK) {
     
                                   /* execute failed */
                                    snprintf(msg, sizeof(msg) ,"%s: execute_cmd() failed - status = %d - testcase did not run\n", 
                                              SUBNAME, execute_result.status);
                                      print_scenario_console_log(msg);

                                     snprintf(where, sizeof(where), "ID = %u", workqueue.ID);
                                     snprintf(S_ActionresultID, sizeof(S_ActionresultID), "%d", actionresultID);
                                     safecpy(S_State,          "Pendingcompletion",      sizeof(S_State));
                                     safecpy(S_Statemodifier,  "Execute_cmd_failed",     sizeof(S_Statemodifier));
                                     rc = perform_update(SUBNAME "xxx9", sql_connection, "daf.workqueue", fields2, values2, where, errmsg, sizeof(errmsg));
                                     if (rc != 0) {
                                        print_scenario_console_log("xxx9");
                                        print_scenario_console_log(errmsg);
                                     }
/* <<<< what about cleaning up the command queue on the remote host ?? */

                                 } else {

                                    /* --------------------------------------------------------------------------------------------- */
                                    /* Update the State and actionresultID field in the workqueue to 'Running'                       */
                                    /* --------------------------------------------------------------------------------------------- */
   
                                    snprintf(where, sizeof(where), "ID = %u", workqueue.ID);
                                    snprintf(S_ActionresultID, sizeof(S_ActionresultID), "%d", actionresultID);
                                    safecpy(S_State, "Running", sizeof(S_State));
                                    rc = perform_update(SUBNAME "xxx10", sql_connection, "daf.workqueue", fields, values, where, errmsg, sizeof(errmsg)); 
                                    if (rc != 0) {
                                       print_scenario_console_log("xxx10");
                                       print_scenario_console_log(errmsg);
                                    }

                                 }
 
                              }
                           }

                        }

                        if (remote_client->cl_auth != NULL) {
                              auth_destroy(remote_client->cl_auth);
                        }

                        clnt_destroy(remote_client);
  
                  }

               }

            } else if (MATCH(workqueue.State, "Pendingcompletion")) {
   
               snprintf(msg, sizeof(msg), "   Completed action in step %u, hostname %s, workqueue.ID %u, pass %u\n", 
                        workqueue.Stepnumber, workqueue.Hostname, workqueue.ID, workqueue.Pass) ;
               print_scenario_console_log(msg);

               /* ------------------------------------------------------------------------------------------- */
               /* Update the actionresult record with the status reportd in the workqueue reoord              */
               /* ------------------------------------------------------------------------------------------- */

               if (MATCH(workqueue.Statemodifier, "None")) {
                  safecpy(S_Status, "Completed",  sizeof(S_Status));
               } else {
                  safecpy(S_Status, workqueue.Statemodifier,  sizeof(S_Status));
               }
               snprintf(S_Pass,          sizeof(S_Pass),           "%d", workqueue.Pass);
               safecpy(S_End,            "XXXX_CURRENT_TIME_XXXX", sizeof(S_End));

               snprintf(where, sizeof(where), "ID = %u", workqueue.ActionresultID);
               snprintf(table_name, sizeof(table_name), "%s.actionresult", status_database_name);
               rc = perform_update(SUBNAME, sql_connection, table_name, fields3, values3, where, errmsg, sizeof(errmsg));
               if (rc != 0) {
                  print_scenario_console_log(errmsg);
               }
  
               /* ------------------------------------------------------------------------------------------- */
               /* Update the workqueue record to set the State to 'Completed'                                 */
               /* ------------------------------------------------------------------------------------------- */

               snprintf(where, sizeof(where), "ID = %u", workqueue.ID);
               rc = perform_update(SUBNAME "xxx2", sql_connection, "daf.workqueue", fields4, values4, where, errmsg, sizeof(errmsg));
               if (rc != 0) {
                      print_scenario_console_log("xxx2");
                  print_scenario_console_log(errmsg);
               }

               /* ------------------------------------------------------------------------------------------- */
               /* Get the standard out/standard erorr and console log files and copy them                     */
               /* (except in the case where we could not identify a remote host to run the action on - as     */
               /* then there is nowhere to copy the files from!                                               */
               /* ------------------------------------------------------------------------------------------- */

               if ((! MATCH(workqueue.Statemodifier, "No_available_host")) && 
                   (! MATCH("No_remote_host_connect", workqueue.Statemodifier))) {

                  rc =  remote_client_get_dirlist(workqueue.Hostname, DAF_SERVICE_LOG_DIR , &pdl_base);    /* check rc <<<<<<<<< */

                  pdl = pdl_base;

                  while (pdl != NULL) {

                     if ((!MATCH(".", pdl->entry)) && (!MATCH("..", pdl->entry))) {
                    
                        safecpy(local_pathname, workqueue.Loglocation, sizeof(local_pathname));  
                        safecat(local_pathname, "/", sizeof(local_pathname));
                        safecat(local_pathname, pdl->entry, sizeof(local_pathname)); 

                        safecpy(remote_pathname, DAF_SERVICE_LOG_DIR, sizeof(remote_pathname)); // <<<<<<<< hardcoded 
                        safecat(remote_pathname, "/", sizeof(remote_pathname));
                        safecat(remote_pathname, pdl->entry, sizeof(remote_pathname));   

                        rc = remote_client_copyfile(workqueue.Hostname, local_pathname, remote_pathname, TRUE);    
                        if (rc == E_OK) {
                           snprintf(msg, sizeof(msg), "   Copied %s from test host %s, log directory %s\n", 
                                    local_pathname, workqueue.Hostname, remote_pathname);  
                           print_scenario_console_log(msg);   
                           rc = remote_client_deletefile(workqueue.Hostname, remote_pathname);    
                           if (rc != E_OK) {
                              snprintf(msg, sizeof(msg), "Problem deleting %s on remote test host %s, log directory %s\n", 
                                       remote_pathname, workqueue.Hostname, remote_pathname);  
                              print_scenario_console_log(msg);   
                           }
                        } else {
                           snprintf(msg, sizeof(msg), "Problem copying %s from remote test host %s, log directory %s\n", 
                                    local_pathname, workqueue.Hostname, remote_pathname);  
                           print_scenario_console_log(msg);   
                        }

                     }
                     pdl = pdl->next;
                  }
                  deallocate_dirlist(pdl_base);

               }

            }
                  
            close_scenario_console_log();

         }

         mysql_free_result(res_set);

      }


   }


}

/*---------------------------------------------------------------------------------------------------*/
/*                                                                                                   */
/* Function:   process_new_scenario_requests                                                         */
/*                                                                                                   */
/* Inputs:     sql_connection      An established connection to the status database                  */
/*                                                                                                   */
/* Outputs:    none                                                                                  */
/*                                                                                                   */
/* Returns:    void                                                                                  */
/*                                                                                                   */
/* Function:   It is intended that this routine is called from the Automation Framework status       */
/*             daemon.  The routine examines the contents of the workrequest table and selects any   */
/*             records that are in the Idle state - these are records that are created by the user   */
/*             when they initiate the execution of a particular scenario.   To allow for future      */
/*             generalisations, the workrequest record contains some self defining fields. These     */
/*             are currently used as follows:                                                        */
/*                                                                                                   */
/*             workrequest field     Use                                                             */
/*                                                                                                   */
/*             ObjectID1             the ID of the scenario record to be used in this request        */
/*             Tablename1            "scenario" - the name of the table used for ObjectID1           */
/*             ObjectID2             the ID of the scenario record to be used in this request        */
/*             Tablename1            "scenarioresult" - the name of the table used for ObjectID2     */
/*             ObjectID3             the ID of the scenarioresult record to be used in this request  */
/*             Tablename1            "testlevel - the name of the table used for ObjectID3           */
/*             ObjectID4             the ID of the teststand record to be used in this request       */
/*             Tablename1            "teststand" - the name of the table used for ObjectID4          */
/*                                                                                                   */
/*             For each workrequest record in the Idle state, the routine sets the state of that     */
/*             record to the Running state and creates a corresponding new ScenarioResult record,    */
/*             also in the Running state.   The routine then examines the first step in the          */
/*             scenario and creates workqueue records in the Idle state for each of the actions in   */
/*             this first step.  (These workqueue records will be subsequently processed by the      */
/*             process_new_workqueue_requests() routine, when their testcases will be launched).     */
/*                                                                                                   */
/*             This routine returns once all the workrequest records that are in the Idle state      */
/*             have been processed.                                                                  */
/*                                                                                                   */
/*---------------------------------------------------------------------------------------------------*/

void process_new_scenario_requests(MYSQL *sql_connection) {

#undef  SUBNAME
#define SUBNAME "process_new_scenario_requests"

   MYSQL_RES   *res_set;
   MYSQL_ROW   row;
   char        status_database_name[DAF_DATABASE_NAME_LEN];   
   char        status_table_name[DAF_TABLE_NAME_LEN];  
   char        scenario_name[DAF_SCENARIO_LEN];
   char        scenario_on_pass_name[DAF_SCENARIO_LEN];
   char        scenario_on_fail_name[DAF_SCENARIO_LEN];
   char        maillist_name[DAF_MAILLIST_LEN];
   char        testlevel_name[DAF_TESTLEVEL_LEN];
   char        teststand_name[DAF_TESTSTAND_LEN];
   char        where[128];
   char        msg[MAX_MSG_LEN];
   char        errmsg[MAX_MSG_LEN];
   int         TesterID;
   int         ScenarioID;
   int         ScenarioresultID;
   int         TestlevelID;
   int         TeststandID;
   int         OutcomeactionID;
   int         MaillistID;
   int         Stepnumber;
   char        Loglocation[128];
   char        temp[128];

   workrequest_t  workrequest;
   int            rc;
   char           *fields[] = {"State", "Start", NULL};   
   char           *values[] = {"Running", "XXXX_CURRENT_TIME_XXXX", NULL};

   int            num_actiontype_records;
   int            num_testcase_records;
   int            num_action_records;
   actiontype_t   *p_actiontypes = NULL;
   testcase_t     *p_testcases = NULL;
   action_t       *p_actions = NULL;  
   scenario_t     scenario;
   scenarioresult_t scenarioresult;
   char           query[1024];
   user_t         user;
   user_t         outcomeaction_user;
   outcomeaction_t outcomeaction;
   int  num_current_actions;
   int  max_allowed_actions;

   /* --------------------------------------------------------------------------------------------- */
   /*                                                                                               */
   /* --------------------------------------------------------------------------------------------- */

   safecpy(query, "SELECT " WORKREQUEST_FIELDS " FROM daf.workrequest WHERE State = 'Idle'", sizeof(query));
   if (perform_query(SUBNAME, sql_connection, query) != 0) { 

      snprintf(msg, sizeof(msg), "%s: query failed %u - %s\n", 
               SUBNAME, mysql_errno(sql_connection), mysql_error(sql_connection));
      print_msg_to_console(msg);

   } else {

      if ((res_set = check_for_res_set(SUBNAME, sql_connection, query))) {

         /* --------------------------------------------------------------------------------------------- */
         /* Look to see if any new Scenarios are to be started - these will appear as entries in the      */
         /* workrequest table that have a State of 'Idle'                                                 */
         /* --------------------------------------------------------------------------------------------- */

         p_actiontypes = allocate_and_get_all_actiontype_records(sql_connection, &num_actiontype_records);
         p_testcases   = allocate_and_get_all_testcase_records(sql_connection, &num_testcase_records);

         while ((row = mysql_fetch_row(res_set)) != NULL) {

            get_workrequest_record(res_set, &row, &workrequest); 

            if (MATCH(workrequest.State, "Idle")) {   /* <<<<<<<<<< */

               ScenarioID       = workrequest.ObjectID1;
               ScenarioresultID = workrequest.ObjectID2;
               TestlevelID      = workrequest.ObjectID3;
               TeststandID      = workrequest.ObjectID4;
               TesterID         = workrequest.TesterID;

               get_scenario(sql_connection, ScenarioID, &scenario);
               OutcomeactionID = scenario.OutcomeactionID;

               get_maillistID_from_outcomeaction(sql_connection, OutcomeactionID, &MaillistID);

               safecpy(Loglocation, workrequest.Logdirectory, sizeof(Loglocation));  
               safecat(Loglocation, "/scenario_", sizeof(Loglocation));
               snprintf(temp, sizeof(temp), "%u", ScenarioresultID);
               safecat(Loglocation, temp, sizeof(Loglocation));
               safecat(Loglocation, "_", sizeof(Loglocation));
               safecpy(temp, scenario.Name, sizeof(temp));
               make_legal_filename(temp);
               safecat(Loglocation, temp, sizeof(Loglocation));
             
               Stepnumber = 1;

               ensure_directory_path_exists(Loglocation);

               get_name_from_ID(sql_connection, "scenario",  ScenarioID,  scenario_name, sizeof(scenario_name));
               get_name_from_ID(sql_connection, "testlevel", TestlevelID, testlevel_name, sizeof(testlevel_name));
               get_name_from_ID(sql_connection, "teststand", TeststandID, teststand_name, sizeof(teststand_name));

               get_user(sql_connection, TesterID, &user);
               get_outcomeaction(sql_connection, OutcomeactionID, &outcomeaction);

               open_scenario_console_log(Loglocation, workrequest.Scenariologfilename);
              
               print_scenario_console_log_doublerule();
               snprintf(msg, sizeof(msg), "Starting new Scenario\n");
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Project           = %s\n", workrequest.Project);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Phase             = %s\n", workrequest.Phase);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Scenario ID       = %u (%s)\n", ScenarioID, scenario_name);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   ScenarioresultID  = %u\n", ScenarioresultID);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Testlevel ID      = %u (%s)\n", TestlevelID, testlevel_name);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Teststand ID      = %u (%s)\n", TeststandID, teststand_name);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   OutcomeactionID   = %u\n", OutcomeactionID);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   TesterID          = %u (%s %s (%s))\n", TesterID, user.Firstname, user.Lastname, user.Email);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   MaillistID        = %u\n", MaillistID);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Loglocation       = %s\n", Loglocation);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Scenario Log file = %s\n", workrequest.Scenariologfilename);
               print_scenario_console_log(msg);
               print_scenario_console_log("\n");

               get_name_from_ID(sql_connection, "scenario",  outcomeaction.Scenariotoberunonpass,  scenario_on_pass_name, sizeof(scenario_on_pass_name));
               get_name_from_ID(sql_connection, "scenario",  outcomeaction.Scenariotoberunonfail,  scenario_on_fail_name, sizeof(scenario_on_fail_name));
               get_name_from_ID(sql_connection, "maillist",  outcomeaction.MaillistID,             maillist_name,         sizeof(maillist_name));
               get_user(sql_connection, outcomeaction.TesterID, &outcomeaction_user);
               snprintf(msg, sizeof(msg), "   Outcomeaction Details:\n");
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Name:                  %s\n", outcomeaction.Name);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Actiononpass:          %s\n", outcomeaction.Actiononpass);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Actiononfail:          %s\n", outcomeaction.Actiononfail);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Scenariotoberunonpass: %d (%s)\n", outcomeaction.Scenariotoberunonpass, scenario_on_pass_name);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Scenariotoberunonfail: %d (%s)\n", outcomeaction.Scenariotoberunonfail, scenario_on_fail_name);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Erroraction:           %s\n", outcomeaction.Erroraction);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Notificationonpass:    %s\n", outcomeaction.Notificationonpass);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   Notificationonfail:    %s\n", outcomeaction.Notificationonfail);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   TesterID:              %d (%s %s (%s))\n", outcomeaction.TesterID, outcomeaction_user.Firstname, 
                                          outcomeaction_user.Lastname, outcomeaction_user.Email);
               print_scenario_console_log(msg);
               snprintf(msg, sizeof(msg), "   MaillistID:            %d (%s)\n", outcomeaction.MaillistID, maillist_name);
               print_scenario_console_log(msg);

               print_scenario_console_log_rule();
   
               /* -------------------------------------------------------------------------- */
               /* Deduce which status database we should record the results in               */                
               /* -------------------------------------------------------------------------- */

               determine_status_database_name(workrequest.Project, workrequest.Phase, status_database_name, sizeof(status_database_name));
               determine_status_table_name(status_database_name, "scenarioresult", 
                                           status_table_name, sizeof(status_table_name));

               /* -------------------------------------------------------------------------- */
               /* Confirm that we have the correct code/hw levels installed on the teststand */                
               /* as specified in the Testlevel for this run - if not, cancel the Scenario   */
               /* -------------------------------------------------------------------------- */

               /* first we need to get the scenarioresult so that we can find out the TestlevelrecordID */
               /* <<<< should we be checking the error code of the next routine here and elsewhere??? */
               get_scenarioresult(sql_connection, ScenarioresultID, status_table_name, &scenarioresult);

               if (validate_object_code_levels(sql_connection, TestlevelID, testlevel_name, 
                           TeststandID, teststand_name, workrequest.Project, workrequest.Phase, scenarioresult.TestlevelrecordID) != E_OK) {

                  /* -------------------------------------------------------------------------- */
                  /* Testlevels do not match - cancel the Workrequest and archive it            */
                  /* -------------------------------------------------------------------------- */

/* <<<< email out completion status ? */

                  snprintf(msg, sizeof(msg), "Testlevel does not match not actual levels in teststand - scenario %s will be canceled\n", 
                                             scenario_name);
                  print_scenario_console_log(msg);

                  snprintf(query, sizeof(query), "UPDATE daf.workrequest SET State='Failedtestlevelvalidation', Start=Now(), End=Now() where "
                                                 "Tablename2='scenarioresult' AND ObjectID2='%d'", ScenarioresultID);
                  perform_query(SUBNAME, sql_connection, query);
                  archive_workrequest(sql_connection, status_database_name, ScenarioresultID);
                  print_scenario_console_log_doublerule();

                 /* ------------------------------------------------------------------------------------------- */
                 /* Move the state of the Scenarioresult record to the same state as the original Workrequest   */
                 /* means the scenarioresult State will be Failedtestlevelvalidation                            */
                 /* ------------------------------------------------------------------------------------------- */

                  snprintf(query, sizeof(query), "UPDATE %s SET State='Failedtestlevelvalidation', End=Now() where "
                                                 "ID='%d'", status_table_name, ScenarioresultID);
                  perform_query(SUBNAME, sql_connection, query);

               } else {

                  /* ----------------------------------------------------------------------------------------------*/
                  /* Check to see if we are going to exceed the number of actions allowed in the licence           */
                  /* ----------------------------------------------------------------------------------------------*/

                  num_current_actions = count_entries_in_workqueue(sql_connection);
                  max_allowed_actions = get_licence_max_actions("/opt/daf/daf_licence_file", errmsg, sizeof(errmsg)); /* <<<< hardcoded */

                  if (max_allowed_actions == 0) {

                     snprintf(msg, sizeof(msg), "Cannot limit total actions as there is a problem with licence file: %s\n", errmsg);
                     print_scenario_console_log(msg);

                  } 

                     if (((num_current_actions + num_action_records) <= max_allowed_actions) || (max_allowed_actions == 0)) { 

                        /* -------------------------------------------------------------------------- */
                        /* A new scenario has been started - update workrequest.State to 'Running'    */
                        /* and                             - update scenarioresult.State to 'Running' */
                        /* -------------------------------------------------------------------------- */

                        snprintf(where, sizeof(where), "ID = %u", workrequest.ID);
                        rc = perform_update(SUBNAME "xxx3", sql_connection, "workrequest", fields, values, where, errmsg, sizeof(errmsg));
                        if (rc != 0) {
                      print_scenario_console_log("xxx3");
                           print_scenario_console_log(errmsg);
                        }

                        snprintf(where, sizeof(where), "ID = %u", ScenarioresultID);
                        rc = perform_update(SUBNAME, sql_connection, status_table_name, fields, values, where, errmsg, sizeof(errmsg)); 
                        if (rc != 0) {
                           print_scenario_console_log(errmsg);
                        }

                        /* -------------------------------------------------------------------------- */
                        /* Start the first step in the scenario                                       */
                        /* -------------------------------------------------------------------------- */

                        snprintf(where, sizeof(where), "ScenarioID = '%d' and Stepnumber = '%d'", ScenarioID, Stepnumber);
                        p_actions = allocate_and_get_action_records(sql_connection, where, &num_action_records);

                        print_scenario_console_log_doublerule();
                        snprintf(msg, sizeof(msg), "Scenario ID %u step %u contains %u action records\n", 
                                 ScenarioID, Stepnumber, num_action_records);
                        print_scenario_console_log(msg);

                        rc = create_actions_for_step(sql_connection,
                                                  workrequest.Project,
                                                  workrequest.Phase,
                                                  p_actions,
                                                  num_action_records,
                                                  p_actiontypes,
                                                  num_actiontype_records,
                                                  p_testcases,
                                                  num_testcase_records,
                                                  ScenarioID,
                                                  Stepnumber,
                                                  ScenarioresultID,
                                                  TestlevelID,
                                                  TeststandID,
                                                  OutcomeactionID,
                                                  TesterID,         
                                                  MaillistID,
                                                  Loglocation,
                                                  workrequest.Scenariologfilename);

                        if (p_actiontypes != NULL) {
                           free(p_actions);
                        }
                     } else {
                        /* tell the user that the licence does not allow this many actions and that the scenario has not been */
                        /* started yet <<<<<<<<<<                                                                             */
                     }

               }
               close_scenario_console_log();
            }

         }

         if (p_actiontypes != NULL) {
            free(p_actiontypes);
         }
         if (p_testcases != NULL) {
            free(p_testcases);
         }

         mysql_free_result(res_set);

      }

   }

} 

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:    is_collector_set_on_this_host                                                            */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/* RETURNS:      void                                                                                     */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

bool_t is_collector_set_on_this_host(int collectortypeID, int hostID, collectorvalue_t *collectorvalue, 
                                     int num_collectorvalue_records, int *collectorvalueID, int *index) {

   int i;
   int found = FALSE;

   for (i=0; i < num_collectorvalue_records; i++) {
      if (((collectorvalue+i)->CollectortypeID == collectortypeID) &&
          ((collectorvalue+i)->HostID == hostID)) {
         *collectorvalueID = (collectorvalue+i)->ID;
         *index = i;
         found = TRUE;
         break;
      }
   }

   return found;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  af_daemon                                                                                  */
/*                                                                                                        */
/* PARAMS IN:  void *p                                                                                    */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    none         (never returns)                                                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/* This routine should be started and in run in a seperate background thread when the daf service is      */
/* started - the routine will then run until the daf service is terminated                                */
/*                                                                                                        */
/* This is the Automation Framework daemon.  The purpose of the daemon is to monitor the WorkRequest      */
/* table in the MYSQL database, and when the user creates a new work request, the daemon carries out      */
/* the individual steps needed to fulfill this request.                                                   */
/*                                                                                                        */
/* The workrequest will specify a particular Scenario that is to be executed.  The daemon will create     */
/* workqueue items that corresponding to individual actions within this scenario.  Each action will       */
/* cause a testcase or other program to run on a remote test host.  The daemon launches the command.      */
/* The command may run for some time.  When the comman completes, the command will change the State field */
/* of the appropriate entry in the workrequest table, to the 'Completed' value.  The daemon periodically  */
/* monitors all the workrequest entries and when it sees that one is completed, it determines if all the  */
/* other actions in the same step of that Scenario has completed.  If they have, then it creates          */
/* workrequests for the next step in the Scenario, or if there are no steps remaining, it then marks      */
/* the scenario as being completed.                                                                       */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void *af_daemon(void *p) {

#undef  SUBNAME
#define SUBNAME "af_daemon"

   af_daemon_thread_data_t *p_af_daemon_thread_data;
   MYSQL                   *sql_connection; 

   p_af_daemon_thread_data  = (af_daemon_thread_data_t *) p;

   pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */

   while (1) {

      sql_connection = do_connect(SUBNAME,
                                  p_af_daemon_thread_data->sql_servername, 
                                  p_af_daemon_thread_data->sql_username, 
                                  p_af_daemon_thread_data->sql_password, 
                                  p_af_daemon_thread_data->sql_databasename,
                                  p_af_daemon_thread_data->sql_port, 
                                  NULL, 
                                  0);

      if (sql_connection != NULL) {

           process_new_scenario_requests(sql_connection);
           process_new_workqueue_requests(sql_connection,
                                          p_af_daemon_thread_data->sql_servername,
                                          p_af_daemon_thread_data->sql_username,
                                          p_af_daemon_thread_data->sql_password,
                                          p_af_daemon_thread_data->sql_databasename,
                                          p_af_daemon_thread_data->sql_port,
                                          p_af_daemon_thread_data->simulator_mode);
           process_workqueue(sql_connection);
           do_disconnect(SUBNAME, sql_connection);

      }
  
      sleep(2);

   }

   return( (void *) NULL);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  af_collector                                                                               */
/*                                                                                                        */
/* PARAMS IN:  void *p                                                                                    */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    none         (never returns)                                                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/* This routine should be started and in run in a seperate background thread when the daf service is     */
/* started - the routine will then run until the daf service is terminated                               */
/*                                                                                                        */
/* This is the Collector daemon.   The purpose of the daemon is to                                        */
/*    1.  determine which agents are alive in the remote test hosts                                       */
/*    2.  ensure the manifest information (ie collectorvalue entries) for each test host are up to date   */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void *af_collector(void *p) {

#undef  SUBNAME
#define SUBNAME "af_collector"

#define TEMP_MAX_NUM_HOSTS 500

   af_collector_thread_data_t        *p_af_collector_thread_data;
   MYSQL                             *sql_connection; 
   host_t                            hosts[TEMP_MAX_NUM_HOSTS];
   int                               num_host_records;
   int                               i,j;

   CLIENT *                          remote_client;            /* the RPC handle to reach the remote test host                 */
   remote_client_query_version_args  args;
   remote_client_query_version_res   remoteresult;
   remote_client_run_cmd_args        run_cmd_args;
   remote_client_run_cmd_res         run_cmd_result;
   enum clnt_stat                    stat;
   char                              msg[MAX_MSG_LEN];
   char                              errmsg[MAX_MSG_LEN];
   static struct timeval             TIMEOUT = { 25, 0 };

   char        S_CollectortypeID[16];
   char        S_HostID[16];
   char        S_Value[128];  /* <<<<<<<<<< who sets this length */
   char        *fields[] = {"CollectortypeID", "HostID", "Value", "Valuestatusdate", NULL};
   char        *values[5];
   char        *fields1[] = {"Value", "Valuestatusdate", NULL};
   char        *values1[3];
   char        where[128];
   int         rc;

   int         num_builtin_collectortypes; 
   int         num_user_collectortypes; 
   int         num_collectortypes; 
   char        *builtin_collector_name;
   char        *user_collector_name;
   int         collectortypeID;
   int         collectorvalueID;
   int         index;

   #define MAX_NUM_COLLECTORTYPES 100
   #define MAX_NUM_COLLECTORVALUES 100

   collectorvalue_t  collectorvalues[MAX_NUM_COLLECTORTYPES];  /* what is the max number per host ?? <<<<<<<<<<<<< */
   int               num_collectorvalue_records;
   collectortype_t   builtin_collectortypes[MAX_NUM_COLLECTORTYPES];  /* what is the max number per status database ?? <<<<<<<<<<<<< */
   collectortype_t   user_collectortypes[MAX_NUM_COLLECTORTYPES];  /* what is the max number per status database ?? <<<<<<<<<<<<< */

   values[0]  = &S_CollectortypeID[0];
   values[1]  = &S_HostID[0];
   values[2]  = &S_Value[0];
   values[3]  = "XXXX_CURRENT_TIME_XXXX";
   values[4]  = NULL;

   values1[0]  = &S_Value[0];
   values1[1]  = "XXXX_CURRENT_TIME_XXXX";
   values1[2]  = NULL;


   p_af_collector_thread_data  = (af_collector_thread_data_t *) p;

   pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */

   /* ------------------------------------------------------------------------------------------------------ */
   /* This loop runs forever and performs the following operations each iteration of the loop                */
   /*                                                                                                        */
   /* Connect to the DAF database                                                                            */
   /* Determine the currnt list of collectortypes                                                            */
   /* Determine the current list of hosts                                                                    */
   /*                                                                                                        */
   /* Contact each host via the DAF agent, and get the current values of the built in collectors for that    */
   /* host.  Compare these values to the values stored in the DAF database, and if they are different,       */
   /* update the DAF database with the values from the host.                                                 */
   /* Then, look through the list of user defined collectortypes to see if any of these apply to the host,   */
   /* if they do, then run the collector on that host and, if the collectorvalue has changed, update the     */
   /* DAF database                                                                                           */
   /*                                                                                                        */
   /* sleep for N seconds and then move onto the next host                                                   */
   /*                                                                                                        */
   /* So if there are H hosts the time taken to examine all the collector values and update them if          */
   /* necessary is  H * N.  So with H = 1000 and N = 1, the time taken will be 16 minutes.                   */
   /*                                                                                                        */
   /* The collector loop is run every 3600 seconds, so it may somewhat longer than an hour for a collector   */
   /* value to be update automatically by this background process                                            */
   /*                                                                                                        */
   /* ------------------------------------------------------------------------------------------------------ */

   while (1) {

      /* ------------------------------------------------------------------------------------------------------ */
      /* Connect to the DAF databse, if the connection cannot be established, skip to end of loop and wait for  */
      /* loop sleep time (3600 secs) before retrying                                                            */
      /* ------------------------------------------------------------------------------------------------------ */

      sql_connection = do_connect(SUBNAME,
                                  p_af_collector_thread_data->sql_servername, 
                                  p_af_collector_thread_data->sql_username, 
                                  p_af_collector_thread_data->sql_password, 
                                  p_af_collector_thread_data->sql_databasename,
                                  p_af_collector_thread_data->sql_port, 
                                  NULL, 
                                  0);

      if (sql_connection != NULL) {

         /* ------------------------------------------------------------------------------------------------------ */
         /* Get a list of the current collectortypes and current hosts                                             */
         /* ------------------------------------------------------------------------------------------------------ */

         get_collectortype_records(sql_connection, 
                                   &(builtin_collectortypes[0]), 
                                   sizeof(builtin_collectortypes),    /* <<<<<<<< wrong sizeof ???? */
                                   &(user_collectortypes[0]), 
                                   sizeof(user_collectortypes),    /* <<<<<<<< wrong sizeof ???? */
                                   NULL, 
                                   &num_builtin_collectortypes,
                                   &num_user_collectortypes,
                                   &num_collectortypes);


         /* <<<<<<<< TODO - what if there are too many hosts to fit into memory ??  */
         get_host_records(sql_connection, &(hosts[0]), TEMP_MAX_NUM_HOSTS, NULL, &num_host_records);

         for (i=0; i < num_host_records; i++) {

            if (!p_af_collector_thread_data->simulator_mode) {

               snprintf(where, sizeof(where), "HostID = %u", hosts[i].ID);
               get_collectorvalue_records(sql_connection, &(collectorvalues[0]), MAX_NUM_COLLECTORVALUES,    /*<<<<<< can we reduce no of transactions? */
                                          where, &num_collectorvalue_records);
   
               remote_client = clnt_create(hosts[i].Name, DAF_PROG, DAF_VERSION, "tcp");

               /* ------------------------------------------------------------------------------------------------------ */
               /* Connect to the host - if the connection cannot be established, skip thist host and go onto the next    */
               /* one immediately                                                                                        */
               /* ------------------------------------------------------------------------------------------------------ */

               if (remote_client == (CLIENT *) NULL) {
   
                   snprintf(errmsg, sizeof(errmsg), "%s clnt_create failed for remote host %s: ", SUBNAME, hosts[i].Name);
                   snprintf(msg,    sizeof(msg),    "%s\n", clnt_spcreateerror(errmsg));
                   print_msg_to_console_log(msg);

               } else {

                  /* --------------------------------------------------------------------------------------------------- */
                  /* Perform a 'query version' operation on the DAF agent in the remote host - this will return the      */
                  /* values of the built-in collectortypes on that host.                                                 */
                  /* ----------------------------------------------------------------------------------------------------*/

                  memset(&args,         0, sizeof(args));
                  memset(&remoteresult, 0, sizeof(remoteresult));

                  args.agent_major_version  = daf_major_version;
                  args.agent_minor_version  = daf_minor_version;
                  args.agent_version_string = daf_version_string;
                  args.msglevel             = 0;

                  if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_QUERY_VERSION,
                                        (xdrproc_t) xdr_remote_client_query_version_args, (caddr_t) &args,
                                        (xdrproc_t) xdr_remote_client_query_version_res,  (caddr_t) &remoteresult,
                                        TIMEOUT)) != RPC_SUCCESS) {
                     clnt_stat_decode(stat, errmsg, sizeof(errmsg));
                     snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n",
                              SUBNAME, (long) CLIENT_REMOTE_CLIENT_QUERY_VERSION, stat, errmsg);
                     print_msg_to_console_log(msg);

                  } else {

                     snprintf(msg, sizeof(msg), "Local daf version %d.%d (%s), Remote service version %d.%d (%s)\n",
                              daf_major_version,
                              daf_minor_version,
                              daf_version_string,
                              remoteresult.remote_client_query_version_res_u.outcome.agent_major_version,
                              remoteresult.remote_client_query_version_res_u.outcome.agent_minor_version,
                              remoteresult.remote_client_query_version_res_u.outcome.agent_version_string);

                     print_msg_to_console_log(msg);
                     snprintf(msg, sizeof(msg), "p_name: %s, p_os_type: %s, p_cpu_architecture: %s, p_os_version: %s, p_os_description: %s\n",
                              remoteresult.remote_client_query_version_res_u.outcome.p_name,
                              remoteresult.remote_client_query_version_res_u.outcome.p_os_type,
                              remoteresult.remote_client_query_version_res_u.outcome.p_cpu_architecture,
                              remoteresult.remote_client_query_version_res_u.outcome.p_os_version,
                              remoteresult.remote_client_query_version_res_u.outcome.p_os_description);
                     print_msg_to_console_log(msg);   /* <<<<<<<< where are these going? logrotate?  */

                     if (remoteresult.remote_client_query_version_res_u.outcome.p_osdata_is_valid) {

                        /* ------------------------------------------------------------------------------------------------------ */
                        /* Work through the built-in collector values, issuing an UPDATE or INSERT collectorvalue SQL command to  */
                        /* update those values of collectors that are different on the host when compared to the value in the     */
                        /* DAF database                                                                                           */
                        /* ------------------------------------------------------------------------------------------------------ */

                        for (j=0; j < num_builtin_collectortypes; j++) {
 
                           builtin_collector_name = builtin_collectortypes[j].Name;
                           if (MATCH(builtin_collector_name, "P_NAME")) {
                              safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_name, sizeof(S_Value));
                           } else if (MATCH(builtin_collector_name, "P_OS_TYPE")) {
                              safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_os_type, sizeof(S_Value));
                           } else if (MATCH(builtin_collector_name, "P_OS_VERSION")) {
                              safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_os_version, sizeof(S_Value));
                           } else if (MATCH(builtin_collector_name, "P_OS_DESCRIPTION")) {
                              safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_os_description, sizeof(S_Value));
                           } else if (MATCH(builtin_collector_name, "P_CPU_ARCHITECTURE")) {
                              safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_cpu_architecture, sizeof(S_Value));
                           } else if (MATCH(builtin_collector_name, "P_AGENT_DESCRIPTION")) {
                              safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.agent_version_string, sizeof(S_Value));
                           } else if (MATCH(builtin_collector_name, "P_AGENT_MAJOR")) {
                              snprintf(S_Value, sizeof(S_Value), "%u", remoteresult.remote_client_query_version_res_u.outcome.agent_major_version);
                           } else if (MATCH(builtin_collector_name, "P_AGENT_MINOR")) {
                              snprintf(S_Value, sizeof(S_Value), "%u", remoteresult.remote_client_query_version_res_u.outcome.agent_minor_version);
                           } else if (MATCH(builtin_collector_name, "P_AGENT_BUILD_MACHINE")) {
                              safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.agent_build_machine, sizeof(S_Value));
                           } else {
                              /* problem */
                              snprintf(msg, sizeof(msg), "%s: Internal error: unexpected value of builtin_collector_name (%s)\n",
                                       SUBNAME, builtin_collector_name);
                              print_scenario_console_log(msg);
                           }

                           if (lookup_collectortypeID(&(builtin_collectortypes[0]), MAX_NUM_COLLECTORTYPES, 
                                                        builtin_collector_name, &collectortypeID)) {
   
                              /* ---------------------------------------------------------------------------- */
                              /* find out if there is a collectorvalue record that corresponds to this hostID */
                              /* ---------------------------------------------------------------------------- */
 
                              if (is_collector_set_on_this_host(collectortypeID, hosts[i].ID, &(collectorvalues[0]), 
                                                                num_collectorvalue_records, &collectorvalueID, &index)) {

                                 /* ----------------------------------------------------------------------------------- */
                                 /* a collectorvalue record does already exists for this host - so find out if it is    */
                                 /* already set to the collector value we have just read from the host - if it is, then */
                                 /* we do not need to waste a transaction on updating it                                */      
                                 /* ----------------------------------------------------------------------------------- */

                                if (!MATCH(S_Value, collectorvalues[index].Value)) {
     
                                    snprintf(where, sizeof(where), "ID = %u", collectorvalueID);
                                    rc = perform_update(SUBNAME "xxx4", sql_connection, "daf.collectorvalue", fields1, values1, where, errmsg, sizeof(errmsg));
                                    if (rc != 0) {
                      print_scenario_console_log("xxx4");
                                       print_scenario_console_log(errmsg);
                                    }
                                 }

                              } else {
  
                                 /* -------------------------------------------------------------------------------------- */
                                 /* a collectorvalue record did not already exists for this host - so we must create one   */
                                 /* -------------------------------------------------------------------------------------- */

                                 snprintf(S_CollectortypeID, sizeof(S_CollectortypeID), "%u",  collectortypeID );
                                 snprintf(S_HostID, sizeof(S_HostID), "%u",  hosts[i].ID );

                                 if (MATCH(builtin_collector_name, "P_NAME")) {
                                    safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_name, sizeof(S_Value));
                                 } else if (MATCH(builtin_collector_name, "P_OS_TYPE")) {
                                    safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_os_type, sizeof(S_Value));
                                 } else if (MATCH(builtin_collector_name, "P_OS_VERSION")) {
                                    safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_os_version, sizeof(S_Value));
                                 } else if (MATCH(builtin_collector_name, "P_OS_DESCRIPTION")) {
                                    safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_os_description, sizeof(S_Value));
                                 } else if (MATCH(builtin_collector_name, "P_CPU_ARCHITECTURE")) {
                                    safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.p_cpu_architecture, sizeof(S_Value));
                                 } else if (MATCH(builtin_collector_name, "P_AGENT_DESCRIPTION")) {
                                    safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.agent_version_string, sizeof(S_Value));
                                 } else if (MATCH(builtin_collector_name, "P_AGENT_MAJOR")) {
                                   snprintf(S_Value, sizeof(S_Value), "%u", remoteresult.remote_client_query_version_res_u.outcome.agent_major_version);
                                 } else if (MATCH(builtin_collector_name, "P_AGENT_MINOR")) {
                                   snprintf(S_Value, sizeof(S_Value), "%u", remoteresult.remote_client_query_version_res_u.outcome.agent_minor_version);
                                 } else if (MATCH(builtin_collector_name, "P_AGENT_BUILD_MACHINE")) {
                                   safecpy(S_Value, remoteresult.remote_client_query_version_res_u.outcome.agent_build_machine, sizeof(S_Value));
                                 } else {
                                    /* problem */
                                    snprintf(msg, sizeof(msg), "%s: Internal error: unexpected value of builtin_collector_name (%s)\n",
                                             SUBNAME, builtin_collector_name);
                                    print_scenario_console_log(msg);
                                 }

                                 rc = perform_insert(SUBNAME, 
                                                     sql_connection,
                                                     "collectorvalue",
                                                     fields,
                                                     values,
                                                     errmsg,
                                                     sizeof(errmsg));

                                 if (rc != 0) {
                                    snprintf(msg, sizeof(msg), "problem with sql update for collectorvalue with ID = %u - %s\n",
                                             collectorvalueID, errmsg);
                                   print_msg_to_console_log(msg);
                                 } else {
                                    snprintf(msg, sizeof(msg), "collectorvalue with ID = %u completed\n",
                                             collectorvalueID);
                                    print_msg_to_console_log(msg);
                                 }
                              }

                           } else {
                              /* problem - could not recognise collector type */
                              snprintf(msg, sizeof(msg), "%s: Internal error: unrecgnised builtin collector type for builtin_collector_name (%s)\n",
                                       SUBNAME, builtin_collector_name);
                              print_scenario_console_log(msg);
                           }
                        }
                     }
                  }

                  /* ------------------------------------------------------------------------------------------------------ */
                  /* Now we checked/updated all the builtin collectors for this host, we turn our attention to any user-    */
                  /* defined collectors for that same host                                                                  */
                  /* ------------------------------------------------------------------------------------------------------ */

                  for (j=0; j < num_user_collectortypes; j++) {
 
                     user_collector_name = user_collectortypes[j].Name;

                     /* ------------------------------------------------------------------------------------------------------ */
                     /* Only apply collector if it matches the OStype of the host in question                                  */
                     /* ------------------------------------------------------------------------------------------------------ */

                     if (!MATCH(user_collectortypes[j].OStype, remoteresult.remote_client_query_version_res_u.outcome.p_os_type)) {
                        break;
                     }

                     /* ------------------------------------------------------------------------------------------------------ */
                     /* Send a request to the remote agent to run the collector and get the value back                         */
                     /* ------------------------------------------------------------------------------------------------------ */

                     memset(&run_cmd_args,   0, sizeof(run_cmd_args));
                     memset(&run_cmd_result, 0, sizeof(run_cmd_result));

                     run_cmd_args.cmdstring = user_collectortypes[j].Invocation;
                     run_cmd_args.msglevel = 0;

                     if ((stat = clnt_call(remote_client, CLIENT_REMOTE_CLIENT_RUN_CMD,
                                           (xdrproc_t) xdr_remote_client_run_cmd_args, (caddr_t) &run_cmd_args,
                                           (xdrproc_t) xdr_remote_client_run_cmd_res,  (caddr_t) &run_cmd_result,
                                           TIMEOUT)) != RPC_SUCCESS) {
                        clnt_stat_decode(stat, errmsg, sizeof(errmsg));
                        snprintf(msg, sizeof(msg) ,"%s: clnt_call() for proc %lu failed - status = %d (%s)\n",
                                 SUBNAME, (long) CLIENT_REMOTE_CLIENT_RUN_CMD, stat, errmsg);
                        print_msg_to_console_log(msg);
                
                     } else {
               
                        if (run_cmd_result.remote_client_run_cmd_res_u.outcome.valid) {
                           safecpy(S_Value, run_cmd_result.remote_client_run_cmd_res_u.outcome.outputline, sizeof(S_Value));   
                        } else {
                           safecpy(S_Value, "unknown - collector failed", sizeof(S_Value));      /* <<<<<<<<< need more details */
                        }
 
                        if (lookup_collectortypeID(&(user_collectortypes[0]), MAX_NUM_COLLECTORTYPES, 
                                                   user_collector_name, &collectortypeID)) {
   
                           /* ---------------------------------------------------------------------------- */
                           /* find out if there is a collectorvalue record that corresponds to this hostID */
                           /* ---------------------------------------------------------------------------- */
 
                           if (is_collector_set_on_this_host(collectortypeID, hosts[i].ID, &(collectorvalues[0]), 
                                                             num_collectorvalue_records, &collectorvalueID, &index)) {

                               /* ----------------------------------------------------------------------------------- */
                               /* a collectorvalue record does already exists for this host - so find out if it is    */
                               /* already set to the collector value we have just read from the host - if it is, then */
                               /* we do not need to waste a transaction on updating it                                */      
                               /* ----------------------------------------------------------------------------------- */

                               if (!MATCH(S_Value, collectorvalues[index].Value)) {
                                  snprintf(where, sizeof(where), "ID = %u", collectorvalueID);
                                  rc = perform_update(SUBNAME, sql_connection, "daf.collectorvalue", fields1, values1, where, errmsg, sizeof(errmsg));
                                  if (rc != 0) {
                                     print_scenario_console_log(errmsg);
                                  }
                               } 

                            } else {
   
                               /* -------------------------------------------------------------------------------------- */
                               /* a collectorvalue record did not already exists for this host - so we must create one   */
                               /* -------------------------------------------------------------------------------------- */

                               snprintf(S_CollectortypeID, sizeof(S_CollectortypeID), "%u",  collectortypeID );
                               snprintf(S_HostID, sizeof(S_HostID), "%u",  hosts[i].ID );
  
                               rc = perform_insert(SUBNAME,
                                                   sql_connection,
                                                   "daf.collectorvalue",
                                                   fields,
                                                   values,
                                                   errmsg,
                                                   sizeof(errmsg));

                               if (rc != 0) {
                                  snprintf(msg, sizeof(msg), "problem with sql update for collectorvalue with ID = %u - %s\n",
                                           collectorvalueID, errmsg);
                                  print_msg_to_console_log(msg);
                              } else {
                                  snprintf(msg, sizeof(msg), "collectorvalue with ID = %u completed\n",
                                           collectorvalueID);
                                  print_msg_to_console_log(msg);
                              }
                           }   

                        } else {
                           /* problem - could not recognise collector type  */
                           snprintf(msg, sizeof(msg), "%s: Internal error: unrecgnised user collector type for user_collector_name (%s)\n",
                                    SUBNAME, user_collector_name);
                           print_scenario_console_log(msg);
                        }

                     }

                  }

                  if (remote_client->cl_auth != NULL) {
                     auth_destroy(remote_client->cl_auth);
                  }

                  clnt_destroy(remote_client);

               }                  

            }  else {

               /* do nothing in simulator mode */

            }

         }

         do_disconnect(SUBNAME, sql_connection);

      }

      sleep(3600);   /* <<<<<<<<<<<<<< what value should we really use ??? */

   }

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  af_agent_status_daemon                                                                     */
/*                                                                                                        */
/* PARAMS IN:  void *p                                                                                    */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    none         (never returns)                                                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/* This routine should be started and in run in a seperate background thread when the daf service is      */
/* started - the routine will then run until the daf service is terminated                                */
/*                                                                                                        */
/* This daemon periodically examines the state of each remote test host, specifically testing to see if   */
/* the DAF agent in the host responds or not.  If a good response is returned, then the appropriate       */
/* field in the record for that test host in the DAF host table is updated.                               */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void *af_agent_status_daemon(void *p) {

#undef  SUBNAME
#define SUBNAME "af_agent_status_daemon"

#define HOSTS_PER_SCAN_ELEMENT 5

   af_agent_status_daemon_thread_data_t *p_af_agent_status_daemon_thread_data;
   MYSQL                   *sql_connection; 
   host_t                  hosts[HOSTS_PER_SCAN_ELEMENT];
   char                    where[MAX_MSG_LEN];
   int                     num_host_records;
   int                     i;
   int                     rc;
   char                    errmsg[MAX_MSG_LEN];

   char        S_Agentstatus[16];

   char        *fields[] = {"Agentstatus", "Agentstatusdate", NULL};
   char        *values[3];

   values[0] = &S_Agentstatus[0];
   values[1] = "XXXX_CURRENT_TIME_XXXX";
   values[2] = NULL;

   p_af_agent_status_daemon_thread_data = (af_agent_status_daemon_thread_data_t *) p;

   pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */

   while (1) {

      sql_connection = do_connect(SUBNAME,
                                  p_af_agent_status_daemon_thread_data->sql_servername, 
                                  p_af_agent_status_daemon_thread_data->sql_username, 
                                  p_af_agent_status_daemon_thread_data->sql_password, 
                                  p_af_agent_status_daemon_thread_data->sql_databasename,
                                  p_af_agent_status_daemon_thread_data->sql_port, 
                                  NULL, 
                                  0);

      if (sql_connection != NULL) {
 
          /* ----------------------------------------------------------------------------------- */
          /* Get the names of the 5 hosts which have the oldest agent status - this guarantees   */
          /* that, as hosts are added and deleted, we don't (eventually) miss any of them        */
          /* ----------------------------------------------------------------------------------- */

          snprintf(where, sizeof(where), "Name LIKE '%%' ORDER BY Agentstatusdate ASC LIMIT %u\n", HOSTS_PER_SCAN_ELEMENT);  /* <<<<<<<<<< */
          get_host_records(sql_connection, &hosts[0], sizeof(hosts), where, &num_host_records);

          for (i=0; i < num_host_records; i++) {

             rc = remote_client_ping_agent(hosts[i].Name, TRUE);

             if (rc == 0) {
                safecpy(S_Agentstatus, "Online", sizeof(S_Agentstatus));
             } else {
                safecpy(S_Agentstatus, "Agent_Offline", sizeof(S_Agentstatus));
             }
             
             snprintf(where, sizeof(where), "ID = %u", hosts[i].ID );
             rc = perform_update(SUBNAME, sql_connection, "daf.host", fields, values, where, errmsg, sizeof(errmsg));
             if (rc != 0) {
                print_scenario_console_log(errmsg);
             }

             sleep(5);

          }

          do_disconnect(SUBNAME, sql_connection);

      }
  
      sleep(5);

   }

   return( (void *) NULL);

}



/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  print_active_steps                                                                         */
/*                                                                                                        */
/* PARAMS IN:  active_steps_object                                                                        */
/*                                                                                                        */
/* PARAMS OUT: none                                                                                       */
/*                                                                                                        */
/* RETURNS:    void                                                                                       */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_active_steps(active_steps_object_t  *active_steps_object, 
                        bool_t                 no_headers,
                        bool_t                 use_delimiter,
                        char                   *delimiter_string) {

#undef  SUBNAME
#define SUBNAME "print_active_steps"

   int               i;
   char              msg[MAX_MSG_LEN];

   char              *fmt_title       = "%12s  %18s  %10s  %16s\n";
   char              *fmt1            = "%12d  %18d  %10d  %16s\n";
   char              *fmt_title_delim = "%s%s%s%s%s%s%s\n";
   char              *fmt1_delim      = "%d%s%d%s%d%s%s\n";


   if (! no_headers) {
      if (use_delimiter) {
         snprintf(msg, sizeof(msg), fmt_title_delim, "ScenarioID", delimiter_string, "ScenarioresultID", delimiter_string, "Stepnumber", 
                  delimiter_string, "State");
      } else {
         snprintf(msg, sizeof(msg), fmt_title, "ScenarioID", "ScenarioresultID", "Stepnumber", "State");
      }
      print_string_to_console(msg);
   }

   lock_active_steps(active_steps_object);

   for  (i = 0; i< active_steps_object->num_active_steps; i++) {

      if (use_delimiter) {
         snprintf(msg, sizeof(msg), fmt1_delim, active_steps_object->step_array[i].ScenarioID, delimiter_string, 
                                    active_steps_object->step_array[i].ScenarioresultID, delimiter_string,
                                    active_steps_object->step_array[i].Stepnumber, delimiter_string,
                                    active_steps_object->step_array[i].State, delimiter_string);
     } else {
         snprintf(msg, sizeof(msg), fmt1, active_steps_object->step_array[i].ScenarioID, 
                                    active_steps_object->step_array[i].ScenarioresultID, 
                                    active_steps_object->step_array[i].Stepnumber, 
                                    active_steps_object->step_array[i].State);
     }
     print_string_to_console(msg);
  }
      
  unlock_active_steps(active_steps_object);

}


