# Let's build a shell!

## Introduction
Most of us use a shell, at least once in a while. Some spend most of their time in one. Often we take it for granted, not knowing or caring how it does what it does.  In this workshop, we'll delve a little into that by building our very own shell!

### Focus of the workshop
Shells have many different functions. This workshop will be focused on how processes are launched, and how their IO can be controlled with pipes. We'll be leaving things like environment variable expansion, signal handling and other things out, but I will give some examples of other directions to go at the end.

__Rule 0.__ In particular, we have zero interest in dealing with a whole bunch of messy *details*, including but not limited to:
- memory management
- parsing and tokenising and other text processing
- ‘proper’ error handling


## 0. Getting started
Download the workshop skeleton code and utility functions.
TODO link

I *highly* recommend using a version control system and making frequent commits.


## 1. Name your shell and read input

The __*most important*__ thing to start with is picking a name for your shell,
and its prompt. I went with `heee> ` while I was putting this workshop together,
so a typical interaction looks like this:
```
heeee> ls
shell   shell.c
```

So, let's get started!

**Exercise 1**: Set up the main function of your shell to repeatedly prompt the user for a line. For now, we'll just print that line back out. Our shell is a slightly glorified `cat`, just more personalised!

Here's a little snippet to read a line in C into a variable `line`:
```
char *line = NULL;
size_t capacity = 0;
getline(&line, &capacity, stdin);
```


## 2. System calls

So now we can read in a user's command. Next up is to actually run it. To do this, we need some help from the kernel, which is in charge of most things related to processes—among *many* other things.

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

So the call looks like `execvp('ls', ['ls', '-l', '/tmp'])`, except that C doesn't let you write the array directly like that.

Now we're ready to run a program. Parsing `ls -l /tmp` into an array of arguments is annoying in C (and not very instructive), so I've written a `parse_command` function that you can use.

__Exercise 2.__ Modify your shell so that after it has read a line from `stdin`, it executes the command with its arguments.

Try that out. Anything unexpected happen?


## 4. Forking a new process

In the last exercise, once the program you ran terminated, you didn't go back to your prompt, but got dumped right back to your regular shell. The `exec` call *transforms* the current process into the command we specify. So our shell's process is *replaced* by `ls` or whatever we chose to run. Once that program is done, the process exits—no more shell.

To avoid this happening, we want the command to run as a new process. We create processes with the `fork` system call. This creates a new process that is a clone of the existing one, right down to the point in the program that's executing. We can then call `exec` in the newly created child process, leaving our shell's process around for future interaction.

The return value of `fork` tells us if we're in the child or the parent: it returns `0` for the child, and the id of the new process in the parent.

__Exercies 3.__ Modify your shell to call `fork`, and then `exec` in the child.

Congratulations! Your shell is now mildly useful. There's still a niggle though: when you run a command, your prompt is probably printing out before the command's output.

## 5. Waiting for children

Looking at your code, you'll see that the parent goes right back to the prompt. To wait for the child process, there's a well-named `wait` system call. For our purposes, we can just call it with a `NULL` argument: `wait(NULL);`.

__Exercise 4.__ Make your shell `wait` on the child process before going back to the prompt.

Now it's starting to act like a real shell! Try out a few of your favourite commands. I'm a fan of `cowsay`, but don't let me stop you from running `ls` or `cat`!


## 5. Builtins

What happens if we try to run `cd`? There's a program at `/bin/cd` that should let us change directories, so try out this sequence:

``` sh
heeee> pwd
/Users/kalmar/projects/conferences/2014/strangeloop/shell-workshop
heeee> cd ..
heeee> pwd
[something... unexpected?]
heeee> cd /tmp
heeee> pwd
[something... unexpected?]
heeee> echo HEEEELLLPPPP!!!!!
HEEEELLLPPPP!!!!!
```

That last little sequence of commands probably surprised you a bit. To understand just why we couldn't change directory, we need to find out a bit about how processes work.

Every process has
* its own memory (where all the global and local variables, the stack and heap)
* extra metadata that the kernel manages, but that the process can't change directly. It can change these with system calls.
  * user id (`setuid`)
  * group id (`setgid`)
  * priority (`setpriority`)
  * working directory (`chdir`)

So let's imagine that we `fork` and `exec` the `/bin/cd` process. 

```
1) parent: pid 1000, working directory /tmp
---- fork ----
2) parent: pid 1000, working directory /tmp
   child: pid 2000, working directory /tmp
---- exec `/bin/cd /home/awesome` in child ----
3) parent: pid 1000, working directory /tmp
   child: pid 2000, working directory /tmp/awesome
---- child exits ----
4) parent: pid 1000, working directory /tmp
```

A process can only use system calls to modify its own state, so the parent shell process still has the same working directory it started with: `/tmp`.

To change the shell's working directory, the shell itself needs to call the `chdir` system call. So the shell has to treat a `cd` command specially: if the command is `cd`, then call `chdir` instead of the usual `fork` / `exec`. The handling of `cd` is then built in to our shell—it's a builtin!

__Exercise 6.__ Add a `cd` builtin to your shell.

`cd` isn't the only builtin in your regular shell. Take a look at `man builtin` to see what else is there. Note that not all of them need to be builtins. For example `echo` works just fine as an external command, but many shells have it as a builtin. Some of these are builtin for efficiency, since creating a new process has some performance overhead.


## 6. Pipelines

Next up we're going to implement pipelines. Try running `ls | wc` in your shell. What happens?

```
heeee> ls | wc
ls: cannot access |: No such file or directory
ls: cannot access wc: No such file or directory
```

Right now it doesn't know that `|` has any special meaning, and so tries to list the files `|` and `wc`. Before we fix that, we need to understand better how pipes work.

Files on Unix-like systems are managed using **file descriptors**. If you're using Linux, you can see all the file descriptors for a process using `ls -l /proc/<pid>/fd`. File descriptors are positive integers (`0` is stdin, `1` is stdout, `2` is stderr, and 3 and up are any other files the process has open).

The `pipe` system call creates two new file descriptors with a special property.

```
int pipe_fds[2];
pipe(pipe_fds);  // puts returns [4, 5] in pipe_fds
```

If `pipe` returns `[4, 5]`, then any data written to `5` can be read out of `4`. What this means is that we need to set `ls`'s stdout to be `5` and `wc`'s stdin to be `4`. The way to change which file descriptor a file points to is with the `dup2` system call. In this example, we'd need to run `dup2(5, 1)` for `ls` and `dup2(4, 0)` for `wc`.

To recap, to create a pipe `ls | wc`, we need to:

1. create a pipe with `pipe`. Imagine that this creates file descriptors `4` and `5`.
2. fork twice (once for `ls` and once for `wc`).
  ```
  parent: pid 1000, stdin: terminal, stdout, terminal
  ---- fork twice ----
  parent: pid 1000, stdin: terminal, stdout: terminal
  child 1: pid 2000, stdin: terminal, stdout: terminal (for `ls`)
  child 2: pid 3000, stdin: terminak, stdout: terminal (for `wc`)
  ```
3. In child 1, run `dup2(pipe_fds[1], 1)` and exec `ls`
4. In child 2, run `dup2(pipe_fds[0], 0)` and exec `wc`
5. Close both ends of the pipe in the parent process.

That's it!

TODO ps | grep shows up grep


**Exercise 7:** Write a `fork_and_exec_with_io(cmd* cmd, int stdout_fd, int stdin_fd)` that forks and execs the specified command and changes its stdin and stdout as needed

**Exercise 8:** Use `fork_and_exec_with_io` to implement pipes with 2 elements.



## 7. What next?

There are a few directions to go in once you've completed the exercises here. I've split these into things that are fairly clear progression from what we've done, and things that are exploring other areas of shell features.

### Natural progresions
#### 1. Longer pipelines
We got a single pipe set up, but longer pipelines are much more useful. The `parse_line` function can return any number of piped commands, it's up to you to make it go!

#### 2. IO redirection to/from files
If you run `grep shel /usr/share/dict/words > /tmp/shel_words` in your regular shell, the list of words containing `shel` will end up in `/tmp/shel_words`. To do this, you'll have to extend the parsing code to pick up these redirections. Once done, the redirection itself won't be too different from how pipe was handled. You'll just have to use `open` instead of `pipe` to get the file descriptor to read from or write to.

#### 3. Redirecting other file descriptors
If you run `some-command 2>/tmp/errors` in your regular shell, you are instructing it to redirect `stderr` (`fd` 2) to `/tmp/errors`. This is useful to capture log output when a command writes it to `stderr`. This is a generalisation of the previous extension.

#### 4. Redirecting *to* other file descriptors
If you run `some-command 2>&1` in your regular shell, data written by `some-command` to its `stderr` (`fd` 2) will go to the same place as its `stdout` (`fd` 1). This is a further extension of redirection.


### Other directions
#### 1. Environment variable expansion
Compare the output of `echo $HOME` in your shell to your regular shell. Your regular shell treats tokens beginning with `$` specially: it looks them up by name in the environment, and substitutes them in the command. The `getenv` function looks up a variable in the environment. This can be handled during or after parsing a command.

#### 2. Setting environment variables on child processes
In your regular shell, compare the output of `date` and `TZ=Pacific/Samoa date`. The `date` command uses the `TZ` environment variable to decide how to display the time. Prefixing a command with a string like `VAR=value` sets `VAR` to `value` in the child's environment. The `setenv` function sets the value of an environment variable. This will require modifying the parser to find these prefixes.

#### 3. Another built-in: `export`
In `bash`, `export` lets us change an environment variable's value in the shell itself. As with `cd`, because a child cannot modify its parent's environment, `export` must be a builtin.

#### 4. Signal handling
Compare what happens when you hit `^C` (control-C) between your shell and your regular shell. `^C` sends a *signal* to your process, in this case `SIGINT`. Signals interrupt the normal execution of your program. By default, this aborts the process. To prevent that, we can register a signal handler function to be called when `SIGINT` is received, or decide to ignore it.

#### 5. Globbing
Compare the output of `wc -c *` between your shell and your regular shell. Expanding the wildcard `*` is done by the shell before passing arguments to the child. The `glob` function in the `<glob.h>` standard header will be useful.

#### 6. Running a command in the background
In your regular shell, if you end a command with `&`, it runs in the background. The command will run, but the shell will not block while it does. Ie, it does not `wait` on the child process.

#### 7. Job control
TODO
