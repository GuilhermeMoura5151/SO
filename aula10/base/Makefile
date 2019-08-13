CC = gcc

CFLAGS = -Wall

LDFLAGS = -lm

all: reg-cri

reg-cri:	sharedMemory.o reg-cri.o semaphore.o
	gcc ${CFLAGS} -o $@ sharedMemory.o semaphore.o reg-cri.o ${LDFLAGS}

reg-cri.o:	sharedMemory.h semaphore.h

clean:
	rm -f *.o *~

cleanall:	clean
	rm -f reg-cri

