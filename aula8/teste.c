#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{
   switch(fork()){
       case 0: printf("a\n");
                break;
        defaul: printf("b\n");
                wait(NULL);
                printf("c\n");
   }
   return 0; // Parent e Child
}