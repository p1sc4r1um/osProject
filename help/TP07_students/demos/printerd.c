#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>


#define BUF_SIZE      			4096
#define NUM_PRINTERS  			3

const char* PRINTER_NAME[] = { 
  "printer1", "printer2", "printer3" 
};

// The printer file descriptors
int printer[NUM_PRINTERS];

void create_printers() {
  int i;
  for (i=0; i<NUM_PRINTERS; i++) {
    unlink(PRINTER_NAME[i]);
    mkfifo(PRINTER_NAME[i], O_CREAT|O_EXCL|0666);
    printer[i] = open(PRINTER_NAME[i], O_RDONLY|O_NONBLOCK);
    assert(printer[i] >= 0);
  }
}

void accept_requests() {
  while (1) {
    fd_set read_set;
    FD_ZERO(&read_set);
    int i;
    for (i=0; i<NUM_PRINTERS; i++)
      FD_SET(printer[i], &read_set);

		if ( select(printer[NUM_PRINTERS-1]+1, &read_set, NULL, NULL, NULL) > 0 ) {
		  for (i=0; i<NUM_PRINTERS; i++)  {
			if (FD_ISSET(printer[i], &read_set)) {
			  printf("[<%s> PRINTING]: ", PRINTER_NAME[i]); 

			  char buf[BUF_SIZE];
			  int n = 0;
			  do {
				n = read(printer[i], buf, BUF_SIZE);
				if (n > 0) {
				  buf[n] = '\0';
				  printf("%s", buf);
				}
			  } while (n > 0);
		 
			  close(printer[i]);
			  printer[i] = open(PRINTER_NAME[i], O_RDONLY|O_NONBLOCK);
			}
  } } }  
}

int main(int argc, char* argv[]) {
  create_printers();
  accept_requests();
  exit(0);
}
