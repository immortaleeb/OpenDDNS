#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stddef.h>

/**
 * Initializes the console, needs to be called
 * before any other functions are called.
 * The only argument specifies the maximum
 * number of commands that will be registered.
 */
void console_init(unsigned int ncommands);
/**
 * Needs to be called before the program
 * ends to free any allocated memory.
 */
void console_destroy();
/**
 * Runs the console with the given prompt and maximum buffer size.
 * This console will be run as long as the stop_func to which a
 * pointer is given does not return 1.
 */
void console_run(const char* prompt, unsigned int buffer_size, int (*stop_func)());

/**
 * Registers a new command by mapping the
 * name of the command onto the function
 * that needs to be executed when the 
 * command is entered.
 */
void register_command(const char* command, void (*func)(int argc, char** argv));
/**
 * Calls the command with the specified name
 * and passes the given arguments to the
 * function associated with this command.
 * Returns 1 if we could call the command and
 * 0 if the command was not found.
 */
int call_command(const char* command, int argc, char** argv);

/**
 * Reads a line from the console by displaying
 * a prompt and saving this into the buffer
 * with a maximum specified size.
 * Returns 0 on failure and 1 on success. 
 */
int read_console_line(const char* prompt, char* buffer, size_t size);

/**
 * Parses all words entered in the given console line
 * by returning them in the argv array. The return
 * value indicates the number of words that is saved
 * inside the argv array.
 */
unsigned int parse_console_line(const char* line, char*** argv);

/**
 * Used internally to free some memory.
 */
void free_argv(char** argv, int argc);

#endif
