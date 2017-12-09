// Compile as: gcc store_solution.c -lpthread -D_REENTRANT -Wall -o store

#include "header.h"

// sync resources


void *triage_work(void* id_ptr) {
	ListP current = patient_list;
	double time_patient;
	int id = *((int*)id_ptr);
	printf("New triage %d\n", id);
	while(1) {
		pthread_mutex_lock(&mutex_threads);
		if(patient_list != NULL){
			current = patient_list;
			current->begin_triage = clock();
			patient_list = patient_list->next;
			current->next = NULL;
			pthread_mutex_unlock(&mutex_threads);
			usleep(current->triagems);
			sem_wait(mutex);
			add_to_MQ(*current);
			(*shared_var).total_triage++;
			time_patient = 1.0* (current->begin_triage - current->start)/CLOCKS_PER_SEC;
			printf("inicio %s: %ld\n", current->name, current->start);
			printf("inicio %s: %ld\n", current->name, current->begin_triage);
			printf("tempo %s: %f\n", current->name, (double)time_patient);
			(*shared_var).average_before_triage = (double)((*shared_var).average_before_triage * ((*shared_var).total_triage-1) + (time_patient * 1000))/(*shared_var).total_triage;
			sem_post(mutex);
			printf("patient %s triaged by triage %d\n", current->name, id);

		}
		else {
			pthread_mutex_unlock(&mutex_threads);
			pthread_cond_wait(&count_threshold_cv,&mutex_threads);
			pthread_mutex_unlock(&mutex_threads);

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
