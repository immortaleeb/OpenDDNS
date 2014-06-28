#include "console.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void (**functions)(int argc, char** argv);
static const char** commands;
static unsigned int num_commands = 0;

void console_init(unsigned int ncommands) {
    commands = malloc(sizeof(const char*) * ncommands);
    functions = malloc(ncommands);
}

void console_destroy() {
    free(functions);
    free(commands);
}

void console_run(const char* prompt, unsigned int buffer_size, int (*stop_func)()) {
    char buffer[buffer_size];

    while (!stop_func()) {
        if (read_console_line(prompt, buffer, buffer_size)) {
            char** argv;
            unsigned int argc;

            argc = parse_console_line(buffer, &argv);
            if (argc > 0) {
                if (!call_command(argv[0], argc, argv)) {
                    printf("Unknown command '%s' entered\n", argv[0]);
                }
            }

            free_argv(argv, argc);
        }
    }
}

void register_command(const char* command, void (*func)(int argc, char** argv)) {
    commands[num_commands] = command;
    functions[num_commands] = func;

    num_commands++;    
}

int call_command(const char* command, int argc, char** argv) {
    int i;

    for (i = 0; i < num_commands; ++i) {
        if (strcmp(commands[i], command) == 0) {
            functions[i](argc, argv);
            return 1;
        }
    }

    return 0;
}

int read_console_line(const char* prompt, char* buffer, size_t size) {
    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    /* Get the line of the given size from stdin */
    if (fgets(buffer, size, stdin) == NULL)
        return 0;

    /* 
     * We don't want to keep the new line character
     * at the end of the line
     */
    if (buffer[strlen(buffer)-1] == '\n')
        buffer[strlen(buffer)-1] = '\0';

    return 1;
}

unsigned int parse_console_line(const char* line, char*** argv) {
    int i = 0;
    char* tokens;
    char* token;
    unsigned int num_tokens = 0;
    unsigned int len = strlen(line);

    tokens = (char*)malloc(sizeof(char) * (len + 1));
    strcpy(tokens, line);

    /* Count number of tokens (not most efficient algorithm, but meh) */
    token = strtok(tokens, " ");
    while (token != NULL) {
        num_tokens++;
        token = strtok(NULL, " ");
    }

    /* Allocate memory for all the tokens */
    *argv = (char**)malloc(sizeof(char*) * num_tokens);

    /* Do another pass to fill the argv array */
    strcpy(tokens, line);
    token = strtok(tokens, " ");
    while (token != NULL) {
        /* Copy the token over */
        (*argv)[i] = (char*)malloc(sizeof(char) * (strlen(token) + 1));
        strcpy((*argv)[i], token);

        i++;
        token = strtok(NULL, " ");
    }

    free(tokens);

    return num_tokens;
}

void free_argv(char** argv, int argc) {
    int i;

    for (i = 0; i < argc; ++i) {
        free(argv[i]);
    }


    free(argv);
}
