
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


#define PIPE_NAME   "np_client_server"

typedef struct
{
  int a;
  int b;
} numbers;

int main(int argc, char *argv[])
{
  // Opens the pipe for writing
  int fd;
  if ((fd=open(PIPE_NAME, O_WRONLY)) < 0)
  {
    perror("Cannot open pipe for writing: ");
    exit(0);
  }

  // Do some work  
	int g = atoi(argv[1]);
    printf("[CLIENT] Sending '%d' for adding\n", atoi(argv[1]));
    write(fd, &g, sizeof(int));
printf("ola");
    sleep(2);

  return 0;
}
