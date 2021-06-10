#include "shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *builtin_str[] = {"cd", "help", "exit"};
int (*builtin_func[])(char **) = {&sh_cd, &sh_help, &sh_exit};

int sh_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }
int sh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "sh: expected arguments to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("sh");
        }
    }
    return 1;
}
int sh_help(char **args) {
    int i;
    printf("A demo shell\n");
    printf("builtin functions:\n");
    for (i = 0; i < sh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    printf("use man for detailed information\n");
    return 1;
}
int sh_exit(char **args) { return 0; }

char *sh_read_line() {
    int bufsize = SH_RL_BUFSIZE, position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;
        // expand buffer space
        if (position > bufsize) {
            bufsize += SH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "sh: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
char **sh_split_line(char *line) {
    int bufsize = SH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;
    const char *const delimiters = "\t\r\n\a";

    if (!tokens) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, delimiters);
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize += SH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "sh: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, delimiters);
    }
    tokens[position] = NULL;
    return tokens;
}

int sh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // child process
        if (execvp(args[0], args) == -1) {
            perror("sh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // error forking
        perror("sh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int sh_execute(char **args) {
    int i;
    if (args[0] == NULL) {
        return 1;
    }
    for (i = 0; i < sh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return sh_launch(args);
}

void sh_loop() {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = sh_read_line();
        args = sh_split_line(line);
        status = sh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv) {
    sh_loop();

    return EXIT_SUCCESS;
}
