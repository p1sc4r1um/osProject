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
int father_pid;


void sigint(int signum) {
	if (getpid() == father_pid) {
		while(wait(NULL)!=-1);
		printf("Ok, bye bye!\n"); 
	} else printf ("Another child is dead!\n");
	exit(0);
}

int main() {
  // Redirects SIGINT to sigint()
  signal(SIGINT, sigint);  
  // Create a pipe
  pipe(channel);
  
  father_pid = getpid();
 // Create the processes
  int i;
  for (i = 0; i < 4; i++) {
	  if (fork() == 0) {
		worker(i);
		exit(0);
	  }
  }
  for (i = 0; i < 4; i++) {
	if (fork() == 0) {
	  master(i);
	  exit(0);	
	}  
  }
  

  while(wait(NULL)!=-1);
  
  return 0;
}

void worker(int id) {
  numbers n;

  close(channel[1]);

  while (1) {
	fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(channel[0], &read_set);
    if ( select(channel[0]+1, &read_set, NULL, NULL, NULL) > 0 ) {
		read(channel[0], &n, sizeof(numbers));
		printf("[WORKER %d] Received (%d,%d) from master to add. Result=%d\n", id,
           n.a, n.b, n.a+n.b);
	} 
  }
}

void master(int id) {
  numbers n;
 
  close(channel[0]);
  srand(getpid());
  while (1) {
    n.a = rand() % 100;
    n.b = rand() % 100;
    printf("[MASTER %d] Sending (%d,%d) for WORKER to add\n", id, n.a, n.b);
    write(channel[1], &n, sizeof(numbers));
    sleep(rand()%2);
  }
}

