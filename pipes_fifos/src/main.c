#include "assgn1.h"

int main(int argc, char **argv)
{
  int i,j,k,x;
  int numprimes;
  int bottom, top;
  int *statarr;
  int prime;
  int n;
  int selact;
  int *readfdarr;
  int **pipefdarr;
  char **fifoarr;
  char *fifoname;
  char *pidstr;
  fd_set fds;
  int *pids;
  int tempfd;
  tempfd = 0;
  numprimes = 0;

  /*remove printf/stdout buffer*/
  setvbuf(stdout, NULL, _IONBF, 0);

  if (argc < 2)
    {
      printf("usage: %s <increasing positive integers>\n",argv[0]);
      exit(-1);
    }

  /*allocate space for array of pipes*/
  pipefdarr = (int **)malloc(argc*sizeof(int *));
  for (i=0; i<argc-1; ++i)
    pipefdarr[i] = (int *)malloc(2*sizeof(int));

  /*allocate space for array of fifo fds*/
  fifoarr = (char **)malloc(argc*sizeof(char *));
  for (i=0; i<argc; ++i)
    fifoarr[i] = (char *)malloc(256*sizeof(char *));

  /*allocate space for fifo filename*/
  fifoname = (char *)malloc(255*sizeof(char));
  pidstr = (char *)malloc(15*sizeof(char));

  pids = (int *)malloc(argc*sizeof(int));
  statarr = (int *)malloc(argc*sizeof(int));
  readfdarr = (int *)calloc(argc,sizeof(int));

  FD_ZERO(&fds);

  for (i=1; i<argc; ++i)
    {
      /*setup pipes*/
      if (i % 2 != 0)
	{
	  if (pipe(pipefdarr[i-1]) < 0)
	    panic("Failed to create pipe!",-3);
	  readfdarr[i-1] = pipefdarr[i-1][0];
	}
      /*setup fifos*/
      else if (i % 2 == 0)
	{
	  sprintf(pidstr,"%d",i-1);
	  strcpy(fifoname,"./cs511_dv_fifo_");
	  strcat(fifoname,pidstr);
	  if ( mkfifo(fifoname,0600) < 0 )
	    panic("Failed to create FIFO!",-3);
	  strcpy(fifoarr[i-1],fifoname);
	  readfdarr[i-1] = open(fifoarr[i-1],O_RDONLY|O_NONBLOCK);
	  if (readfdarr[i-1] < 0)
	    {
	      puts(strerror(errno));
	      panic("Failed to open FIFO for writing",-5);
	    }
	}
    }
  free(pidstr);
  free(fifoname);
  /*forking loop*/
  for (i=1; i<argc; ++i)
    {
      /*failure case*/
      if ( (pids[i-1]=fork()) < 0 )
	panic("Failed to fork child process",-1);

      /*parent*/
      else if (pids[i-1] > 0)
	{
	  /*fork all processes first*/
	  if (i != argc-1)
	    continue;

	  /* wait for procsses to finish */
	  for (j=1; j<argc; ++j)
	    waitpid(pids[j-1],&(statarr[j-1]), 0);

	  for (j=1; j<argc; ++j)
	    {
	      /*waitpid(pids[j-1],&(statarr[j-1]), 0);*/
	      /*setup for select*/
	      FD_ZERO(&fds);
	      for (k=1; k<argc; ++k)
		FD_SET(readfdarr[k-1],&fds);
	      selact = select(get_max(readfdarr,argc)+1,&fds,NULL,NULL,NULL);
	      /*if select returns*/
	      if (selact>0)
		{
		  for (x=1; x<argc; ++x)
		    {
		      if ( (FD_ISSET(readfdarr[x-1],&fds)) && WIFEXITED(statarr[x-1]) )
			{
			  for (numprimes=0; numprimes!=WEXITSTATUS(statarr[x-1]); ++numprimes)
			    {
			      n = read(readfdarr[x-1],&prime,sizeof(int));
			      if (n > 0)
				{
				  printf("%d is prime\n",prime);
				}
			    }
			  
			  if (numprimes == WEXITSTATUS(statarr[x-1]))
			    printf("child %d exited correctly\n",pids[x-1]);
			  
			  else
			    printf("child %d did NOT exit correctly\n",pids[x-1]);
			}
		      /*child did not find any primes*/
		      else if ( (!FD_ISSET(readfdarr[x-1],&fds)) && WIFEXITED(statarr[x-1])
				&& WEXITSTATUS(statarr[x-1] == 0) )
			printf("child %d exited correctly\n",pids[x-1]);
		    }
		}
	      /*close fd and unlink FIFO if applicable*/
	      close(readfdarr[j-1]);
	      if (j%2 == 0)
		if (unlink(fifoarr[j-1]) < 0)
		  perror(strerror(errno));
	    }
	}
      
      /*child*/
      else
	{
	  if (i==1)
	    bottom = 2;
	  else
	    bottom = atoi(argv[i-1])+1;
	  top = atoi(argv[i]);

	  printf("child %d: bottom=%d, top=%d\n",getpid(),bottom,top);

	  if (i%2 != 0)
	    {
	      close(pipefdarr[i-1][0]);
	      tempfd = pipefdarr[i-1][1];
	    }
	  else if (i%2 == 0)
	    {
	      if ( (tempfd = open(fifoarr[i-1],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR)) < 0 )
		panic("Failed to open FIFO for writing",-5);
	    }

	  numprimes = get_primes(bottom,top,tempfd);
	  close(tempfd);
	  exit(numprimes);
	}
    }

  return 0;
}
