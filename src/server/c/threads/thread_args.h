#ifndef THREAD_ARGS_H_
#define THREAD_ARGS_H_

#include "../map/dns_map.h"

typedef struct thread_args {
    int (*has_finished)(void);
    void (*set_finished)(void);
    dns_map* map;
} thread_args_t;

#endif
