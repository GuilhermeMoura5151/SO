CC = gcc

CFLAGS = -Wall

LDFLAGS = -lm

all: reg-cri

reg-cri:	sharedMemory.o reg-cri.o
	gcc ${CFLAGS} -o $@ $^ ${LDFLAGS}

reg-cri.o:	sharedMemory.h


.PHONY: clean cleanall

clean:
	rm -f *.o *~

cleanall:	clean
	rm -f reg-cri

