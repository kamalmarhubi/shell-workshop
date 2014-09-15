#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LEN 1024
#define TOKEN_SEP " \t\n\r"

typedef struct {
  char* cmd;
  char* _to_free;
  char* args[];
} cmd;

char* next_non_empty(char **line) {
  char *tok;
  while ((tok = strsep(line, TOKEN_SEP)) && !*tok);

  return tok;
}

void print_command(cmd* command) {
  char** arg = command->args;
  fprintf(stderr, "cmd: %s\n", command->cmd);
  fprintf(stderr, "args[0]: %s\n", *arg);
  while(*arg++) {
    fprintf(stderr, "arg: %s\n", *arg);
  }
}
 

cmd* parse_command(char* line) {
  char* cur_cmd;
  char* copy = strndup(line, MAX_LEN);
  //while((cur_cmd = strsep(&copy, "|"))) {
    char* token;
    int i = 0;

    cmd* ret = calloc(sizeof(cmd) + MAX_LEN * sizeof(char*), 1);
    while((token = next_non_empty(&copy))) {
      ret->args[i++] = token;
    }
    ret->cmd = ret->args[0];
    ret->_to_free = copy;
    //print_command(ret);
  //}
  return ret;
}

int my_exec(char* cmd, char** args) {
  return execvp(cmd, args);
}

ssize_t prompt_and_get_input(const char* prompt,
                            char **line,
                            size_t *len) {
  fputs(prompt, stderr);
  return getline(line, len, stdin);
}

int exec_with_redir(cmd* command, int in_fd, int out_fd) {
  if (in_fd != -1) {
    dup2(in_fd, STDIN_FILENO);
  }
  if (out_fd != -1) {
    dup2(out_fd, STDOUT_FILENO);
  }
  return execvp(command->cmd, command->args);
}

int main(int argc, char **argv) {
  pid_t child_pid;
  cmd* command;
  int stat_loc = 0;
  char *line = NULL, *cmd = NULL;
  size_t len = 0;

  while(prompt_and_get_input("heeee> ", &line, &len) > 0) {
    command = parse_command(line);

    child_pid = fork();

    if (child_pid) {  /* We are the parent. */
      switch(child_pid) {
        case -1:
          fprintf(stderr, "Oh dear.\n");
          break;
        default:
          wait(&stat_loc);
      }
    } else {  // We are the child. */
      exec_with_redir(command, -1, -1);
      perror("OH DEAR");
      return 0;
    }
  }
  fputs("\n", stderr);
  return 0;

}
