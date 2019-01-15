

all : config.o nature.o app.o
	gcc -o gms.exe config.o nature.o app.o -lm -pedantic -ansi -g
config.o : config.c structures.h config.h nature.h
	gcc -c config.c -lm -pedantic -ansi -g
nature.o : nature.c structures.h config.h nature.h
	gcc -c nature.c -lm -pedantic -ansi -g
app.o : app.c structures.h config.h nature.h
	gcc -c app.c -lm -pedantic -ansi -g
clean :
	rm gms.exe config.o nature.o app.o