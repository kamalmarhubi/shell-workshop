#ifndef UTILS_H
#define UTILS_H

#define MAX_LEN 1024
#define TOKEN_SEP " \t\n\r"

/**
 * Struct to represent a command and its arguments.
 */
typedef struct {
  /** The name of the executable. */
  char* progname;
  /** The arguments; must be NULL-terminated. */
  char* args[];
} cmd_struct;


/**
 * Struct to represent a pipeline of commands. The intention is that cmd[i]'s
 * output goes to cmd[i+1]'s input.
 */
typedef struct {
  /** The total number of commands. */
  int n_cmds;
  /** The commands themselves. */
  cmd_struct* cmds[];
} pipeline_struct;


/**
 * Parses str into a freshly allocated cmd_struct and returns a pointer to it.
 */
cmd_struct* parse_command(char* str);


/**
 * Parses str into a freshly allocated pipeline_struct and returns a pointer to
 * it.  All cmd_structs in cmds will also be freshy allocated.
 */
pipeline_struct* parse_pipeline(char* str);


/* For debugging purposes. */
void print_command(cmd_struct* command);
void print_pipeline(pipeline_struct* pipeline);


#endif  /* UTILS_H */
