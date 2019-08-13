CC = gcc

CFLAGS = -Wall

LK = gcc

LDFLAGS = -lm

all:	probMsgFilos msgFilos

probMsgFilos:	message.o probMsgFilos.o
		$(LK) -o $@ $^ $(LDFLAGS)

msgFilos:	message.o msgFilos.o
		$(LK) -o $@ $^ $(LDFLAGS)

probMsgFilos.o:	message.h

msgFilos.o:	message.h

clean:
			rm -f *.o *~

cleanall:	clean
			rm -f probMsgFilos msgFilos
			rm -f *.o
			rm -f err?
