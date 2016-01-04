NAME     = primes
SRCS     = main.c panic.c get_primes.c is_prime.c get_max.c
OBJS     = $(SRCS:.c=.o)
IPATH    = ../include
LDLIBS  += -lm
CFLAGS  += -I$(IPATH) -g -O2 -Wall -pedantic-errors
RM       = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(LDLIBS)

clean:
	-$(RM) *.o *~ \#*
	-$(RM) $(NAME)

fclean: clean

re: fclean all
