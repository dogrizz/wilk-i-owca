#!/bin/bash
gcc -L/usr/X11R6/lib -lX11 wio.c -o wio -Wall $1 $2 $3

