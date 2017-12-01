// Compile as: gcc store_solution.c -lpthread -D_REENTRANT -Wall -o store

#include "header.h"

// sync resources


void *triage_work(void* id_ptr) {
	ListP current = patient_list;
	int id = *((int*)id_ptr);
	printf("New triage %d\n", id);
	while(1) {
		printf("erterterter\n");
		pthread_mutex_lock(&mutex_threads);
		if(patient_list != NULL){
			printf("xau\n");
			current = patient_list;
			patient_list = patient_list->next;
			current->next = NULL;
			pthread_mutex_unlock(&mutex_threads);
			sleep(current->triagems);
			sem_wait(mutex);
			printf("patient %s triaged\n", current->name);
			(*shared_var).total_triage++;
			sem_post(mutex);
		}
		else {
			printf("skah\n");
			pthread_mutex_unlock(&mutex_threads);
			pthread_cond_wait(&count_threshold_cv,&mutex_threads);
			printf("fshdsf\n");
		}
	}
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
