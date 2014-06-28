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

typedef struct dnsmsg_t {

    dnsmsg_header_t header;
    dnsmsg_questions_t questions;
    dnsmsg_answers_t answers;
    dnsmsg_authorities_t authorities;
    dnsmsg_additionals_t additionals;

} dnsmsg_t;

typedef struct dnsmsg_header_t {

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

// Resource record
typedef struct dnsmsg_rr_t {

    int8_t name_size;
    int8_t* name; // NO PADDING IS REQUIRED!
    int16_t type;
    int16_t class;
    int32_t ttl;
    int16_t data_size;
    int8_t* data; // NO PADDING IS REQUIRED!

} dnsmsg_rr_t;

// Questions to NS
typedef struct dnsmsg_questions_t {

    dnsmsg_question_t* questions;

} dnsmsg_questions_t;

typedef struct dnsmsg_question_t {

    int8_t name_size;
    int8_t* name; // NO PADDING IS REQUIRED!
    int16_t type;
    int16_t class;

} dnsmsg_question_t;

// RR's answering the question
typedef struct dnsmsg_answers_t {

    dnsmsg_rr_t* answers;

} dnsmsg_answers_t;

// RR's pointing towards an authority
typedef struct dnsmsg_authorities_t {

    dnsmsg_rr_t* authorities;

} dnsmsg_authorities_t;

// RR's with additional information
typedef struct dnsmsg_additionals_t {

    dnsmsg_rr_t* additionals;

} dnsmsg_additionals_t;

#endif /* MESSAGE_H_ */
