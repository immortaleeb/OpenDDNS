#include "message.h"
#include "codec.h"

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

/**
 * Print the labels in a question or resource record.
 */
void print_labels(dnsmsg_label_t* labels, uint16_t labels_size) {
    int i, j;

    for(i = 0; i < labels_size; i++) {
        for(j = 0; j < labels[i].name_size; j++) {
            printf("%c", labels[i].name[j]);
        }
        printf(".");
    }
}

/**
 * Print the given amount of resource records.
 */
void print_rr(dnsmsg_rr_t* rr, uint16_t rr_size) {
    int i, j;

    for(i = 0; i < rr_size; i++) {
        printf("<RR>\n");
        printf("\t<label>");
        print_labels(rr[i].labels, rr[i].labels_size);
        printf("<label>\n");
        printf("\t<type>%i</type>\n", rr[i].type);
        printf("\t<class>%i</class>\n", rr[i].class);
        printf("\t<ttl>%i</ttl>\n", rr[i].ttl);
        printf("\t<data>");
        for(j = 0; j < rr[i].data_size; j++) {
            // This print only works good for A requests.
            printf("%i.", rr[i].data[j]);
        }
        printf("</data>\n");
        printf("</RR>\n");
    }
}

/**
 * Print the given message to stdout.
 */
void print_message(dnsmsg_t message) {
    int i, qr, opcode, aa, tc, rd, ra, rcode;

    printf("------------------------------\n");
    printf("<HEADER>\n");
    printf("\t<id>%i</id>\n", message.header.id);
    decode_status_flags(message.header.status_flags, &qr, &opcode, &aa, &tc, &rd, &ra, &rcode);
    printf("\t<flags>\n");
    printf("\t\t<qr>%i</qr>\n", qr);
    printf("\t\t<opcode>%i</opcode>\n", opcode);
    printf("\t\t<aa>%i</aa>\n", aa);
    printf("\t\t<tc>%i</tc>\n", tc);
    printf("\t\t<rd>%i</rd>\n", rd);
    printf("\t\t<ra>%i</ra>\n", ra);
    printf("\t\t<rcode>%i</rcode>\n", rcode);
    printf("\t</flags>\n");
    printf("\t<query_count>%i</query_count>\n", message.header.query_count);
    printf("\t<answer_count>%i</answer_count>\n", message.header.answer_count);
    printf("\t<authority_count>%i</authority_count>\n", message.header.authority_count);
    printf("\t<additional_count>%i</additional_count>\n", message.header.additional_count);
    printf("</HEADER>\n");

    for(i = 0; i < message.header.query_count; i++) {
        printf("<QUESTION>\n");
        printf("\t<label>");
        print_labels(message.questions[i].labels, message.questions[i].labels_size);
        printf("<label>\n");
        printf("\t<type>%i</type>\n", message.questions[i].type);
        printf("\t<class>%i</class>\n", message.questions[i].class);
        printf("</QUESTION>\n");
    }

    printf("<ANSWERS>\n");
    print_rr(message.answers, message.header.answer_count);
    printf("</ANSWERS>\n");

    printf("<AUTHORITIES>\n");
    print_rr(message.authorities, message.header.authority_count);
    printf("</AUTHORITIES>\n");

    printf("<ADDITIONALS>\n");
    print_rr(message.additionals, message.header.additional_count);
    printf("</ADDITIONALS>\n");
    printf("------------------------------\n");

}
