
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

int main()
{
  // Creates the named pipe if it doesn't exist yet
  if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST))
  {
    perror("Cannot create pie: ");
    exit(0);
  }

  // Opens the pipe for reading
  int fd;
  if ((fd=open(PIPE_NAME, O_RDWR)) < 0)
  {
    perror("Cannot open pipe for reading: ");
    exit(0);
  }

  // Do some work
  int n;
  while (1) {
    read(fd, &n, sizeof(int));
    printf("[SERVER] Received '%d'",n);
  }

  return 0;
}
