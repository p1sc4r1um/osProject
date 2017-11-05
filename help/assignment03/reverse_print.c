#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
	int status;
	pid_t mypid;
	pid_t childpid;
	int num=4;
	pid_t shellpid = getppid();
	for(int i=num;i>0;i--) 	{
		childpid = fork();
		
		if(childpid !=0) {
			sleep(i);
        		printf("Eu sou o processo %ld e ",(long)getpid());
			mypid = getppid();
			if(mypid != shellpid) {
        		printf("eu sou o processo pai %ld\n",(long)getppid());
			break;
			}
			else {
				printf("eu sou o processo original");
				break;
			}
			}
	}
	return 0;

}
