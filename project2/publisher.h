#include <stdio.h>

typedef struct {
	pid_t pid;
	int topic, cmd, readpipe, writepipe;
} publisher;
