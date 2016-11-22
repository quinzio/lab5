#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char* argv[])
{
	int nread;
  char str1[1024];
  fprintf(stdout, "out\n");
  fprintf(stderr, "err\n");
  while(fgets(str1, 1023, stdin) !=  NULL)
  {
    //fprintf(stdout, "what I read is %s", str1);
		nread = atoi(str1);
		if (nread %2 == 0) fprintf(stdout, "%d\n", nread); 
		if (nread %2 == 1) fprintf(stderr, "%d\n", nread); 
  }
  
  return 0;
}
