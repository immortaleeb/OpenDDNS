#ifndef CODEC_H_
#define CODEC_H_

#include <stdio.h>
#include <stdlib.h>
#include "message.h"

dnsmsg_t* interpret_question(char* buffer, ssize_t buffer_size);
int8_t pop_int8(char** buffer, ssize_t* buffer_size);
int16_t pop_int16(char** buffer, ssize_t* buffer_size);

void serialize_message(dnsmsg_t message, char** buffer, ssize_t* buffer_size);
size_t calc_message_size(dnsmsg_t message);
size_t calc_resource_records_size(dnsmsg_rr_t* resource_records, int16_t amount);
void append_int8(char** buffer, int* index, int8_t value);
void append_int16(char** buffer, int* index, int16_t value);
void append_int32(char** buffer, int* index, int16_t value);
void append_resource_records(char** buffer, int* index, dnsmsg_rr_t* resource_records,
        int16_t amount);

int16_t encode_status_flags(int qr, int opcode, int aa, int tc, int rd, int rcode);
void decode_status_flags(int16_t status_flags , int* qr, int* opcode, int* aa, int* tc, int* rd, int* rcode);

#endif /* CODEC_H_ */
