/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* daf exerciser                                                                                                        */
/*                                                                                                                      */
/* licence.c                                                                                                            */
/*                                                                                                                      */
/* This file is part of the daf test automation programme.                                                             */
/*                                                                                                                      */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "licence.h"
#include "daf_util.h"

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  display_licence                                                                            */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int display_licence(char *licence_pathname) {

   int rc = E_OK;   
   char line[256];
   FILE  *fptr = NULL;
   char msg[MAX_MSG_LEN];
   
   if ((fptr = fopen(licence_pathname, "rb")) == NULL ) {

      snprintf(msg, sizeof(msg), "%s: Could not open %s\n", "display_licence", licence_pathname);
      rc = E_NOTOK;

   } else {

      while(fgets(line, sizeof(line)-1, fptr) != NULL) {
         print_msg_to_console(line);
      }
   }

   if (fptr != NULL) {
      fclose(fptr);  
   }
   return (rc);

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  read_licence                                                                               */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int read_licence(char *licence_pathname, licence_t* licence, bool_t *valid, bool_t *current, char *errmsg, int max_msg_len) {

   Iu8 licence_bytes[20];  /* hardcoded <<<< */
   FILE  *fptr = NULL;
   int   num_fields_read;   
   Iu16  checksum;
   int   i;
   int   temp;

   int rc = E_OK;
   strncpy(errmsg, "", max_msg_len);
   *current = TRUE;
   *valid = TRUE;
   memset(licence, 0, sizeof(licence));

   if ((fptr = fopen(licence_pathname, "rb")) == NULL ) {

      snprintf(errmsg, max_msg_len, "%s: Could not open %s\n", "validate_licence", licence_pathname);
      *valid = FALSE;
      *current = FALSE; 
      rc = E_NOTOK;

   } else {

      i = 0;
      num_fields_read = 0;
      while (i < 20) {
         num_fields_read = num_fields_read + fscanf(fptr, "%x", &temp);
         *(Iu16 *) &(licence_bytes[i]) = temp;
         temp = licence_bytes[i];     /* Swap endianness */
         licence_bytes[i] = licence_bytes[i+1];
         licence_bytes[i+1] = temp;
         i = i+2;
      }

      if (num_fields_read == 10) {

         licence->Type     = *(Iu16*) (&(licence_bytes[0]));
         licence->Serial   = *(Iu32*) (&(licence_bytes[2]));
         licence->Origin   = *(Iu32*) (&(licence_bytes[6]));
         licence->Duration = *(Iu32*) (&(licence_bytes[10]));
         licence->Actions  = *(Iu16*) (&(licence_bytes[14]));
         licence->Spare    = *(Iu16*) (&(licence_bytes[16]));
         licence->Checksum = *(Iu16*) (&(licence_bytes[18]));

         if (licence->Type != 10) {
            *valid = FALSE;
            *current = FALSE; 
         }

         if (licence->Serial < 567) {
            *valid = FALSE;
            *current = FALSE; 
         }

         checksum = 0;
         for (i=0; i<sizeof(licence_bytes); i=i+2) {
            checksum = checksum ^ *((Iu16 *) &(licence_bytes[i]));
         }

         if (checksum != 0) {
            *valid = FALSE; 
            *current = FALSE; 
         }  

         if (valid) {
            if (licence->Duration == 0) {
               *current = TRUE;
            } else {
               if ((licence->Origin + licence->Duration * 24 * 60 * 60) > time(0)) {
                  *current = TRUE;
               }
            }  
         }

      } else {
         *valid = FALSE; 
         *current = FALSE; 
      }

   }

   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  validate_licence                                                                           */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/* Prints a message to stdout indicating the validity of the licence in the specified licence file        */
/* The message is of the format:                                                                          */
/*                                                                                                        */
/* Licence:v:s:d:t:c                                                                                      */
/*                                                                                                        */
/* v is VALID or NOTVALID                                                                                 */
/* s is a serial number, must be 567 or greater                                                           */
/* t is the duration in days of the licence, if set to 0 the licence is valid forever                     */
/* d is the origin date of the licence, as a unix timestamp                                               */
/* t is the typenumer of the licence - at present only type 10 is defined                                 */
/* c is the max number of concurrent steps that may be scheduled, if set to 0 then any number is allowed  */
/* eg                                                                                                     */
/*                                                                                                        */
/* Licence:VALID:00001001:60:1324042820:0:100                                                             */
/*                                                                                                        */
/* provides a licence with serial number 1001 valid for 60 days, for 100 steps, with an origin date of    */
/* 16 Dec 2011, (1324042820 is 2011-12-16 13:40:20Z).  An invalid licence file will give an output of     */
/*                                                                                                        */
/* Licence:INVALID:00001001:0:0:0:0                                                                       */
/*                                                                                                        */
/* Since the Unix timestamp is used here this licensing scheme will fail on 19 Jan 2038                   */
/*                                                                                                        */
/* The format of the licence file is                                                                      */
/*                                                                                                        */
/*   Iu16  Type;                                                                                          */
/*   Iu32  Serial;                                                                                        */
/*   Iu32  Origin;                                                                                        */
/*   Iu32  Duration;                                                                                      */
/*   Iu16  Actions;                                                                                       */
/*   Iu16  Spare;                                                                                         */
/*   Iu16  Checksum;         16 bit XOR over the other fields so that the total XOR is 0                  */
/*--------------------------------------------------------------------------------------------------------*/

int validate_licence(char *licence_pathname) {

   licence_t licence; 
   char  msg[MAX_MSG_LEN];
   int   valid = TRUE;
   int   current = TRUE;

   int rc = E_OK;

   sprintf(msg, "Licence file %s contains:\n", licence_pathname);
   print_msg_to_console(msg);
   rc = display_licence(licence_pathname);

   if (rc == 0) {

      rc = read_licence(licence_pathname, &licence, &valid, &current, msg, sizeof(msg));

      if (rc != E_OK) {

         print_msg_to_console(msg);
         valid = FALSE;

      } else {

         sprintf(msg, "type      %d\n", licence.Type); print_msg_to_console(msg);
         sprintf(msg, "serial    %d\n", licence.Serial); print_msg_to_console(msg);
         sprintf(msg, "origin    %d\n", licence.Origin); print_msg_to_console(msg);
         sprintf(msg, "spare     %d\n", licence.Duration); print_msg_to_console(msg);
         sprintf(msg, "steps     %d\n", licence.Actions); print_msg_to_console(msg);
         sprintf(msg, "spare     %d\n", licence.Spare); print_msg_to_console(msg);
         sprintf(msg, "checksum  %x\n", licence.Checksum); print_msg_to_console(msg);

      }
   } else {
     valid = FALSE;
   }

   if (! valid) {

      print_msg_to_console("Licence:INVALID:00000000:0:0:0:0\n");
      rc = E_NOTOK;

   } else {

      /* Licence:VALID:00001001:60:1324042820:0:100                                                             */
      sprintf(msg, "Licence:VALID:%08d:%d:%d:%d:%d\n", licence.Serial, licence.Duration, licence.Origin, licence.Type, licence.Actions);
      print_msg_to_console(msg);
      rc = E_OK;
   }

   return rc;

}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  get_licence_max_actions                                                                    */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    0 if the licence allows an indefinite number of actions, or if the licence is not valid    */
/*             otherwise returns the max number of concurrent actions allowed in the licence              */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int get_licence_max_actions(char *licence_pathname, char *errmsg, int max_msg_len) {

   licence_t licence;
   int       valid = TRUE;
   int       current = TRUE;

   if (read_licence(licence_pathname, &licence, &valid, &current, errmsg, max_msg_len) != E_OK) {
      return 0;
   } else {
      return licence.Actions;
   }
}

/*--------------------------------------------------------------------------------------------------------*/
/*                                                                                                        */
/* PROCEDURE:  write_licence                                                                              */
/*                                                                                                        */
/* PARAMS IN:                                                                                             */
/*                                                                                                        */
/* PARAMS OUT:                                                                                            */
/*                                                                                                        */
/* RETURNS:    E_OK for success, E_NOTOK if call did not succeed                                          */
/*                                                                                                        */
/* FUNCTION:                                                                                              */
/*                                                                                                        */
/*--------------------------------------------------------------------------------------------------------*/

int write_licence(char *licence_pathname, char *licence_string) {

   int   rc = E_OK;
   FILE  *fptr = NULL;
   char  msg[MAX_MSG_LEN];

   if (strlen(licence_pathname) == 0) {
      
      snprintf(msg, sizeof(msg), "%s: licence pathname is of zero length\n", "write_licence");
      print_msg_to_console(msg);
      rc = E_NOTOK;

   } else if (strlen(licence_string) == 0) {
      
      snprintf(msg, sizeof(msg), "%s: licence string is of zero length\n", "write_licence");
      print_msg_to_console(msg);
      rc = E_NOTOK;

   } else {

      if ((fptr = fopen(licence_pathname, "wb")) == NULL ) {

         snprintf(msg, sizeof(msg), "%s: Could not open %s\n", "write_licence", licence_pathname);
         print_msg_to_console(msg);
         rc = E_NOTOK;

      } else {

         /* num_bytes_written = fwrite(licence, sizeof(Iu8), strlen(licence), fptr);

         if (num_bytes_written != strlen(licence)) {
            snprintf(msg, sizeof(msg), "%s: Could not write requested number of bytes (%d) to %s - only %d written\n",
                     "write_licence", (int) strlen(licence), licence_pathname, num_bytes_written);
            print_msg_to_console(msg);
            rc = E_NOTOK;
         } */

         fprintf(fptr, "%s\n", licence_string);

      }

      if (fptr != NULL) {
         fclose(fptr);
      }
   }

   return rc;

}

