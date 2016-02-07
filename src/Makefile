#Makefile

CC = gcc
CFLAGS = -O -Wall -pedantic

OBJS = main.o signallib.o morselib.o log.o

myprogram: ${OBJS}
	${CC} -o myprogram ${CFLAGS} ${OBJS}

log.o: log.c log.h
	${CC} ${CFLAGS} -c log.c

morselib.o: morselib.c morselib.h
	${CC} ${CFLAGS} -c morselib.c

signallib.o: signallib.c signallib.h
	${CC} ${CFLAGS} -c signallib.c

main.o: main.c
	${CC} ${CFLAGS} -c main.c

clean:
	rm -f myprogram ${OBJS}
	@echo "all cleaned up!"

