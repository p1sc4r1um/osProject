//compile with: gcc -Wall -pthread sharedvariable_posix.c -o svar
//using POSIX named semaphores
#include "header.h"


int shared_memory_stats() {
  int shmid;
	// Create shared memory
	if( (shmid = shmget(IPC_PRIVATE,sizeof(Stats),IPC_CREAT|0766)) > 0) {
    //printf("\nShared memory created\n");
    // Attach shared memory
    shared_var = (Stats *) shmat(shmid,NULL,0);
    (*shared_var).total_triage = 0;
    (*shared_var).average_after_triage = 0;
    (*shared_var).average_all = 0;
    (*shared_var).average_before_triage = 0;

    #ifdef DEBUG
    printf("################## DEBUG ###################\n");
    printf("SHM - total_triage: %d, average_after_triage: %d, average_all: %d, average_before_triage: %d\nshmid: %d\n",(*shared_var).total_triage,(*shared_var).average_after_triage,(*shared_var).average_all,(*shared_var).average_before_triage,shmid);
    printf("################## DEBUG END ###################\n");
    #endif
    return shmid;

  }
  else {
    perror("\nERROR IN CREATING MEMORY\n");
    return 0;
  }

}
