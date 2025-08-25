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

char pre_path[PATH_MAX];
char *input_history[MAX_LEN];
int history_count = 0;
int history_index = 0;
char *arg[PATH_MAX];

void run_command(char **arg, char *input) {
  bool background = check_background(input);
  if (strcmp(arg[0], "exit") == 0) {
    if (arg[1] != NULL) {
      write(STDERR_FILENO, "exit: too many arguments\n", 25);
    } else {
      for (int i = 0; i < history_count; i++) {
        if (input_history[i] != NULL) {
          free(input_history[i]);
        }
      }
      exit(0);
    }
  } else if (strcmp(arg[0], "pwd") == 0) {
    if (arg[1] != NULL) {
      write(STDERR_FILENO, "pwd: too many arguments\n", 24);
    } else {
      pwd();
    }
  } else if (strcmp(arg[0], "cd") == 0) {
    if (arg[1] != NULL && arg[2] != NULL) {
      write(STDERR_FILENO, "cd: too many arguments\n", 23);
    } else {
      cd(arg, input);
    }
  } else if (strcmp(arg[0], "help") == 0) {
    if (arg[1] != NULL && arg[2] != NULL) {
      write(STDERR_FILENO, "help: too many arguments\n", 25);
    } else {
      help(arg, input);
    }
  } else if (strcmp(arg[0], "history") == 0) {
    if (arg[1] != NULL) {
      write(STDERR_FILENO, "history: too many arguments\n", 28);
    } else {
      print_history();
    }
  } else if (strcmp(arg[0], "!!") == 0) {
    if (arg[1] != NULL) {
      write(STDERR_FILENO, "history: command invalid\n", 25);
    } else {
      print_single_history();
    }
  } else if (input[0] == '!') {
    if (!isdigit(input[1])) {
      write(STDERR_FILENO, "history: command invalid\n", 25);
    } else {
      print_n_history(input);
    }
  } else {
    execute_command(arg, background, input);
  }
}

void shell_prompt() {
  char *pwd = (char *)malloc(PATH_MAX * sizeof(char));
  if (getcwd(pwd, PATH_MAX) != NULL) {
    write(STDOUT_FILENO, pwd, strlen(pwd));
    write(STDOUT_FILENO, "$ ", 2);
    free(pwd);
  } else {
    write(STDOUT_FILENO, "shell: unable to get current directory\n", 39);
    free(pwd);
  }
}

void pwd() {
  char *pwd = (char *)malloc(PATH_MAX * sizeof(char));
  if (getcwd(pwd, PATH_MAX) != NULL) {
    write(STDOUT_FILENO, pwd, strlen(pwd));
    write(STDOUT_FILENO, "\n", 1);
    free(pwd);
    add_to_history("pwd");
  } else {
    write(STDERR_FILENO, "shell: unable to get current directory\n", 39);
    free(pwd);
  }
}

char *get_input(char *buffer) {
  ssize_t len = read(STDIN_FILENO, buffer, 1023);
  if (len == -1) {
    free(buffer);
    write(STDERR_FILENO, "shell: unable to read command\n", 30);
    exit(EXIT_FAILURE);
  }
  buffer[len - 1] = '\0';
  return buffer;
}

bool check_background(char *input) {
  size_t len = strlen(input);
  if (input[len - 1] == '&' && len > 0) {
    input[len - 1] = '\0';
    return true;
  }
  return false;
}

void tokenising(char *input, char **arg) {
  char *token;
  int i = 0;
  char *rest;
  token = strtok_r(input, " ", &rest);
  while (token != NULL) {
    arg[i] = token;
    i++;
    token = strtok_r(NULL, " ", &rest);
  }
  arg[i] = NULL;
}

void execute_command(char **arg, bool background, char *input) {
  pid_t pid = fork();
  if (pid == -1) {
    write(STDERR_FILENO, "shell:unable to fork\n", 22);
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    if (execvp(arg[0], arg) == -1) {
      write(STDERR_FILENO, "shell: unable to execute command\n", 33);
      exit(EXIT_FAILURE);
    }
  } else if (pid > 0) {
    if (!background) {
      if (waitpid(pid, NULL, 0) == -1) {
        write(STDERR_FILENO, "shell: unable to wait for child\n", 32);
      } else {
        add_to_history(input);
      }
    } else {
      add_to_history(input);
    }
  }
}

void help(char **arg, char *input) {
  if (arg[1] == NULL) {
    write(STDOUT_FILENO, "exit: exit the shell\n", 21);
    write(STDOUT_FILENO, "pwd: print the current directory\n", 33);
    write(STDOUT_FILENO, "cd: change the current directory\n", 33);
    write(STDOUT_FILENO, "help: shoe help information\n", 28);
    write(STDOUT_FILENO, "history: print the history of commands\n", 39);
    add_to_history("help");
  } else if (strcmp(arg[1], "exit") == 0) {
    write(STDOUT_FILENO, "exit: exit the shell\n", 21);
    add_to_history("help exit");
  } else if (strcmp(arg[1], "pwd") == 0) {
    write(STDOUT_FILENO, "pwd: print the current directory\n", 33);
    add_to_history("help pwd");
  } else if (strcmp(arg[1], "cd") == 0) {
    write(STDOUT_FILENO, "cd: change the current directory\n", 33);
    add_to_history("help cd");
  } else if (strcmp(arg[1], "help") == 0) {
    write(STDOUT_FILENO, "help: show help information\n", 28);
    add_to_history("help help");
  } else if (strcmp(arg[1], "history") == 0) {
    write(STDOUT_FILENO, "history: print the history of commands\n", 39);
    add_to_history("help history");
  } else {
    write(STDOUT_FILENO, arg[1], strlen(arg[1]));
    write(STDOUT_FILENO, ": external command or application\n", 34);
    add_to_history(input);
  }
}

void cd(char **arg, char *input) {
  pid_t pid = getuid();
  struct passwd *pw = getpwuid(pid);
  char *home = pw->pw_dir;
  char *path = (char *)malloc(PATH_MAX * sizeof(char));
  char current[PATH_MAX];

  getcwd(current, sizeof(current));
  if (arg[1] == NULL) {
    if (chdir(home) == -1) {
      write(STDERR_FILENO, "cd: unable to change diretory\n", 31);
    } else {
      strcpy(pre_path, current);
      add_to_history("cd");
    }
  } else {
    if (arg[1][0] == '~') {
      if (chdir(home) == -1) {
        write(STDERR_FILENO, "cd: unable to change directory\n", 31);
      } else {
        if (arg[1][1] != '\0') {
          int i = 0;
          int j = 2;
          while (arg[1][j] != ' ') {
            path[i] = arg[1][j];
            i++;
            j++;
          }
          if (chdir(path) == -1) {
            write(STDERR_FILENO, "cd: unable to change directory\n", 31);
          } else {
            add_to_history(input);
          }
        } else {
          add_to_history("cd ~");
        }
      }
    } else if (arg[1][0] == '-') {
      if (chdir(pre_path) == -1) {
        write(STDERR_FILENO, "cd: unable to change directory\n", 31);
      } else {
        add_to_history("cd -");
        strcpy(pre_path, current);
      }
    } else {
      if (chdir(arg[1]) == -1) {
        write(STDERR_FILENO, "cd: unable to change directory\n", 31);
      } else {
        add_to_history(input);
        strcpy(pre_path, current);
      }
    }
  }
  free(path);
}

void add_to_history(char *input) {
  if (history_count >= MAX_LEN) {
    free(input_history[0]);
    for (int i = 1; i < history_count; i++) {
      input_history[i - 1] = input_history[i];
    }
    history_count--;
  }
  input_history[history_count] = strdup(input);
  history_count++;
  history_index++;
}

void print_history() {
  if (history_count == 0) {
    write(STDERR_FILENO, "history: no command entered\n", 28);
  } else {
    add_to_history("history");
    int i = 0;
    char buffer[1024];
    for (int j = history_count - 1; j >= 0; j--) {
      int len = snprintf(buffer, sizeof(buffer), "%d\t%s",
                         history_index - i - 1, input_history[j]);
      write(STDOUT_FILENO, buffer, len);
      write(STDOUT_FILENO, "\n", 1);
      i++;
    }
  }
}

void print_single_history() {
  if (history_count == 0) {
    write(STDERR_FILENO, "history: no command entered\n", 28);
  } else {
    char *last = input_history[history_count - 1];
    char *temp = (char *)malloc(strlen(last));
    strcpy(temp, last);
    write(STDOUT_FILENO, last, strlen(last));
    write(STDOUT_FILENO, "\n", 1);
    tokenising(last, arg);
    run_command(arg, temp);
    free(temp);
  }
}

void print_n_history(char *input) {
  if (history_count == 0) {
    write(STDERR_FILENO, "history: no command entered\n", 28);
  } else {
    int num = atoi(&input[1]);
    if (num >= history_count || num < 0) {
      write(STDERR_FILENO, "history: command invalid\n", 26);
    } else {
      char *cmd = input_history[num];
      char *temp = (char *)malloc(strlen(cmd));
      strcpy(temp, cmd);
      write(STDOUT_FILENO, cmd, strlen(cmd));
      write(STDOUT_FILENO, "\n", 1);
      tokenising(cmd, arg);
      run_command(arg, temp);
      free(temp);
    }
  }
}

void handle_sigint(int sig) {
  write(STDOUT_FILENO, "\n", 1);
  help(arg, "help");
  shell_prompt();
}

void sigchld_handler(int sig) {
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
}
