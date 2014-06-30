#ifndef DNS_THREAD_H_
#define DNS_THREAD_H_

#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include "../dns/message.h"
#include "../dns/answer.h"
#include "../dns/codec.h"
#include "thread_args.h"

void* dns_thread_main(void* args);

int initialize_listener(int* listen_socket);
void handle_datagram(unsigned char* buffer_in, ssize_t buffer_size_in, unsigned char** buffer_out,
        ssize_t* buffer_size_out, dns_map* map);
int start_listener(int* listen_socket, dns_map* map);

#endif
