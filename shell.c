#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

int main() {
  char command[] = "ls -a -l";
  char pipeline[] = "ls -a -l | wc -l";

  cmd_struct* c = parse_command(command);
  print_command(c);

  pipeline_struct* p = parse_pipeline(pipeline);
  print_pipeline(p);
  return 0;
}
