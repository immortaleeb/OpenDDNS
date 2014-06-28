#include <pthread.h>
#include <stdio.h>

#include "threads/thread_args.h"
#include "threads/dns_thread.h"
#include "threads/console_thread.h"

// Indiciates wether we have executed the "stop" command
static int finished = 0;

void set_finished();
int has_finished();

int main(int argc, char** argv) {
    thread_args_t args;
    pthread_t dns_thread, console_thread;
    int res;

    // Create the necessary arguments for the threads
    args.has_finished = has_finished;
    args.set_finished = set_finished;

    // Create dns thread
    res = pthread_create(&dns_thread, NULL, dns_thread_main, &args);
    if (res) {
        fprintf(stderr, "Error creating the DNS thread: %d\n", res);
        return 1;
    }

    // Create console thread
    res = pthread_create(&console_thread, NULL, console_thread_main, &args);
    if (res) {
        fprintf(stderr, "Error creating the console thread: %d\n", res);
        return 2;
    }

    // Wait for all threads to finish
    pthread_join(dns_thread, NULL);
    pthread_join(console_thread, NULL);

    return 0;
}

void set_finished() {
    finished = 1;
}

int has_finished() {
    return finished;
}
