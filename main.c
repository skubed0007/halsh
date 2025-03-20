#include <ctype.h>
#include <dirent.h>
#include <stdio.h>      // Standard headers must come first
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "config_parser.h"  // Your config parser functions

#define BUFFER_SIZE 1024
#define MAX_ALIASES 100
#define MAX_HISTORY 100

/* Global variables */
char *aliases[MAX_ALIASES][2] = {0};
char *prompt_format;
char *history_arr[MAX_HISTORY];
int history_count = 0;

/* Function prototypes */
void load_config(void);
void save_config(void);
char *set_prompt(void);
char *expand_tilde(const char *path);
void add_to_history(const char *command);
char **parse_input(char *line);
void execute_command(char **args);
void execute_builtin_command(char **args);
char *get_alias_command(const char *alias);
void handle_signal(int signal);
void load_history(void);
void save_history(void);
void run_shell(void);

/* --- Helper Function Prototypes --- */
static char *trim_whitespace(char *str);

/* --- Helper Function Definitions --- */
static char *trim_whitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = '\0';
    return str;
}

/* --- Custom Key Binding Functions --- */

/* When double Escape is pressed, insert "sudo " at the beginning */
int my_double_escape(int count, int key) {
    // Replace current line with "sudo " + current line
    char newtext[BUFFER_SIZE];
    snprintf(newtext, BUFFER_SIZE, "sudo %s", rl_line_buffer);
    rl_replace_line(newtext, 0);
    rl_point = 5; // Place cursor after "sudo "
    return 0;
}


/* --- Shell Functions --- */

char *set_prompt(void) {
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

void load_history(void) {
    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return;
    }
    char history_path[BUFFER_SIZE];
    snprintf(history_path, sizeof(history_path), "%s/.halshhis", home);
    FILE *history_file = fopen(history_path, "r");
    if (history_file) {
        char line[BUFFER_SIZE];
        while (fgets(line, sizeof(line), history_file)) {
            line[strcspn(line, "\n")] = '\0';
            if (history_count < MAX_HISTORY) {
                history_arr[history_count++] = strdup(line);
                add_history(line);
            }
        }
        fclose(history_file);
    } else {
        perror("No history file found\nMay be the first time you run halsh");
    }
    /* Ensure an empty entry exists at the end of Readline history */
    if (history_count == 0 || (history_count > 0 && strlen(history_arr[history_count - 1]) != 0)) {
        add_history("");
    }
}

void save_history(void) {
    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return;
    }
    char history_path[BUFFER_SIZE];
    snprintf(history_path, sizeof(history_path), "%s/.halshhis", home);
    FILE *history_file = fopen(history_path, "w");
    if (history_file) {
        for (int i = 0; i < history_count; i++) {
            fprintf(history_file, "%s\n", history_arr[i]);
        }
        fclose(history_file);
    } else {
        perror("Error opening history file");
    }
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
        free(history_arr[0]);
        memmove(&history_arr[0], &history_arr[1], (MAX_HISTORY - 1) * sizeof(char *));
        history_count--;
    }
    history_arr[history_count++] = strdup(command);
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
        save_history();
        exit(0);
    } else if (strcmp(args[0], "history") == 0) {
        for (int i = 0; i < history_count; i++) {
            printf("%d: %s\n", i + 1, history_arr[i]);
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
            if (args[i][0] == '$') {
                char *envname = args[i] + 1;
                char *value = getenv(envname);
                if (value)
                    printf("%s ", value);
                else
                    printf("$%s ", envname);
            } else {
                printf("%s ", args[i]);
            }
        }
        printf("\n");
    }
}

char *get_alias_command(const char *alias) {
    for (int i = 0; i < MAX_ALIASES; i++) {
        if (aliases[i][0] && strcmp(aliases[i][0], alias) == 0)
            return aliases[i][1];
    }
    return NULL;
}

/* Signal handler: When Ctrl+C is pressed, clear current input using Readline */
void handle_signal(int signal) {
    if (signal == SIGINT) {
        printf("\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    } else if (signal == SIGTSTP) {
        const char *msg = "\nStop signal received, suspending shell...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    } else if (signal == SIGCONT) {
        const char *msg = "\nContinuing after suspension...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    } else if (signal == SIGQUIT) {
        const char *msg = "\nQuit signal received, shell exiting...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        exit(1);
    } else if (signal == SIGTERM) {
        const char *msg = "\nTermination signal received, shutting down...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        exit(0);
    } else if (signal == SIGSEGV) {
        const char *msg = "\nSegmentation fault detected, shutting down...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        exit(1);
    } else if (signal == SIGPIPE) {
        const char *msg = "\nPipe signal received, ignoring...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    } else {
        char msg[128];
        snprintf(msg, sizeof(msg), "\nReceived signal %d, not implemented.\n", signal);
        dprintf(STDOUT_FILENO, "%s", msg);
    }
}

/* run_shell: If the input line contains "&&", split it and execute each subcommand.
   The entire input line is stored in history as one entry.
*/
void run_shell(void) {
    load_config();
    load_history();
    signal(SIGINT, handle_signal);

    /* Bind custom keys:
       Double Escape inserts "sudo " at beginning.
       Ctrl+Delete (escape sequence "\e[3;5~") kills a word forward.
    */
    rl_bind_keyseq("\\e\\e", my_double_escape);

    while (1) {
        char *prompt = get_prompt_config();
        char *line = readline(prompt);
        free(prompt);
        if (!line)
            break;
        if (strlen(line) == 0) {
            free(line);
            continue;
        }
        /* Add the entire line to history as one entry */
        add_to_history(line);
        if (strstr(line, "&&") != NULL) {
            char *line_copy = strdup(line);
            char *start = line_copy;
            char *subcmd;
            while ((subcmd = strstr(start, "&&")) != NULL) {
                *subcmd = '\0';
                char *command = trim_whitespace(start);
                if (strlen(command) > 0) {
                    char **args = parse_input(command);
                    char *alias_command = get_alias_command(args[0]);
                    if (alias_command) {
                        free(args);
                        args = parse_input(alias_command);
                    }
                    if (strcmp(args[0], "cd") == 0 ||
                        strcmp(args[0], "exit") == 0 ||
                        strcmp(args[0], "history") == 0 ||
                        strcmp(args[0], "alias") == 0 ||
                        strcmp(args[0], "echo") == 0)
                        execute_builtin_command(args);
                    else
                        execute_command(args);
                    free(args);
                }
                start = subcmd + 2;
            }
            char *command = trim_whitespace(start);
            if (strlen(command) > 0) {
                char **args = parse_input(command);
                char *alias_command = get_alias_command(args[0]);
                if (alias_command) {
                    free(args);
                    args = parse_input(alias_command);
                }
                if (strcmp(args[0], "cd") == 0 ||
                    strcmp(args[0], "exit") == 0 ||
                    strcmp(args[0], "history") == 0 ||
                    strcmp(args[0], "alias") == 0 ||
                    strcmp(args[0], "echo") == 0)
                    execute_builtin_command(args);
                else
                    execute_command(args);
                free(args);
            }
            free(line_copy);
        } else {
            char **args = parse_input(line);
            char *alias_command = get_alias_command(args[0]);
            if (alias_command) {
                free(args);
                args = parse_input(alias_command);
            }
            if (strcmp(args[0], "cd") == 0 ||
                strcmp(args[0], "exit") == 0 ||
                strcmp(args[0], "history") == 0 ||
                strcmp(args[0], "alias") == 0 ||
                strcmp(args[0], "echo") == 0)
                execute_builtin_command(args);
            else
                execute_command(args);
            free(args);
        }
        free(line);
    }
    save_history();
}

int main() {
    srand((unsigned) time(NULL));
    load_history();
    run_shell();
    return 0;
}
