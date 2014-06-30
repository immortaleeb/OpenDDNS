#ifndef ANSWER_H_
#define ANSWER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codes.h"
#include "message.h"
#include "../map/dns_map.h"
#include "../../../common/c/common.h"

dnsmsg_t make_reply(dnsmsg_t question_message, dns_map* map);
dnsmsg_rr_t make_rr_reply(dnsmsg_question_t question, int* error_flag, dns_map* map);
dnsmsg_rr_t* make_rr_reply_all(dnsmsg_question_t* question, uint16_t amount, int* error_flag, dns_map* map);
uint32_t get_ttl(dnsmsg_label_t* labels, uint16_t labels_size);
char* labels_to_domain(dnsmsg_label_t* labels, uint16_t labels_size);
uint8_t* get_ip(dnsmsg_label_t* labels, uint16_t labels_size, int* error_flag, dns_map* map);

#endif /* ANSWER_H_ */
