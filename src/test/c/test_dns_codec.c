/**
 * Test the coding and decoding of DNS messages.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <assert.h>
#include "test_dns_codec.h"

/**
 * Make a dummy message with some valid content, queries and answers can be mixed.
 */
dnsmsg_t make_message() {
    int i;
    dnsmsg_t message;
    char* name = "testname";

    message.header.id = 257;
    message.header.status_flags = encode_status_flags(0, OPCODE_QUERY, 0, 0, 0, RCODE_NOERROR);
    message.header.query_count = 2;
    message.header.answer_count = 2;
    message.header.authority_count = 2;
    message.header.additional_count = 2;

    message.questions = calloc(sizeof(dnsmsg_question_t), message.header.query_count);
    for(i = 0; i < message.header.query_count; i++) {
        message.questions[i].name_size = strlen(name) + 1;
        message.questions[i].name = malloc(sizeof(char) * message.questions[i].name_size);
        strcpy((char*) message.questions[i].name, name);
        message.questions[i].type = RR_TYPE_A;
        message.questions[i].class = RR_CLASS_IN;
    }

    message.answers = make_rr_reply_all(message.questions, message.header.query_count);
    message.authorities = make_rr_reply_all(message.questions, message.header.query_count);
    message.additionals = make_rr_reply_all(message.questions, message.header.query_count);

    return message;
}

/**
 * Compare two resource records with assertions.
 */
void compare_rr(dnsmsg_rr_t* rr_1, dnsmsg_rr_t* rr_2, int16_t amount) {
    int i, j;

    for(i = 0; i < amount; i++) {
        assert(rr_1[i].name_size == rr_2[i].name_size);
        for(j = 0; j < rr_1[i].name_size; j++) {
            assert(rr_1[i].name[j] == rr_2[i].name[j]);
        }
        assert(rr_1[i].type == rr_2[i].type);
        assert(rr_1[i].class == rr_2[i].class);
        assert(rr_1[i].ttl == rr_2[i].ttl);
        assert(rr_1[i].data_size == rr_2[i].data_size);
        for(j = 0; j < rr_1[i].data_size; j++) {
            assert(rr_1[i].data[j] == rr_2[i].data[j]);
        }
    }

}

/**
 * Compare two DNS messages with assertions.
 */
void compare_messages(dnsmsg_t message_1, dnsmsg_t message_2) {
    int i, j;

    assert(message_1.header.id == message_2.header.id);
    assert(message_1.header.status_flags == message_2.header.status_flags);
    assert(message_1.header.query_count == message_2.header.query_count);
    assert(message_1.header.answer_count == message_2.header.answer_count);
    assert(message_1.header.authority_count == message_2.header.authority_count);
    assert(message_1.header.additional_count == message_2.header.additional_count);

    for(i = 0; i < message_1.header.query_count; i++) {
        assert(message_1.questions[i].name_size == message_2.questions[i].name_size);
        for(j = 0; j < message_1.questions[i].name_size; j++) {
            assert(message_1.questions[i].name[j] == message_2.questions[i].name[j]);
        }
        assert(message_1.questions[i].type == message_2.questions[i].type);
        assert(message_1.questions[i].class == message_2.questions[i].class);
    }

    compare_rr(message_1.answers, message_2.answers, message_1.header.answer_count);
    compare_rr(message_1.authorities, message_2.authorities, message_1.header.authority_count);
    compare_rr(message_1.additionals, message_2.additionals, message_1.header.additional_count);
}

int main(int argc, const char* argv[]) {
    dnsmsg_t message, decoded_message;
    char* buffer;
    ssize_t buffer_size;

    message = make_message();
    serialize_message(message, &buffer, &buffer_size);
    fprintf(stdout, "Serialized to %ld bytes.\n", buffer_size);
    decoded_message = interpret_question(buffer, buffer_size);

    fprintf(stdout, "Comparing messages.\n");
    compare_messages(message, decoded_message);
    fprintf(stdout, "Compared messages.\n");

    free_message(message);
    free_message(decoded_message);
}
