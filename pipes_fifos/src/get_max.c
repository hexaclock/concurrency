#include "assgn1.h"

int get_max(int *arr, unsigned int len)
{
  unsigned int i;
  int max;
  max = 0;

  for (i=0; i<len; ++i)
    if (arr[i] > max)
      max = arr[i];

  return max;
}
