#include "message.h"

/**
 * Free the given labels, including the allocated space for the labels itself.
 */
void free_labels(dnsmsg_label_t* labels, uint16_t labels_size) {
    unsigned int i;

    for(i = 0; i < labels_size; i++) {
        free(labels[i].name);
    }
    if(labels_size) {
        free(labels);
    }
}

/**
 * Free everything inside a given message.
 */
void free_message(dnsmsg_t message) {
    unsigned int i, j;

    for(i = 0; i < message.header.query_count; i++) {
        free_labels(message.questions[i].labels, message.questions[i].labels_size);

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
    unsigned int i,j;

    for(i = 0; i < amount; i++) {
        free_labels(rr[i].labels, rr[i].labels_size);
        // The current implementation does not need free-ing of this data, because this is stored in a global hashmap.
        //free(rr[i].data);
    }
    if(amount) {
        free(rr);
    }
}
