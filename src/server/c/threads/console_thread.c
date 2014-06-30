#include "console_thread.h"

#include "thread_args.h"
#include "../../../common/c/console.h"
#include "../../../common/c/common.h"
#include "../map/dns_map.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_CONSOLE_COMMANDS 2
#define CONSOLE_BUFFER_SIZE 255

static int (*has_finished)(void);
static void (*set_finished)(void);
static dns_map* map;

void register_func(int argc, char** argv);
void stop_func(int argc, char** argv);

void* console_thread_main(void* args) {
    // Get the necessary arguments
    thread_args_t* arguments = args;
    has_finished = arguments->has_finished;
    set_finished = arguments->set_finished;
    map = arguments->map;

    // Init console
    console_init(NUM_CONSOLE_COMMANDS);

    // Register commands
    register_command("stop", stop_func);
    register_command("register", register_func);

    // Run the console
    console_run("> ", CONSOLE_BUFFER_SIZE, has_finished);

    // destroy the console
    console_destroy();

    return NULL;
}

void register_func(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <domain> <token>\n", argv[0]);
        return;
    } else {
        char* domain;
        char* current_token;
        char* token;

        domain = (char*) malloc(sizeof(char) * (strlen(argv[1]) + 1));
        token = (char*) malloc(sizeof(char) * (TOKEN_SIZE + 1));
        strcpy(domain, argv[1]);
        memcpy(token, argv[2], TOKEN_SIZE);
        token[TOKEN_SIZE] = '\0';

        current_token = dns_map_get_token_from_domain(map, domain);
        if (current_token != NULL) {
            printf("domain '%s' is already linked to token '%s', registration failed\n", 
                    domain, current_token);
            free(domain);
            free(token);
            return;
        }

        dns_map_put_token(map, domain, token, 1);
        printf("Succesfully linked domain '%s' to token '%s'\n",
                domain, token);
    }
}

void stop_func(int argc, char** argv) {
    printf("Shutting down...\n");
    set_finished(); 
}
