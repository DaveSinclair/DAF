
#ifndef _H_DAF_SERVICE_ENVIRONMENT_H
#define _H_DAF_SERVICE_ENVIRONMENT_H

#define  DAF_SERVICE_BINARY_DIR             "/opt/daf"
#define  DAF_SERVICE_LOG_DIR                "/opt/daf/logs"

#if defined DAF_AGENT

#define  DAF_BINARY                         "daf_agent"
#define  DAF_SERVICE_BINARY_PATH            "/opt/daf/daf_agent"
#define  DAF_SERVICE_LOGFILE_PATH           DAF_SERVICE_LOG_DIR "/daf_agent.log"
#define  DAF_SERVICE_START_SCRIPT_PATH      DAF_SERVICE_BINARY_DIR "/start_daf_agent_service"
#define  DAF_SERVICE_START_SCRIPT_CONTENTS  {". " DAF_SERVICE_BINARY_DIR "/daf_agent_command_profile", \
                                                DAF_SERVICE_BINARY_DIR "/daf_agent -service run -consolelog " DAF_SERVICE_LOG_DIR "/daf_agent.log >/dev/null 2>&1", \
                                                (char *) 0}
#define  DAF_SERVICE_PROFILE_PATH           DAF_SERVICE_BINARY_DIR "/daf_agent_command_profile"
#define  DAF_SERVICE_PROFILE_CONTENTS       {"# " DAF_SERVICE_BINARY_DIR "/daf_agent_command_profile", (char *) 0}
#define  DAF_SERVICE_INVOCATION             DAF_SERVICE_BINARY_DIR "/start_daf_agent_service"
#define  DAF_SERVICE_INVOCATION_SHORT       "daf_agent -service run"

#define  DAF_DEBIAN_ETC_INIT_CONF_PATH      "/etc/init/daf_agent.conf"
#define  DAF_DEBIAN_ETC_INIT_CONF           { "description     \"DAF server\"", \
                                                 "start on (net-device-up and local-filesystems and runlevel [2345])", \
                                                 "stop on runlevel [016]", \
                                                 "env HOME=" DAF_SERVICE_BINARY_DIR "/opt/daf", \
                                                 "umask 007", \
                                                 "pre-start script", \
                                                 "end script", \
                                                 "exec " DAF_SERVICE_BINARY_DIR "/start_daf_agent_service", \
                                                 "post-start script", \
                                                 "end script", \
                                                 (char *) 0 }

#else

#define  DAF_BINARY                         "daf"
#define  DAF_SERVICE_BINARY_PATH            "/opt/daf/daf"
#define  DAF_SERVICE_LOGFILE_PATH           DAF_SERVICE_LOG_DIR "/daf.log"
#define  DAF_SERVICE_START_SCRIPT_PATH      DAF_SERVICE_BINARY_DIR "/start_daf_service"
#define  DAF_SERVICE_START_SCRIPT_CONTENTS  {". " DAF_SERVICE_BINARY_DIR "/daf_command_profile", \
                                               DAF_SERVICE_BINARY_DIR "/daf -service run -consolelog " DAF_SERVICE_LOG_DIR "/dafservice.log >/dev/null 2>&1", \
                                                (char *) 0}
#define  DAF_SERVICE_PROFILE_PATH           DAF_SERVICE_BINARY_DIR "/daf_command_profile"
#define  DAF_SERVICE_PROFILE_CONTENTS       {"# " DAF_SERVICE_BINARY_DIR "/daf_command_profile", (char *) 0}
#define  DAF_SERVICE_INVOCATION             DAF_SERVICE_BINARY_DIR "/start_daf_service"
#define  DAF_SERVICE_INVOCATION_SHORT       "daf -service run"

#define  DAF_DEBIAN_ETC_INIT_CONF_PATH      "/etc/init/daf.conf"
#define  DAF_DEBIAN_ETC_INIT_CONF           { "description     \"DAF server\"", \
                                                 "start on (net-device-up and local-filesystems and runlevel [2345])", \
                                                 "stop on runlevel [016]", \
                                                 "env HOME=" DAF_SERVICE_BINARY_DIR "/opt/daf", \
                                                 "umask 007", \
                                                 "pre-start script", \
                                                 "end script", \
                                                 "exec " DAF_SERVICE_BINARY_DIR "/start_daf_service", \
                                                 "post-start script", \
                                                 "end script", \
                                                 (char *) 0 }

#endif

unsigned long long is_daf_service_running(void);
int stop_daf_service_running(void);
int start_daf_service_running(void);
int create_daf_service_start_script(void);
int daf_service_install(char *daf_binary_path, char *argv0);
int daf_service_delete(void);
int get_current_executable_path(char *executable, char *path, int max_path_length);

#endif

