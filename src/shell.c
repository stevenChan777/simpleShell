#include "../include/method.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#define MAX_LEN 10

int main() {
  char *buffer = (char *)malloc(1024);
  char *arg[PATH_MAX];
  struct sigaction child;
  child.sa_handler = sigchld_handler;
  child.sa_flags = SA_RESTART;
  sigemptyset(&child.sa_mask);
  sigaction(SIGCHLD, &child, NULL);

  struct sigaction handler;
  handler.sa_handler = handle_sigint;
  handler.sa_flags = SA_RESTART;
  sigemptyset(&handler.sa_mask);
  sigaction(SIGINT, &handler, NULL);
  while (1) {
    shell_prompt();
    char *input = get_input(buffer);
    char *tmp = (char *)malloc(strlen(input));
    strcpy(tmp, input);
    tokenising(input, arg);
    run_command(arg, tmp);
    free(tmp);
  }
  free(buffer);
  return 0;
}
