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
dnsmsg_t make_reply(dnsmsg_t question_message, dns_map* map) {
    int qr, opcode, aa, tc, rd, ra, rcode, return_error, search_ip_error_flag, send_tc;
    dnsmsg_t message;
    //char* name = "testname";
    search_ip_error_flag = 0;
    return_error = RCODE_NOERROR;
    send_tc = 0; // TODO: Truncation not yet implemented

    message.header.id = question_message.header.id;
    decode_status_flags(question_message.header.status_flags, &qr, &opcode, &aa, &tc, &rd, &ra, &rcode);
    if(qr) {
        fprintf(stderr, "\nQuestion %i is not marked as question.", question_message.header.id);
        return_error = RCODE_FORMATERROR;
    }
    if(opcode != OPCODE_QUERY && !return_error) {
        fprintf(stderr, "\nQuestion %i did ask for something else than a query operation.",
                question_message.header.id);
        return_error = RCODE_FORMATERROR;
    }
    if(tc && !return_error) {
        fprintf(stderr, "\nQuestion %i is truncated, ignore (not implemented yet).",
                question_message.header.id);
        return_error = RCODE_NOTIMPLEMENTED; // TODO
    }

    message.header.query_count = 0;
    message.header.answer_count = question_message.header.query_count;
    message.header.authority_count = 0;
    message.header.additional_count = 0;
    message.answers = make_rr_reply_all(question_message.questions,
            question_message.header.query_count, &search_ip_error_flag, map);
    if(search_ip_error_flag && !return_error) {
        fprintf(stderr, "\nQuestion %i triggered a name error.",
                question_message.header.id);
        return_error = RCODE_NAMEERROR;
    }
    message.header.status_flags = encode_status_flags(1, OPCODE_QUERY, 1, send_tc, rd, 0, return_error);

    return message;
}

/**
 * Make a resource record reply to a query.
 * The error flag will be set to 1 if no ip address can be found for the question domain.
 */
dnsmsg_rr_t make_rr_reply(dnsmsg_question_t question, int* error_flag, dns_map* map) {
    unsigned int i;
    dnsmsg_rr_t rr;

    rr.labels_size = question.labels_size;
    rr.labels = malloc(rr.labels_size * sizeof(dnsmsg_label_t));
    for(i = 0; i < rr.labels_size; i++) {
        rr.labels[i].name_size = question.labels[i].name_size;
        rr.labels[i].name = malloc(rr.labels[i].name_size * sizeof(uint8_t));
        memcpy(rr.labels[i].name, question.labels[i].name, rr.labels[i].name_size);
    }
    rr.type = question.type;
    rr.class = question.class;

    // We only allow internet classes and A type requests for now.
    if (RR_CLASS_IN == question.class && (RR_TYPE_A == question.type
            || RR_TYPE_ANY == question.type)) {
        rr.ttl = get_ttl(question.labels, question.labels_size);
        rr.data_size = RR_TYPE_A_SIZE; // Hard-coded A type size.
        rr.data = get_ip(question.labels, question.labels_size, error_flag, map);
    }

    return rr;
}

/**
 * Make resource record replies for all the given queries.
 * The error flag will be set to 1 if no ip address can be found for the question domain.
 */
dnsmsg_rr_t* make_rr_reply_all(dnsmsg_question_t* question, uint16_t amount, int* error_flag, dns_map* map) {
    int i;

    dnsmsg_rr_t* rr = malloc(sizeof(dnsmsg_rr_t) * amount);
    for(i = 0; i < amount; i++) {
        rr[i] = make_rr_reply(question[i], error_flag, map);
    }

    return rr;
}

/**
 * Get the TTL for a given name.
 */
uint32_t get_ttl(dnsmsg_label_t* labels, uint16_t labels_size) {
    return HARD_TTL;
}

/**
 * Convert a set of DNS labels to a dot-separated domain name.
 */
char* labels_to_domain(dnsmsg_label_t* labels, uint16_t labels_size) {
    unsigned int i, j, index;
    size_t size;
    char* domain;

    size = 0;
    for(i = 0; i < labels_size; i++) {
        size += labels[i].name_size + 1;
    }
    size--; // We don't need a '.' at the last position.

    domain = malloc(size * sizeof(char));

    index = 0;
    for(i = 0; i < labels_size; i++) {
        for(j = 0; j < labels[i].name_size; j++) {
            domain[index] = labels[i].name[j];
            index++;
        }
        domain[index] = 46; // A dot
        index++;
    }

    return domain;
}

/**
 * Get the IP-address for a given name.
 * This is always one unsigned 32-bit value.
 * The error flag will be set to 1 if no ip address can be found for the question domain.
 */
uint8_t* get_ip(dnsmsg_label_t* labels, uint16_t labels_size, int* error_flag, dns_map* map) {
    char *domain, *token;
    uint8_t* ip;

    token = malloc(TOKEN_SIZE * sizeof(char));
    ip = malloc(4 * sizeof(uint8_t));

    domain = labels_to_domain(labels, labels_size);
    token = dns_map_get_token_from_domain(map, domain);
    if(token) {
        ip = dns_map_get_ipv4_from_token(map, token);
        if(!ip) {
            *error_flag = 1;
            fprintf(stderr, "\nNo IP found for the given token.");
        }
    } else {
        *error_flag = 1;
        fprintf(stderr, "\nNo token found for the given domain.");
    }

    return ip;
}
