#include "header.h"

/*
		TODO: corrigir mmf
		TODO: criar processo quando numero de pacientes>MQ_MAX
		TODO: Comando para alterar numero de triagens
*/


void put_in_list(ListP node) {
	ListP temp = patient_list;
	if(temp != NULL) {
		while(temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = temp;
	}
	else {
		patient_list = node;
		pthread_cond_signal(&new_pacient_signal);
	}
}


void force_exit() {
	printf("\033[93m############exiting...######################\n\033[0m");

	msync(addr_mmf,offset,MS_SYNC);
	munmap(addr_mmf,offset);
	msgctl(MQ_id, IPC_RMID, NULL);
	sem_close(mutex);
	sem_close(mutex_files);
	shmctl(shmid, IPC_RMID, NULL);
	exit(0);
}

void print_stats() {
	printf("\n\n#################STATS#################\n\n");
	printf("[shared_var] Total Triaged: %d\n", (*shared_var).total_triage);
	printf("[shared_var] Total Treated: %d\n", (*shared_var).total_treated);
	printf("[shared_var] average [before triage]: %f\n", (*shared_var).average_before_triage);
	printf("[shared_var] average [after_triage]: %f\n", (*shared_var).average_after_triage);
	printf("[shared_var] average [all]: %f\n\n", (*shared_var).average_all);
}

void* read_named_pipe () {
	int i;
	int new_triage;
	//Creates the named pipe if it doesn't exist yet
	if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
		perror("\033[91mCannot create pipe: ");
		exit(0);
	}
	//Opens the pipe for reading
	int fd;
	if ((fd=open(PIPE_NAME, O_RDWR)) < 0) {
		perror("\033[91mCannot open pipe for reading: ");
		exit(0);
	}

	ListP node;
	node = (ListP) malloc(sizeof(Patient));
	while (1) {
		read(fd, node, sizeof(Patient));
		if(strcmp(node->name,"thread123") != 0) {
			node->start = time(NULL);
			node->next = NULL;
			put_in_list(node);
			printf("client %s was put in the list by the pipe!\n", node->name);
			pthread_cond_signal(&new_pacient_signal);
		}
		else {
			new_triage = node->triagems;
			triage_change = new_triage - num_of_triages;
			for (i = 0; i < triage_change; i++) {
				 ids[num_of_triages] = num_of_triages;
				 printf("\nextra thread\n");
				if (pthread_create(&triage_threads[num_of_triages], NULL, triage_work, &ids[num_of_triages]) != 0) {
					perror("\033[91mError creating the thread!");
					exit(1);
				}
				num_of_triages++;
			}
			for(i = 0; i>triage_change; i--) {
				printf("\nkilling thread\n");
				  pthread_kill(triage_threads[num_of_triages-1], SIGUSR2);
					num_of_triages--;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	create_MQ();
	create_MMF();
	main_pid = getpid();
	printf("main pid: %d\n", main_pid);
	struct timespec tp;
	pthread_mutex_init(&mutex_threads, NULL);
	pthread_mutex_init(&mutex_threads2, NULL);


	pthread_cond_init(&new_pacient_signal, NULL);
	pthread_cond_init(&extra_doctor, NULL);
	patient_list = NULL;
	signal(SIGINT,force_exit);
	sem_unlink("memory_mutex");
	mutex = sem_open("memory_mutex", O_CREAT, 777, 1);
	sem_unlink("file_mutex");
	mutex_files = sem_open("file_mutex", O_CREAT|O_EXCL, 777, 1);
	sem_unlink("extra_doctors");
	mutex_extra_doctor = sem_open("extra_doctors", O_CREAT|O_EXCL, 777, 1);

	//create thread to read input_pipe
	if (pthread_create(&triage_read_pipe, NULL, read_named_pipe, 0) != 0) {
		perror("\033[91mError creating the thread!");
		exit(0);
		return 1;
	}
	//pthread_join(triage_read_pipe, NULL);
	ListP node = malloc(sizeof(Patient));
	strcpy(node->name, "alberto");
	node-> triageNum = 123;
	node-> attendanceNum = 12;
	node-> triagems = 1;
	node-> attendancems = 5;
	node-> priority = 2;
	clock_gettime(CLOCK_REALTIME, &tp);
	node->start = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
	node->next = NULL;
	patient_list = node;

	node = malloc(sizeof(Patient));
	strcpy(node->name, "frederico");
	node-> triageNum = 123;
	node-> attendanceNum = 12;
	node-> triagems = 1;
	node-> attendancems = 9;
	node-> priority = 1;
	clock_gettime(CLOCK_REALTIME, &tp);
	node->start = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;

	node->next = NULL;
	patient_list->next= node;


	//read file
	if(read_config(&triage, &doctors, &shift_length, &mq_max)) {
		printf("%d, %d, %d, %d\n", triage, doctors, shift_length, mq_max);
	}
	else {
		perror("\033[91mError while reading the configuration file\n");
		return 0;
	}
	num_of_triages = triage;
	//create shared memory
	if(!(shmid = shared_memory_stats())) {
		perror("\033[91mError while creating shared memory\n");
		return 0;
	}
	signal(SIGUSR1, print_stats);
	create_triages(triage);
	sleep(7);
	create_doctors(doctors, shift_length);


	shmctl(shmid, IPC_RMID, NULL);
	int i;
	for(i = 0; i<triage; i++)
		pthread_join(triage_threads[i], NULL);
	return 0;
}
