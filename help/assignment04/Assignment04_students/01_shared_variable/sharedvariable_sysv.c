//compile with: gcc -Wall sharedvariable_sysv.c semlib.c -o svar
//using SysV semaphores

/*insert here the libraries needed*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "semlib.h"


int * shared_var;
int shmid, semid;

void worker()
	{
	usleep(1000000 + rand()%11*100000); //uses microseconds sleep and waits 1 to 2 seconds in periods of 0.1 secs
	sem_wait(semid,0);
	(*shared_var)++;
	sem_post(semid,0);
	}

int main(int argc, char *argv[])
	{
	int i, n_procs;

	if(argc!=2) {
		perror("Wrong number of parameters\n");
		exit(0);
		}

	n_procs=atoi(argv[1]);

	// Create shared memory
	if( (shmid = shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|0766)) > 0) {
		printf("\nShared memory created\n");
	}

	else {
		perror("\nERROR IN CREATING MEMORY\n");
		exit(1);
	}

	// Attach shared memory
	shared_var = (int *) shmat(shmid,NULL,0);
	
	// Create semaphores
	if (semid = sem_get(1,1) == -1) {
		perror("\nERROR CREATING SEMAPHORE\n");
		exit(1);
	}

	// initialize variable in shared memory
	(*shared_var) = 1000;

	// Create worker processes
	for(i = 0; i < n_procs; i++) {
		if ( fork() == 0 ) {
			worker();
		exit(0);
		}
	}
	// Wait for all worker processes

	for(i = 0; i < n_procs; i++) wait(NULL);

	// print final result
	printf("Shared Var: %d\n",(*shared_var));

	// remove resources (shared memory and semaphores)
	shmdt(shared_var);
	shmctl(shmid,IPC_RMID,NULL);	

	exit(0);
}
