# Process-Scheduler
University Project
Implementation of Process Scheduler using, FCFS, SJF, Round Robin and Prio in C. Software Systems University Project 2022-2023. 
The project used the above functions:
execl: for execting files.
signals: SIGCHLD (for capturing the finished process), SIGSTOP AND SIGCONT (for stoping and starting a process).
nanosleep: The quantum of the RR and PRIO

#How to run:
./scheduler [method] [quantum(optional)] [file name]
#Usage:
1. Go to work file and make the Makefile. Produces 7 executable files from work.c, with different running times.
2. Then go to scheduler file and compile scheduler.c gcc scheduler.c -o scheduler.
3. Test the program with ./run.sh for some possible cases.

#PS
The project was implemented as a part of a university project. I claim no right.
