#include "assgn2.h"
#include "cbuf.h"

sem_t mutex;

void *filler(void *fm)
{
  char quitmsg[] = "QUIT";
  filemisc_t *fmisc = (filemisc_t *)fm;
  size_t linelen;
  ssize_t nread;
  linelen = 0;

  while ((nread = getline( &(fmisc->linebuf), &linelen, fmisc->in)) != -1)
    {
      usleep(fmisc->fillsleep);
      /*if can't fit a single line longer than buffer*/
      if (nread >= CBUF_CAPACITY)
	{
	  puts("fill thread: Current line too long for the buffer.");
	  return (void *)-2;
	}
      if (nread > cbuf_space_available())
	{
	  printf("fill thread: Could not write [%s] -- not enough space\n", fmisc->linebuf);
	  while (nread > cbuf_space_available());
	}
      sem_wait(&mutex);
      /*begin critical section*/
      cbuf_copy_in(fmisc->linebuf);
      /*end critical section*/
      sem_post(&mutex);
      printf("fill thread: wrote [%s] into buffer (nwritten=%d)\n",fmisc->linebuf,(int)nread+1);
    }

  if (sizeof(quitmsg) > cbuf_space_available())
    {
      printf("fill thread: Could not write [%s] -- not enough space\n", quitmsg);
      while (sizeof(quitmsg) > cbuf_space_available());
    }
  
  sem_wait(&mutex);
  /*begin critical section*/
  cbuf_copy_in(quitmsg);
  /*end critical section*/
  sem_post(&mutex);
  printf("fill thread: wrote [%s] into buffer (nwritten=%d)\n",quitmsg,(int)sizeof(quitmsg));

  return (void *)0;
}

void *drainer(void *fm)
{
  char line[CBUF_CAPACITY];
  int linelen;
  filemisc_t *fmisc = (filemisc_t *)fm;
  
  while (1)
    {
      usleep(fmisc->drainsleep);
      if (!cbuf_data_is_available())
	{
	  printf("drain thread: no new string in buffer\n");
	  while (!cbuf_data_is_available());
	}
      
      sem_wait(&mutex);
      /*begin critical section*/
      linelen = cbuf_copy_out(line);
      if (!strncmp(line,"QUIT",4))
	{
	  sem_post(&mutex);
	  printf("drain thread: read [%s] from buffer (nread=%d)\n",line,(int)linelen);
	  break;
	}
      fwrite(line,sizeof(char),linelen-1,fmisc->out);
      /*end critical section*/
      sem_post(&mutex);
      printf("drain thread: read [%s] from buffer (nread=%d)\n",line,(int)linelen);
    }
  return (void *)0;
}

/*
 *two threads, one semaphore*
 *given an input filename and an output filename
 *truncates output file
 *copies each line of input file to output file
*/
int main(int argc, char **argv)
{
  pthread_t fill,drain;
  int fillret, drainret;
  filemisc_t fm;
  setvbuf(stdout, NULL, _IOLBF,0);

  if (argc != 5)
    {
      printf("usage: %s <inputfile> <outputfile> <fill usleep time> <drain usleep time>\n",argv[0]);
      return 2;
    }
  if ((fm.in = fopen(argv[1],"r")) == NULL)
    {
      puts("Failed to open input file for reading");
      return -1;
    }
  if ((fm.out = fopen(argv[2],"w")) == NULL)
    {
      puts("Failed to create/open output file for writing");
      return -1;
    }
  fm.fillsleep = atoi(argv[3]);
  fm.drainsleep = atoi(argv[4]);
  if (fm.fillsleep < 0 || fm.drainsleep < 0)
    {
      puts("Please set a valid, >=0 sleep time in microseconds.");
      return 2;
    }
  fm.linebuf = NULL;
  fm.filebuf = (char *)calloc(FBUFSIZ,sizeof(char));
  if (fm.filebuf == NULL)
    {
      puts("Failed to allocate memory for filebuf");
      return -1;
    }

  cbuf_init();
  sem_init(&mutex,0,1);
  fillret = pthread_create(&fill, NULL, &filler, (void *)&fm);
  drainret = pthread_create(&drain,NULL,&drainer,(void *)&fm);
  pthread_join(fill,NULL);
  pthread_join(drain,NULL);

  if (fillret != 0)
    {
      puts("Fill thread failed");
      return -2;
    }
  if (drainret != 0)
    {
      puts("Drain thread failed");
      return -3;
    }
  
  sem_destroy(&mutex);
  cbuf_terminate();

  fclose(fm.in);
  fclose(fm.out);
  free(fm.linebuf);
  free(fm.filebuf);
  return 0;
}
