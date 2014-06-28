/**
 * Message structure elements for DNS.
 * NS: name server
 * RR: resource record
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdint.h>

// Properties of DNS as defined by RFC 1035
#define PORT 53
#define BUFFER_SIZE 512

typedef struct dnsmsg_header {

    int16_t id;
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
    int16_t status_flags;
    int16_t query_count;
    int16_t answer_count;
    int16_t authority_count;
    int16_t additional_count;

} dnsmsg_header_t;

typedef struct dnsmsg_question {

    int8_t name_size;
    int8_t* name; // NO PADDING IS REQUIRED!
    int16_t type;
    int16_t class;

} dnsmsg_question_t;

// Resource record
typedef struct dnsmsg_rr {

    int8_t name_size;
    int8_t* name; // NO PADDING IS REQUIRED!
    int16_t type;
    int16_t class;
    int32_t ttl;
    int16_t data_size;
    int8_t* data; // NO PADDING IS REQUIRED!

} dnsmsg_rr_t;

typedef struct dnsmsg {

    dnsmsg_header_t header;
    dnsmsg_question_t* questions;
    dnsmsg_rr_t* answers;
    dnsmsg_rr_t* authorities;
    dnsmsg_rr_t* additionals;

} dnsmsg_t;

#endif /* MESSAGE_H_ */
