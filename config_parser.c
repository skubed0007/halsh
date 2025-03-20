#include "config_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>

#define CONFIG_FILE ".halshrc"

extern char *aliases[MAX_ALIASES][2];
extern int history_count;

static char *prompt_config = NULL;

static const char *lookup_color(const char *token) {
    if (strcmp(token, "BLUE") == 0) return "\033[1;34m";
    if (strcmp(token, "BLACK") == 0) return "\033[0;30m";
    if (strcmp(token, "RED") == 0) return "\033[1;31m";
    if (strcmp(token, "GREEN") == 0) return "\033[1;32m";
    if (strcmp(token, "YELLOW") == 0) return "\033[1;33m";
    if (strcmp(token, "MAGENTA") == 0) return "\033[1;35m";
    if (strcmp(token, "CYAN") == 0) return "\033[1;36m";
    if (strcmp(token, "WHITE") == 0) return "\033[0;37m";
    if (strcmp(token, "BOLD") == 0) return "\033[1m";
    if (strcmp(token, "UNDERLINE") == 0) return "\033[4m";
    if (strcmp(token, "DIM") == 0) return "\033[2m";
    if (strcmp(token, "RESET") == 0) return "\033[0m";
    if (token[0] == '#' && strlen(token) == 7) {
        int r, g, b;
        if (sscanf(token + 1, "%02x%02x%02x", &r, &g, &b) == 3) {
            static char ansi[32];
            snprintf(ansi, sizeof(ansi), "\033[38;2;%d;%d;%dm", r, g, b);
            return ansi;
        }
    }
    return "";
}

static char *lookup_var(const char *var) {
    if (strcmp(var, "USER") == 0) {
        char *u = getenv("USER");
        return u ? strdup(u) : strdup("unknown");
    } else if (strcmp(var, "HOST") == 0) {
        char hostname[BUFFER_SIZE];
        if (gethostname(hostname, sizeof(hostname)) == 0)
            return strdup(hostname);
        else
            return strdup("unknown");
    } else if (strcmp(var, "WORKDIR") == 0) {
        char cwd[BUFFER_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            return strdup(cwd);
        else
            return strdup("unknown");
    } else if (strcmp(var, "OS") == 0) {
        FILE *fp = fopen("/etc/os-release", "r");
        if (fp) {
            char line[256];
            while (fgets(line, sizeof(line), fp)) {
                if (strncmp(line, "ID=", 3) == 0) {
                    char *id = line + 3;
                    id[strcspn(id, "\n")] = '\0';
                    if (id[0] == '"' && id[strlen(id)-1] == '"') {
                        id[strlen(id)-1] = '\0';
                        id++;
                    }
                    fclose(fp);
                    return strdup(id);
                }
            }
            fclose(fp);
        }
        return strdup("unknown");
    }
    return strdup("");
}

/* Parse prompt string:
   - Replace "\n" with an actual newline.
   - If a backslash precedes '[' or '{' or ']' or '}', output that literal character.
   - Replace {VAR} with its variable value.
   - Replace [COLOR] with the corresponding ANSI escape sequence.
   All other text is copied as-is.
*/
static char *parse_prompt_string(const char *input) {
    if (!input) return strdup("");
    size_t bufsize = 4096;
    char *result = malloc(bufsize);
    if (!result) return NULL;
    result[0] = '\0';
    size_t i = 0;
    while (input[i] != '\0') {
        if (input[i] == '\\' && input[i+1] == 'n') {
            strncat(result, "\n", bufsize - strlen(result) - 1);
            i += 2;
        } else if (input[i] == '\\' && (input[i+1] == '[' || input[i+1] == ']' ||
                                        input[i+1] == '{' || input[i+1] == '}')) {
            size_t curr = strlen(result);
            result[curr] = input[i+1];
            result[curr+1] = '\0';
            i += 2;
        } else if (input[i] == '{') {
            size_t start = i + 1;
            while (input[i] && input[i] != '}') i++;
            if (input[i] == '}') {
                size_t token_len = i - start;
                char var[token_len + 1];
                strncpy(var, input + start, token_len);
                var[token_len] = '\0';
                char *var_value = lookup_var(var);
                strncat(result, var_value, bufsize - strlen(result) - 1);
                free(var_value);
                i++;
            } else {
                strncat(result, "{", bufsize - strlen(result) - 1);
                i++;
            }
        } else if (input[i] == '[') {
            size_t start = i + 1;
            while (input[i] && input[i] != ']') i++;
            if (input[i] == ']') {
                size_t token_len = i - start;
                char token[token_len + 1];
                strncpy(token, input + start, token_len);
                token[token_len] = '\0';
                const char *color = lookup_color(token);
                strncat(result, color, bufsize - strlen(result) - 1);
                i++;
            } else {
                strncat(result, "[", bufsize - strlen(result) - 1);
                i++;
            }
        } else {
            size_t curr = strlen(result);
            result[curr] = input[i];
            result[curr+1] = '\0';
            i++;
        }
    }
    return result;
}

void load_config(void) {
    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return;
    }
    char config_path[BUFFER_SIZE];
    snprintf(config_path, sizeof(config_path), "%s/%s", home, CONFIG_FILE);
    FILE *fp = fopen(config_path, "r");
    if (!fp) {
        return;
    }
    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n')
            continue;
        line[strcspn(line, "\n")] = '\0';
        if (strncmp(line, "prompt=", 7) == 0) {
            free(prompt_config);
            prompt_config = strdup(line + 7);
        } else if (strncmp(line, "alias ", 6) == 0) {
            char *alias_line = line + 6;
            char *equal_sign = strchr(alias_line, '=');
            if (equal_sign) {
                *equal_sign = '\0';
                char *name = alias_line;
                char *value = equal_sign + 1;
                for (int i = 0; i < MAX_ALIASES; i++) {
                    if (aliases[i][0] == NULL) {
                        aliases[i][0] = strdup(name);
                        aliases[i][1] = strdup(value);
                        break;
                    }
                }
            }
        }
    }
    fclose(fp);
}

void save_config(void) {
    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return;
    }
    char config_path[BUFFER_SIZE];
    snprintf(config_path, sizeof(config_path), "%s/%s", home, CONFIG_FILE);
    FILE *fp = fopen(config_path, "w");
    if (!fp) {
        perror("Error opening config file for writing");
        return;
    }
    if (prompt_config) {
        fprintf(fp, "prompt=%s\n", prompt_config);
    }
    for (int i = 0; i < MAX_ALIASES; i++) {
        if (aliases[i][0]) {
            fprintf(fp, "alias %s=%s\n", aliases[i][0], aliases[i][1]);
        }
    }
    fclose(fp);
}

char *get_prompt_config(void) {
    if (prompt_config)
        return parse_prompt_string(prompt_config);
    else
        return strdup("");
}
