CC = gcc
CFLAGS = -Wall

LK = gcc
LDFLAGS = -lpthread -lm

objs = monFilos.o probMonFilos.o monFilosAlt.o probMonFilosAlt.o

probMonFilos:	monFilos.o probMonFilos.o
		$(LK) -o $@ $^ $(LDFLAGS)

probMonFilosAlt:monFilosAlt.o probMonFilosAlt.o
		$(LK) -o $@ $^ $(LDFLAGS)

$(objs):	%.o:	%.c monFilos.h
		$(CC) $(CFLAGS) -c -o $@ $<

clean:
		rm -f *.o *~

cleanall:	clean
		rm -f probMonFilos probMonFilosAlt
