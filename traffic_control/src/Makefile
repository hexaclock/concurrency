CC       = gcc
NAME1    = trafficmgr
SRC1     = q.c trafficmgr.c monitor.c
OBJS     = $(SRCS:.c=.o)
IPATH    = ../include
LDLIBS  += -lpthread
CFLAGS  += -I$(IPATH) -g -Wall -pedantic-errors
RM       = rm -f

all: $(NAME1)

$(NAME1):
	$(CC) $(CFLAGS) -o $(NAME1) $(SRC1) $(LDLIBS)

clean:
	-$(RM) *.o *~ \#*
	-$(RM) $(NAME1)

fclean: clean

re: fclean all
