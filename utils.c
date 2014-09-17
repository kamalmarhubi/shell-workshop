#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_LEN 1024
#define TOKEN_SEP " \t\n\r"

// For internal use. Returns the next non-empty token according to TOKEN_SEPin
// *line. Returns NULL if no token remains. Updates line to point to remainder
// after removal of the token.  Modifies the string *line.
char* next_non_empty(char **line) {
  char *tok;
  while ((tok = strsep(line, TOKEN_SEP)) && !*tok);

  return tok;
}


void print_command(cmd_struct* command) {
  char** arg = command->args;
  int i = 0;
  fprintf(stderr, "progname: %s\n", command->progname);
  for (i = 0, arg = command->args; *arg; ++arg, ++i) {
    fprintf(stderr, " args[%d]: %s\n", i, *arg);
  }
}
 

cmd_struct* parse_command(char* line) {
  char* copy = strndup(line, MAX_LEN);
    char* token;
    int i = 0;

    cmd_struct* ret = calloc(sizeof(cmd_struct) + MAX_LEN * sizeof(char*), 1);
    while((token = next_non_empty(&copy))) {
      ret->args[i++] = token;
    }
    ret->progname = ret->args[0];
  return ret;
}


