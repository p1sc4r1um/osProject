#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PIPE_NAME   "input_pipe"
#define RAND_MAX 149

typedef struct patient* ListP;
typedef struct patient {
	char name [50];
	int triageNum, attendanceNum, triagems, attendancems, priority;
	clock_t start, begin_triage, begin_attendance;
}Patient;

int is_number(char *string) {
	int i;
	for(i = 0; i<strlen(string); i++) {
		if(!isdigit(string[i])) {
			return 0;
		}
	}
	return 1;
}

int main(int argc, char *argv[]) {
	int i;
	srand(time(NULL));
	ListP node;
	if(argc != 5) {
		perror ("usage: <first_name/numer_of_pacients> <triage_time_ms> <attendance_time_ms> <priority>");
		exit(1);
	}
	// Opens the pipe for writing
	int fd;
	if ((fd=open(PIPE_NAME, O_WRONLY)) < 0) {
		perror("Cannot open pipe for writing: ");
		exit(1);
	}

	char names[150][20] = {"Laura", "Libbie", "Kent", "Susanne", "Adriana", "Dawn", "Teressa", "Linsey", "Carlena", "Buster", "Alvaro", "Charlie", "Teisha", "Brenton", "Ona", "Rosendo", "Lieselotte", "Jessika", "Cassi", "Ardath", "Clemencia", "Bette", "Pearl", "Hailey", "Charlette", "Malorie", "Miki", "Joeann", "Katy", "Filomena", "Jacquiline", "Elizebeth", "Idella", "Domitila", "Yessenia", "Loreen", "Douglass", "Porter", "Lu", "Gabriele", "Brigida", "Macy", "Sherry", "Dylan", "Shaunte", "Karma", "Merrill", "Britteny", "Jerri", "Shantelle", "Sade", "Dayna", "Doyle", "Rasheeda", "Rocky", "Lashandra", "Mariann", "Kent", "Keneth", "Theda", "Terese", "Elena", "Hortensia", "Terina", "Deonna", "Retta", "Ginette", "Lizeth", "Jayna", "Grant", "Will", "Doria", "Suanne", "Leontine", "Gerard", "Hertha", "Mackenzie", "Melva", "Tobi", "Kaitlyn", "Napoleon", "Mozella", "Gene", "Donette", "Helaine", "Marie", "Leonore", "Hassan", "Joie", "Kristal", "Mira", "Xiao", "Melodie", "Felicitas", "Calandra", "Neta", "Penni", "Roni", "Abbey", "Luna", "Carylon", "Codi", "Roberto", "Rosio", "Myrna", "Kristel", "Eveline", "Anglea", "Sebrina", "Nerissa", "Milan", "Lilli", "Verna", "Lawanna", "Whitney", "Teddy", "Wonda", "Malissa", "Nguyet", "Nenita", "Yael", "Lurline", "Isabella", "Leatha", "Kira", "Caitlyn", "Elba", "Gala", "Deanne", "Ranee", "Romeo", "Misty", "Claudio", "Ivory", "Guillermina", "Misha", "Verline", "Johnette", "Marcy", "Marylee", "Myrtis", "Barbar", "Georgia", "Alethea", "Karri", "Carmen", "Malorie", "Reba", "Lovie", "Donnette"};

	node = (ListP) malloc(sizeof(Patient));

	if(is_number(argv[1])) {

		for(i = 0; i < atoi(argv[1]); i++) {

			strcpy(node->name, names[rand()]);
			node->triagems = atoi(argv[2]);
			node->attendancems = atoi(argv[3]);
			node->priority = atoi(argv[4]);
			printf("[PIPE] Sending (name: %s,triage_time: %d ms,attendance_time: %d ms,priority: %d)\n", node->name, node->triagems, node->attendancems, node->priority);
			write(fd, node, sizeof(Patient));
		}
	}
	else {
		strcpy(node->name, argv[1]);
		node->triagems = atoi(argv[2]);
		node->attendancems = atoi(argv[3]);
		node->priority = atoi(argv[4]);
		printf("[PIPE] Sending (name: %s,triage_time: %d ms,attendance_time: %d ms,priority: %d)\n", node->name, node->triagems, node->attendancems, node->priority);
		write(fd, node, sizeof(Patient));
	}
	close(fd);
	return 0;
}
