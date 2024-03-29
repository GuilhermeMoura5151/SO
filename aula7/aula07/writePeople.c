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
    int i, t;
    Person p = {35, 1.65, "xpto"};

    /* Validate number of arguments */
    if(argc != 2)
    {
        printf("USAGE: %s fileName\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open the file provided as argument */
    errno = 0;
    fp = fopen(argv[1], "wb");
    if(fp == NULL)
    {
        perror ("Error opening file!");
        return EXIT_FAILURE;
    }

     printf("Numero de pessoas a guardar: ");
     scanf("%d",&t);

    /* Write 10 itens on a file 
    for(i = 0 ; i < 10 ; i++)
    {    
        p.age = p.age+1;
        p.height = p.height+0.03;
        fwrite(&p, sizeof(Person), 1, fp);
    }
    */

    for(i = 0 ; i < t ; i++)
    {   
 	printf("Nome da pessoa #%d: ", i+1);
	scanf(" %[^\n]", p.name);
	printf("Idade da pessoa #%d: ", i+1);
	scanf("%d", &p.age);
	printf("Tamanho da pessoa #%d: ", i+1);
	scanf("%lf", &p.height);
     
        fwrite(&p, sizeof(Person), 1, fp);
    }

    fclose(fp);

    return EXIT_SUCCESS;
}
