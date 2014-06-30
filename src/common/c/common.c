#include "common.h"

void print_ipv4(FILE* f, uint8_t* ip) {
    int i;
    fprintf(f, "%d", ip[0]);
    for (i = 1; i < 4; ++i) {
        fprintf(f, ".%d", ip[i]);
    }   
}
