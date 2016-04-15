#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "daf_util.h"

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  send_email_message                                                                         */
/*                                                                                                        */
/* PARAMS IN:  destinations      array of pointers to strings that contain the internet address of the    */
/*                               intended recipients terminated by a null pointer                         */
/*             subject           subject msg that goes in the email                                       */
/*             message_lines     array of pointers to strings that make up the message text.  The strings */
/*                               should not contain \n unless each element of the array is to appear on   */
/*                               separate line in the email                                               */
/*             max_errmsg_len    the max alllowed size of errmsg, including the terminating \0            */
/*                               must be > 0                                                              */
/*                                                                                                        */
/* PARAMS OUT: errmsg            an error message if routine fails,                                       */
/*                                                                                                        */
/* RETURNS:    0 for success, 1 for failure                                                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/* Sends email to a list of recipients - the email consists of a subject line and file attachment         */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int send_email_message(char *destinations[], char *subject, char *message_lines[], char *errmsg, int max_errmsg_len)
{

#undef  SUBNAME
#define SUBNAME "send_email_message"

    char cmd[512];
    int rc = 0;
    int status;
    FILE *fp;
    int email_exit_status;
    int email_exit_signal;
    char mailx[20] = "/bin/mailx";

    errmsg[0] = 0;

    // the mailx program can be in /usr/bin or just in /bin so find out which

    if (! does_file_exist("/bin/mailx")) {
    	if (does_file_exist("/usr/bin/mailx")) {
    	   safecpy(mailx, "/usr/bin/mailx", sizeof(mailx));
    	} else {
           rc = 1;
           snprintf(errmsg, max_errmsg_len, "%s: cannot find mailx at /bin/mailx or /usr/bin/mailx - not sending mail", SUBNAME);
           return rc;
    	}
    }

    while (*destinations != NULL)
    {
        snprintf(cmd, sizeof(cmd), "%s -s '%s' '%s'", mailx, subject, *destinations);
        fp = popen(cmd, "w");

        if (fp == NULL)
        {
            /* Handle error */;
            rc = 1;
            snprintf(errmsg, max_errmsg_len, "%s: popen failed on cmd %s, errno=%d (%s)", SUBNAME, cmd, errno, strerror(errno));
            break;
        }
        else
        {
            while (*message_lines != NULL)
            {
                if (fputs(*message_lines, fp) == EOF)
                {
                    snprintf(errmsg, max_errmsg_len, "%s: fputs failed writing to popen stream, errno=%d (%s)", SUBNAME, errno, strerror(errno));
                    rc = 1;
                    break;
                }

                message_lines++;
            }

            status = pclose(fp);

            if (status == -1)
            {
                snprintf(errmsg, max_errmsg_len, "%s: plcose - no termination status available for %s", SUBNAME, cmd);
            }
            else if (status > 0)
            {
                if (WIFEXITED(status))
                {
                    email_exit_status = WEXITSTATUS(status);
                    snprintf(errmsg, max_errmsg_len, "%s: email exit status: %d", SUBNAME, email_exit_status);
                    rc = 1;
                }
                else if (WIFSIGNALED(status))
                {
                    email_exit_signal = WTERMSIG(status);
                    snprintf(errmsg, max_errmsg_len, "%s: email exit signal: %d", SUBNAME, email_exit_signal);
                    rc = 1;
                }
            }
        }

        destinations++;

    }

    return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  send_file_as_email_attachment                                                              */
/*                                                                                                        */
/* PARAMS IN:  destinations      array of pointers to strings that contain the internet address of the    */
/*                               intended recipients terminated by a null pointer                         */
/*             pathname          path of the file that is to be sent                                      */
/*             subject           subject msg that goes in the email                                       */
/*             max_errmsg_len    the max alllowed size of errmsg, including the terminating \0            */
/*                               must be > 0                                                              */
/*                                                                                                        */
/* PARAMS OUT: errmsg            an error message if routine fails,                                       */
/*                                                                                                        */
/* RETURNS:    0 for success, 1 for failure                                                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/* Sends email to a list of recipients - the email consists of a subject line and file attachment         */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int send_file_as_email_attachment(char *destinations[], char *pathname, char *subject, char *errmsg, int max_errmsg_len)
{

#undef  SUBNAME
#define SUBNAME "send_file_as_email_attachment"

    char cmd[512];
    int rc = 0;
    int status;
    FILE *fp;
    int email_exit_status;
    int email_exit_signal;

    errmsg[0] = 0;

    while (*destinations != NULL)
    {

        snprintf(cmd, sizeof(cmd), "/usr/bin/mailx -s '%s' -a '%s' '%s'", subject, pathname, *destinations);
        fp = popen(cmd, "w");

        if (fp == NULL)
        {
            /* Handle error */;
            rc = 1;
            snprintf(errmsg, max_errmsg_len, "%s: popen failed on cmd %s, errno=%d (%s)", SUBNAME, cmd, errno, strerror(errno));
            break;
        }
        else
        {

            status = pclose(fp);

            if (status == -1)
            {
                snprintf(errmsg, max_errmsg_len, "%s: plcose - no termination status available for %s", SUBNAME, cmd);
            }
            else if (status > 0)
            {
                if (WIFEXITED(status))
                {
                    email_exit_status = WEXITSTATUS(status);
                    snprintf(errmsg, max_errmsg_len, "%s: email exit status: %d", SUBNAME, email_exit_status);
                    rc = 1;
                }
                else if (WIFSIGNALED(status))
                {
                    email_exit_signal = WTERMSIG(status);
                    snprintf(errmsg, max_errmsg_len, "%s: email exit signal: %d", SUBNAME, email_exit_signal);
                    rc = 1;
                }
            }
        }

        destinations++;

    }

    return rc;

}

/*
 *
 * How to test this code

int main(int argc, char **argv) {
  char  *addr[] = {"davesinclair1066@gmail.com", "davesinclair1066@googlemail.com", NULL};
  char  *msglines[] = {"line number 1\n", "line number 2\n", "line number 3\n", NULL};
  int rc;
  char errmsg[256];
  rc = send_file_as_email_attachment(addr, "/root/daf/fred", "hello dave", errmsg, sizeof(errmsg));
  printf("rc = %d %s\n", rc, errmsg);
  rc = send_email_message(addr, "hello dave - here is a msg", msglines, errmsg, sizeof(errmsg));
  printf("rc = %d %s\n", rc, errmsg);
}  */

