
/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                        */
/*                                                                                                                      */
/* daf_serice_agent_environment.c                                                                                       */
/*                                                                                                                      */
/* This file is part of the daf NFS test automation program.                                                            */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

#include "daf_util.h"
#include "daf_shared_memory.h"
#include "daf_service_environment.h"

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           get_current_executable_path                         */
/*                                                                      */
/*  FUNCTION:       attempt to find the full pathname of the            */
/*                  executable that is currently running                */
/*                                                                      */
/*                  If executable begines with '/' or './' then         */
/*                  we assume that the executable name passed in is     */
/*                  a full pathname or a relative path respectively     */
/*                                                                      */
/*                  NOTE: we assume that this subroutine is being       */
/*                  run in the same directory that the executable was   */
/*                  invoked in.                                         */
/*                                                                      */
/*  PARAMS IN:      executable   the name of the program that we think  */
/*                               is running this is typically argv[0]   */
/*                               that was passed into main()            */
/*                                                                      */
/*  PARAMS OUT:     path         the full pathname of the executable    */
/*                               that called this routine               */
/*                  max_path_length   the max length of path            */
/*                                                                      */
/*  RETURNS:        0 if successful, non zero if not                    */
/*                                                                      */
/*----------------------------------------------------------------------*/
int get_current_executable_path(char *executable, char *path, int max_path_length)
{

    char cwdpath[64];
#if defined AIX
    char cmd[256];
    char msg[1024];
#elif defined LINUX
    char cmd[256];
    char msg[1024];
#elif defined SOLARIS
    const char *pexecpath;
#elif defined HPUX
    char *pexecpath;
#elif defined WIN32

#endif

    if (strlen(executable) == 0 )
    {
        safecpy(path, "notknown", max_path_length);
        return(1);
    }

    if (executable[0] == '/')
    {
        safecpy(path, executable, max_path_length);
        return(0);
    }

    if (strlen(executable) > 1)
    {
        if ((executable[0] == '.') && (executable[1] == '/'))
        {
            getcwd(cwdpath, sizeof(cwdpath));
            safecpy(path, cwdpath, max_path_length);
            safecat(path, &executable[1], max_path_length);
            return(0);
        }
    }

#if defined AIX

    sprintf(cmd, "which %s", executable);

    if (run_system_cmd_and_capture_single_line_output(cmd, path , max_path_length))
    {
        sprintf(msg, "command to find path of current executable: %s failed\n", cmd);
        print_msg_to_console(msg);
        return(1);
    }

#elif defined LINUX

    //  readlink("/proc/self/exe", path, max_path_length);

    sprintf(cmd, "which %s", executable);

    if (run_system_cmd_and_capture_single_line_output(cmd, path , max_path_length))
    {
        sprintf(msg, "command to find path of current executable: %s failed\n", cmd);
        print_msg_to_console(msg);
        return(1);
    }

#elif defined SOLARIS

    getcwd(cwdpath, sizeof(cwdpath));
    pexecpath = getexecname();

    safecpy(path, pexecpath, max_path_length);

    if (path[0] != '/')
    {
        safecpy(path, cwdpath, max_path_length);
        safecat(path, pexecpath, max_path_length);
    }

#elif defined HPUX

    pexecpath = pathfind(getenv("PATH"), executable, "x");
    safecpy(path, pexecpath, max_path_length);

#elif defined WIN32 || defined NETWARE

    safecpy(path, "not done yet", max_path_length);

#endif

    chomp(path);
    return(0);

}


/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           is_daf_service_running                              */
/*                                                                      */
/*  FUNCTION:                                                           */
/*                                                                      */
/*  PARAMS IN:                                                          */
/*                                                                      */
/*  PARAMS OUT:                                                         */
/*                                                                      */
/*  RETURNS:       0 if daf service is not running, pid if it is        */
/*                                                                      */
/*----------------------------------------------------------------------*/
unsigned long long is_daf_service_running(void)
{

    /* char cmd[1024];
    char msg[1024];
    char process_id[64];
    int  pid = 0; */

    char errmsg[1024];
    shm_daf_t *p;

    if (! get_shared_segment_process_pointer(&p, errmsg, sizeof(errmsg)))
    {
        return 0;
    }

    if (time(0) > (p->daemon_tick + 10))
    {
        /* daemon tick is more than 10 seconds out of date - so assume that daemon is not running */
        return 0;
    }
    else
    {
        return (int) p->daemon_pid;
    }


    /* safecpy(cmd, "ps -ef | grep \"", sizeof(cmd));
    safecat(cmd, DAF_SERVICE_INVOCATION_SHORT, sizeof(cmd));
    safecat(cmd, "\" | grep -v grep | awk {'print $2'}", sizeof(cmd));
    if (run_system_cmd_and_capture_single_line_output(cmd, process_id, sizeof(process_id)) != 0) {
       sprintf(msg, "ps -ef command to determine id failed: %s\n", cmd);
       print_msg_to_console(msg);
       return(0);
    }
    if (strlen(process_id) > 0) {
       pid = strtoul(process_id, NULL, 0);
    }
    return(pid); */

}

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           stop_daf_service_running                            */
/*                                                                      */
/*  FUNCTION:                                                           */
/*                                                                      */
/*  PARAMS IN:                                                          */
/*                                                                      */
/*  PARAMS OUT:                                                         */
/*                                                                      */
/*  RETURNS:                                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/
int stop_daf_service_running(void)
{

    char cmd[1024];
    char msg[1024];
    unsigned long long  pid;
    shm_daf_t *p;

    if ((pid = is_daf_service_running()) != 0)
    {

        sprintf(cmd, "kill -9 %llu", pid);          // should really use SIGINT to signal graceful exit

        if (run_system_cmd_and_print_all_output(cmd) != 0)
        {
            sprintf(msg, "kill of process id %llu failed: %s\n", pid, cmd);
            print_msg_to_console(msg);
            return(1);
        }
        else
        {
            sprintf(msg, "Process id %llu was killed with: %s\n", pid, cmd);
            print_msg_to_console(msg);
        }
    }

    if (! get_shared_segment_process_pointer(&p, msg, sizeof(msg)))
    {
        return 0;
    }

    /* make daemon tick 0 - to show that daemon is no longer running */
    /* otherwise, anyone waiting to see if the daemon had stopped would have have to wait 10 secs */
    /* before calling is_daf_service_running()  */
    p->daemon_tick = 0;
    return 0;


}


/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           start_daf_service_running                           */
/*                                                                      */
/*  FUNCTION:                                                           */
/*                                                                      */
/*  PARAMS IN:                                                          */
/*                                                                      */
/*  PARAMS OUT:                                                         */
/*                                                                      */
/*  RETURNS:                                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/
int start_daf_service_running(void)
{

    char cmd[1024];
    char msg[1024];
    char errmsg[1024];
    unsigned long long pid;

    if ((pid = is_daf_service_running()) == 0)
    {
        safecpy(cmd, DAF_SERVICE_INVOCATION, sizeof(cmd));

        if (run_system_cmd_in_background(cmd) != 0)
        {
            sprintf(msg, "command: %s failed\n%s\n", cmd, errmsg);
            print_msg_to_console(msg);
            return(1);
        }
    }
    else
    {
        print_msg_to_console("start_daf_service_running: daf service is already running so was not started again\n");
    }

    return(0);
}

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           create_default_daf_service_profile                  */
/*                                                                      */
/*  FUNCTION:                                                           */
/*                                                                      */
/*  PARAMS IN:                                                          */
/*                                                                      */
/*  PARAMS OUT:                                                         */
/*                                                                      */
/*  RETURNS:                                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/
int create_default_daf_service_profile(void)
{

#undef  SUBNAME
#define SUBNAME "create_daf_service_profile"

    char msg[1024];
    char *lines[]   = DAF_SERVICE_PROFILE_CONTENTS;
    int  line_count = 0;
    FILE *pf;

    if ((pf = fopen(DAF_SERVICE_PROFILE_PATH, "w")) != NULL)
    {
        while (lines[line_count] != (char *) 0)
        {
            fprintf(pf, "%s\n", lines[line_count]);
            line_count++;
        }

        fclose(pf);
    }
    else
    {
        sprintf(msg, "%s: could not write to profile: %s\n", SUBNAME, DAF_SERVICE_PROFILE_PATH);
        print_msg_to_console(msg);
        return(1);
    }

    if (chmod(DAF_SERVICE_PROFILE_PATH, 0x755) != 0)
    {
        sprintf(msg, "%s: could not set execute permission on: %s\n", SUBNAME, DAF_SERVICE_PROFILE_PATH);
        print_msg_to_console(msg);
        return(1);
    }

    return(0);
}

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           create_daf_service_start_script                     */
/*                                                                      */
/*  FUNCTION:                                                           */
/*                                                                      */
/*  PARAMS IN:                                                          */
/*                                                                      */
/*  PARAMS OUT:                                                         */
/*                                                                      */
/*  RETURNS:                                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/
int create_daf_service_start_script(void)
{

#undef  SUBNAME
#define SUBNAME "create_daf_service_start_script"

    char msg[1024];
    char *lines[]   = DAF_SERVICE_START_SCRIPT_CONTENTS;
    int  line_count = 0;
    FILE *pf;

    if ((pf = fopen(DAF_SERVICE_START_SCRIPT_PATH, "w")) != NULL)
    {
        while (lines[line_count] != (char *) 0)
        {
            fprintf(pf, "%s\n", lines[line_count]);
            line_count++;
        }

        fclose(pf);
    }
    else
    {
        sprintf(msg, "%s: could not write to start script: %s\n", SUBNAME, DAF_SERVICE_START_SCRIPT_PATH);
        print_msg_to_console(msg);
        return(1);
    }

    if (chmod(DAF_SERVICE_START_SCRIPT_PATH, 0x755) != 0)
    {
        sprintf(msg, "%s: could not set execute permission on: %s\n", SUBNAME, DAF_SERVICE_START_SCRIPT_PATH);
        print_msg_to_console(msg);
        return(1);
    }

    return(0);
}

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           create_etc_init_conf                                */
/*                                                                      */
/*  FUNCTION:   Creates a /etc/init/daf.conf or daf_agent.conf - this   */
/*              is used on Debian/Ubuntu instead of an /etc/inittab     */
/*              entry                                                   */
/*                                                                      */
/*  PARAMS IN:                                                          */
/*                                                                      */
/*  PARAMS OUT:                                                         */
/*                                                                      */
/*  RETURNS:                                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/
int create_etc_init_conf(void)
{

#undef  SUBNAME
#define SUBNAME "create_etc_init_conf"

    char msg[1024];
    char *lines[]   = DAF_DEBIAN_ETC_INIT_CONF;
    int  line_count = 0;
    FILE *pf;

    if ((pf = fopen(DAF_DEBIAN_ETC_INIT_CONF_PATH, "w")) != NULL)
    {
        while (lines[line_count] != (char *) 0)
        {
            fprintf(pf, "%s\n", lines[line_count]);
            line_count++;
        }

        fclose(pf);
    }
    else
    {
        sprintf(msg, "%s: could not write to: %s\n", SUBNAME, DAF_DEBIAN_ETC_INIT_CONF_PATH);
        print_msg_to_console(msg);
        return(1);
    }

    if (chmod(DAF_DEBIAN_ETC_INIT_CONF_PATH, 0x555) != 0)
    {
        sprintf(msg, "%s: could not set 0x555 permission on: %s\n", SUBNAME, DAF_DEBIAN_ETC_INIT_CONF_PATH);
        print_msg_to_console(msg);
        return(1);
    }

    return(0);
}

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           daf_service_install                                 */
/*                                                                      */
/*  FUNCTION:                                                           */
/*                                                                      */
/*  PARAMS IN:                                                          */
/*                                                                      */
/*  daf_binary_path    The path from where to get the daf binary that   */
/*                     will be copied to the daf_service_binary_path    */
/*                     if set to null, the routine will try to          */
/*                     the path of the current executable and copy that */
/*                     to daf_service_binary_path                       */
/*                                                                      */
/*  PARAMS OUT:                                                         */
/*                                                                      */
/*  RETURNS:                                                            */
/*                                                                      */
/* Install the daf service daemon                                       */
/*                                                                      */
/* 1.  Copy the daf binary that we want to use into                     */
/*     daf_service_binary_path                                          */
/* 1.  stop any existing daf daemon that is running                     */
/* 2.  make an entry for the daemon in /etc/inittab                     */
/* 3.  start a process that runs the daemon                             */
/*                                                                      */
/*----------------------------------------------------------------------*/
int daf_service_install(char *daf_binary_path, char *argv0)
{

    char cmd[1024];
    char msg[1024];
    char daf_path[64];
    char daf_binary_to_install[64];
    char timestamp[16];
    char backup_inittab_filename[64];

    struct stat fileStat;
    char *pathname = "/etc/inittab";

    if (daf_binary_path == NULL)
    {
        if (get_current_executable_path(argv0, daf_binary_to_install, sizeof(daf_binary_to_install)) !=0)
        {
            print_msg_to_console("Problem trying to determine path of current executable\n");
            return(1);
        }
    }
    else
    {
        safecpy(daf_binary_to_install, daf_binary_path, sizeof(daf_binary_to_install));
    }

    // ensure that directories that are needed do actually exist and delelete existing profile
    // and start up script

    ensure_directory_path_exists(DAF_SERVICE_BINARY_DIR);
    ensure_directory_path_exists(DAF_SERVICE_LOG_DIR);
    delete_file(DAF_SERVICE_START_SCRIPT_PATH);
    delete_file(DAF_SERVICE_PROFILE_PATH);

    // are we trying to copy the same file onto itself, -if so then
    // do not do the copy as it it will fail
    if (strcmp(daf_binary_to_install, DAF_SERVICE_BINARY_PATH) == 0)
    {

        sprintf(msg, "Requested install image %s is identical to install destination: %s\n",
                daf_binary_to_install, DAF_SERVICE_BINARY_PATH);
        print_msg_to_console(msg);

    }
    else
    {

        if (copy_file(daf_binary_to_install, DAF_SERVICE_BINARY_PATH) != 0)
        {
            /* make sure the SETuid bit is on so that daf can assume root priviledges if it is run by a non root user */
            /* is this a security hole ?? <<<<<<<<<< */
            sprintf(msg, "Could not copy %s into installation directory at %s\n", daf_binary_to_install, DAF_SERVICE_BINARY_PATH);
            print_msg_to_console(msg);
            return(1);
        }
        else
        {
            if (chmod_file(DAF_SERVICE_BINARY_PATH, "u+s") != 0)
            {
                sprintf(msg, "Could not set setuid permission bit on %s\n", DAF_SERVICE_BINARY_PATH);
                print_msg_to_console(msg);
                return(1);
            }
        }

    }

    if (create_default_daf_service_profile() != 0)
    {
        print_msg_to_console("Could not install command profile into standard daf service directory\n");
        return(1);
    }
    else
    {
        print_msg_to_console("daf service command profile installed successfully\n");
    }


    if (create_daf_service_start_script() != 0)
    {
        print_msg_to_console("Could not install start script into standard daf service directory\n");
        return(1);
    }
    else
    {
        print_msg_to_console("daf service start script installed successfully\n");
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------

    safecpy(daf_path, DAF_SERVICE_BINARY_PATH, sizeof(daf_path));

    if (is_daf_service_running())
    {
        print_msg_to_console("daf daemon is running - and will now be stopped\n");

        if (stop_daf_service_running() != 0)
        {
            print_msg_to_console("Problem trying to stop daf daemon\n");
        }
    }

    print_msg_to_console("Updating /etc/inittab entry\n");

#if defined AIX

    //--------------------------------------------------------------------------
    // remove any previous entry
    //--------------------------------------------------------------------------

    print_msg_to_console("Removing any previous daf service entry in /etc/inittab\n");
    safecpy(cmd, "cat /etc/inittab | grep \"daf:\"", sizeof(cmd));

    if (run_system_cmd(cmd, 1) == 0)
    {

        safecpy(cmd, "rmitab \"daf\"", sizeof(cmd));

        if (run_system_cmd_and_print_all_output(cmd) != 0)
        {
            safecpy(msg, "Removal of daf service entry in /etc/inittab failed: ", sizeof(msg));
            safecat(msg, cmd, sizeof(msg));
            safecat(msg, "\n", sizeof(msg));
            print_msg_to_console(msg);
            return(1);
        }
    }

    //--------------------------------------------------------------------------
    // Add daf daemon to /etc/inittab
    //--------------------------------------------------------------------------

    safecpy(backup_inittab_filename, "/etc/inittab.", sizeof(backup_inittab_filename));
    get_current_time_as_timestamp(timestamp, sizeof(timestamp), '.');
    safecat(backup_inittab_filename, timestamp, sizeof(backup_inittab_filename));
    sprintf(msg, "Backing up /etc/inittab in %s\n", backup_inittab_filename);
    print_msg_to_console(msg);

    if (copy_file("/etc/inittab", backup_inittab_filename) != 0)
    {
        safecpy(msg, "Backup of inittab failed: ", sizeof(msg));
        safecat(msg, "Could not copy /etc/inittab to %s", backup_inittabl_filename, sizeof(msg));
        print_msg_to_console(msg);
        return(1);
    }

    print_msg_to_console("Adding daf service entry to /etc/inittab\n");

    safecpy(cmd, "mkitab \"daf:2:once:", sizeof(cmd));
    safecat(cmd, DAF_SERVICE_INVOCATION, sizeof(cmd));
    safecat(cmd, " >/dev/null 2>&1", sizeof(cmd));
    safecat(cmd, "\"", sizeof(cmd));

    if (run_system_cmd_and_print_all_output(cmd) != 0)
    {
        safecpy(msg, "Update of inittab failed: ", sizeof(msg));
        safecat(msg, cmd, sizeof(msg));
        safecat(msg, "\n", sizeof(msg));
        print_msg_to_console(msg);
        return(1);
    }

#elif defined LINUX || defined SOLARIS || defined HPUX

    //--------------------------------------------------------------------------
    // Add daf daemon to /etc/inittab
    // First check to see if /etc/inittab exists - if it does not this may be
    // Debian or Ubuntu in which case we must create an empty file
    // (Debian/Ubuntu use the Startup mechanism instead of /etc/inittab but
    // still support the old /etc/inittab file it is present
    // see for instance:  http://upstart.ubuntu.com/cookbook/
    //--------------------------------------------------------------------------

    if (!does_file_exist("/etc/inittab"))
    {
        sprintf(msg, "/etc/inittab did not exist - assuming Debian/Ubuntu - so creating /etc/init entry\n");
        print_msg_to_console(msg);
        create_etc_init_conf();
        return(0);
    }

    if (stat(pathname, &fileStat) < 0) {

       sprintf(msg, "stat(%s) failed - it seems %s is missing !!!!!!!!!!!!!!!!!!!!! \n", pathname, pathname );
       print_msg_to_console(msg);
       return(1);
    }

    // Later versions of Redhat etc do not use /etc/inittab (but will still honour it if exists - so if the file is empty,
    // we just put a single comment line into so that later code that updates the /etc/inittab file will work
    if (fileStat.st_size == 0) {

       if (run_system_cmd("cat '* just a comment' >>/etc/inittab", 0) != 0)
       {
          safecpy(msg, "cat \"* just a comment\" >>/etc/inittab failed\n", sizeof(msg));
          print_msg_to_console(msg);
    	  return(1);
    	}
    }

    //--------------------------------------------------------------------------
    // remove any previous entry, making sure we backup /etc/inittab
    //--------------------------------------------------------------------------

    safecpy(backup_inittab_filename, "/etc/inittab.", sizeof(backup_inittab_filename));
    get_current_time_as_timestamp(timestamp, sizeof(timestamp), '.');
    safecat(backup_inittab_filename, timestamp, sizeof(backup_inittab_filename));
    sprintf(msg, "Backing up /etc/inittab in %s\n", backup_inittab_filename);
    print_msg_to_console(msg);
    print_msg_to_console("Removing any previous daf service entry in /etc/inittab\n");

    if (copy_file("/etc/inittab", backup_inittab_filename) != 0)
    {
        snprintf(msg, sizeof(msg), "Copy of /etc/inittab to %s failed\n", backup_inittab_filename);
        print_msg_to_console(msg);
        return(1);
    }

    safecpy(cmd, "cat ", sizeof(cmd));
    safecat(cmd, backup_inittab_filename, sizeof(cmd));
    safecat(cmd, " | grep -v \""DAF_SERVICE_INVOCATION"\" > /etc/inittab", sizeof(cmd));

    if (run_system_cmd(cmd, 0) != 0)
    {
        safecpy(msg, "Removal of previous daf service in inittab failed: ", sizeof(msg));
        safecat(msg, cmd, sizeof(msg));
        safecat(msg, "\n", sizeof(msg));
        print_msg_to_console(msg);
        return(1);
    }

    safecpy(cmd, "cat ", sizeof(cmd));
    safecat(cmd, backup_inittab_filename, sizeof(cmd));
    safecat(cmd, " | grep -v \""DAF_SERVICE_INVOCATION"\" > /etc/inittab", sizeof(cmd));

    if (run_system_cmd(cmd, 0) != 0)
    {
        safecpy(msg, "Removal of previous daf service in inittab failed: ", sizeof(msg));
        safecat(msg, cmd, sizeof(msg));
        safecat(msg, "\n", sizeof(msg));
        print_msg_to_console(msg);
        return(1);
    }

    //--------------------------------------------------------------------------
    // Add daf daemon to /etc/inittab
    //--------------------------------------------------------------------------

    print_msg_to_console("Adding daf service entry to /etc/inittab\n");

    safecpy(cmd, "echo \"daf:345:once:", sizeof(msg));
    safecat(cmd, DAF_SERVICE_INVOCATION, sizeof(cmd));
    safecat(cmd, " \" 2>/dev/null >>/etc/inittab", sizeof(cmd));

    if (run_system_cmd(cmd, 0) != 0)
    {
        safecpy(msg, "Adding daf service entry to /etc/inittab: ", sizeof(msg));
        safecat(msg, cmd, sizeof(msg));
        safecat(msg, "\n", sizeof(msg));
        print_msg_to_console(msg);
        return(1);
    }

#endif

    print_msg_to_console("/etc/inittab entry updated successfully\n");

    return(0);

}


/*----------------------------------------------------------------------*/
/*                                                                      */
/*  NAME:           daf_service_delete                                  */
/*                                                                      */
/*  FUNCTION:                                                           */
/*                                                                      */
/* Delete the daf service from the machine                              */
/* 1. Stop any existing daf daemon that is running                      */
/* 2. Remove an inittab entry for daf service                           */
/*                                                                      */
/*  PARAMS IN:                                                          */
/*                                                                      */
/*  PARAMS OUT:                                                         */
/*                                                                      */
/*  RETURNS:                                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/
int daf_service_delete(void)
{

    char cmd[1024];
    char msg[1024];
    char timestamp[16];
    char backup_inittab_filename[64];

#if defined AIX

    print_msg_to_console("Updating /etc/inittab entry\n");

    //--------------------------------------------------------------------------
    // remove any previous entry
    //--------------------------------------------------------------------------

    safecpy(backup_inittab_filename, "/etc/inittab.", sizeof(backup_inittab_filename));
    get_current_time_as_timestamp(timestamp, sizeof(timestamp), '.');
    safecat(backup_inittab_filename, timestamp, sizeof(backup_inittab_filename));
    sprintf(msg, "Backing up /etc/inittab in %s\n", backup_inittab_filename);
    print_msg_to_console(msg);

    if (copy_file("/etc/inittab", backup_inittab_filename) != 0)
    {
        safecpy(msg, "Backup of inittab failed: ", sizeof(msg));
        safecat(msg, cmd, sizeof(msg));
        safecat(msg, "\n", sizeof(msg));
        print_msg_to_console(msg);
        return(1);
    }

    print_msg_to_console("  Removing any previous daf service entry in /etc/inittab\n");
    safecpy(cmd, "cat /etc/inittab | grep \"daf:\"", sizeof(cmd));

    if (run_system_cmd(cmd, 1) == 0)
    {

        safecpy(cmd, "rmitab \"daf\"", sizeof(cmd));

        if (run_system_cmd_and_print_all_output(cmd) != 0)
        {
            safecpy(msg, "Removal of daf service entry in /etc/inittab failed: ", sizeof(msg));
            safecat(msg, cmd, sizeof(msg));
            safecat(msg, "\n", sizeof(msg));
            print_msg_to_console(msg);
            return(1);
        }

    }
    else
    {
        safecpy(msg, "No daf entry found in /etc/inittab so none was removed\n", sizeof(msg));
        print_msg_to_console(msg);
    }

    print_msg_to_console("/etc/inittab entry updated successfully\n");

#elif defined LINUX || defined SOLARIS || defined HPUX

    //--------------------------------------------------------------------------
    // remove any previous entry
    //--------------------------------------------------------------------------

    // This could be Debian/Ubuntu and could be before we have installed DAF so there might not
    // be a /etc/inittab file existing yet - check for this

    if (! does_file_exist("/etc/inittab"))
    {

        if (does_file_exist(DAF_DEBIAN_ETC_INIT_CONF_PATH))
        {
            safecpy(cmd, "rm " DAF_DEBIAN_ETC_INIT_CONF_PATH, sizeof(cmd));

            if (run_system_cmd(cmd, 1) != 0)
            {
                safecpy(msg, "Removal of " DAF_DEBIAN_ETC_INIT_CONF_PATH " failed\n", sizeof(msg));
                print_msg_to_console(msg);
                return(1);
            }
        }

        print_msg_to_console(DAF_DEBIAN_ETC_INIT_CONF_PATH " removed\n");

    }
    else
    {

        print_msg_to_console("Updating /etc/inittab entry\n");

        safecpy(backup_inittab_filename, "/etc/inittab.", sizeof(backup_inittab_filename));
        get_current_time_as_timestamp(timestamp, sizeof(timestamp), '.');
        safecat(backup_inittab_filename, timestamp, sizeof(backup_inittab_filename));
        sprintf(msg, "Backing up /etc/inittab in %s\n", backup_inittab_filename);
        print_msg_to_console(msg);

        if (copy_file ("/etc/inittab", backup_inittab_filename) != 0)
        {
            safecpy(msg, "Backup of inittab failed: ", sizeof(msg));
            safecat(msg, cmd, sizeof(msg));
            safecat(msg, "\n", sizeof(msg));
            print_msg_to_console(msg);
            return(1);
        }

        print_msg_to_console("  Removing any previous daf service entry in /etc/inittab\n");
        safecpy(cmd, "cat ", sizeof(cmd));
        safecat(cmd, backup_inittab_filename, sizeof(cmd));
        safecat(cmd, " | grep -v \""DAF_SERVICE_INVOCATION"\" > /etc/inittab", sizeof(cmd));

        if (run_system_cmd(cmd, 0) != 0)
        {
            safecpy(msg, "Removal of previous daf service in inittab failed: ", sizeof(msg));
            safecat(msg, cmd, sizeof(msg));
            safecat(msg, "\n", sizeof(msg));
            print_msg_to_console(msg);
            return(1);
        }

        print_msg_to_console("/etc/inittab entry updated successfully\n");
    }


#endif

    if (is_daf_service_running())
    {
        print_msg_to_console("daf daemon is running - and will now be stopped\n");

        if (stop_daf_service_running() != 0)
        {
            print_msg_to_console("Problem trying to stop daf daemon\n");
        }
    }

    return(0);

}

