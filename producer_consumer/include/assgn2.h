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

struct filemisc
{
  int fillsleep;
  int drainsleep;
  char *filebuf;
  char *linebuf;
  FILE *in, *out;
};
typedef struct filemisc filemisc_t;

#define FBUFSIZ 64

#endif
