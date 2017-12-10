// Compile as: gcc store_solution.c -lpthread -D_REENTRANT -Wall -o store

#include "header.h"

// sync resources


void *triage_work(void* id_ptr) {
	struct msqid_ds buf;
	char to_write[MAXBUFLEN];
	char temp[10];
	struct timespec tp;
	ListP current = patient_list;
	double time_patient;
	int id = *(int*)id_ptr;
	printf("[triage] New triage %d\n", id);
	while(1) {
		pthread_mutex_lock(&mutex_threads);
		if(patient_list != NULL){
			current = patient_list;
			current->begin_triage = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
			sprintf(temp, "%d", id);
			strcpy(to_write, "Triage ");
			strcat(to_write, temp);
			strcat(to_write, "started triage of ");
			strcat(to_write, current->name);
			strcat(to_write, "\n");
			write_to_mmf(to_write, strlen(to_write));
			clock_gettime(CLOCK_REALTIME, &tp);
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
			msgctl(MQ_id, IPC_STAT, &buf);
			if(buf.msg_qnum > mq_max) {
				pthread_cond_signal(&extra_doctor);
			}

			sem_post(mutex);
			strcpy(to_write, "Triage ");
			strcat(to_write, temp);
			strcat(to_write, "finished triage of ");
			strcat(to_write, current->name);
			strcat(to_write, "\n");
			write_to_mmf(to_write, strlen(to_write));
			printf("[triage] patient %s triaged by triage %d. elapsed time during triage: %f seconds\n", current->name, id, 1.0*(current->end_triage-current->begin_triage));
		}
		else {
			pthread_mutex_unlock(&mutex_threads);
			pthread_cond_wait(&new_pacient_signal,&mutex_threads);
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
