#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>
#include <ctype.h>
#define N 9

// Signal handler
void sig_hnd(int signo) {
	fprintf(stdout, "Received sig. %d\n", signo);
}

int main(int argc, char*argv[])
{
	pid_t pidc1, pidc2;
	int i;

	signal(SIGALRM, sig_hnd);  
	pidc1 = fork();
	if (pidc1) {
		//parent
		pidc2 = fork();
		if (pidc2) {
			//parent
			int fd;
			int status;
			fd = open("pids", O_WRONLY | O_CREAT | O_TRUNC , 0666);
			write(fd, &pidc1, sizeof(pidc1));
			write(fd, &pidc2, sizeof(pidc2));
			close(fd);
			sleep(1);
			kill(pidc1, SIGALRM);
			kill(pidc2, SIGALRM);
			waitpid(pidc1, &status, 0); 
			waitpid(pidc2, &status, 0); 
			system("rm pids");
		}
		else {
			//child 2
			int fd;
			char str1[1024];
			char str2[1024];
			pause(); // wait for parent to write the file with pids
			fd = open("pids", O_RDONLY, 0);
			read(fd, &pidc1, sizeof(pidc1));
			read(fd, &pidc2, sizeof(pidc2));
			fprintf(stdout, "Child 2 pidc1 %d pidc2 %d\n", pidc1, pidc2);
			close(fd);
			sleep(1);
			fd = open("exchange", O_RDONLY | O_SYNC, 0666);
			for(;;) {
				fprintf(stdout, "Child 2 entering pause\n");
				pause();
				//sleep(1);
				fprintf(stdout, "Child 2 woke up\n");
				i=0;
				lseek(fd, 0, SEEK_SET);
				do{ read(fd, &str1[i], 1); i++; } while (str1[i-1]!='\n');	
				str1[i]=0;
				fprintf(stdout, "%d\n", i);
				i=0;
				while(str1[i]!='\n') {
					str2[i] = toupper(str1[i]);
					i++;
				}
				fprintf(stdout, "%d\n", i);
				str2[i]='\n';
				i++;
				str2[i]=0;
				fprintf(stdout, "%s", str2);
				if (strncmp(str1, "end\n", 4)==0) {
					break;
				}
				sleep(1);
				kill(pidc1, SIGALRM);
			}
			close(fd);
		}
	}
	else {
		//child 1
		char str1[1024];
		int fd;
		pause();
		fd = open("pids", O_RDONLY, 0);
		read(fd, &pidc1, sizeof(pidc1));
		read(fd, &pidc2, sizeof(pidc2));
		fprintf(stdout, "Child 1 pidc1 %d pidc2 %d\n", pidc1, pidc2);
		close(fd);
		fd = open("exchange", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, 0666);
		for(;;)
		{
			sleep(1);
			fgets(str1, 1023, stdin);
			lseek(fd, 0, SEEK_SET);
			fprintf(stdout, "stringa immessa %s %d\n", str1, strlen(str1));
			write(fd, str1, strlen(str1)+1);		
			kill(pidc2, SIGALRM);
			if (strncmp(str1, "end\n", 4)==0) break;
			pause();
			fprintf(stdout, "Child woke up\n");
		}
		close(fd);
	} 
	return 0;
}
