#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* SUGESTÂO: utilize as páginas do manual para conhecer mais sobre as funções usadas:
 man fopen
 man fgets
*/

#define LINEMAXSIZE 80 /* or other suitable maximum line size */


int main(int argc, char *argv[])
{
    int i;
    FILE *fp = NULL;
    char line [LINEMAXSIZE]; 

    /* Validate number of arguments */
    if(argc < 2)
    {
        printf("USAGE: %s fileName\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    /* Open the file provided as argument */
    errno = 0;
for(int i=1;i<argc;i++)
    {

    fp = fopen(argv[i], "r");
    if(fp == NULL)
        {
        perror ("Error opening file!");
        return EXIT_FAILURE;
        }

    }
    /* read all the lines of the file */
    int num=0;
    int newline=1;
    while(fgets(line, sizeof(line), fp) != NULL )
    {

        if(newline) {
		     num++;
             printf("%3d ->",num);
             newline=0;
        }

         printf("-> %s", line);

         if(line[strlen(line)-1] == '\n') newline = 1;
    }

    fclose (fp);

    return EXIT_SUCCESS;
}
