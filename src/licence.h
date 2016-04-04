/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                        */
/*                                                                                                                      */
/* licence.h                                                                                                            */
/*                                                                                                                      */
/* This file is part of the daf test automation programme.                                                              */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#include "daf_protocol.h"

typedef struct licence
{
    Iu16  Type;
    Iu32  Serial;
    Iu32  Origin;
    Iu32  Duration;
    Iu16  Actions;
    Iu16  Spare;
    Iu16  Checksum;
} licence_t;

int display_licence(char *licence_pathname);
int read_licence(char *licence_pathname, licence_t *licence, bool_t *valid, bool_t *current, char *errmsg, int max_msg_len);
int write_licence(char *licence_pathname, char *licence_string);
int validate_licence(char *licence_pathname);
int get_licence_max_actions(char *licence_pathname, char *errmsg, int max_msg_len);
