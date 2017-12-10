
#include "header.h"


void kill_thread() {
	pthread_exit(NULL);
}

void *triage_work(void* id_ptr) {
	struct msqid_ds buf;
	char to_write[MAXBUFLEN];
	struct timespec tp;
	ListP current = patient_list;
	double time_patient;
	int id = *(int*)id_ptr;
	printf("\033[93m[triage] New triage %d\n\033[0m", id);
	sprintf(to_write, "New triage %d", id);
	sem_wait(mutex_files);
	write_to_mmf(to_write, strlen(to_write));
	sem_post(mutex_files);
	signal(SIGUSR2, kill_thread);
	while(1) {
		pthread_mutex_lock(&mutex_threads);
		if(patient_list != NULL){
			current = patient_list;
			clock_gettime(CLOCK_REALTIME, &tp);
			current->begin_triage = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
			sprintf(to_write, "Triage %d started triage of ", id);
			strcat(to_write, current->name);
			strcat(to_write, "\n");
			sem_wait(mutex_files);
			write_to_mmf(to_write, strlen(to_write));
			sem_post(mutex_files);
			clock_gettime(CLOCK_REALTIME, &tp);
			patient_list = patient_list->next;
			current->next = NULL;
			pthread_mutex_unlock(&mutex_threads);
			sleep(current->triagems);
			sem_wait(mutex);
			(*shared_var).total_triage++;
			time_patient = (double)1.0*(current->begin_triage - current->start);
			(*shared_var).average_before_triage = (double)((*shared_var).average_before_triage * ((*shared_var).total_triage-1) + ((double)(1.0*time_patient)))/(*shared_var).total_triage;
			clock_gettime(CLOCK_REALTIME, &tp);
			current->end_triage = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
			add_to_MQ(*current);
			msgctl(MQ_id, IPC_STAT, &buf);
			printf("\033[1m\n\nnumber of patients waiting for doctor:%ld\n\n\033[0m",buf.msg_qnum);
			if(buf.msg_qnum > mq_max && !(*shared_var).there_is_extra_doctor) {
				pthread_cond_signal(&extra_doctor);
			}
			sem_post(mutex);
			sprintf(to_write, "Triage %d ended triage of ", id);
			strcat(to_write, current->name);
			strcat(to_write, "\n");
			sem_wait(mutex_files);
			write_to_mmf(to_write, strlen(to_write));
			sem_post(mutex_files);
			printf("[triage] patient %s triaged by triage %d. elapsed time during triage: %f seconds\n", current->name, id, 1.0*(current->end_triage-current->begin_triage));


			#ifdef DEBUG
			printf("\033[4m################## DEBUG ###################\n");
			printf("name: %s\n",  current->name);
			printf("EPOCH->patient creation: %f\nEPOCH->triage starts\nEPOCH->triage ends\nPacient creation->triage starts: %f\n", current->start, current->begin_triage,current->end_triage ,time_patient);
			printf("################## DEBUG END ###################\n\033[0m");
			#endif
		}
		else {
			pthread_mutex_unlock(&mutex_threads);
			pthread_cond_wait(&new_pacient_signal,&mutex_threads);
			pthread_mutex_unlock(&mutex_threads);
		}
	}
}

int create_triages(int triage) {
	int i;

// create threads client

	for (i = 0; i < triage; i++) {
		ids[i] = i;
		if (pthread_create(&triage_threads[i], NULL, triage_work, &ids[i]) != 0) {
			perror("\033[91mError creating the thread!");
			exit(0);
			return 1;
		}
	}
	return 0;
}
