#include "header.h"

void doctor (int shift_length) {

	clock_t start = clock();
	clock_t end = clock();
	double shift_time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("I am doctor %ld and started at %f\n", (long)getpid(), (double) start / CLOCKS_PER_SEC);
 while (/*getPatient() && */((int)shift_time < shift_length)) {

	 end = clock();
	 shift_time = (double)(end - start) / CLOCKS_PER_SEC;


	 (*shared_var).total_treated++;
	 
	 //printf("total treated: %d\n", (*shared_var).total_treated);
	 /*TODO: solve patient*/

//     int time_doc = end / CLOCKS_PER_SEC;
//	 (*shared_var).average_after_triage = ((*shared_var).average_after_triage * ((*shared_var).total_treated-1) + time_doc-patient.end_triage)/(*shared_var).total_treated;
//	 (*shared_var).average_all = ((*shared_var).average_all * ((*shared_var).total_treated-1) + time_doc-patient.start)/(*shared_var).total_treated;

    #ifdef DEBUG
     printf("DOCTOR - start: %d\n", patient.end_triage);
     printf("DOCTOR - end: %d\n", time_doc);
     printf("DOCTOR - shift duration: %d\n", shift_time);
     printf("DOCTOR - average_after_triage: %d\n", (*shared_var).average_after_triage);
     printf("DOCTOR - average_all: %d\n", (*shared_var).average_all);
    #endif
 }
 printf("end of shift of doctor %ld\n", (long)getpid());
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
