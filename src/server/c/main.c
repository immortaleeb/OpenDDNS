#include <pthread.h>
#include <stdio.h>

#include "threads/thread_args.h"
#include "threads/dns_thread.h"
#include "threads/console_thread.h"
#include "threads/binding_thread.h"
#include "map/dns_map.h"

#define DOMAIN2TOKEN_FILENAME "domain2token.data"
#define TOKEN2IP_FILENAME "token2ip.data"

// Indicates wether we have executed the "stop" command
static int finished = 0;

void set_finished();
int has_finished();

int main(int argc, char** argv) {
    thread_args_t args;
    pthread_t dns_thread, binding_thread, console_thread;
    dns_map* map;
    FILE *h_domain2token, *h_token2ip;
    int res;

    // Open necessary files
    if ( !(h_domain2token = fopen(DOMAIN2TOKEN_FILENAME, "a+b")) ) {
        fprintf(stderr, "Failed to open file %s: %d\n", DOMAIN2TOKEN_FILENAME, errno);
        return 1;
    }
    if ( !(h_token2ip = fopen(TOKEN2IP_FILENAME, "a+b")) ) {
        fprintf(stderr, "Failed to open file %s: %d\n", TOKEN2IP_FILENAME, errno);
        return 2;
    }

    // Initialize dns_map
    map = create_dns_map(h_domain2token, h_token2ip);

    // Create the necessary arguments for the threads
    args.has_finished = has_finished;
    args.set_finished = set_finished;
    args.map = map;

    // Create dns thread
    res = pthread_create(&dns_thread, NULL, dns_thread_main, &args);
    if (res) {
        fprintf(stderr, "Error creating the DNS thread: %d\n", res);
        return 3;
    }

    // Create binding thread
    res = pthread_create(&binding_thread, NULL, binding_thread_main, &args);
    if (res) {
        fprintf(stderr, "Error creating the binding thread: %d\n", res);
        return 3;
    }

    // Create console thread
    res = pthread_create(&console_thread, NULL, console_thread_main, &args);
    if (res) {
        fprintf(stderr, "Error creating the console thread: %d\n", res);
        return 4;
    }

    // Wait for all threads to finish
    pthread_join(dns_thread, NULL);
    pthread_join(binding_thread, NULL);
    pthread_join(console_thread, NULL);

    // Destroy the dns map
    destroy_dns_map(map);

    // Close open files
    fclose(h_domain2token);
    fclose(h_token2ip);

    return 0;
}

void set_finished() {
    finished = 1;
}

int has_finished() {
    return finished;
}
