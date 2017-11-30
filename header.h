#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#define PIPE_NAME   "input_pipe"
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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

/*STRUCTS*/

typedef struct patient *ListP;
typedef struct patient {
	char name [50];
	int triageNum, attendanceNum, triagems, attendancems, priority;
	clock_t start, begin_triage, begin_attendance;
	ListP next;
}Patient;


typedef struct stats {
	int total_triage, total_treated, average_before_triage, average_after_triage, average_all;
}Stats;

//#define DEBUG

Stats *shared_var;
int main_pid;
int doctors, triage, shift_length, mq_max, shmid;
ListP patient_list;
pthread_t *triage_threads;
pthread_t triage_read_pipe;


/*FUNCTIONS*/
void force_exit();
void* read_named_pipe();
int read_config(int *triage,int *doctors,int *shift_length,int *mq_max);

int shared_memory_stats();

void *triage_work(void* id_ptr);
int create_triages(int triage);

void doctor(int shift_length);
void create_doctors(int doctors, int shift_length);


#endif
