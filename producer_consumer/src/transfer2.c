#include "assgn2.h"
#include "bettercbuf.h"

sem_t spaces,items;
sem_t mutex;

void *filler(void *fm)
{
  char *copyme;
  char quitmsg[] = "QUIT";
  filemisc_t *fmisc = (filemisc_t *)fm;
  size_t linelen;
  ssize_t nread;
  linelen = 0;

  while (1)
    {
      usleep(fmisc->fillsleep);
      nread = getline( &(fmisc->linebuf), &linelen, fmisc->in);
      /*if can't fit a single line longer than buffer*/
      if (nread >= ENTRYMAX)
	{
	  puts("fill thread: Current line too long.");
	  return (void *)-2;
	}
      else
	{
	  if (nread == -1)
	    copyme = quitmsg;
	  else
	    copyme = fmisc->linebuf;
	  sem_wait(&spaces);
	  sem_wait(&mutex);
	  /*begin critical section*/
	  cbuf_copy_in(copyme);
	  /*end critical section*/
	  sem_post(&mutex);
	  sem_post(&items);
	  if (nread != -1)
	    printf("fill thread: wrote [%s] into buffer (nwritten=%d)\n",
		   fmisc->linebuf,(int)nread+1);
	  if (nread == -1)
	    {
	      printf("fill thread: wrote [%s] into buffer (nwritten=%d)\n",
		     quitmsg,(int)sizeof(quitmsg));
	      break;
	    }
	}
    }
  return (void *)0;
}

void *drainer(void *fm)
{
  char line[ENTRYMAX];
  char timetodie;
  int linelen;
  filemisc_t *fmisc = (filemisc_t *)fm;
  timetodie = 0;

  while (1)
    {
      usleep(fmisc->drainsleep);
      sem_wait(&items);
      sem_wait(&mutex);
      /*begin critical section*/
      cbuf_copy_out(line);
      /*end critical section*/
      sem_post(&mutex);
      sem_post(&spaces);
      linelen = strlen(line);
      if (!strncmp(line,"QUIT",4))
	timetodie = 1;
      if (!timetodie)
	fwrite(line,sizeof(char),linelen,fmisc->out);
      printf("drain thread: read [%s] from buffer (nread=%d)\n",line,linelen+1);
      if (timetodie)
	break;
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
  sem_init(&spaces,0,cbuf_space_available());
  sem_init(&items,0,0);

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
  sem_destroy(&items);
  sem_destroy(&mutex);
  cbuf_terminate();

  fclose(fm.in);
  fclose(fm.out);
  free(fm.linebuf);
  free(fm.filebuf);
  return 0;
}
