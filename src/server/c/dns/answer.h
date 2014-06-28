#ifndef ANSWER_H_
#define ANSWER_H_

#include "message.h"

dnsmsg_rr_t* make_rr(dnsmsg_question_t* question);
int32_t get_ttl(int8_t* name, int8_t name_size);
int8_t* get_ip(int8_t* name, int8_t name_size);

#endif /* ANSWER_H_ */
