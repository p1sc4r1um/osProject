#include "header.h"


int read_config(int *triage,int *doctors,int *shift_length,int *mq_max){
  FILE *f;
  int i, j = 0, choose = 1;
  bool bool1 = false;
  char value[100];
  if((f = fopen("config.txt","r"))) {
    char line[255];
    while(fgets(line,255,(FILE*) f)) {
      for(i = 0; i < strlen(line); i++) {
        if(bool1 == 1){
          value[j] = line[i];
          j++;
        }
        if(line[i] == '=') {
          bool1 = 1;
        }
      }
      switch(choose){
        case(1):
          *triage = atoi(value);
          triage_threads =(pthread_t*) malloc(*triage * sizeof(pthread_t));
          break;
        case(2):
          *doctors = atoi(value);
          break;
        case(3):
          *shift_length = atoi(value);
          break;
        case(4):
          *mq_max= atoi(value);
          break;
      }
      choose++;
      j = 0;
      bool1 = 0;
    }
    #ifdef DEBUG
    printf("##################### DEBUG #######################\n");
    printf("READINFO: triage: %ls, doctors: %ls, shift_length: %ls, mq_max: %ls\n",triage,doctors,shift_length,mq_max);
    printf("################## DEBUG END ###################\n");
    #endif
	fclose(f);
	return 1;
  }
  else {
    	perror("Error while reading the configuration file\n");
	return 0;
  }
}
