#include "header.h"

void kill_proc() {
	printf("end of shift of doctor %ld\n", (long)getpid());
	exit(0);
}

void doctor (int shift_length) {
	MQ_patient patient;
	double time_patient, total_time;
	signal(SIGALRM, kill_proc);
	ualarm(shift_length, 0);
	clock_t start = clock();
	//double shift_time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("I am doctor %ld and started at %f\n", (long)getpid(), (double) start / CLOCKS_PER_SEC);
 	//while (((int)shift_time < shift_length)) {
	while(1) {	
		signal(SIGALRM, SIG_IGN);
		msgrcv(MQ_id, &patient, sizeof(MQ_patient)-sizeof(long), -3, 0);
		current->begin_attendance = clock();
		usleep(current->attendancems);
		clock_t end = clock();
		//shift_time = (double)(end - start) / CLOCKS_PER_SEC;
		time_patient = 1.0 * (current->begin_attendance - current->end_triage)/CLOCKS_PER_SEC;
	 	sem_wait(mutex);
		(*shared_var).total_treated++;
		(*shared_var).average_after_triage = ((*shared_var).average_after_triage * ((*shared_var).total_treated-1)) + (time_patient * 1000)/(*shared_var).total_treated;
		total_time = 1.0* (end - current->start)/CLOCKS_PER_SEC;
	 	(*shared_var).average_all = ((*shared_var).average_all * ((*shared_var).total_treated-1)) + (total_time * 1000)/(*shared_var).total_treated; 
		sem_post(mutex);

	 //printf("total treated: %d\n", (*shared_var).total_treated);
		signal(SIGALRM, kill_proc);
		#ifdef DEBUG
		 printf("DOCTOR - start: %d\n", patient.end_triage);
		 printf("DOCTOR - end: %d\n", time_doc);
		 printf("DOCTOR - shift duration: %d\n", shift_time);
		 printf("DOCTOR - average_after_triage: %d\n", (*shared_var).average_after_triage);
		 printf("DOCTOR - average_all: %d\n", (*shared_var).average_all);
		#endif
	}
}

void create_doctors (int doctors, int shift_length) {

    int i;
    for (i = 0; i < doctors; i++) {
        if (fork() == 0) {
            #ifdef DEBUG
            printf("DOCTOR - creating one doctor.....\n");
            #endif

            doctor(shift_length);
            exit(0);
        }
    }
    while (/*get patient*/false) {
      wait(NULL);
      /*if(queue > mq_max) {
                 if (fork() == 0) {
	          #ifdef DEBUG
	          printf("DOCTOR - creating one additional doctor.....\n");
	          #endif

	        doctor(shift_length);
        exit(0);
      }

      }*/
      if (fork() == 0) {
	#ifdef DEBUG
        printf("DOCTOR - creating one additional doctor.....\n");
        #endif

        doctor(shift_length);
        exit(0);
      }
    }



    for (i = 0; i < doctors; i++) {
      wait(NULL);
    }


}
