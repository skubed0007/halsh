#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <time.h>
#include <readline/readline.h>
#include <readline/history.h>

#define BUFFER_SIZE 1024
#define MAX_ALIASES 100
#define MAX_HISTORY 100

char *aliases[MAX_ALIASES][2];
char *prompt_format;
char *history[MAX_HISTORY];
int history_count = 0;

void load_config();
void save_config();
char *set_prompt();
char *expand_tilde(const char *path);
void add_to_history(const char *command);
char **parse_input(char *line);
void execute_command(char **args);
void execute_builtin_command(char **args);
char *get_alias_command(const char *alias);
void handle_signal(int signal);
void run_shell();

void load_config() {
    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return;
    }

    char config_path[BUFFER_SIZE];
    snprintf(config_path, sizeof(config_path), "%s/.halshrc", home);

    FILE *config_file = fopen(config_path, "r");
    if (config_file) {
        char line[BUFFER_SIZE];
        while (fgets(line, sizeof(line), config_file)) {
            if (strncmp(line, "alias", 5) == 0) {
                char *alias_name = strtok(line + 6, "=");
                char *alias_command = strtok(NULL, "\n");
                if (alias_name && alias_command) {
                    for (int i = 0; i < MAX_ALIASES; i++) {
                        if (!aliases[i][0]) {
                            aliases[i][0] = strdup(alias_name);
                            aliases[i][1] = strdup(alias_command);
                            break;
                        }
                    }
                }
            }
        }
        fclose(config_file);
    }
}

void save_config() {
    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return;
    }

    char config_path[BUFFER_SIZE];
    snprintf(config_path, sizeof(config_path), "%s/.halshrc", home);

    FILE *config_file = fopen(config_path, "w");
    if (config_file) {
        for (int i = 0; i < MAX_ALIASES; i++) {
            if (aliases[i][0]) {
                fprintf(config_file, "alias %s=%s\n", aliases[i][0], aliases[i][1]);
            }
        }
        fclose(config_file);
    }
}

char *set_prompt() {
    char hostname[BUFFER_SIZE];
    char cwd[BUFFER_SIZE];
    char time_str[BUFFER_SIZE];
    time_t raw_time;
    struct tm *time_info;

    gethostname(hostname, sizeof(hostname));
    getcwd(cwd, sizeof(cwd));
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", time_info);

    char *final_prompt = malloc(BUFFER_SIZE);
    snprintf(final_prompt, BUFFER_SIZE, 
    "\033[1;36m╭──(\033[1;32m%s\033[0m@\033[1;33m%s\033[0m)─[\033[1;34m%s\033[0m]─[\033[1;35m%s\033[0m]\n\033[1;37m╰─$ \033[0m", 
    getenv("USER"), hostname, cwd, time_str);

    return final_prompt;
}

char *expand_tilde(const char *path) {
    if (path[0] == '~') {
        const char *home = getenv("HOME");
        if (home) {
            size_t len = strlen(home) + strlen(path);
            char *expanded = malloc(len + 1);
            strcpy(expanded, home);
            strcat(expanded, path + 1);
            return expanded;
        }
    }
    return strdup(path);
}

void add_to_history(const char *command) {
    if (history_count >= MAX_HISTORY) {
        free(history[0]);
        memmove(&history[0], &history[1], (MAX_HISTORY - 1) * sizeof(char *));
        history_count--;
    }
    history[history_count++] = strdup(command);
    add_history(command);
}

char **parse_input(char *line) {
    int bufsize = 64;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Allocation error.\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " \t\n");
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "Allocation error.\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " \t\n");
    }
    tokens[position] = NULL;
    return tokens;
}

void execute_command(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shell");
    } else {
        wait(NULL);
    }
}

void execute_builtin_command(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
    } else if (strcmp(args[0], "exit") == 0) {
        save_config();
        exit(0);
    } else if (strcmp(args[0], "history") == 0) {
        for (int i = 0; i < history_count; i++) {
            printf("%d: %s\n", i + 1, history[i]);
        }
    } else if (strcmp(args[0], "alias") == 0) {
        if (args[1] == NULL) {
            for (int i = 0; i < MAX_ALIASES; i++) {
                if (aliases[i][0]) {
                    printf("%s='%s'\n", aliases[i][0], aliases[i][1]);
                }
            }
        } else {
            char *command = get_alias_command(args[1]);
            if (command) {
                printf("%s='%s'\n", args[1], command);
            } else {
                printf("Alias '%s' not found\n", args[1]);
            }
        }
    } else if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; args[i] != NULL; i++) {
            char *expanded = getenv(args[i]);
            if (expanded) {
                printf("%s ", expanded);
            } else {
                printf("%s ", args[i]);
            }
        }
        printf("\n");
    }
}

char *get_alias_command(const char *alias) {
    for (int i = 0; i < MAX_ALIASES; i++) {
        if (aliases[i][0] && strcmp(aliases[i][0], alias) == 0) {
            return aliases[i][1];
        }
    }
    return NULL;
}

void handle_signal(int signal) {
    if (signal == SIGINT) {
        char *msg = "\nInterrupt signal received, returning to shell...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return;
    }
    else if (signal == SIGTSTP) {
        char *msg = "\nStop signal received, suspending shell...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return;
    }
    else if (signal == SIGCONT) {
        char *msg = "\nContinuing after suspension...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    }
    else if (signal == SIGQUIT) {
        char *msg = "\nQuit signal received, shell exiting...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        exit(1);
    }
    else if (signal == SIGTERM) {
        char *msg = "\nTermination signal received, shutting down...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        exit(0);
    }
    else if (signal == SIGSEGV) {
        char *msg = "\nSegmentation fault detected, shutting down...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        exit(1);
    }
    else if (signal == SIGPIPE) {
        char *msg = "\nPipe signal received, ignoring...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return;
    }
    else {
        char *msg = "\nReceived signal %d, custom handling not implemented.\n";
        dprintf(STDOUT_FILENO, msg, signal);
        return;
    }
}



void run_shell() {
    load_config();
    signal(SIGINT, handle_signal);
    read_history(NULL);

    while (1) {
        char *prompt = set_prompt();
        char *line = readline(prompt);
        free(prompt);

        if (!line) {
            break;
        }

        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        add_to_history(line);
        char **args = parse_input(line);

        char *alias_command = get_alias_command(args[0]);
        if (alias_command) {
            free(args);
            args = parse_input(alias_command);
        }

        if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "exit") == 0 || strcmp(args[0], "history") == 0 ||
            strcmp(args[0], "alias") == 0 || strcmp(args[0], "echo") == 0) {
            execute_builtin_command(args);
        } else {
            execute_command(args);
        }

        free(line);
        free(args);
    }

    write_history(NULL);
}

int main() {
    run_shell();
    return 0;
}
