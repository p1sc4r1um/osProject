#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

char* decrypt (char * word) {
	char str[999],str2[999];
	int c,cont=0;
	int i=0,j=0;
	FILE * file2;
        if (file2=fopen("decrypt.txt","r")) {
	        while (!feof(file2)) {
        	        if (fgets(str2,sizeof(str2),file2)) {
				cont=0;
                	        for (c=0;c<strlen(str2)) {
			                if(line[c] == ' ') {
        		                	cont++;
                			}
               				 else if (cont == 1 && line[c] != ' ' && line[c]!= '\0' && line[c] !='\n') {
                        			str[i] = line[c];
                        			i++;
                			}
                			else{
                        			str2[j] = line[c];
                        			j++;
                			}
				}
				if (!strcmp(str,word)) {
					return str2;
				}
                       	 }
		}
		fclose(file2);
		return " ";
	}
}

int main(void) {
	char str[999],str2[999],word[999];
	FILE * file;
	pid_t mypid,childpid;
	int n=0;
	if (file=fopen("text.txt","r")) {
		while (!feof(file)) {
			childpid = fork();
			if (childpid==0 && fgets(str,sizeof(str),file)) {
				for (c=0;c<strlen(str)) {
					if (line[c] != ' ') {
						str2[i] = line[c];
					}
					else if(line[c] == ' ') {
						word = decrypt(str2);
						if (!strcmp(word," ")) {
                                                	
						}
						str2="";
					}
				}
				break;
			}
		}
		fclose(file);
	}
}
