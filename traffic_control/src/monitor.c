#include "assgn3.h"

static pthread_mutex_t arrival_lock, intersection_lock, leave_lock;
static sem_t semarr[4];

static char cartorder[] = "nwse";
static int curcart = 0;
static int firstcart = 1;

void monitor_init()
{
  int i;
  if ( pthread_mutex_init(&arrival_lock,NULL)
       || pthread_mutex_init(&intersection_lock,NULL)
       || pthread_mutex_init(&leave_lock,NULL))
    {
      printf("Could not initialize lock\n");
      exit(-2);
    }
  for (i=0;i<4;++i)
    sem_init(&(semarr[i]),0,0);
}

void monitor_arrive(struct cart_t *cart)
{
  printf("cart %d has arrived at intersection\n",cart->num);

  if ( !firstcart )
    {
      printf("cart %d must wait before entering intersection\n",cart->num);
      if (cart->dir == 'n')
	sem_wait(&(semarr[0]));
      else if (cart->dir == 'w')
	sem_wait(&(semarr[1]));
      else if (cart->dir == 's')
	sem_wait(&(semarr[2]));
      else if (cart->dir == 'e')
	sem_wait(&(semarr[3]));
    }
  /*runs only once*/
  else
    {
      pthread_mutex_lock(&arrival_lock);
      firstcart = 0;
      pthread_mutex_unlock(&arrival_lock);
      if (cart->dir == 'n')
	curcart = 0;
      else if (cart->dir == 'w')
	curcart = 1;
      else if (cart->dir == 's')
	curcart = 2;
      else if (cart->dir == 'e')
	curcart = 3;
    }
}

void monitor_cross(struct cart_t *cart)
{
  pthread_mutex_lock(&intersection_lock);

  printf("cart %d allowed to proceed.\n",cart->num);
  q_cartHasEntered(cart->dir);

  printf("cart %d has entered intersection\n",cart->num);
  sleep(10);
  printf("cart %d has crossed intersection\n",cart->num);
}

void monitor_leave(struct cart_t *cart)
{
  int done;
  int i,orig;

  pthread_mutex_lock(&leave_lock);
  done = 0;

  for (i=curcart,orig=curcart; i<(curcart+4); ++i)
    {
      if (q_cartIsWaiting( cartorder[(i+1)%4] ))
	{
	  sem_post( &(semarr[(i+1)%4]) );
	  printf("thread for direction %c signals thread for direction %c with OK to proceed\n"
		 ,cartorder[i%4],cartorder[(i+1)%4]);
	  done = 1;
	  break;
	}
    }
  /*case where we have >1 cart from a single 
    direction that has not "arrived" yet*/
  if (!done)
    {
      /*printf("sem_posting %c\n",cartorder[curcart]);*/
      /*printf("sem_posting %c\n",cartorder[(orig+1)%4]);*/
      for (i=curcart; i<(curcart+4); ++i)
	{
	  sem_post(&(semarr[i%4]));
	}
      /*sem_post(&(semarr[(orig+1)%4]));*/
      curcart = orig;
      /*curcart = (i+1) % 4;*/
    }
  else
    curcart = i % 4;

  printf("cart %d has left intersection\n\n",cart->num);
  free(cart);
  pthread_mutex_unlock(&leave_lock);
  pthread_mutex_unlock(&intersection_lock);
}

void monitor_shutdown()
{
  int i;
  pthread_mutex_destroy(&intersection_lock);
  pthread_mutex_destroy(&arrival_lock);
  pthread_mutex_destroy(&leave_lock);

  for (i=0; i<4; ++i)
    sem_destroy(&(semarr[i]));

}
