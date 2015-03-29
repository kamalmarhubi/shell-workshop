all: shell

clean:
	rm -vf shell utils.o

# _BSD_SOURCE is needed for strsep; _POSIX_C_SOURCE >= 200809L for strndup.
CPPFLAGS := -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS := -Wall -Wextra -std=c11 -pedantic -Werror

shell: shell.o utils.o
