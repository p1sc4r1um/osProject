# osProject
Simple program in C to simulate hospital urgencies with multiple threads to triage the patients, multiple proccesses to treat the patients and synchronisation mechanisms to ensure the good program's control.

## How it works


## How to run
In terminal: 
```
./exe
```

To send clients (in other terminal):
```
./input_pipe <name> <triagems> <attendancems> <priority> 
or 
./input_pipe <number_of_people> <triagems> <attendancems> <priority>
```
## How to use
When patients are send through named pipe, main program receives it and put them in linked list that threads will triage by arriving order then send them to message queue and finally doctors treat them

### Print Stats
Stats can be viewed by sending SIGUSR1 signal (in other terminal)
```
kill -SIGUSR1 <pid>
```

### Exit the program
Exit the program waiting for medical appointments in course and close the program safely
```
Control+C
```
