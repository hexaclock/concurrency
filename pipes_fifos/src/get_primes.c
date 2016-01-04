#include "assgn1.h"

int get_primes(int bottom, int top, int fd)
{
  int i;
  int n;
  int numprimes;

  for (numprimes=0,i=bottom; i<=top; ++i)
    {
      if (is_prime(i))
	{
	  ++numprimes;
	  if ( (n=write(fd,(void *)&i,sizeof(i))) < 0 )
	    printf("Error sending prime to parent from %d\n",getpid());
	}
    }
  return numprimes;
}
