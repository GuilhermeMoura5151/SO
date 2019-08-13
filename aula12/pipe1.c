
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int fdp[2],fdp2[2];
    char buf[1024];

    if(pipe(fdp)!=0) {
        perror(argv[0]);
        return 1;
    }

    if(pipe(fdp2)!=0) {
        perror(argv[0]);
        return 1;
    }

    switch(fork()) {
       case -1:
              perror(argv[0]);
              return 1;
       case 0:
              //Child
              close(fdp[0]);
              write(fdp[1],"Hello Dad!",11); //11= numero de caracteres da frase, incluindo NULL caractere
              close(fdp2[1]);
              read(fdp2[0],buf,1024);
              printf("Child: %s\n",buf);
              break;
       default:
              //Parent
              close(fdp[1]);
              read(fdp[0],buf,1024);
              printf("Parent: %s\n",buf);
              close(fdp2[0]);
              write(fdp2[1],"Good Morning!",14);
              close(fdp2[1]);
   }
   return 0;
}
