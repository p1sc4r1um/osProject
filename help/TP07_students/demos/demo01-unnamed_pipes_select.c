#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef struct {
  int a;
  int b;
} numbers;

void worker();
void master();

// File descriptors for the pipe channel
int channel[2];

int main() {
  // Create a pipe
  pipe(channel);

 // Create the processes
  if (fork() == 0) {
    worker();
    exit(0);
  }
  master();
  wait(NULL);
  
  return 0;
}

void worker() {
  numbers n;

  close(channel[1]);
  fd_set read_set;
  

  while (1) {
	  FD_ZERO(&read_set);
	  FD_SET(channel[0], &read_set);
	  if ( select(channel[0]+1, &read_set, NULL, NULL, NULL) > 0 ) {
		read(channel[0], &n, sizeof(numbers));
		printf("[WORKER] Received (%d,%d) from master to add. Result=%d\n", 
           n.a, n.b, n.a+n.b);
	  }
  }
}

void master() {
  numbers n;
 
  close(channel[0]);
 
  while (1) {
    n.a = rand() % 100;
    n.b = rand() % 100;
 
    printf("[MASTER] Sending (%d,%d) for WORKER to add\n", n.a, n.b);
    write(channel[1], &n, sizeof(numbers));
    sleep(2);
  }
}
