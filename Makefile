CC=gcc

all: wio.c wio.h 
	$(CC) -L/usr/X11R6/lib -lX11 wio.c -o wio -Wall

clean: 
	rm wio
