#ifndef _ASSGN1_H_
#define _ASSGN1_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
int main(int argc, char **argv);
char is_prime(int n);
int get_primes(int bottom, int top, int fd);
int get_max(int *arr, unsigned int len);
void panic(char *s, unsigned int n);

#endif
