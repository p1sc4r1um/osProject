#include "header.h"

void kill_proc() {
	printf("end of shift of doctor %ld\n", (long)getpid());
	exit(0);
}

void doctor (int shift_length) {
	struct timespec tp;
	MQ_patient patient;
	double time_patient, total_time;
	signal(SIGALRM, kill_proc);
	ualarm(shift_length * 1000,0);
	//clock_gettime(CLOCK_REALTIME, &tp);
	//double start = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
	printf("[doctor] I am doctor %ld\n", (long)getpid());
 	//while (((int)shift_time < shift_length)) {
	while(1) {
		//signal(SIGALRM, SIG_IGN);

		msgrcv(MQ_id, &patient, sizeof(MQ_patient)-sizeof(long), -3, 0);
		printf("[doctor] Doctor [%ld] going to treat %s\n",(long)getpid(),patient.info.name);
		clock_gettime(CLOCK_REALTIME, &tp);
		patient.info.begin_attendance = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;

		usleep(patient.info.attendancems*1000);

		clock_gettime(CLOCK_REALTIME, &tp);
		double end = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;

		time_patient = 1.0 * (patient.info.begin_attendance - patient.info.end_triage);
		total_time = 1.0* (end - patient.info.start);
	 	sem_wait(mutex);
		(*shared_var).total_treated++;
		(*shared_var).average_after_triage = (double)((*shared_var).average_after_triage * ((*shared_var).total_treated-1) + ((double)(1.0*time_patient)))/(*shared_var).total_treated;
	 	(*shared_var).average_all = (double)((*shared_var).average_all * ((*shared_var).total_treated-1) + ((double)(1.0*total_time)))/(*shared_var).total_treated;
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
            doctor(shift_length);
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
