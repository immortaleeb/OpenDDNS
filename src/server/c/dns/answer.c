/**
 * Functions to construct possible answers to DNS queries.
 */

#include <stdio.h>
#include <stdlib.h>
#include "answer.h"
#include "codes.h"

// TODO: make this value dependent on the DEBUG definition
#define HARD_TTL 10

dnsmsg_rr_t* make_rr(dnsmsg_question_t* question) {
    dnsmsg_rr_t* rr = calloc(sizeof(dnsmsg_rr_t), 1);
    rr->name_size = question->name_size;
    rr->name = question->name;
    rr->type = question->type;
    rr->class = question->class;

    // We only allow internet classes and A type requests for now.
    if (RR_CLASS_IN == question->class && RR_TYPE_A == question->type) {
        rr->ttl = get_ttl(question->name, question->name_size);
        rr->data_size = RR_TYPE_A_SIZE; // Hard-coded A type size.
        rr->data = get_ip(question->name, question->name_size);
    }
    return 0;
}

/**
 * Get the TTL for a given name.
 */
int32_t get_ttl(int8_t* name, int8_t name_size) {
    return HARD_TTL;
}

/**
 * Get the IP-address for a given name.
 * This is always one unsigned 32-bit value.
 */
int8_t* get_ip(int8_t* name, int8_t name_size) {
    int8_t* ip = calloc(sizeof(int8_t), 4);
    ip[0] = 192;
    ip[1] = 168;
    ip[2] = 3;
    ip[3] = 33;
    return ip;// TODO
}
