/**
 * Message structure elements for DNS.
 * NS: name server
 * RR: resource record
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Properties of DNS as defined by RFC 1035
#define PORT 53
#define BUFFER_SIZE 512

typedef struct dnsmsg_header {

    uint16_t id;
    /*
     * Flag name: bit size:
     * QR: 1
     * OPCODE: 4
     * AA: 1
     * TC: 1
     * RD: 1
     * Z: 4 (must be zero)
     * RCODE: 4
     */
    uint16_t status_flags;
    uint16_t query_count;
    uint16_t answer_count;
    uint16_t authority_count;
    uint16_t additional_count;

} dnsmsg_header_t;

typedef struct dnsmsg_label {

    uint8_t name_size;
    uint8_t* name;

} dnsmsg_label_t;

typedef struct dnsmsg_question {

    uint16_t labels_size;
    dnsmsg_label_t* labels;
    uint16_t type;
    uint16_t class;

} dnsmsg_question_t;

// Resource record
typedef struct dnsmsg_rr {

    uint16_t labels_size;
    dnsmsg_label_t* labels;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t data_size;
    uint8_t* data; // NO PADDING IS REQUIRED!

} dnsmsg_rr_t;

typedef struct dnsmsg {

    dnsmsg_header_t header;
    dnsmsg_question_t* questions;
    dnsmsg_rr_t* answers;
    dnsmsg_rr_t* authorities;
    dnsmsg_rr_t* additionals;

} dnsmsg_t;

void free_labels(dnsmsg_label_t* labels, uint16_t labels_size);
void free_message(dnsmsg_t message);
void free_rr(dnsmsg_rr_t* rr, uint16_t amount);

void print_labels(dnsmsg_label_t* labels, uint16_t labels_size);
void print_rr(dnsmsg_rr_t* rr, uint16_t rr_size);
void print_message(dnsmsg_t message);

#endif /* MESSAGE_H_ */
