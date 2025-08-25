#ifndef METHOD_H
#define METHOD_H

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

void shell_prompt();
void run_command(char **arg, char *input);
void pwd();
char *get_input(char *buffer);
bool check_background(char *input);
void tokenising(char *input, char **arg);
void cd(char **arg, char *input);
void help(char **arg, char *input);
void execute_command(char **arg, bool background, char *input);
void print_history();
void print_single_history();
void print_n_history(char *input);
void add_to_history(char *input);
void handle_sigint(int sig);
void sigchld_handler(int sig);
#endif
