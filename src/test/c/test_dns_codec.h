#ifndef TEST_DNS_CODEC_H_
#define TEST_DNS_CODEC_H_

#include "../../server/c/dns/message.h"
#include "../../server/c/dns/codec.h"
#include "../../server/c/dns/codes.h"
#include "../../server/c/dns/answer.h"

dnsmsg_t make_message();
void compare_rr(dnsmsg_rr_t* rr_1, dnsmsg_rr_t* rr_2, int16_t amount);
void compare_messages(dnsmsg_t message_1, dnsmsg_t message_2);

int main(int argc, const char* argv[]);

#endif /* TEST_DNS_CODEC_H_ */
