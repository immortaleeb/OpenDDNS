#include "message.h"

/**
 * Free everything inside a given message.
 */
void free_message(dnsmsg_t message) {
    int i, j;

    for(i = 0; i < message.header.query_count; i++) {
        free(message.questions[i].name);

    }
    if(message.header.query_count) {
        free(message.questions);
    }

    free_rr(message.answers, message.header.answer_count);
    free_rr(message.authorities, message.header.authority_count);
    free_rr(message.additionals, message.header.additional_count);
}

/**
 * Free everything inside a given resource record, including the allocated space for the rr itself.
 */
void free_rr(dnsmsg_rr_t* rr, uint16_t amount) {
    int i,j;

    for(i = 0; i < amount; i++) {
        free(rr[i].name);
        free(rr[i].data);
    }
    if(amount) {
        free(rr);
    }
}
