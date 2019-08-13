#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
   int ret;

   printf ("Antes do fork: PID = %d, PPID = %d\n", getpid (), getppid ()); // Parent
   if ((ret = fork ()) < 0) { // Parent e Child
        perror ("erro na duplicação do processo");
        return EXIT_FAILURE;   // Parent
   }
   if (ret > 0) sleep (1); // Parent e Child -- Parent (sleep)
   printf ("Quem sou eu?\nApós o fork: PID = %d, PPID = %d\n", getpid (), getppid ()); // Parent e Child

   return EXIT_SUCCESS; // Parent e Child
}

// i) 1x1 linha + 2x2 linhas
/*
	 | PID=100
	 | printf("antes...)
	 |
       /    \
      /      \
     /        \
returns==105    fork() 
PID 100         return==0
spleep printf   PID=105
(parent)        printf
                (child)
*/
