/**
 * Functions to construct possible answers to DNS queries.
 */

#include "answer.h"
#include "codec.h"

// TODO: make this value dependent on the DEBUG definition
#define HARD_TTL 10

/**
 * Make a reply message given a question message.
 * This will return null if an invalid question was supplied.
 */
dnsmsg_t make_reply(dnsmsg_t question_message) {
    int qr, opcode, aa, tc, rd, ra, rcode, return_error, search_ip_error_flag, send_tc;
    dnsmsg_t message;
    char* name = "testname";
    return_error = RCODE_NOERROR;
    send_tc = 0; // TODO: Truncation not yet implemented

    message.header.id = question_message.header.id;
    decode_status_flags(question_message.header.status_flags, &qr, &opcode, &aa, &tc, &rd, &ra, &rcode);
    if(!qr) {
        fprintf(stderr, "\nQuestion %i is not marked as question.", question_message.header.id);
        return_error = RCODE_FORMATERROR;
    }
    if(opcode != OPCODE_QUERY && !return_error) {
        fprintf(stderr, "\nQuestion %i did ask for something else than a query operation.",
                question_message.header.id);
        return_error = RCODE_FORMATERROR;
    }
    if(!tc && !return_error) {
        fprintf(stderr, "\nQuestion %i is truncated, ignore (not implemented yet).",
                question_message.header.id);
        return_error = RCODE_NOTIMPLEMENTED; // TODO
    }

    message.header.query_count = 0;
    message.header.answer_count = question_message.header.query_count;
    message.header.authority_count = 0;
    message.header.additional_count = 0;
    message.answers = make_rr_reply_all(question_message.questions,
            question_message.header.query_count, &search_ip_error_flag);
    if(search_ip_error_flag && !return_error) {
        return_error = RCODE_NAMEERROR;
    }
    message.header.status_flags = encode_status_flags(1, OPCODE_QUERY, 1, send_tc, rd, 0, return_error);

    return message;
}

/**
 * Make a resource record reply to a query.
 * The error flag will be set to 1 if no ip address can be found for the question domain.
 */
dnsmsg_rr_t make_rr_reply(dnsmsg_question_t question, int* error_flag) {
    dnsmsg_rr_t rr;
    rr.name_size = question.name_size;
    rr.name = malloc(rr.name_size * sizeof(int8_t));
    memcpy(rr.name, question.name, rr.name_size);
    rr.type = question.type;
    rr.class = question.class;

    // We only allow internet classes and A type requests for now.
    if (RR_CLASS_IN == question.class && (RR_TYPE_A == question.type
            || RR_TYPE_ANY == question.type)) {
        rr.ttl = get_ttl(question.name, question.name_size);
        rr.data_size = RR_TYPE_A_SIZE; // Hard-coded A type size.
        rr.data = get_ip(question.name, question.name_size, error_flag);
    }

    return rr;
}

/**
 * Make resource record replies for all the given queries.
 * The error flag will be set to 1 if no ip address can be found for the question domain.
 */
dnsmsg_rr_t* make_rr_reply_all(dnsmsg_question_t* question, int16_t amount, int* error_flag) {
    int i;

    dnsmsg_rr_t* rr = malloc(sizeof(dnsmsg_rr_t) * amount);
    for(i = 0; i < amount; i++) {
        rr[i] = make_rr_reply(question[i], error_flag);
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
 * The error flag will be set to 1 if no ip address can be found for the question domain.
 */
int8_t* get_ip(int8_t* name, int8_t name_size, int* error_flag) {
    int8_t* ip = calloc(sizeof(int8_t), 4);
    ip[0] = 192;
    ip[1] = 168;
    ip[2] = 3;
    ip[3] = 33;
    return ip;// TODO
}
