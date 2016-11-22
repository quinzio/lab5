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
  sem_t sem1, sem2;
  pid_t pid, pid_old;
  int i, j;
  unsigned char vala, valb;
  int fd=-1;

  // check correct argument number
  if (argc != 2) {
    fprintf(stderr, "Bad argument number\n");
    exit(-1);
  }

  // initialize semaphores
  if (sem_init(&sem1, 1, 0)!=0) {
    fprintf(stderr, "error creating semaphores\n");
    exit(-1);
  }
  if (sem_init(&sem2, 1, 0)!=0) {
    fprintf(stderr, "error creating semaphores\n");
    exit(-1);
  }
  
  //body
  //for(i=0; i<1; i++) {
  for(i=0; i<N-1; i++) {
    fprintf(stdout, "Enter loop 1\n");
    pid=0;
    for(j=N-2-i; j>=0; j--) {
      fprintf(stdout, "Enter loop 2\n");
      pid_old = pid;
      pid = fork();
      if (pid) fprintf(stdout, "pid %d\n", pid);
      if (!pid) {
        signal(SIGALRM, sig_hnd);
        // open file
        fd = open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
        lseek(fd, j, SEEK_SET);
        sem_post(&sem1);
        fprintf(stdout, "Pause on loop i j %d %d\n", i, j);
        pause();
        fprintf(stdout, "Exit pause on loop i j %d %d\n", i, j);
        read(fd, &vala, 1);
        read(fd, &valb, 1);
        fprintf(stdout, "vala %c valb %c\n", vala, valb);
        if (vala > valb) {
          fprintf(stdout, "Write to file\n");
          lseek(fd, j, SEEK_SET);
          write(fd, &valb, 1);
          write(fd, &vala, 1);
        }
        close(fd);
        if (pid_old) {
          kill(pid_old, SIGALRM);
          fprintf(stdout, "Sent kill to pid %d\n", pid_old);
        }
        sem_post(&sem2);  
        fprintf(stdout, "Exit process on loop i j %d %d\n", i, j);
        exit(0);
      }    
    }
    // wait all children to be in pause
    fprintf(stdout, "Witing on pause\n");
    for (j=0; j<N-1-i; j++) {
      fprintf(stdout, "Witing on semaphore %d\n", j);
      //sem_wait(&sem1);
    }
    // trigger first child
    sleep(1);
    kill(pid, SIGALRM); 
    sleep(1);
    fprintf(stdout, "Sent kill to pid %d\n", pid);
    // wait for all children to finish
    for (j=0; j<N-1; j++) {
      //sem_wait(&sem2);
    }
   
  }
  fprintf(stdout, "finish\n");
  // destroy semaphores
  sem_destroy(&sem1);
  sem_destroy(&sem2);
  close(fd);
  return 0;
}
