all: Compilador clean start

Compilador: main.c
		gcc main.c -g -o Compilador

start: Compilador
	./Compilador

clean:
	clear