#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>

#include "../../common/c/common.h"

#define SERVER_ADDR "localhost"
#define SERVER_PORT 5000
#define TOKEN "SOME TOKEN"

int send_token_to_server(int client_socket, const char* token, const int token_size, unsigned char* response_code);
void process_response_code(unsigned char response_code);

int main(int argc, char** argv) {
    int client_socket, res;
    unsigned char resp_code;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    struct hostent* hp;

    char* hostname;
    int port;
    char token[TOKEN_SIZE + 1];

    if (argc > 4) {
        fprintf(stderr, "Usage: %s [hostname] [port] [token]\n", argv[0]);
        return -1;
    }

    // Read arguments
    hostname = (argc >= 2) ? argv[1] : SERVER_ADDR;
    port = (argc >= 3) ? atoi(argv[2]) : SERVER_PORT;
    if (argc == 4) {
        memcpy(token, argv[3], TOKEN_SIZE);
        token[TOKEN_SIZE] = '\0';
    }

    // Create a client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        fprintf(stderr, "Could not create socket: %d\n", client_socket);
        return 1;
    }

    // Bind the client socket
    memset((char*) &client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(0);
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    res = bind(client_socket, (struct sockaddr*) &client_addr, sizeof(client_addr));
    if (res < 0) {
        fprintf(stderr, "Could not bind the socket: %d\n", res);
        return 2;
    }

    // Lookup the ip address of the server
    hp = gethostbyname(hostname);
    if (!hp) {
        fprintf(stderr, "Could not find hostname %s\n", SERVER_ADDR);
        return 3;
    }

    // Set the server address and port
    memset((char*) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    memcpy((void *) &server_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    // Connect to the server
    res = connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (res < 0) {
        fprintf(stderr, "Could not connect to the server: %d\n", res);
        return 4;
    }

    // Send the token to the server
    res = send_token_to_server(client_socket, (const char*) token, TOKEN_SIZE, &resp_code);
    if (res < 0) {
        // Something went wrong, shutdown the socket
        fprintf(stderr, "Error while communicating with the server (%d), shutting down\n", res);
        shutdown(client_socket, SHUT_RDWR);
        return 5;
    }

    // Interpret the answer of the server
    process_response_code(resp_code);

    // Shutdown the connection
    shutdown(client_socket, SHUT_RDWR);

    return 0;
}

int send_token_to_server(int client_socket, const char* token, const int token_size, unsigned char* response_code) {
    int nbytes;
    bind_req_packet req;
    bind_resp_packet resp;


    // Send a binding request to the server
    memcpy(&(req.token), token, token_size);

    printf("Sending token...\n");

    nbytes = send(client_socket, (void*) &req, sizeof(req), 0);
    if (nbytes != sizeof(req)) 
        return -1;

    // Receive response from the server
    printf("Receiving response from server...\n");

    nbytes = recv(client_socket, (void*) &resp, sizeof(resp), 0);
    if (nbytes != sizeof(resp))
        return -2;

    *response_code = resp.response_code;

    return 1;
}

void process_response_code(unsigned char response_code) {
    switch (response_code) {
        case RESP_OK:
            printf("Succesfully updated your IP\n");
            break;
        case RESP_INVALID_TOKEN:
            fprintf(stderr, "Could not update your IP because an invalid token was supplied\n");
            break;
        default:
            fprintf(stderr, "Received an unknown response from the server: %d\n", response_code);
    }
}
