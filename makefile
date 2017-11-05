############################# Makefile ##########################
CC = gcc
CFLAGS =	-pthread -D_REENTRANT -Wall -g
all:	 main.o proccess_doctors.o read_config.o shared_memory_stats.o threads_triage.o header.h

	$(CC)	$(CFLAGS)	-o	exe main.o proccess_doctors.o read_config.o shared_memory_stats.o threads_triage.o
main.o: main.c header.h
	$(CC) $(CFLAGS) -c main.c
#menu.o: menu.c header.h
#	        $(CC) $(CFLAGS) -c menu.c
proccess_doctors.o: proccess_doctors.c header.h
	$(CC) $(CFLAGS) -c proccess_doctors.c
read_config.o: read_config.c header.h
	$(CC) $(CFLAGS) -c read_config.c
shared_memory_stats.o: shared_memory_stats.c header.h
	$(CC) $(CFLAGS) -c shared_memory_stats.c
threads_triage.o: threads_triage.c header.h
	$(CC) $(CFLAGS) -c threads_triage.c
clean:	
	rm -rf *.o

mrproper:	clean
		rm -rf header
