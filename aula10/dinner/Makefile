CC = gcc

CFLAGS = -Wall

LK = gcc

LDFLAGS = -lm

all:	probSemFilos semFilos

probSemFilos:	semaphore.o sharedMemory.o probSemFilos.o
		$(LK) -o $@ $^ $(LDFLAGS)

semFilos:	semaphore.o sharedMemory.o semFilos.o
		$(LK) -o $@ $^ $(LDFLAGS)

probSemFilos.o:	semaphore.h sharedMemory.h

semFilos.o:	semaphore.h sharedMemory.h

clean:
			rm -f *.o *~

cleanall:	clean
			rm -f probSemFilos semFilos
			rm -f *.o
			rm -f err?
