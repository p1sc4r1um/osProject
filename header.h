#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
/**/
#define FILEMODE S_IRWXU | S_IRGRP | S_IROTH
#define LOG_FILE_NAME   "log_file.log"
#define MAXBUFLEN 50
#define _GNU_SOURCE
/**/

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
#include <sys/msg.h>
#include<sys/mman.h>
#include<fcntl.h>





/*STRUCTS*/

typedef struct patient *ListP;
typedef struct patient {
	char name [50];
	int triageNum, attendanceNum, triagems, attendancems, priority;
	double start, begin_triage, end_triage, begin_attendance;
	ListP next;
}Patient;

typedef struct {
	long priority;
	Patient info;
}MQ_patient;

typedef struct stats {
	int total_triage, total_treated;
	double average_before_triage, average_after_triage, average_all;
}Stats;

//#define DEBUG

Stats *shared_var;
int main_pid;
int doctors, triage, shift_length, mq_max, shmid;
ListP patient_list;
pthread_t *triage_threads;
pthread_t triage_read_pipe;
pthread_t thread_check_doctors;

sem_t *mutex;
sem_t *mutex_files;
pthread_mutex_t mutex_threads;
pthread_mutex_t mutex_threads2;
pthread_cond_t new_pacient_signal;
pthread_cond_t extra_doctor;
int MQ_id;
int offset;
/*FUNCTIONS*/
void force_exit();
void* read_named_pipe();
int read_config(int *triage,int *doctors,int *shift_length,int *mq_max);
int shared_memory_stats();
void create_MMF();

void *triage_work(void* id_ptr);
int create_triages(int triage);

void doctor(int shift_length);
void create_doctors(int doctors, int shift_length);
void create_MQ();
void add_to_MQ(Patient patient);
void write_to_mmf (char* buffer, int buffer_size);

void print_stats();

int fd_mmf;
char* addr_mmf;

#endif
