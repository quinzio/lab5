#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>
#define N 9

// Signal handler
void sig_hnd(int signo) {
  fprintf(stdout, "Received sig. %d\n", signo);
}

int main(int argc, char*argv[])
{
  pid_t pidc;
  int i;

  signal(SIGALRM, sig_hnd);  
  pidc = fork();
  if (pidc) {
    //parent
    for(i=0; i<10; i++) {
      pause();
      fprintf(stdout, "Parent woke up\n");
      sleep(1);
      kill(pidc, SIGALRM);
    }
  }
  else {
    //child
    for(i=0; i<10; i++)
    {
      sleep(1);
      kill(getppid(), SIGALRM);
      pause();
      fprintf(stdout, "Child woke up\n");
    }
  } 
  return 0;
}
