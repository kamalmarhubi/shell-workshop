all: argv-installer simple-exec simplest-exec

clean:
	rm -vf argv-explorer simple-exec simplest-exec

argv-installer: argv-explorer.c
	gcc -o argv-explorer argv-explorer.c

simple-exec: simple-exec.c
	gcc -o simple-exec simple-exec.c

simplest-exec: simplest-exec.c
	gcc -o simplest-exec simplest-exec.c
