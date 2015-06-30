#include <stdio.h>

typedef struct {
	pid_t pid;
	int topicCount, cmd, readpipe, writepipe;
} subscriber;
