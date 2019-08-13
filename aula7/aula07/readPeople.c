#include <stdio.h>
#include <stdlib.h>
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

int main (int argc, char *argv[])
{
    FILE *fp = NULL;
    Person p[100];
    char r;
    int t;

    /* Validate number of arguments */
    if(argc != 2)
    {
        printf("USAGE: %s fileName\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open the file provided as argument */
    errno = 0;
    fp = fopen(argv[1], "rb+");
    if(fp == NULL)
    {
        perror ("Error opening file!");
        return EXIT_FAILURE;
    }


    int npessoas=fread(p,sizeof(Person),100,fp);
    /* read all the itens of the file */
    for(int i=0; i<npessoas; i++)
    {
        printPersonInfo(&p[i]);
    }

    printf("Quer adicionar mais pessoas?(y/n)");
    scanf("%c",&r);

    if(r=='y')
    {
	    printf("Numero de pessoas a guardar: ");
        scanf("%d",&t);

        for(int i = 0 ; i < t ; i++)
    		{   
 		printf("Nome da pessoa #%d: ", i+1);
		scanf(" %[^\n]", p[i+npessoas].name);
		printf("Idade da pessoa #%d: ", i+1);
		scanf("%d", &p[i+npessoas].age);
		printf("Tamanho da pessoa #%d: ", i+1);
		scanf("%lf", &p[i+npessoas].height);
                
		fp;
        	fwrite(&p, sizeof(Person), npessoas+t, fp);
		fclose(fp);
    		}
    }
    else fclose(fp);

    fclose(fp);

    return EXIT_SUCCESS;
}
