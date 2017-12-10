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

#define PIPE_NAME "input_pipe"
#define RAND_MAX 149

typedef struct patient* ListP;
typedef struct patient {
	char name [50];
	int triageNum, attendanceNum, triagems, attendancems, priority;
	double start, begin_triage, begin_attendance;
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
	if(argc != 5 && argc != 2) {
		perror ("usage: <first_name/numer_of_pacients> <triage_time_ms> <attendance_time_ms> <priority> or usage: <new_threads_number");
		exit(1);
	}
	else if (atoi(argv[1]) < 1) {
		perror("\033[91mnumber of thread must be greater than 0\n")
		exit(1);
	}
	// Opens the pipe for writing
	int fd;
	if ((fd=open(PIPE_NAME, O_WRONLY)) < 0) {
		perror("\033[91mCannot open pipe for writing: ");
		exit(1);
	}
	if(argc == 5) {
		char names[150][20] = {"Laura\0", "Libbie\0", "Kent\0", "Susanne\0", "Adriana\0", "Dawn\0", "Teressa\0", "Linsey\0", "Carlena\0", "Buster\0", "Alvaro\0", "Charlie\0", "Teisha\0", "Brenton\0", "Ona\0", "Rosendo\0", "Lieselotte\0", "Jessika\0", "Cassi\0", "Ardath\0", "Clemencia\0", "Bette\0", "Pearl\0", "Hailey\0", "Charlette\0", "Malorie\0", "Miki\0", "Joeann\0", "Katy\0", "Filomena\0", "Jacquiline\0", "Elizebeth\0", "Idella\0", "Domitila\0", "Yessenia\0", "Loreen\0", "Douglass\0", "Porter\0", "Lu\0", "Gabriele\0", "Brigida\0", "Macy\0", "Sherry\0", "Dylan\0", "Shaunte\0", "Karma\0", "Merrill\0", "Britteny\0", "Jerri\0", "Shantelle\0", "Sade\0", "Dayna\0", "Doyle\0", "Rasheeda\0", "Rocky\0", "Lashandra\0", "Mariann\0", "Kent\0", "Keneth\0", "Theda\0", "Terese\0", "Elena\0", "Hortensia\0", "Terina\0", "Deonna\0", "Retta\0", "Ginette\0", "Lizeth\0", "Jayna\0", "Grant\0", "Will\0", "Doria\0", "Suanne\0", "Leontine\0", "Gerard\0", "Hertha\0", "Mackenzie\0", "Melva\0", "Tobi\0", "Kaitlyn\0", "Napoleon\0", "Mozella\0", "Gene\0", "Donette\0", "Helaine\0", "Marie\0", "Leonore\0", "Hassan\0", "Joie\0", "Kristal\0", "Mira\0", "Xiao\0", "Melodie\0", "Felicitas\0", "Calandra\0", "Neta\0", "Penni\0", "Roni\0", "Abbey\0", "Luna\0", "Carylon\0", "Codi\0", "Roberto\0", "Rosio\0", "Myrna\0", "Kristel\0", "Eveline\0", "Anglea\0", "Sebrina\0", "Nerissa\0", "Milan\0", "Lilli\0", "Verna\0", "Lawanna\0", "Whitney\0", "Teddy\0", "Wonda\0", "Malissa\0", "Nguyet\0", "Nenita\0", "Yael\0", "Lurline\0", "Isabella\0", "Leatha\0", "Kira\0", "Caitlyn\0", "Elba\0", "Gala\0", "Deanne\0", "Ranee\0", "Romeo\0", "Misty\0", "Claudio\0", "Ivory\0", "Guillermina\0", "Misha\0", "Verline\0", "Johnette\0", "Marcy\0", "Marylee\0", "Myrtis\0", "Barbar\0", "Georgia\0", "Alethea\0", "Karri\0", "Carmen\0", "Malorie\0", "Reba\0", "Lovie\0", "Donnette\0"
	};

		node = (ListP) malloc(sizeof(Patient));
		char person[20];
		if(is_number(argv[1])) {

			for(i = 0; i < atoi(argv[1]); i++) {
				strcpy(person, names[rand()%RAND_MAX]);
				strcpy(node->name, person);
				printf("%s\n", node->name);
				node->triagems = atoi(argv[2]);
				node->attendancems = atoi(argv[3]);
				node->priority = atoi(argv[4]);
				printf("[PIPE] Sending (name: %s,triage_time: %d ms,attendance_time: %d ms,priority: %d)\n", node->name, node->triagems, node->attendancems, node->priority);
				write(fd, node, sizeof(Patient));
				sleep(1);
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
	}
	else if(argc == 2) {
		node = (ListP) malloc(sizeof(Patient));

		strcpy(node->name, "thread123");
		node->triagems = atoi(argv[1]);
		node->attendancems = -1;
		node->priority = -1;
		write(fd, node, sizeof(Patient));
	}
	close(fd);
	return 0;
}
