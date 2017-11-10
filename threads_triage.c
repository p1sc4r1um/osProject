// Compile as: gcc store_solution.c -lpthread -D_REENTRANT -Wall -o store

#include "header.h"

// sync resources
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *triage_work(void* id_ptr) {
	int id = *((int*)id_ptr);
	printf("New triage %d\n", id);

	//while(get patient) {
		pthread_mutex_lock(&mutex);
		//////TRIAGE
		//////STATS WRITE
		////if patients == 0 exit

		(*shared_var).total_triage++;
		pthread_mutex_unlock(&mutex);
	//}
}

int create_triages(int triage) {
	int ids[triage];
	int i;

// create threads client

	for (i = 0; i < triage; i++) {
		ids[i] = i;
		if (pthread_create(&triage_threads[i], NULL, triage_work, &ids[i]) != 0) {
			perror("Error creating the thread!");
			exit(0);
			return 1;
		}
	}

// wait until all threads triage finish

	for (i = 0; i < triage; i++) {
		pthread_join(triage_threads[i], NULL);
	}
	return 0;
}
