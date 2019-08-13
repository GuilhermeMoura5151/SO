
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int fdp[2];
    char buf[1024];

    if(pipe(fdp)!=0) {
       perror(argv[0]);
       return 1;
    }

    switch(fork()) {
       case -1:
              perror(argv[0]);
              return 1;
       case 0:
              close(fdp[0]);
              dup2(fdp[1],1); // child -> stdout to pipe (fd1 is writing to pipe), printf wrting to pipe
              close(fdp[1]);
              printf("Hello Dad!\nHow are you?");
              fflush(stdout);
              execlp("ls","ls",NULL);
              break;
       default:
              close(fdp[1]);
              dup2(fdp[0],0); // parent -> stdin to pipe (fd0 is reading from pipe), scanf reads from pipe
              close(fdp[0]);
              while(scanf("%[^\n]%*c",buf)>0) {
                  printf("Parent: %s\n",buf);
              }
   }
   return 0;
}
