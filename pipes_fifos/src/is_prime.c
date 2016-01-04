#include "assgn1.h"

char is_prime(int n)
{
  int i;
  int maxm;
  maxm = sqrt(n);
  for (i=2;i<=maxm;++i)
    {
      if (n % i == 0)
	return 0;
    }
  return 1;
}
