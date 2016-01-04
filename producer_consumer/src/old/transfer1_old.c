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
      if (nread >= FBUFSIZ)
	{
	  puts("fill thread: Current line too long for the buffer.");
	  return (void *)-2;
	}
      if (fmisc->track+nread >= FBUFSIZ)
	{
	  printf("fill thread: Could not write [%s] -- not enough space\n", fmisc->linebuf);
	  while (fmisc->track+nread >= FBUFSIZ);
	}
      /*begin critical section*/
      sem_wait(&mutex);
      strcat(fmisc->filebuf,fmisc->linebuf);
      fmisc->track += nread;
      printf("fill thread: wrote [%s] into buffer (nwritten=%d)\n",fmisc->linebuf,(int)nread+1);
      sem_post(&mutex);
      /*end critical section*/
    }

  if (fmisc->track+sizeof(quitmsg) >= FBUFSIZ)
    {
      printf("fill thread: Could not write [%s] -- not enough space\n", fmisc->linebuf);
      while ( (fmisc->track+sizeof(quitmsg)) >= FBUFSIZ);
    }

  /*begin critical section*/
  sem_wait(&mutex);
  strcat(fmisc->filebuf,quitmsg);
  fmisc->track += sizeof(quitmsg);
  printf("fill thread: wrote [%s] into buffer (nwritten=%d)\n",quitmsg,(int)sizeof(quitmsg));
  sem_post(&mutex);
  /*end critical section*/

  return (void *)0;
}

void *drainer(void *fm)
{
  /*need circular buffer for nicer line-by-line reading*/
  filemisc_t *fmisc = (filemisc_t *)fm;
  
  while (1)
    {
      usleep(fmisc->drainsleep);
      if (fmisc->track == 0)
	{
	  printf("drain thread: no new string in buffer\n");
	  while (fmisc->track == 0);
	}
      /*begin critical section*/
      sem_wait(&mutex);
      printf("drain thread: read [%s] from buffer (nread=%d)\n",fmisc->filebuf,
	     (int)strlen(fmisc->linebuf)+1);
      if (strstr(fmisc->filebuf,"QUIT") != NULL)
	{
	  fwrite(fmisc->filebuf,sizeof(char),fmisc->track-5,fmisc->out);
	  sem_post(&mutex);
	  break;
	}
      fwrite(fmisc->filebuf,sizeof(char),fmisc->track,fmisc->out);
      memset(fmisc->filebuf,0,FBUFSIZ*sizeof(char));
      fmisc->track = 0;
      sem_post(&mutex);
      /*end critical section*/
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
  fm.track = 0;

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
  fm.track = 0;
  fm.filebuf = (char *)calloc(FBUFSIZ,sizeof(char));
  if (fm.filebuf == NULL)
    {
      puts("Failed to allocate memory for filebuf");
      return -1;
    }


  sem_init(&mutex,0,1);
  fillret = pthread_create(&fill, NULL, &filler, (void *)&fm);
  drainret = pthread_create(&drain,NULL,&drainer,(void *)&fm);
  pthread_join(fill,NULL);
  pthread_join(drain,NULL);
  sem_destroy(&mutex);

  /*fwrite(fm.filebuf,1,fm.track,fm.out);*/

  fclose(fm.in);
  fclose(fm.out);
  free(fm.linebuf);
  free(fm.filebuf);
  return 0;
}
