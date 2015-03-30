#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_LEN 1024
#define TOKEN_SEP " \t\n\r"


/*
 * For internal use. Returns the next non-empty token according in *line,
 * splitting on characters in TOKEN_SEP. Returns NULL if no token remains.
 * Updates line to point to remainder after removal of the token.  Modifies the
 * string *line.
 */
char* next_non_empty(char **line) {
  char *tok;

  /* Consume empty tokens. */
  while ((tok = strsep(line, TOKEN_SEP)) && !*tok);

  return tok;
}


cmd_struct* parse_command(char* str) {
  /* Copy the input line in case the caller wants it later. */
  char* copy = strndup(str, MAX_LEN);
  char* token;
  int i = 0;

  /*
   * Being lazy (Rule 0) and allocating way too much memory for the args array.
   * Using calloc to ensure it's zero-initialised, which is important because
   * execvp expects a NULL-terminated array of arguments.
   */
  cmd_struct* ret = calloc(sizeof(cmd_struct) + MAX_LEN * sizeof(char*), 1);

  while ((token = next_non_empty(&copy))) {
    ret->args[i++] = token;
  }
  ret->progname = ret->args[0];
  ret->redirect[0] = ret->redirect[1] = -1;
  return ret;
}


pipeline_struct* parse_pipeline(char *str) {
  char* copy = strndup(str, MAX_LEN);
  char* cmd_str;
  int n_cmds = 0;
  int i = 0;
  pipeline_struct* ret;

  /*
   * Count pipe characters that appear in pipeline to know how much space to
   * allocate for the cmds array.
   */
  for (char* cur = copy; *cur; cur++) {
    if (*cur == '|') ++n_cmds;
  }

  ++n_cmds;  /* There is one more command than there are pipe characters. */

  ret = calloc(sizeof(pipeline_struct) + n_cmds * sizeof(cmd_struct*), 1);
  ret->n_cmds = n_cmds;

  while((cmd_str = strsep(&copy, "|"))) {
    ret->cmds[i++] = parse_command(cmd_str);
  }

  return ret;
}


void print_command(cmd_struct* command) {
  char** arg = command->args;
  int i = 0;

  fprintf(stderr, "progname: %s\n", command->progname);

  for (i = 0, arg = command->args; *arg; ++arg, ++i) {
    fprintf(stderr, " args[%d]: %s\n", i, *arg);
  }
}


void print_pipeline(pipeline_struct* pipeline) {
  cmd_struct** cmd = pipeline->cmds;
  int i = 0;

  fprintf(stderr, "n_cmds: %d\n", pipeline->n_cmds);

  for (i = 0; i < pipeline->n_cmds; ++i) {
    fprintf(stderr, "cmds[%d]:\n", i);
    print_command(cmd[i]);
  }
}
