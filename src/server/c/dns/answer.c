/**
 * Functions to construct possible answers to DNS queries.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "answer.h"
#include "codes.h"

// TODO: make this value dependent on the DEBUG definition
#define HARD_TTL 10

/**
 * Make a resource record reply to a query.
 */
dnsmsg_rr_t make_rr_reply(dnsmsg_question_t question) {
    dnsmsg_rr_t rr;
    rr.name_size = question.name_size;
    rr.name = malloc(rr.name_size * sizeof(int8_t));
    memcpy(rr.name, question.name, rr.name_size);
    rr.type = question.type;
    rr.class = question.class;

    // We only allow internet classes and A type requests for now.
    if (RR_CLASS_IN == question.class && RR_TYPE_A == question.type) {
        rr.ttl = get_ttl(question.name, question.name_size);
        rr.data_size = RR_TYPE_A_SIZE; // Hard-coded A type size.
        rr.data = get_ip(question.name, question.name_size);
    }

    return rr;
}

/**
 * Make resource record replies for all the given queries.
 */
dnsmsg_rr_t* make_rr_reply_all(dnsmsg_question_t* question, int16_t amount) {
    int i;

    dnsmsg_rr_t* rr = malloc(sizeof(dnsmsg_rr_t) * amount);
    for(i = 0; i < amount; i++) {
        rr[i] = make_rr_reply(question[i]);
    }

    return rr;
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
