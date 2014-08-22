#include <stdio.h>
#include <unistd.h>

/* Simple program to investigate argv. */
int main(int argc, char **argv) {
  int i;

  printf("Looping using argc\n");
  for (i = 0; i <= argc; ++i) {
    /* Iterating up to and including argc is safe, as we are guaranteed that
     * the argv will have argc non-null values followed by a null.
     */
    printf("argv[%d]: %16p value: %s\n", i, argv[i], argv[i] ? argv[i] : "");
  }

  return 0;
}
