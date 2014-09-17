# Shell workshop

## Introduction
Shells blah blah scary but they're just a program like any other.

### What you'll take away
The aim of this workshop is to get some insight into how our processes are managed by our OS. 

### Focus of the workshop
The workshop will be focused on how processes are launched, and how their IO can be controlled. We'll be leaving things like environment variable expansion

### Structure of the workshop
We're going to write a shell that can run commands, and handle pipes.

## Getting started
### Download the workshop skeleton code
- Clone the skeleton
- Run `make test` to check it all works

## 1. Name your shell and read input

The __*most important*__ thing to start with is picking a name for your shell,
and its prompt. I went with `heee> ` while I was putting this workshop together,
so a typical interaction looks like this:
```
heeee> 
TODO fill out
```

Let's give it a whirl! 

**Exercise 1**: Set up the main function to repeatedly prompt the user for a line. For now, we'll just print that line back out. Our shell is a slightly glorified `cat`, just more personalised!

Here's a little snippet to read a line in C into a variable `line`:
```
char *line = NULL;
int capacity = 0;
getline(&line, &capacity, stdin);
```

## 2. System calls

So now we can read in a user's command. Next up is to actually run it. To do this, we need some help from the kernel, which is in charge of most things related to process—among *many* other things.

The way our programs talk to the kernel is through *system calls*. These are operations we can tell the kernel to do on our behalf. Examples include:

* opening files (`open`)
* reading and writing to them (`read`, `write`)
* sending and receiving data over the network. (`sendto`, `recvfrom`) 
* starting programs (`execve`) 

In fact, more or less everything that involves our programs interacting with the ‘real world’ involves system calls.

Right now, we're interested in executing a program. That uses the `execve` system call, and we'll use a wrapper function called `execvp`. Not confusing at all, right?

## 3. Running a program

Before we run a program, we need to understand how program arguments get passed.

When a program is run, it gets an array of arguments passed in by the user, traditionally called `argv`. By convention, the first of these is the name of the program. So, when we run something like `ls -l /tmp`, the `argv` that `ls` receives consists of `{ "ls", "-l", "/tmp" }`.

So the call looks like `execvp('ls', ['ls', '-l', '/tmp'])`, except in C.

Now we're ready to run a program. Parsing `ls -l /tmp` into an array of arguments is annoying in C (and not very instructive), so I've written a `parse_command` function that you can use.

**Exercise 2**: After your program has read a line from stdin, execute the command with its arguments.

