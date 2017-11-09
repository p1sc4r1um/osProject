#include <stdio.h>
#include <unistd.h>int main(int argc, char **argv)  {
  // create a pipe for associating "ls" with "sort"  int  fd[2];  pipe(fd);
    if(fork() == 0){	// child proc
  	// Redirect stdout to the input of the pipe and
  	// close unneeded file descriptors
  	dup2(fd[1],fileno(stdout));	// redirect stdout    close(fd[0]);    close(fd[1]);
    
    // Become "ls"    execlp("ls","ls",NULL);    }  else {		// father proc
    // Redirect stdout to the exit of the pipe and
    // close unneeded file descriptors
    dup2(fd[0],fileno(stdin));		// redirect stdin    close(fd[0]);    close(fd[1]);
    
    // Become "sort"    execlp("sort","sort",NULL);    }  return(0);  }