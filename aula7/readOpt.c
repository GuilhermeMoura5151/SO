#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

typedef struct
{
    int age;
    double height;
    char name[64];
} Person;

void printPersonInfo(Person *p)
{
    printf("Person: %s, %d, %f\n", p->name, p->age, p->height);
}

int main(int argc, char *argv[])
{
    int flags, opt;
    int nsecs, tfnd;
    FILE *fp = NULL;
    Person p[100];
    char r;
    int i,t, npessoas;

    nsecs = 0;
    tfnd = 0;
    flags = 0;

   
    

    /* The getopt() function parses the command-line arguments. Its arguments
      argc and argv are the argument count and array as passed to the main()
      function on program invocation. An element of argv that starts with
      '-' (and is not exactly "-" or "--") is an option element. The charac‐
      ters of this element (aside from the initial '-') are option charac‐
      ters.  If getopt() is called repeatedly, it returns successively each
      of the option characters from each of the option elements.

      For more information check the man page (man 3 getopt) 
     */
    while ((opt = getopt(argc, argv, "n:f:t:")) != -1)
    {
        switch (opt)
        {
	    case 'f':
                errno = 0;
    		fp = fopen(argv[argc-1], "rb+");
    		if(fp == NULL)
    		{
        		perror ("Error opening file!");
        		return EXIT_FAILURE;
    		}

		/* read all the itens of the file */
    		npessoas=fread(p,sizeof(Person),100,fp);
    		
    		
		flags=1;
		break;
	    case 'n':
		errno = 0;
    		fp = fopen(argv[argc-1], "rb+");
    		if(fp == NULL)
    		{
        		perror ("Error opening file!");
        		return EXIT_FAILURE;
    		}

		/* read all the itens of the file */
    		npessoas=fread(p,sizeof(Person),100,fp);
		flags=2;

                for(i=0; i<npessoas; i++)
    		{
        	printPersonInfo(&p[i]);
    		}
		break;
            case 't':
                nsecs = atoi(optarg);
                tfnd = 1;

                break;
                /* If getopt() finds an option character in argv that was not included in
                  optstring, or if it detects a missing option argument, it returns '?'
                 */
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

	for(i = 0 ; i < nsecs ; i++)
    	{   
 		printf("Nome da pessoa #%d: ", i+1);
		scanf(" %[^\n]", p[i+npessoas].name);
		printf("Idade da pessoa #%d: ", i+1);
		scanf("%d", &p[i+npessoas].age);
		printf("Tamanho da pessoa #%d: ", i+1);
		scanf("%lf", &p[i+npessoas].height);
                
		fp;
        	fwrite(&p, sizeof(Person), npessoas+nsecs, fp);
		fclose(fp);
    	}

    printf("flags=%d; tfnd=%d; nsecs=%d; optind=%d\n", flags, tfnd, nsecs, optind);

    /* By default, getopt() permutes the contents of argv as it scans, so that
      eventually all the nonoptions are at the end.
     */
    if (optind >= argc)
    {
        fprintf(stderr, "Expected argument after options\n");
        return EXIT_FAILURE;
    }

    printf("name argument = %s\n", argv[optind]);

    /* Other code omitted */
    
    fclose(fp);

    return EXIT_SUCCESS;
}
