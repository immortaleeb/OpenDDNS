#ifndef THREAD_ARGS_H_
#define THREAD_ARGS_H_

typedef struct thread_args {
    int (*has_finished)(void);
    void (*set_finished)(void);
} thread_args_t;

#endif
