/*
 * Created by Alexander Swanson on 11/30/18.
 * Copyright (c) 2018, Alexander Joseph Swanson Villares
 * alexjosephswanson@gmail.com
 *
 * Resource: https://brennan.io/2015/01/16/write-a-shell-in-c/
 */


/* Imports */
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


lsh_execute();


int lsh_launch(char **args) {

    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {

        // Child process.
        if (execvp(args[0], args) == -1) {

            perror("lsh");

        }

        exit(EXIT_FAILURE);

    } else if (pid < 0) {

        // Error forking.
        perror("lsh");

    } else {

        // Parent process.
        do {

            wpid = waitpid(pid, &status, WUNTRACED);

        } while (WIFEXITED(status) && !WIFSIGNALED(status));

    }

    return 1;

}


#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line) {

    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {

        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);

    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {

        tokens[position] = token;
        position++;

        if (position >= LSH_TOK_BUFSIZE) {

            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) {

                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);

            }

        }

        token = strtok(NULL, LSH_TOK_DELIM);

    }

    tokens[position] = NULL;
    return tokens;

}


#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void) {

    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {

        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);

    }

    while (1) {

        // Read a character.
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {

            buffer[position] = '\0';
            return buffer;

        } else {

            buffer[position] = c;

        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {

            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);

            if (!buffer) {

                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);

            }

        }

    }

}


void lsh_loop(void) {

    char *line;
    char **args;
    int status;

    do {

        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);

    } while (status);

}


/**
 * Runs the program.
 * @return
 */
int main(int argc, char **argv) {

    // Load configuration files if necessary.

    // Run command loop.
    lsh_loop();

    // Perform any shutdown/cleanup.

    // Return exit code.
    return EXIT_SUCCESS;

}