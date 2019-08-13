CC = gcc
CFLAGS = -Wall

LK = gcc
LDFLAGS = -lpthread -lm

objs = fifo.o prodcon.o

prodcon:	fifo.o prodcon.o
		$(LK) -o $@ $^ $(LDFLAGS)

$(objs):	%.o:	%.c fifo.h
		$(CC) $(CFLAGS) -c -o $@ $<

clean:
		rm -f *.o *~

cleanall:	clean
		rm -f prodcon
