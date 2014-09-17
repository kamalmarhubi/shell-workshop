#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024
#define TOKEN_SEP " \t\n\r"

// Struct to represent a command and its arguments.
typedef struct {
  char* progname;
  char* args[];
} cmd_struct;

// Struct to represent a pipeline of commands. It should be understood that
// cmd[i]'s output goes to cmd[i+1]'s input. The total number of commands is
// stored in n_cmds.
typedef struct {
  int n_cmds;
  cmd_struct* cmds[];
} pipeline_struct;

// Parses str into a freshly allocated cmd_struct and returns a pointer to it.
cmd_struct* parse_command(char* str);

// Parses str into a freshly allocated pipeline_struct and returns a pointer to it.
pipeline_struct* parse_pipeline(char* str);

// For debugging purposes. Prints out command's progname and args to stderr.
void print_command(cmd_struct* command);

#endif  // UTILS_H
