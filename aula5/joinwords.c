#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if(argc<2)
    {
        printf("ERROR: Argumentos insuficientes.\n");
        return EXIT_FAILURE;
    }

    int i, v = 0, size = argc - 1;  // size = numero de argumentos

    char *str = (char *)malloc(v);  // malloc = alocar espaco para ponteiro

    for(i = 1; i <= size; i++) {
        str = (char *)realloc(str, (v + strlen(argv[i])));
        strcat(str, argv[i]);
    }

    printf("%s\n", str);

    return EXIT_SUCCESS;
}