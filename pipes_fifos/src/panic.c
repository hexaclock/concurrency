#include "assgn1.h"

/*
 *given a string and an int for error code
 *prints message and exits with error code
*/
void panic(char *s, unsigned int n)
{
  if (s != NULL)
    puts(s);
  exit(n);
}

