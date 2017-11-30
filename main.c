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




void force_exit() {
	int i;
	/*if(getpid() == main_pid) {
		for (i = 0; i < triage; i++) {
			pthread_join(triage_threads[i], NULL);
		}
	}*/
	wait(NULL);
	shmctl(shmid, IPC_RMID, NULL);
	exit(0);
}

void* read_named_pipe () {
	fd_set read_set;
	//Creates the named pipe if it doesn't exist yet
	if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
		perror("Cannot create pipe: ");
		exit(0);
	}
	//Opens the pipe for reading
	int fd;
	if ((fd=open(PIPE_NAME, O_RDONLY)) < 0) {
		perror("Cannot open pipe for reading: ");
		exit(0);
	}
	ListP node;
	node = (ListP) malloc(sizeof(Patient));
	while (1) {
		read(fd, node, sizeof(Patient));
		printf("[SERVER] Received (name: %s,triage_time: %d ms,attendance_time: %d ms,priority: %d)\n", node->name, node->triagems, node->attendancems, node->priority);
	}
}

int main(int argc, char *argv[]) {




	/*ListP node;
	node = (ListP)malloc(sizeof(Patient));
	node->triageNum = 1;
	node->attendanceNum = 1;
	node->triagems = 0;
	node->attendancems = 0;
	node->priority = 1;
	node->start = 0;
	node->begin_triage = 0;
	node->begin_attendance = 0;
	node->next = NULL;
	patient_list = (ListP)malloc(sizeof(Patient));
	patient_list->triageNum = 1;
	patient_list->attendanceNum = 1;
	patient_list->triagems = 0;
	patient_list->attendancems = 0;
	patient_list->priority = 1;
	patient_list->start = 0;
	patient_list->begin_triage = 0;
	patient_list->begin_attendance = 0;
	patient_list->next = node;*/

	if (pthread_create(&triage_read_pipe, NULL, read_named_pipe, 0) != 0) {
		perror("Error creating the thread!");
		exit(0);
		return 1;
	}
	pthread_join(triage_read_pipe, NULL);



	printf("supamos%d\n", patient_list->next->triageNum);
	main_pid = getpid();
	if(read_config(&triage, &doctors, &shift_length, &mq_max)) {
		printf("%d, %d, %d, %d\n", triage, doctors, shift_length, mq_max);
	}
	else {
		perror("Error while reading the configuration file\n");
		return 0;
	}
	signal(SIGINT,force_exit);
	if(!(shmid = shared_memory_stats())) {
		perror("Error while creating shared memory\n");
		return 0;
	}
	create_triages(triage);
	create_doctors(doctors, shift_length);
	printf("\nTotal Triaged: %d\n", (*shared_var).total_triage);
	printf("\nTotal Treated: %d\n\n", (*shared_var).total_treated);


	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
