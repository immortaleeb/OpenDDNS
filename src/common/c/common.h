#ifndef _OPENDDNS_COMMON_H_
#define _OPENDDNS_COMMON_H_

#include <stdio.h>
#include <stdint.h>

// Token size (in bytes)
#define TOKEN_SIZE 4

/**
 * Structutre of the packet sent by the
 * client when it sends a binding request
 * (i.e. an updat of its dynamic dns).
 **/
typedef struct {
    char token[TOKEN_SIZE];
} bind_req_packet;

/**
 * Structure of the packet sent by the
 * server as a response to a binding
 * request.
 */
typedef struct {
   unsigned char response_code; 
} bind_resp_packet;

/**
 * Prints an ipv4 address to the given FILE*
 * (without newline)
 */
void print_ipv4(FILE* file, uint8_t* ip);

// Response codes
#define RESP_OK 0
#define RESP_INVALID_TOKEN 1

#endif
