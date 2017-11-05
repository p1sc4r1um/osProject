#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
	int status;	
	pid_t mypid;
	pid_t childpid;
	mypid = getpid();
	childpid = fork();
	if(childpid==0) {
//		sleep(2);
		printf("child mypid : %ld\n",(long)mypid); //pid
		printf("child getpid() :%ld\n",(long)getpid()); //pid do filho
		printf("child getppid() :%ld\n",(long)getppid()); //supostamente igual ao pai
	}
	else {
		//sleep(2);
                printf("mypid : %ld\n",(long)mypid); // pid = acima
                printf("getpid() :%ld\n",(long)getpid()); // pid do pai
                printf("getppid() :%ld\n",(long)getppid()); // pid do pai do pai (shell neste caso)
		wait(&status);
	}
	return 0;
}
