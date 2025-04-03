#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_INPUT 1024

typedef struct Variable {
    char *name;
    char *value;
    struct Variable *next;
} Variable;

Variable *head = NULL;


void set_local_variable(char *name, char *value) {
    Variable *temp = head;
    while (temp) {
        if (strcmp(temp->name, name) == 0) {
            free(temp->value);
            temp->value = strdup(value);
            return;
        }
        temp = temp->next;
    }
    Variable *new_var = (Variable *)malloc(sizeof(Variable));
    new_var->name = strdup(name);
    new_var->value = strdup(value);
    new_var->next = head;
    head = new_var;
}


char *get_local_variable(char *name) {
    Variable *temp = head;
    while (temp) {
        if (strcmp(temp->name, name) == 0)
            return temp->value;
        temp = temp->next;
    }
    return NULL;
}


void export_variable(char *name) {
    char *value = get_local_variable(name);
    if (value) {
        setenv(name, value, 1);
    } else {
        printf("Variable %s not found\n", name);
    }
}


void execute_command(char *input) {
    char *args[MAX_INPUT / 2 + 1];
    char *token;
    int arg_count = 0;
    char *input_file = NULL, *output_file = NULL, *error_file = NULL;
    int in_fd, out_fd, err_fd;

    token = strtok(input, " ");
    while (token) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            input_file = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            output_file = token;
        } else if (strcmp(token, "2>") == 0) {
            token = strtok(NULL, " ");
            error_file = token;
        } else {
            args[arg_count++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

    if (arg_count == 0) return;

    if (strcmp(args[0], "exit") == 0) {
        printf("Good Bye :)\n");
        exit(0);
    }

    if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        }
        return;
    }

    if (strcmp(args[0], "cd") == 0) {
        if (arg_count < 2) {
            printf("cd: missing argument\n");
            return;
        }
        if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
        return;
    }

    if (strcmp(args[0], "export") == 0) {
        export_variable(args[1]);
        return;
    }

    if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; i < arg_count; i++) {
            if (args[i][0] == '$') {
                char *value = get_local_variable(args[i] + 1);
                printf("%s ", value ? value : "");
            } else {
                printf("%s ", args[i]);
            }
        }
        printf("\n");
        return;
    }

    char *equals = strchr(args[0], '=');
    if (equals) {
        *equals = '\0';
        char *name = args[0];
        char *value = equals + 1;
        set_local_variable(name, value);
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        if (input_file) {
            in_fd = open(input_file, O_RDONLY);
            if (in_fd < 0) {
                perror("Error opening input file");
                exit(1);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (output_file) {
            out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) {
                perror("Error opening output file");
                exit(1);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        if (error_file) {
            err_fd = open(error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (err_fd < 0) {
                perror("Error opening error file");
                exit(1);
            }
            dup2(err_fd, STDERR_FILENO);
            close(err_fd);
        }
        execvp(args[0], args);
        perror("Execution failed");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("Fork failed");
    }
}

int main() {
    char input[MAX_INPUT];

    while (1) {
        printf("Micro Shell Prompt > ");
        if (fgets(input, MAX_INPUT, stdin) == NULL)
            break;
        input[strcspn(input, "\n")] = 0;
        execute_command(input);
    }
    return 0;
}

