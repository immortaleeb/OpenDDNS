/**
 * Functions to interpret and process DNS questions.
 * It reads through the buffer of a given size and binds it to a DNS message.
 */

#include "codec.h"

/**
 * Convert a buffer to a DNS message.
 */
dnsmsg_t interpret_question(char* buffer, ssize_t buffer_size) {
    int i, j;
    dnsmsg_t message;

    message.header.id = pop_int16(&buffer, &buffer_size);
    message.header.status_flags = pop_int16(&buffer, &buffer_size);
    message.header.query_count = pop_int16(&buffer, &buffer_size);
    message.header.answer_count = pop_int16(&buffer, &buffer_size);
    message.header.authority_count = pop_int16(&buffer, &buffer_size);
    message.header.additional_count = pop_int16(&buffer, &buffer_size);

    message.questions = malloc(sizeof(dnsmsg_question_t) * message.header.query_count);
    for(i = 0; i < message.header.query_count; i++) {
        message.questions[i].name_size = pop_int8(&buffer, &buffer_size);
        message.questions[i].name = calloc(sizeof(int8_t), message.questions[i].name_size);
        for(j = 0; j < message.questions[i].name_size; j++) {
            message.questions[i].name[j] = pop_int8(&buffer, &buffer_size);
        }
        message.questions[i].type = pop_int16(&buffer, &buffer_size);
        message.questions[i].class = pop_int16(&buffer, &buffer_size);
    }

    message.answers = interpret_rr(message.header.answer_count, &buffer, &buffer_size);
    message.authorities = interpret_rr(message.header.authority_count, &buffer, &buffer_size);
    message.additionals = interpret_rr(message.header.additional_count, &buffer, &buffer_size);

    return message;
}

/**
 * Read from a buffer to make a given amount of resource records.
 * This will reduce the buffer_size by the amount of bytes read from the buffer.
 * The buffer will also be reduced to the part without the read resource records.
 */
dnsmsg_rr_t* interpret_rr(int16_t amount, char** buffer, ssize_t* buffer_size) {
    int i, j;
    dnsmsg_rr_t* rr;

    rr = malloc(sizeof(dnsmsg_rr_t) * amount);
    for(i = 0; i < amount; i++) {
        rr[i].name_size = pop_int8(buffer, buffer_size);
        rr[i].name = malloc(sizeof(int8_t) * rr[i].name_size);
        for(j = 0; j < rr[i].name_size; j++) {
            rr[i].name[j] = pop_int8(buffer, buffer_size);
        }
        rr[i].type = pop_int16(buffer, buffer_size);
        rr[i].class = pop_int16(buffer, buffer_size);
        rr[i].ttl = pop_int32(buffer, buffer_size);
        rr[i].data_size = pop_int16(buffer, buffer_size);
        rr[i].data = malloc(sizeof(int8_t) * rr[i].data_size);
        for(j = 0; j < rr[i].data_size; j++) {
            rr[i].data[j] = pop_int8(buffer, buffer_size);
        }
    }

    return rr;
}

/**
 * Take the first 8 bits from the start of the buffer.
 * The buffer_size will then also be reduced by 1 byte.
 */
int8_t pop_int8(char** buffer, ssize_t* buffer_size) {
    int8_t value;
    value = *((*buffer)++);
    *buffer_size -= 1;
    if(*buffer_size < 0) {
        fprintf(stderr, "\nPopping from buffer outside of buffer size, something has gone wrong!");
    }
    return value;
}

/**
 * Take the first 16 bits from the start of the buffer.
 * The buffer_size will then also be reduced by 2 bytes.
 */
int16_t pop_int16(char** buffer, ssize_t* buffer_size) {
    return pop_int8(buffer, buffer_size) << 8 | pop_int8(buffer, buffer_size);
}

/**
 * Take the first 32 bits from the start of the buffer.
 * The buffer_size will then also be reduced by 4 bytes.
 */
int32_t pop_int32(char** buffer, ssize_t* buffer_size) {
    return pop_int16(buffer, buffer_size) << 16 | pop_int16(buffer, buffer_size);
}

/**
 * Serialize a given message to a buffer and also store the size of the constructed buffer.
 * This function will allocate the required space itself.
 * This won't do any checks for whether the buffer size exceeds the maximum datagram size
 * for DNS packets, that's your own damn responsibility!
 */
void serialize_message(dnsmsg_t message, char** buffer, ssize_t* buffer_size) {
    // The index will be incremented by one every place byte in the buffer.
    int i, j, index;
    *buffer_size = calc_message_size(message);
    *buffer = malloc(*buffer_size);

    // Go through the message to place it inside the buffer.
    index = 0;
    append_int16(buffer, &index, message.header.id);
    append_int16(buffer, &index, message.header.status_flags);
    append_int16(buffer, &index, message.header.query_count);
    append_int16(buffer, &index, message.header.answer_count);
    append_int16(buffer, &index, message.header.authority_count);
    append_int16(buffer, &index, message.header.additional_count);

    for(i = 0; i < message.header.query_count; i++) {
        append_int8(buffer, &index, message.questions[i].name_size);
        for(j = 0; j < message.questions[i].name_size; j++) {
            append_int8(buffer, &index, message.questions[i].name[j]);
        }
        append_int16(buffer, &index, message.questions[i].type);
        append_int16(buffer, &index, message.questions[i].class);
    }

    append_resource_records(buffer, &index, message.answers, message.header.answer_count);
    append_resource_records(buffer, &index, message.authorities, message.header.authority_count);
    append_resource_records(buffer, &index, message.additionals, message.header.additional_count);
}

/**
 * Calculate the full size of a message.
 * Meaning that dynamic arrays will also be followed and taken into account
 * for the size calculation.
 */
size_t calc_message_size(dnsmsg_t message) {
    int i;
    size_t size = sizeof(dnsmsg_t);

    // Referenced parts of question
    size += message.header.query_count * sizeof(dnsmsg_question_t);
    for(i = 0; i < message.header.query_count; i++) {
        size += message.questions[i].name_size * sizeof(int8_t);
    }

    size += calc_resource_records_size(message.answers, message.header.answer_count);
    size += calc_resource_records_size(message.authorities, message.header.authority_count);
    size += calc_resource_records_size(message.additionals, message.header.additional_count);

    return size;
}

/**
 * Calculate the full size of a list of resource records.
 */
size_t calc_resource_records_size(dnsmsg_rr_t* resource_records, int16_t amount) {
    int i;
    size_t size = amount * sizeof(dnsmsg_rr_t);

    for(i = 0; i < amount; i++) {
        size += resource_records[i].name_size * sizeof(int8_t);
        size += resource_records[i].data_size * sizeof(int8_t);
    }

    return size;
}

/**
 * Append 8 bits to the buffer and increment the index by one.
 */
void append_int8(char** buffer, int* index, int8_t value) {
    (*buffer)[(*index)++] = value;
}

/**
 * Append 16 bits to the buffer and increment the index by two.
 */
void append_int16(char** buffer, int* index, int16_t value) {
    append_int8(buffer, index, value >> 8);
    append_int8(buffer, index, value & 255);
}

/**
 * Append 32 bits to the buffer and increment the index by four.
 */
void append_int32(char** buffer, int* index, int32_t value) {
    append_int16(buffer, index, value >> 16);
    append_int16(buffer, index, value & 65535);
}

/**
 * Add a full list of resource records to the buffer and increment the index accordingly.
 */
void append_resource_records(char** buffer, int* index, dnsmsg_rr_t* resource_records,
        int16_t amount) {
    int i, j;
    for(i = 0; i < amount; i++) {
        append_int8(buffer, index, resource_records[i].name_size);
        for(j = 0; j < resource_records[i].name_size; j++) {
            append_int8(buffer, index, resource_records[i].name[j]);
        }
        append_int16(buffer, index, resource_records[i].type);
        append_int16(buffer, index, resource_records[i].class);
        append_int32(buffer, index, resource_records[i].ttl);
        append_int16(buffer, index, resource_records[i].data_size);
        for(j = 0; j < resource_records[i].data_size; j++) {
            append_int8(buffer, index, resource_records[i].data[j]);
        }
    }
}

/**
 * Encode header status flags into 16 bits.
 * QR: 1 bit; 0 if query, 1 if response.
 * OPCODE: 4 bits: 0 if query, 1 if inverse query, 2 if status request
 * AA: 1 bit: (only valid in responses) 1 if this is an authoritive answer
 * TC: 1 bit: if this message (ID) is truncated, should be 0 on the last of truncated message parts.
 * RD: 1 bit: if recursion is required, this bit is copied from query to response if the recursion
 * was denied.
 * RA: 1 bit: (only valid in responses) if this server can accept recursive requests.
 * RCODE: 4 bits: see errorcodes in codes.h
 */
int16_t encode_status_flags(int qr, int opcode, int aa, int tc, int rd, int rcode) {
    int z = 0; // Must always be zero
    return qr << 15 | opcode << 11 | aa << 10 | tc << 9 | rd << 8 | z << 4 | rcode;
}

/**
 * Decode 16 bits header status flags.
 * QR: 1 bit; 0 if query, 1 if response.
 * OPCODE: 4 bits: 0 if query, 1 if inverse query, 2 if status request
 * AA: 1 bit: (only valid in responses) 1 if this is an authoritive answer
 * TC: 1 bit: if this message (ID) is truncated, should be 0 on the last of truncated message parts.
 * RD: 1 bit: if recursion is required, this bit is copied from query to response if the recursion
 * was denied.
 * RA: 1 bit: (only valid in responses) if this server can accept recursive requests.
 * RCODE: 4 bits: see errorcodes in codes.h
 */
void decode_status_flags(int16_t status_flags , int* qr, int* opcode, int* aa, int* tc, int* rd, int* rcode) {
    *qr = (status_flags >> 15) & 1;
    *opcode = (status_flags >> 11) & 7;
    *aa = (status_flags >> 10) & 1;
    *tc = (status_flags >> 9) & 1;
    *rd = (status_flags >> 8) & 1;
    // status_flags >> 4 & 4; Would be the Z.
    *rcode = status_flags & 4;
}
