#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define TOKEN_SEP " \t\n\r"

typedef struct {
  char* cmd;
  char* args[];
} cmd;

char* next_non_empty(char **line) {
  char *tok;
  while ((tok = strsep(line, TOKEN_SEP))
      && !*tok);

  return tok;
}

int my_exec(char* cmd, char** args) {
  return execvp(cmd, args);
}

int main(int argc, char **argv) {
  pid_t child_pid;
  int stat_loc = 0;
  char *line = NULL, *cmd = NULL;
  size_t len = 0;;
  fprintf(stderr, "heeee> ");

  size_t n_chars = getline(&line, &len, stdin);
  fprintf(stderr, "we read %s\n", line);

  while((cmd = next_non_empty(&line))) fprintf(stderr, "GOT AN THING: %s\n", cmd);
return 0;
  
  child_pid = fork();

  if (child_pid) {  /* We are the parent. */
    switch(child_pid) {
      case -1:
        fprintf(stderr, "Oh dear.\n");
        break;
      default:
        wait(&stat_loc);
        fprintf(stderr, "All done!\n");
        return 0;
    }
  } else {  // We are the child. */
    fprintf(stderr, "HI FROM CHILD\n");
    char* args[] = {cmd, 0};
    execvp(cmd, args);
    fprintf(stderr, "OH DEAR\n");
    return 0;
  }
}
