CC       = gcc
NAME1    = rw
NAME2    = transfer1
NAME3    = transfer2
SRC1     = rw.c
SRC2     = transfer1.c cbuf.c
SRC3     = transfer2.c bettercbuf.c
OBJS     = $(SRCS:.c=.o)
IPATH    = ../include
LDLIBS  += -lpthread
CFLAGS  += -I$(IPATH) -g -Wall -pedantic-errors
RM       = rm -f

all: $(NAME1) $(NAME2) $(NAME3)

$(NAME1):
	$(CC) $(CFLAGS) -o $(NAME1) $(SRC1) $(LDLIBS)

$(NAME2):
	$(CC) $(CFLAGS) -o $(NAME2) $(SRC2) $(LDLIBS)

$(NAME3):
	$(CC) $(CFLAGS) -o $(NAME3) $(SRC3) $(LDLIBS)

clean:
	-$(RM) *.o *~ \#*
	-$(RM) $(NAME1) $(NAME2) $(NAME3)

fclean: clean

re: fclean all
