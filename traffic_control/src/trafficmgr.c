#include "assgn3.h"

void *north(void *arg)
{
  char direction;
  struct cart_t *cart;

  direction = 'n';

  fprintf(stderr, "thread for direction %c starts\n", direction);
  cart = q_getCart(direction);
  while (cart != NULL) {
    fprintf(stderr, "thread for direction %c gets cart %i\n",
	    direction, cart->num);
    monitor_arrive(cart);
    monitor_cross(cart);
    monitor_leave(cart);
    cart = q_getCart(direction);
  }

  fprintf(stderr, "thread for direction %c exits\n", direction);

  return NULL;
}

void *south(void *arg)
{
  char direction;
  struct cart_t *cart;

  direction = 's';

  fprintf(stderr, "thread for direction %c starts\n", direction);
  cart = q_getCart(direction);
  while (cart != NULL) {
    fprintf(stderr, "thread for direction %c gets cart %i\n",
	    direction, cart->num);
    monitor_arrive(cart);
    monitor_cross(cart);
    monitor_leave(cart);
    cart = q_getCart(direction);
  }

  fprintf(stderr, "thread for direction %c exits\n", direction);

  return NULL;
}

void *east(void *arg)
{
  char direction;
  struct cart_t *cart;

  direction = 'e';

  fprintf(stderr, "thread for direction %c starts\n", direction);
  cart = q_getCart(direction);
  while (cart != NULL) {
    fprintf(stderr, "thread for direction %c gets cart %i\n",
	    direction, cart->num);
    monitor_arrive(cart);
    monitor_cross(cart);
    monitor_leave(cart);
    cart = q_getCart(direction);
  }

  fprintf(stderr, "thread for direction %c exits\n", direction);

  return NULL;
}

void *west(void *arg)
{
  char direction;
  struct cart_t *cart;

  direction = 'w';

  fprintf(stderr, "thread for direction %c starts\n", direction);
  cart = q_getCart(direction);
  while (cart != NULL) {
    fprintf(stderr, "thread for direction %c gets cart %i\n",
	    direction, cart->num);
    monitor_arrive(cart);
    monitor_cross(cart);
    monitor_leave(cart);
    cart = q_getCart(direction);
  }

  fprintf(stderr, "thread for direction %c exits\n", direction);

  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t nthread;
  pthread_t sthread;
  pthread_t ethread;
  pthread_t wthread;
  char *dirs;
  int i;

  setvbuf(stdout, NULL, _IOLBF,0);

  if (argc != 2)
    {
      printf("usage: %s <directions>\n",argv[0]);
      return 1;
    }

  dirs = argv[1];

  /*initialize four queues; one for each direction*/
  q_init();
  /*add carts to queues from command line args*/
  for (i=0; dirs[i] != '\0'; ++i)
    {
      if (dirs[i] == 'n')
	q_putCart('n');
      else if (dirs[i] == 's') 
	q_putCart('s');
      else if (dirs[i] == 'e')
	q_putCart('e');
      else if (dirs[i] == 'w')
	q_putCart('w');
      else
	printf("Invalid character %c supplied, ignoring...\n",dirs[i]);
    }
  
  printf("\nStarting queues:\n");
  q_print('n');
  q_print('w');
  q_print('s');
  q_print('e');
  printf("\n");

  /*init monitor*/
  monitor_init();

  /*start threads*/
  if (pthread_create(&nthread,NULL,&north,NULL))
    {
      printf("North thread creation failed.\n");
      return -1;
    }
  if (pthread_create(&sthread,NULL,&south,NULL))
    {
      printf("South thread creation failed.\n");
      return -1;
    }
  if (pthread_create(&ethread,NULL,&east,NULL))
    {
      printf("East thread creation failed.\n");
      return -1;
    }
  if (pthread_create(&wthread,NULL,&west,NULL))
    {
      printf("West thread creation failed.\n");
      return -1;
    }

  /*wait until all threads exit*/
  pthread_join(nthread,NULL);
  pthread_join(sthread,NULL);
  pthread_join(ethread,NULL);
  pthread_join(wthread,NULL);

  printf("\nEnding queues (should all be NULL):\n");
  q_print('n');
  q_print('w');
  q_print('s');
  q_print('e');
  printf("\n");

  /*free up memory and exit program*/
  q_shutdown();
  monitor_shutdown();

  return 0;
}
