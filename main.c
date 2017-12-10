#include "header.h"

/*	TODO: corrigir mmf
		TODO: criar processo quando numero de pacientes>MQ_MAX
		TODO: Comando para alterar numero de triagens */
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
		/*if(getpid() == main_pid) {
		for (i = 0; i < triage; i++) {
			pthread_join(triage_threads[i], NULL);
		}
	}*/
	if((msync(addr_mmf,offset,MS_SYNC)) < 0)
			 perror("Error in msync");

	 if( munmap(addr_mmf,offset) == -1)
			 perror("Error in munmap");
	msgctl(MQ_id, IPC_RMID, NULL);
	sem_close(mutex);
	wait(NULL);
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
	create_MQ();
	//Creates the named pipe if it doesn't exist yet
	if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
		perror("Cannot create pipe: ");
		exit(0);
	}
	//Opens the pipe for reading
	int fd;
	if ((fd=open(PIPE_NAME, O_RDWR)) < 0) {
		perror("Cannot open pipe for reading: ");
		exit(0);
	}

	ListP node;
	node = (ListP) malloc(sizeof(Patient));
	while (1) {
		read(fd, node, sizeof(Patient));
		node->start = time(NULL);
		node->next = NULL;
		put_in_list(node);
		printf("client %s was put in the list by the pipe!\n", node->name);
		pthread_cond_signal(&new_pacient_signal);
	}
}

int main(int argc, char *argv[]) {
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
	mutex_files = sem_open("file_mutex", O_CREAT, 777, 1);


	//create tread to read input_pipe
	if (pthread_create(&triage_read_pipe, NULL, read_named_pipe, 0) != 0) {
		perror("Error creating the thread!");
		exit(0);
		return 1;
	}
	//pthread_join(triage_read_pipe, NULL);
	ListP node = malloc(sizeof(Patient));
	strcpy(node->name, "alberto");
	node-> triageNum = 123;
	node-> attendanceNum = 12;
	node-> triagems = 20;
	node-> attendancems = 20;
	node-> priority = 2;
	clock_gettime(CLOCK_REALTIME, &tp);
	node->start = tp.tv_sec + ((double)1.0*tp.tv_nsec)/1000000000;
	node->next = NULL;
	patient_list = node;

	node = malloc(sizeof(Patient));
	strcpy(node->name, "frederico");
	node-> triageNum = 123;
	node-> attendanceNum = 12;
	node-> triagems = 11;
	node-> attendancems = 20;
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
		perror("Error while reading the configuration file\n");
		return 0;
	}

	//create shared memory
	if(!(shmid = shared_memory_stats())) {
		perror("Error while creating shared memory\n");
		return 0;
	}
	signal(SIGUSR1, print_stats);
	create_triages(triage);
	create_doctors(doctors, shift_length);


	shmctl(shmid, IPC_RMID, NULL);
	int i;
	for(i = 0; i<triage; i++)
		pthread_join(triage_threads[i], NULL);
	return 0;
}
