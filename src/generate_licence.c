
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

typedef unsigned char   Iu8;
typedef unsigned short  Iu16;
typedef unsigned int    Iu32;

#define MAX_MSG_LEN 1024

void print_usage()
{

    printf("generate_licence -type NNNN -serial NNNN -origin NNNN -duration NNNN -steps NNNN -spare NNNN -checksum NNNN\n\n");
    printf("  defaults:\n");
    printf("       type         10\n");
    printf("       serial       567\n");
    printf("       origin       the current date/time\n");
    printf("       duration     0\n");
    printf("       steps        100\n");
    printf("       spare        0\n");
    printf("       checksum     a valid, correct checksum\n");
    printf("\n");

}

int main(int argc, char **argv)
{

    char                 msg[MAX_MSG_LEN];
    char                 statusmsg[MAX_MSG_LEN];
    char                 errmsg[MAX_MSG_LEN];
    int                  i;

    Iu16  type = 10;
    int  type_flag = 0;
    int type_offset = 0;
    Iu32  serial = 567;
    int  serial_flag = 0;
    int serial_offset = 2;
    Iu32  origin;
    int  origin_flag = 0;
    int origin_offset = 6;
    Iu32  duration = 0;
    int  duration_flag = 0;
    int duration_offset = 10;
    Iu16  steps = 100;
    int  steps_flag = 0;
    int steps_offset = 14;
    Iu16  spare = 0;
    int  spare_flag = 0;
    int spare_offset = 16;
    Iu16  checksum;
    int  checksum_flag = 0;
    int checksum_offset = 18;

    Iu8   licence[20];

    for (i = 1; i < argc; i++)
    {

        if ((strcmp(argv[i], "-?") == 0) || (strcmp(argv[i], "-h") ==0) || (strcmp(argv[i], "-help") ==0))
        {
            print_usage();
            exit(0);
        }
        else if ((strcmp(argv[i], "-type") == 0))
        {
            i++;

            if (i < argc)
            {
                type = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("missing value for -type parameter\n");
                exit(1);
            }
        }
        else if ((strcmp(argv[i], "-serial") == 0))
        {
            i++;

            if (i < argc)
            {
                serial = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("missing value for -serial parameter\n");
                exit(1);
            }
        }
        else if ((strcmp(argv[i], "-origin") == 0))
        {
            i++;

            if (i < argc)
            {
                origin = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("missing value for -origin parameter\n");
                exit(1);
            }
        }
        else if ((strcmp(argv[i], "-duration_flag") == 0))
        {
            i++;

            if (i < argc)
            {
                duration = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("missing value for -serial parameter\n");
                exit(1);
            }
        }
        else if ((strcmp(argv[i], "-steps_flag") == 0))
        {
            i++;

            if (i < argc)
            {
                steps = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("missing value for -steps parameter\n");
                exit(1);
            }
        }
        else if ((strcmp(argv[i], "-spare_flag") == 0))
        {
            i++;

            if (i < argc)
            {
                spare = strtoul(argv[i], NULL, 0);
            }
            else
            {
                printf("missing value for -spare parameter\n");
                exit(1);
            }
        }
        else
        {
            printf("unexpected parameter: %s\n", argv[i]);
            exit(1);
        }
    }

    if (! origin_flag)
    {
        origin = time(0);
    }

    if (! checksum_flag)
    {
        origin = time(0);
    }

    * (Iu16 *) &(licence[type_offset])     = type;
    * (Iu32 *) &(licence[serial_offset])   = serial;
    * (Iu32 *) &(licence[origin_offset])   = origin;
    * (Iu32 *) &(licence[duration_offset]) = duration;
    * (Iu16 *) &(licence[steps_offset])    = steps;
    * (Iu16 *) &(licence[spare_offset])    = spare;

    if (! checksum_flag)
    {
        checksum = 0;

        for (i=0; i<(sizeof(licence)-2); i=i+2)
        {
            checksum = checksum ^ *(Iu16 *) &(licence[i]);
        }
    }

    * (Iu16 *) &(licence[checksum_offset]) = checksum;

    printf(" Type         %d\n", type);
    printf(" Serial       %d\n", serial);
    printf(" Origin       0x%08x\n", origin);
    printf(" Duration     %d\n", duration);
    printf(" Steps        %d\n", steps);
    printf(" Spare        %d\n", spare);
    printf(" Checksum     0x%04x\n\n", checksum);

    printf("Licence is: ");

    for (i=0; i<sizeof(licence); i++)
    {
        if (((i % 2) == 0) && (i != 0))
        {
            printf(" ");
        }

        printf("%02x", licence[i]);
    }

    printf("\n");
    exit(0);

}





