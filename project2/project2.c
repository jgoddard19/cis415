#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>
//#include "queue.h"
#include "publisher.h"
#include "subscriber.h"
//#include "sublist.h"
//#include "di.h"
#define MAX_VAL 256
#define MAX_ENTRIES 1000

pid_t pid;
int fd[2], fd1[2], fd2[2], fd3[2];
char *subconnect;
char *pubconnect;
char *subaccept;
char buf[MAX_VAL];

//subconnect
//subread
//openpipes

void openpipes() {
	pipe(fd);
	pipe(fd1);
	pipe(fd2);
	pipe(fd3);
}

void connectSub() {
	int len = strlen(subconnect)+1;
	if(close(fd[0]) == -1) {
		exit(0);
	}
	if(write(fd[1], subconnect, len) == -1) {
		exit(1);	
	}
	if(close(fd[1]) == -1) {
		exit(1);
	}
}

void readSub() {
	char *msg = "SERVER sub accept";
	read(fd[0], buf, MAX_VAL);
	int num = strcmp(buf, "sub connect");
	if(num == 0) {
		printf("SUB server read \"%s\"\n", buf);
		write(fd1[1], msg, MAX_VAL);
	}
}

int main(int argc, char *argv[]) {
	int i, n, m, t, topic;
	unsigned char *str;
	subconnect = malloc(MAX_VAL);
	subconnect = "sub connect";
	subaccept = malloc(MAX_VAL);
	subaccept = "SERVER sub accept";
//	struct sublist *sublist;
//	init_queue(&q);
	
	printf("Enter number of publisher processes:");
	scanf("%d", &n); //n = number of publisher processes
	printf("Enter number of subscriber processes:");
	scanf("%d", &m); //m = number of subscriber processes
	printf("Enter number of topics:");
	scanf("%d", &topic); //topic = number of topics
	printf("Ints are: %d and %d, topic count is: %d\n", n, m, topic);

	publisher *pub;
	pub = malloc(sizeof(publisher));
	subscriber *sub;
	sub = malloc(sizeof(subscriber));

	openpipes();

	if(pub == NULL || sub == NULL) {
		perror("pub or sub is null\n");
		exit(1);
	}

//	publisher pubarray[n];

	for(i = 0; i < m; i++) { //subscriber
		printf("Enter the number of topics this subscriber is interested in: \n");
		scanf("%d", &sub->topicCount);
		t = sub->topicCount;
		printf("Number of topics for subscriber %d is %d\n", i, t);
		t = *str-0;
		pid = fork();
		sub->pid = pid;
//		sublist->pushToBack(sub);
//		printf("pid of subscriber %d is %d\n", i, sub->pid);
	
		if(pid == -1) {
			perror("fork error\n");
			exit(1);
			break;
		}
		if(pid == 0) {
			printf("SUB in child\n");
			connectSub();
			read(fd1[0], buf, MAX_VAL);
			int num = strcmp(buf, "SERVER sub accept");
			if(num != 0) {
				printf("Server rejected connection");
				exit(0);
			}
			//sprintf(c, "%d", t);
			if(close(fd2[0]) == -1) {
				exit(0);
			}
			if(write(fd2[1], &t, MAX_VAL) == -1) {
				printf("Writing to server\n");
				exit(1);
			}
			if(close(fd2[1]) == -1) {
				exit(1);
			}
			exit(0);
		} else {
			printf("SERVER sub in server\n");
			readSub();
			read(fd2[0], buf, MAX_VAL);
			printf("SERVER received: %s\n", buf);
		}
	}

	/*for(i = 0; i < n; i++) { //publisher
		pubarray[i] = *pub;
		pid = fork();
		pub->pid = pid;
//		printf("pid of publisher %d is %d\n", i, pubarray[i].pid);
			
		if(pid == -1) {
			perror("fork error\n");
			exit(1);
			break;
		}
		if(pid == 0) {
			printf("Child process: writing to the pipe\n");
//			write(fd2[1], "test", 5);
			exit(0);
		} else {
			printf("Parent process: reading from the pipe\n");
			read(fd2[0], buf, 5);
//			printf("Parent: read \"%s\"\n", buf);
//			wait(NULL);
		}
	}*/

	return 0;
}
