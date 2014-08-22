all: argv-installer

clean:
	rm -vf argv-explorer

argv-installer: argv-explorer.c
	gcc -o argv-explorer argv-explorer.c

