
#ifndef _EMAIL_H
#define _EMAIL_H

int send_file_as_email_attachment(char *destinations[], char *pathname, char *subject, char *errmsg, int max_errmsg_len);
int send_email_message(char *destinations[], char *subject, char **message_lines, char *errmsg, int max_errmsg_len);

#endif

