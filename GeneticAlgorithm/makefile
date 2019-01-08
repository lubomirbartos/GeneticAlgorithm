

run : config.o nature.o app.o
	gcc -o run config.o nature.o app.o -lm -pedantic -ansi
config.o : config.c structures.h config.h nature.h
	gcc -c config.c -lm -pedantic -ansi
nature.o : nature.c structures.h config.h nature.h
	gcc -c nature.c -lm -pedantic -ansi
app.o : app.c structures.h config.h nature.h
	gcc -c app.c -lm -pedantic -ansi
clean :
	rm run config.o nature.o app.o