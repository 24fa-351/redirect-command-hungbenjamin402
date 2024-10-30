#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAX_CMD_PARTS 64
#define MAX_PATH 1024

char** split_command(const char* cmd) {
    char** args = malloc(MAX_CMD_PARTS * sizeof(char*));
    char* cmd_copy = strdup(cmd);
    char* token = strtok(cmd_copy, " ");
    int i = 0;
    
    while (token != NULL && i < MAX_CMD_PARTS - 1) {
        args[i] = strdup(token);
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
    free(cmd_copy);
    return args;
}

void free_args(char** args) {
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

char* find_command_path(const char* cmd) {
    char* path_env = strdup(getenv("PATH"));
    char* path = malloc(MAX_PATH);
    char* dir = strtok(path_env, ":");

    if (cmd[0] == '/' || strncmp(cmd, "./", 2) == 0 || strncmp(cmd, "../", 3) == 0) {
        strcpy(path, cmd);
        free(path_env);
        return path;
    }
    
    while (dir != NULL) {
        snprintf(path, MAX_PATH, "%s/%s", dir, cmd);
        if (access(path, X_OK) == 0) {
            free(path_env);
            return path;
        }
        dir = strtok(NULL, ":");
    }
    
    free(path_env);
    free(path);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        exit(1);
    }
    
    char* input_file = argv[1];
    char* command = argv[2];
    char* output_file = argv[3];

    char** args = split_command(command);

    char* cmd_path = find_command_path(args[0]);
    if (cmd_path == NULL) {
        fprintf(stderr, "Command not found: %s\n", args[0]);
        free_args(args);
        exit(1);
    }
    free(args[0]);
    args[0] = cmd_path;
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        free_args(args);
        exit(1);
    }
    
    if (pid == 0) {
        if (strcmp(input_file, "-") != 0) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("open input");
                exit(1);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                perror("dup2 input");
                exit(1);
            }
            close(fd_in);
        }

        if (strcmp(output_file, "-") != 0) {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                perror("open output");
                exit(1);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2 output");
                exit(1);
            }
            close(fd_out);
        }
        
        // Execute command
        execv(cmd_path, args);
        perror("execv");  // Only reaches here if execv fails
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    
    free_args(args);
    return WEXITSTATUS(status);
}