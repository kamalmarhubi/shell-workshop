all: argv-installer simple-exec simplest-exec shell

clean:
	rm -vf argv-explorer simple-exec simplest-exec shell utils.o

argv-installer: argv-explorer.c
	gcc -o argv-explorer argv-explorer.c

simple-exec: simple-exec.c
	gcc -o simple-exec simple-exec.c

simplest-exec: simplest-exec.c
	gcc -o simplest-exec simplest-exec.c

utils.o: utils.h utils.c
	gcc -c utils.c

shell: shell.c utils.o
	gcc -o shell shell.c utils.o
