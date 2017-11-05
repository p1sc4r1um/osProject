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
        for(int i=num;i>0;i--)  {
                childpid = fork();

                if(childpid !=0) {


		}
	}
	return 0;
}
