#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
   printf ("Filho: PID = %d, PPID = %d\n", getpid (), getppid ());
   sleep (3);
   printf ("Filho: PID = %d, PPID = %d\n", getpid (), getppid ());

   return EXIT_SUCCESS;
}
