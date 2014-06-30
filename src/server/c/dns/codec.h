#ifndef CODEC_H_
#define CODEC_H_

#include <stdio.h>
#include <stdlib.h>
#include "message.h"

dnsmsg_t interpret_question(unsigned char* buffer, ssize_t buffer_size, int* error_flag);
dnsmsg_rr_t* interpret_rr(uint16_t amount, unsigned char** buffer, ssize_t* buffer_size);
uint8_t pop_int8(unsigned char** buffer, ssize_t* buffer_size);
uint16_t pop_int16(unsigned char** buffer, ssize_t* buffer_size);
uint32_t pop_int32(unsigned char** buffer, ssize_t* buffer_size);

void serialize_message(dnsmsg_t message, unsigned char** buffer, ssize_t* buffer_size);
size_t calc_message_size(dnsmsg_t message);
size_t calc_resource_records_size(dnsmsg_rr_t* resource_records, uint16_t amount);
void append_int8(unsigned char** buffer, unsigned int* index, uint8_t value);
void append_int16(unsigned char** buffer, unsigned int* index, uint16_t value);
void append_int32(unsigned char** buffer, unsigned int* index, uint32_t value);
void append_resource_records(unsigned char** buffer, unsigned int* index, dnsmsg_rr_t* resource_records,
        uint16_t amount);

uint16_t encode_status_flags(int qr, int opcode, int aa, int tc, int rd, int ra, int rcode);
void decode_status_flags(uint16_t status_flags , int* qr, int* opcode, int* aa, int* tc, int* rd, int* ra, int* rcode);
int is_truncated(dnsmsg_t message);

#endif /* CODEC_H_ */
