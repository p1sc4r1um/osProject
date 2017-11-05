#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
/*LIBRARIES*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

/*STRUCTS*/

typedef struct patient {
	char name [50], triageNum [50], attendanceNum [50], priority [50];
	clock_t start, end_triage;
}Patient;

typedef struct stats {
	int total_triage, total_treated, average_before_triage, average_after_triage, average_all;
}Stats;

#define DEBUG

Stats *shared_var;


/*FUNCTIONS*/
int read_config(int *triage,int *doctors,int *shift_length,int *mq_max);

int shared_memory_stat();

void *triage_work(void* id_ptr);
int create_triages(int triage);

void doctor(int shift_length);
void create_doctors(int doctors, int shmid, int shift_length);


#endif
