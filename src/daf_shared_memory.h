#ifndef _H_DAF_SHARED_MEMORY
#define _H_DAF_SHARED_MEMORY

#include "OSstuff.h"
#include "daf_protocol.h"

#define SIZE_shm_daf_BYTES 300000  /* 300k bytes */

#define DAF_SHM_VERSION        1
#define DAF_MAX_PROCESSES      200
#define DAF_MAX_COMMANDS       200
#define MAX_NUM_RPC_CLIENTS     10           /* <<<<<<<<<<<<<<<<<<<< */

#define DAF_SHM_CMDLINE_LENGTH 256

#define DAF_PROCESS_NEVER_RUN 0
#define DAF_PROCESS_ACTIVE    1
#define DAF_PROCESS_COMPLETED 2
#define DAF_PROCESS_STALE     3
#define DAF_PROCESS_FREE      4

enum command_state_types { COMMAND_NEVER_RUN, COMMAND_INITIALISING, COMMAND_ACTIVE, COMMAND_COMPLETED };
enum system_command_state_types { SYSTEM_COMMAND_NEVER_RUN, SYSTEM_COMMAND_IN_PROGRESS, SYSTEM_COMMAND_COMPLETED};

typedef struct cmd_log_state_entry {
    enum cmd_log_state  cmd_log_state_num;
    const char          *cmd_log_state_name;
} cmd_log_state_entry_t;

typedef struct cmd_log_object {
    pthread_mutex_t   mp;
    cmd_log_entry     cmd_array[DAF_MAX_COMMANDS];
} cmd_log_object_t;

#define MAX_NUM_STEPS 100    /* <<<<<<<<<< */

typedef struct active_step_element {
    int ScenarioID;
    int ScenarioresultID;
    int Stepnumber;
    char State[16];   /*  <<<<<<  use a DEFINE ?? */
} active_step_element_t;

typedef struct active_steps_object {
    pthread_mutex_t         mp;
    int                     num_active_steps;
    active_step_element_t   step_array[MAX_NUM_STEPS];
} active_steps_object_t;

typedef struct shm_daf {

  Iu32                   shm_version;
  Iu32                   size_shm_shared_bytes;
  Iu32                   epoch_time_last_cleared;
  Iu32                   spare2;
  Iu32                   spare3;
  Iu32                   spare4;
  Iu64                   daemon_pid;
  Iu32                   daemon_tick;
  Iu32                   max_num_slots;
  Iu32                   next_slot_to_use;
  Iu32                   current_cmd_tag;
  cmd_log_object_t       service_cmd_log_object;
  active_steps_object_t  active_steps_object;
  pthread_mutex_t        service_steps_mutex;

} shm_daf_t;


bool_t get_shared_segment_process_pointer( shm_daf_t **p, char *errmsg, int max_msg_length);
bool_t find_cmd_log(cmd_log_object_t **p_service_cmd_log_object, char *errmsg, int max_msg_length);
bool_t find_active_steps(active_steps_object_t  **p_service_active_steps_object, char *errmsg, int max_msg_length);
void active_state(int active, char *active_string, int max_msg_length);
int initialise_shared_segment(char *statusmsg, char *errmsg, int max_msg_length);
int update_process_exit_code_in_shared_segment(int exit_rc, char *errmsg, int max_msg_length);
int terminate_use_of_shared_segment(char *errmsg, int max_msg_length);
int query_shared_segment_process_table(int *num_active_daf_processes, int *num_completed_daf_processes, char *errmsg, int max_msg_length);
shm_daf_t *query_shared_segment_process_pointer(void);
int initialise_shared_segment_values(char *errmsg, int max_msg_length);
int clear_cmd_array(char *errmsg, int max_msg_length);
int delete_shared_segment(char *errmsg, int max_msg_length);
int print_shared_segment_details(char *errmsg, int max_msg_length);
int print_shared_segment_audit_log(char *errmsg, int max_msg_length);

#endif

