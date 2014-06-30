#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#include "../../../common/c/common.h"
#include "../map/dns_map.h"
#include "thread_args.h"

#define SERVER_PORT 5000

static int (*has_finished)(void);
static void (*set_finished)(void);

void process_client_connection(int client_socket, struct sockaddr_in* client_addr, dns_map* map);

void* binding_thread_main(void* args) {
    int server_socket;
    struct sockaddr_in server_addr;
    int sockopt = 1;
    dns_map* map;

    // Get the necessary arguments
    thread_args_t* arguments = (thread_args_t*) args;
    has_finished = arguments->has_finished;
    set_finished = arguments->set_finished;
    map = arguments->map;

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        fprintf(stderr, "ERROR Could not create socket: %d\n", server_socket);
        return NULL;
    }

    // Allow immediate reuse of the socket
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(int));

    // Setup address to bind to
    memset((char*) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "ERROR could not bind socket\n");
        return NULL;
    }

    // Start listening
    if (listen(server_socket, 5) < 0) {
        fprintf(stderr, "ERROR could not listen\n");
        return NULL;
    }

    printf("Binding thread listening...\n");

    // Accept connections
    while (!has_finished()) {
        int client_socket;
        struct sockaddr_in client_addr;
        socklen_t alen;

        // Keep trying to accept a connection
        while ((client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &alen)) < 0) {
            if ((errno != ECHILD) && (errno != EINTR)) {
                fprintf(stderr, "accept failed");
                return NULL;
            }
        }

        process_client_connection(client_socket, &client_addr, map);
    }

    // Close socket
    printf("Closing binding thread socket...\n");
    shutdown(server_socket, SHUT_RDWR);

    return NULL;
}

void print_ip(uint8_t* ip) {
    int i;
    printf("%d", ip[0]);
    for (i = 1; i < 4; ++i) {
        printf(".%d", ip[i]);
    }
    printf("\n");
}

void process_client_connection(int client_socket, struct sockaddr_in* client_addr, dns_map* map) {
    int nbytes;
    bind_req_packet req;
    bind_resp_packet resp;

    printf("Accepted connection from ");
    print_ip((uint8_t*) &(client_addr->sin_addr.s_addr));

    // Receive the token from the client
    nbytes = recv(client_socket, (void*) &req, sizeof(req), 0);
    if (nbytes != sizeof(req)) {
        fprintf(stderr, "Error while receiving the client request, terminating connection\n");
        shutdown(client_socket, SHUT_RDWR);
        return;
    }

    // Check if the token is a valid
    if (!dns_map_has_token(map, req.token)) {
        // Invalid token, send an invalid response
        fprintf(stderr, "Client supplied an invalid token, sending response\n");

        resp.response_code = RESP_INVALID_TOKEN;

        nbytes = send(client_socket, (void*) &resp, sizeof(resp), 0);
        if (nbytes != sizeof(resp)) {
            fprintf(stderr, "Error while sending response, terminating connection\n");
        }

        shutdown(client_socket, SHUT_RDWR);
        return;
    }

    // Valid token, do update of IP here
    printf("Received valid token from client, updating IP\n");
    dns_map_put_ipv4(map, req.token, (uint8_t*) &(client_addr->sin_addr.s_addr), 0);

    printf("IP updated, sending response to client\n");
    
    // Send response to client
    resp.response_code = RESP_OK;

    nbytes = send(client_socket, (void*) &resp, sizeof(resp), 0);
    if (nbytes != sizeof(resp)) {
        fprintf(stderr, "Error while sending response, terminating connection\n");
        shutdown(client_socket, SHUT_RDWR);
    }

    // Shutdown connection, we're done with this client!
    shutdown(client_socket, SHUT_RDWR);
}

