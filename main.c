#include "header.h"

/* CREATE FILE TXT "config.txt" WITH THIS DATA*/

// #define TRIAGE 5 //NUMBER OF THREADS IN TRIAGE
// #define DOCTORS 10 //NUMBER OF DOCTOR PROCCESSES
// #define SHIFT_LENGTH 5 //TIME OF EACH DOCTOR SHIFT
// #define MQ_MAX 5 //MAX QUEUE LENGTH TO ATTENDANCE
/*TWO TYPES OF INPUT PARAMETERS:
1 - SPECIFIC PATIENT, EX: "João 10 50 1 // Paciente João: 10ms triagem, 50ms atendimento, prioridade 1"
2 - GROUP OF PATIENTS, ASSIGN AUTOMATICALLY THE NAMES OR NUMBER OF THE PATIENTS, EX: "8 10 65 3 // 8 pacientes: 10ms de triagem, 65ms atendimento, prioridade 3"
*/



/*NOTES
3.2
THE "config.txt" FILE CAN BE CHANGE MANUALLY IN THE PROGRAM
THE BEGGINING AND THE END OF EACH THREAD MUST BE WRITTEN IN THE LOG FILE
WRITE EVERY EVENT IN LOG FILE
TRIAGE --- ASSIGN THE PATIENT THE PRIORITY(IN THE DATA FROM PIPE), THIS OPERATION TAKES THE TIME WRITTEN IN THE DATA. AFTER ASSIGN THE PATIENT TO THE MESSAGES QUEUE TO RECEIVE ATTENDANCE FROM THE DOCTOR.
WHEN CONCLUDED WRITE TO LOG FILE


LOG FILE ---> MEMORY-MAPPED FILE


3.3
DOCTORS --> AMOUNT OF PROCCESSES, EACH PROCESS TREATS ONE PATIENT FROM THE QUEUE, RESPECTING THE PRIORITY DEFINED
WHEN ATTENDING, WRITE TO LOG FILE
WAITS THE PREDEFINED TIME IN THE DATA FROM PIPE AND THEN WRITE TO LOG AGAIN
REGIST THE STATS TO THE SHARED MEMORY (3.4)
EACH DOCTOR -- SHIFT_LENGTH
WHEN SHIFT ENDS THE DOCTOR FINISHES THE PATIENT AND LEAVES (FATHER PROCCESS DETECTS AND WRITE TO LOG AND WRITE TO LOG)
THE NUMBER OF DOCTORS (DEFINE) CAN'T CHANGE MANUALLY
TEMPORARY DOCTOR CAN BE CREATED IF THE QUEUE REACHES THE MQ_MAX AND ENDS AFTER THE QUEUE DOWNS TO 80% OF MQ_MAX

3.4
STATS IN SHARED MEMORY TO EVERY DOCTOR/TRIAGE ACCESS AND CHANGE
AFTER SIGUSR1 SIGNAL, FATHER PROCCESS WRITES THE TOTAL PATIENTS TRIAGE, ATTENDAND..., MEDIUM TIME BEFORE TRIAGE AND END OF TRIAGE AND BEGIN OF ATTENDANCE, AND MEDIUM TIME SINCE ARRIVE IN THE SYSTEM AND LEAVES

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
