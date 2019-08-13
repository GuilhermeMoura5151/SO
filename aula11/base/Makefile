CC = gcc

CFLAGS = -Wall

LD = gcc

LDFLAGS = -lm

TARGETS = server client client2

all: $(TARGETS)

comm-msg.o:	comm.h

$(TARGETS):		%:  %.o comm-msg.o message.o
		$(LD) -o $@ $^ $(LDFLAGS)

clean:
		rm -f *.o *~

cleanall:	clean
		rm -f $(TARGETS)

