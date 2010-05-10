CC=gcc
LIBS=$(shell pkg-config --libs x11)

all: wio.c wio.h 
	$(CC) ${LIBS} wio.c -o wio -Wall

clean: 
	rm wio
