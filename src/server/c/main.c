#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

// Properties of DNS as defined by RFC 1035
#define PORT 53
#define BUFFER_SIZE 512

int main(int argc, const char* argv[]) {
    int listen_socket;

    printf("\nStarting listener...\n\n");
    if(!initialize_listener(&listen_socket)) {
        exit(1);
    }

    printf("\nStarted listener.\n\n");
    if(!start_listener(&listen_socket)) {
        exit(1);
    }
}

int initialize_listener(int* listen_socket) {
    struct sockaddr_in listen_addr;
    int sockoptval = 1;

    // Create a listen socket over IP for UDP.
    if ((*listen_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket");
        return 0;
    }

    // Allow immediate reuse of the port.
    setsockopt(*listen_socket, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

    // Define socket listening location.
    memset((char*) &listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET; // IPv4
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Host address
    listen_addr.sin_port = htons(PORT); // Port to listen to

    // Bind the listen socket to a port on an interface.
    if (bind(*listen_socket, (struct sockaddr*) &listen_addr, sizeof(listen_addr)) < 0) {
        perror("bind failed");
        return 0;
    }

    return 1;
}

void handle_datagram(char* buffer, ssize_t buffer_size) {
    printf("\nHey, you there! I received a packet!\n");
}

int start_listener(int* listen_socket) {
    char buffer[BUFFER_SIZE + 1];
    struct sockaddr_storage client_addr;
    socklen_t client_addr_length = sizeof(client_addr);
    ssize_t received_size;

    for (;;) {
        /*
         * Try reading an udp packet in our buffer. The client (who sent the datagram) details are
         * also saved.
         * Fourth argument is zero, we don't need any special behavior for this function.
         */
        while ((received_size = recvfrom(*listen_socket, buffer, sizeof(buffer), 0,
            (struct sockaddr*) &client_addr, &client_addr_length)) < 0) {
            if ((errno != ECHILD) && (errno != EINTR)) {
                perror("accept failed");
                return 0;
            }
        }

        if (received_size == BUFFER_SIZE + 1) {
            // If the client sent a valid DNS request, this is impossible to happen.
            warn("datagram too large for buffer: truncated");
        } else {
            handle_datagram(buffer, received_size);
        }
    }

    return 1;
}
