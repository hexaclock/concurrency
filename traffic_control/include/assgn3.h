#ifndef _ASSGN2_H_
#define _ASSGN2_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/queue.h>
#include "cart.h"
#include "q.h"
#include "monitor.h"

void *north(void *arg);
void *south(void *arg);
void *east(void *arg);
void *west(void *arg);
int main(int argc, char **argv);

#endif
