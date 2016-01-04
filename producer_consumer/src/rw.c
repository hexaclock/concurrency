#include "assgn2.h"

/*
 *single-threaded*
 *given an input filename and an output filename
 *truncates output file
 *copies each line of input file to output file
*/
int main(int argc, char **argv)
{
  char *linebuf = NULL;
  FILE *in;
  FILE *out;
  size_t linelen = 0;
  ssize_t nread;

  if (argc != 3)
    {
      printf("usage: %s <inputfile> <outputfile>\n",argv[0]);
      return 2;
    }
  if ((in = fopen(argv[1],"r")) == NULL)
    {
      puts("Failed to open input file for reading");
      return -1;
    }
  if ((out = fopen(argv[2],"w")) == NULL)
    {
      puts("Failed to create/open output file for writing");
      return -1;
    }

  while ((nread = getline(&linebuf,&linelen,in)) != -1)
    fwrite(linebuf,1,nread,out);

  fclose(in);
  fclose(out);
  free(linebuf);
  return 0;
}
