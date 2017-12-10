#include "header.h"

void kill_proc() {
	char to_write[MAXBUFLEN];
	sprintf(to_write, " Process %d finished.\n", getpid());
	sem_wait(mutex_files);
	write_to_mmf(to_write, strlen(to_write));
	sem_post(mutex_files);
	printf("End of shift of doctor %ld\n", (long)getpid());
	exit(0);
}

void doctor (int shift_length) {
	struct msqid_ds buf;
	struct timespec tp;
	char to_write[MAXBUFLEN];
	char temp[10];
	MQ_patient patient;
	double time_patient, total_time;

	alarm(shift_length);
	printf("[doctor] I am doctor %ld\n", (long)getpid());

	sprintf(temp, "%d", getpid());
	strcpy(to_write, "Process ");
	strcat(to_write, temp);
	strcat(to_write, " started.\n");
	sem_wait(mutex_files);
	write_to_mmf(to_write, strlen(to_write));
	sem_post(mutex_files);

	clock_gettime(CLOCK_REALTIME, &tp);
	double shift_beginning = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
	double current_time = shift_beginning;
	while(current_time-shift_beginning<shift_length) {
		signal(SIGALRM, kill_proc);
		msgrcv(MQ_id, &patient, sizeof(MQ_patient)-sizeof(long), -3, 0);
		signal(SIGALRM, SIG_IGN);
		printf("[doctor] Doctor [%ld] going to treat %s\n",(long)getpid(),patient.info.name);

		clock_gettime(CLOCK_REALTIME, &tp);
		patient.info.begin_attendance = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
		printf("sleeping attendance length....... %d seconds\n\n", patient.info.attendancems);
		sleep(patient.info.attendancems);
		clock_gettime(CLOCK_REALTIME, &tp);
		double end = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;

		time_patient = 1.0 * (patient.info.begin_attendance - patient.info.end_triage);
		total_time = 1.0* (end - patient.info.start);
		sem_wait(mutex);
		(*shared_var).total_treated++;
		(*shared_var).average_after_triage = (double)((*shared_var).average_after_triage * ((*shared_var).total_treated-1) + ((double)(1.0*time_patient)))/(*shared_var).total_treated;
		(*shared_var).average_all = (double)((*shared_var).average_all * ((*shared_var).total_treated-1) + ((double)(1.0*total_time)))/(*shared_var).total_treated;
		sem_post(mutex);
		printf("********************************Doctor %d has just treated %s\n", getpid(), patient.info.name);
		sem_wait(mutex_extra_doctor);
		msgctl(MQ_id, IPC_STAT, &buf);
		if((*shared_var).there_is_extra_doctor && buf.msg_qnum < mq_max*0.8){
			printf("\n###########exiting extra doctor.............\n");
			(*shared_var).there_is_extra_doctor = false;
			exit(0);
		}
		sem_post(mutex_extra_doctor);
		alarm(shift_length);
		clock_gettime(CLOCK_REALTIME, &tp);
		double current_time = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;

		#ifdef DEBUG
		printf("##################### DEBUG #######################\n");
		printf("DOCTOR - start: %f\n", shift_beginning);
		printf("DOCTOR - starts treating %s at: %f\n", patient.info.begin_attendance);
		printf("DOCTOR - shift duration: %f\n", current_time-shift_beginning);
		printf("DOCTOR - average_after_triage: %d\n", (*shared_var).average_after_triage);
		printf("DOCTOR - average_all: %d\n", (*shared_var).average_all);
		printf("################## DEBUG END ###################\n");
		#endif
	}
	sprintf(to_write, " Process %d finished.\n", getpid());
	sem_wait(mutex_files);
	write_to_mmf(to_write, strlen(to_write));
	sem_post(mutex_files);
	printf("End of shift of doctor %ld\n", (long)getpid());
	exit(0);
}

void* doctors_after_shift() {
	while (1) {
		wait(NULL);
		if(fork()==0) {
			doctor(shift_length);
		}
	}
}

void create_doctors (int doctors, int shift_length) {
	int i;
	for (i = 0; i < doctors; i++) {
    if (fork() == 0) {
      doctor(shift_length);
    }
	}
	if (pthread_create(&thread_check_doctors, NULL, doctors_after_shift, 0) != 0) {
		perror("\033[91mError creating the thread!");
		exit(1);
	}
	while(1) {
		pthread_cond_wait(&extra_doctor, &mutex_threads2);
		(*shared_var).there_is_extra_doctor = true;
		if (fork() == 0) {
			printf("\n\n\n*******************DOCTOR - creating one additional doctor.....***********************\n\n\n");
			doctor(shift_length);
		}
	}
}
