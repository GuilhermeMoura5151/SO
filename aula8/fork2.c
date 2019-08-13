#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
   int ret;

   printf ("Antes do fork: PID = %d, PPID = %d\n", getpid (), getppid ());
   if ((ret = fork ()) < 0) { 
        perror ("erro na duplicação do processo");
        return EXIT_FAILURE;
   }
   if (ret > 0) sleep (1);

   if(ret==0)
   printf ("FILHO Quem sou eu?\nApós o fork: PID = %d, PPID = %d, retorno do fork = %d\n",
           getpid (), getppid (), ret);
   else printf ("PAI Quem sou eu?\nApós o fork: PID = %d, PPID = %d, retorno do fork = %d\n",
           getpid (), getppid (), ret);

   return EXIT_SUCCESS;
}
