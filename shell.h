#ifndef GUARD_shell_h

char *sh_read_line();
void sh_loop();
char **sh_split_line(char *line);
int sh_launch(char **args);

int sh_cd(char **args);
int sh_help(char **args);
int sh_exit(char **args);
int sh_num_builtins();

#define SH_RL_BUFSIZE 1024
#define SH_TOK_BUFSIZE 64
#endif

// possible better implementations
/*
 *char *lsh_read_line() {
 *    char *line = NULL;
 *    ssize_t bufsize = 0;
 *    if (getline(&line, &bufsize, stdin) == -1) {
 *        if (feof(stdin)) {
 *            exit(EXIT_SUCCESS);
 *        } else {
 *            perror("readline");
 *            exit(EXIT_FAILURE);
 *        }
 *    }
 *    return line;
 *}
 */
