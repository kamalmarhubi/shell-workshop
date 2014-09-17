all: shell

clean:
	rm -vf shell utils.o

utils.o: utils.h utils.c
	gcc -c utils.c

shell: shell.c utils.o
	gcc -o shell shell.c utils.o
