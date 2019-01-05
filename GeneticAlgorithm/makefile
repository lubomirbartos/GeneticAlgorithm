CC=gcc
SRCDIR = ./src/
LIBDIR = ./lib/



CFLAGS = -Wall -pedantic -ansi












LIBS = ./lib/config.h ./lib/nature.h ./lib/structures.h
SRCS = ./src/config.c ./src/nature.c ./src/app.c


run : config.o nature.o app.o
	gcc -o run config.o nature.o app.o -lm

config.o : ./src/config.c ./lib/structures.h ./lib/config.h ./lib/nature.h
	gcc -c ./src/config.c
nature.o : ./src/nature.c ./lib/structures.h ./lib/config.h ./lib/nature.h
	gcc -c ./src/nature.c
app.o : ./src/app.c ./lib/structures.h ./lib/config.h ./lib/nature.h
	gcc -c ./src/app.c
clean :
	rm run config.o nature.o app.o