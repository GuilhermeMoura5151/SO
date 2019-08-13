#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    unsigned int i;

    printf("PID = %u\n", getpid());
    i = 0;
    while (i <= 10) { 
        printf ("\r%08u ", i++);
        fflush (stdout);
        sleep(1);
    }
    printf ("\n");

    return EXIT_SUCCESS;
}
