// Compile as: gcc store_solution.c -lpthread -D_REENTRANT -Wall -o store

#include "header.h"

// sync resources


void *triage_work(void* id_ptr) {
	struct timespec tp;
	ListP current = patient_list;
	double time_patient;
	int id = *(int*)id_ptr;
	printf("[triage] New triage %d\n", id);
	while(1) {
		pthread_mutex_lock(&mutex_threads);
		if(patient_list != NULL){
			current = patient_list;
			clock_gettime(CLOCK_REALTIME, &tp);
			current->begin_triage = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
			patient_list = patient_list->next;
			current->next = NULL;
			pthread_mutex_unlock(&mutex_threads);
			usleep(current->triagems*1000);
			sem_wait(mutex);
			(*shared_var).total_triage++;
			time_patient = (double)1.0*(current->begin_triage - current->start);
			(*shared_var).average_before_triage = (double)((*shared_var).average_before_triage * ((*shared_var).total_triage-1) + ((double)(1.0*time_patient)))/(*shared_var).total_triage;
			clock_gettime(CLOCK_REALTIME, &tp);
			current->end_triage = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
			add_to_MQ(*current);

			sem_post(mutex);

			printf("[triage] patient %s triaged by triage %d. elapsed time during triage: %f seconds\n", current->name, id, 1.0*(current->end_triage-current->begin_triage));
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
		//printf("id:%d\n", ids[i]);
		if (pthread_create(&triage_threads[i], NULL, triage_work, &ids[i]) != 0) {
			perror("Error creating the thread!");
			exit(0);
			return 1;
		}
	}

// wait until all threads triage finish

	/*for (i = 0; i < triage; i++) {
		pthread_join(triage_threads[i], NULL);
	}*/
	return 0;
}
