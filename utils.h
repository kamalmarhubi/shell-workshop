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

  /**
   * IO redirections; redirect[i] should be used as fd i in the child.
   * A value of -1 indicates no redirect.
   */
  int redirect[2];

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
 * The redirects in the returned cmd_struct will be set to -1, ie no redirect.
 */
cmd_struct* parse_command(char* str);


/**
 * Parses str into a freshly allocated pipeline_struct and returns a pointer to
 * it.  All cmd_structs in cmds will also be freshy allocated, and have their
 * redirects set to -1, ie no redirect.
 */
pipeline_struct* parse_pipeline(char* str);


/* For debugging purposes. */
void print_command(cmd_struct* command);
void print_pipeline(pipeline_struct* pipeline);


#endif  /* UTILS_H */
