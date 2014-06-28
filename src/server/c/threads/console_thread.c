#include "console_thread.h"

#include "thread_args.h"
#include "../../../common/c/console.h"

#define NUM_CONSOLE_COMMANDS 1
#define CONSOLE_BUFFER_SIZE 255

static int (*has_finished)(void);
static void (*set_finished)(void);

void stop_func(int argc, char** argv);

void* console_thread_main(void* args) {
    // Get the necessary arguments
    thread_args_t* arguments = args;
    has_finished = arguments->has_finished;
    set_finished = arguments->set_finished;

    // Init console
    console_init(NUM_CONSOLE_COMMANDS);

    // Register commands
    register_command("stop", stop_func);

    // Run the console
    console_run("> ", CONSOLE_BUFFER_SIZE, has_finished);

    // destroy the console
    console_destroy();

    return NULL;
}

void stop_func(int argc, char** argv) {
    printf("Shutting down...\n");
    set_finished(); 
}
