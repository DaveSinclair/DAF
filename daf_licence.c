/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                        */
/*                                                                                                                      */
/* daf_licence.c                                                                                                        */
/*                                                                                                                      */
/* This file is part of the daf test automation programme.                                                              */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "daf_protocol.h"
#include "daf_util.h"
#include "licence.h"

#define E_OK  0
#define E_NOTOK 1

typedef struct params { 

	bool_t                 validate_licence_flag;
	bool_t                 write_licence_flag;
	char                   licence_pathname[MAX_PATHNAME_LEN+1];
	char                   licence[DAF_LICENCE_LEN+1];

} parameters_t;

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* Globals                                                                                                */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

parameters_t       parameters;

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* Private routines                                                                                       */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  print_usage                                                                                */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void print_usage() {

	printf("daf_licence - An daf licence program\n\n");
	printf("   daf_licence\n");
	printf("        -help\n");
	printf("   or\n");
	printf("   daf_licence\n");
	printf("         -validate_licence licencepathname\n");
	printf("         -write_licence    licencepathname -licence NNNNN\n");

	printf("\n");
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  initialise_args                                                                            */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

void initialise_args(parameters_t *parameters) {

        parameters->validate_licence_flag = FALSE;
        parameters->write_licence_flag = FALSE;
        strncpy(parameters->licence_pathname, "", sizeof(parameters->licence_pathname));
        strncpy(parameters->licence, "", sizeof(parameters->licence));

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  parse_args                                                                                 */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int parse_args(int argc, char **argv, parameters_t *parameters ) {

	int i;

	if (argc == 1) {
		print_usage();
		exit(0);
	}

	initialise_args(parameters);

	for (i = 1; i < argc; i++) {

		if ((strcmp(argv[i], "-?") == 0) || (strcmp(argv[i], "-h") ==0) || (strcmp(argv[i], "-help") ==0)) {
			print_usage();
			exit(0);
		} else if (strcmp(argv[i], "-write_licence") == 0) {
                        parameters->write_licence_flag = TRUE;
			i++;
			if (i < argc) {
				safecpy(parameters->licence_pathname, argv[i], sizeof(parameters->licence_pathname));
			} else {
				printf("daf_licence: missing value for -write_licence parameter\n");
				return E_NOTOK;
			}
		} else if (strcmp(argv[i], "-validate_licence") == 0) {
                        parameters->validate_licence_flag = TRUE;
			i++;
			if (i < argc) {
				safecpy(parameters->licence_pathname, argv[i], sizeof(parameters->licence_pathname));
			} else {
				printf("daf_licence: missing value for -validiate_licence parameter\n");
				return E_NOTOK;
			}
		} else if (strcmp(argv[i], "-licence") == 0) {
			i++;
			if (i < argc) {
				safecpy(parameters->licence, argv[i], sizeof(parameters->licence));
			} else {
				printf("daf_licence: missing value for -licence parameter\n");
				return E_NOTOK;
			}
		} else {
			printf("daf_licence: unrecognised argument: %s\n", argv[i]);
			return E_NOTOK;
		}

	}

	return E_OK;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  main                                                                                       */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    0 for success, 1 for failure                                                               */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int main(int argc, char **argv) {

   int exit_rc = 0;

   if (parse_args(argc, argv, &parameters) != E_OK) {
      exit(1);
   }

   if (parameters.validate_licence_flag) {

      exit_rc = validate_licence(parameters.licence_pathname);

   } else if (parameters.write_licence_flag) {

      exit_rc = write_licence(parameters.licence_pathname, parameters.licence);

   }  else {

      printf("no command specified\n");
      exit_rc = E_NOTOK;

     }

     exit(exit_rc);
}
