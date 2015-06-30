/*
	Jared Goddard
	Project 1 CIS415
	10/27/2014
	Queue code pulled from ww3.cs.stonybrook.edu/~skiena/392/programs/queue.c on 10/27/2014

*/

#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <proc/readproc.h>

#define MAX_LINE 256
#define MAX_LEN 256
#define QUEUESIZE 1000

typedef struct {
	pid_t q[QUEUESIZE+1];
	int first;
	int last;
	int count;
} queue;

queue q;
volatile int *sig_flag;

void init_queue(queue *q) {
	q->first = 0;
	q->last = QUEUESIZE-1;
	q->count = 0;
}

void enqueue(queue *q, int x) {
	if(q->count >= QUEUESIZE) {
		printf("Warning: queue overflow enqueue x=%d\n", x);
	} else {
		q->last = (q->last+1) % QUEUESIZE;
		q->q[ q->last ] = x;
		q->count = q->count + 1;
	}
	printf("Queue %d\n", q->q[q->last]);
}

pid_t dequeue(queue *q) {
	pid_t x;

	if(q->count <= 0) {
		printf("Warning: empty queue dequeue.\n");
	} else {
		x = q->q[ q->first ];
		q->first = (q->first+1) % QUEUESIZE;
		q->count = q->count-1;
	}
	printf("Dequeue %d\n", x);
	return x;
}

int empty(queue *q) {
	if(q->count <= 0) {
		return 1;
	} else {
		return 0;
	}
}

void print_queue(queue *q) {
	int i;
	i = q->first;

	while(i != q->last) {
		printf("%c ", q->q[i]);
		i = (i+1) % QUEUESIZE;
	}

	printf("%2d ", q->q[i]);
	printf("\n");
}

void signal_handler(int sig) {
	*sig_flag = 1;
}


void print_proc(pid_t pid[]) {
	PROCTAB* proc = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS | PROC_PID, pid);
	proc_t proc_info;
	memset(&proc_info, 0, sizeof(proc_info));
	while(readproc(proc, &proc_info) != NULL) {
		printf("%s \t%lu\n", proc_info.cmd, proc_info.size);
	}
	closeproc(proc);
}

int main(int argc, char *argv[]) {
	sig_flag = malloc(sizeof(int));
	*sig_flag = 1;
	signal(SIGALRM, signal_handler);
	sigset_t SIG_SET;
	sigemptyset(&SIG_SET);
	sigaddset(&SIG_SET, SIGUSR1);
	sigprocmask(SIG_BLOCK, &SIG_SET, NULL);
	FILE *f=fopen(argv[1], "r");
	char buffer[MAX_LEN][MAX_LEN];
	char line[MAX_LINE];
	int numPrograms=0;
	int i, len, sig;
	pid_t pid[numPrograms];
	wordexp_t p;
	wordexp_t wordArray[MAX_LEN];
	init_queue(&q);
	PROCTAB* proc;

	/*if(argc != 2) {
		printf("Input correct number of arguments\n");
	}*/

	if(f != NULL) {

		while(fgets(line, sizeof(line), f) != NULL) {
			len=strlen(line);
			if(line[len-1]=='\n') {
				line[len-1]='\0';
			}
			strcpy(buffer[numPrograms], line);
			wordexp(buffer[numPrograms], &p, 0);
			wordArray[numPrograms]=p;
			numPrograms++;
		}

		for(i=0; i<numPrograms; i++) {
			pid_t temp=fork();
			if(temp<0) {
				printf("Failed to fork");
				exit(1);
			}
			if(temp==0) { //child
				sigwait(&SIG_SET, &sig);
				//printf("in child process\n");
				if(execvp(wordArray[i].we_wordv[0], wordArray[i].we_wordv)==-1) {
					printf("Error executing program\n");
					exit(1);
				}
			} else if(temp>0) { //parent
				//printf("in parent\n");
				pid[i] = temp;
				enqueue(&q, temp);
			}
		}
		
		pid_t procPID[numPrograms+1];
		for(i=0; i<numPrograms; i++) {
			procPID[i] = pid[i];
		}
		procPID[numPrograms+1] = 0;

		for(i=0; i<numPrograms; i++) {
			kill(pid[i], SIGUSR1);
		}
		for(i=0; i<numPrograms; i++) {
			kill(pid[i], SIGSTOP);
		}
		/*for(i=0; i<numPrograms; i++) {
			kill(pid[i], SIGCONT);	
			//signal(SIGCONT, signal_handler);
		}*/
		pid_t temp = dequeue(&q);
		while(empty(&q) != 1) {
			if(*sig_flag==1) {
				int r;
				*sig_flag = 0;
				alarm(1);
				kill(temp, SIGSTOP);
				pid_t w = waitpid(temp, &r, WNOHANG);
				if(w==0) {
					enqueue(&q, temp);
				}
				temp = dequeue(&q);
				kill(temp, SIGCONT);
				print_proc(procPID);
			}
		}
		for(i=0; i<numPrograms; i++) {
			int r;
			wordexp_t w = wordArray[i];
			pid_t returnPID = waitpid(pid[i], &r, 0);
			if(returnPID==-1) {
				printf("No child process\n");
			} else {
				wordfree(&w);
			}
		}
	}
	fclose(f);
	return 0;
}
