#ifndef DNS_MAP_H_
#define DNS_MAP_H_

#include "hash_map.h"
#include <stdint.h>
#include <stdio.h>

typedef struct dns_map {
    hash_map* domain_to_token;
    hash_map* token_to_ip;
} dns_map;

dns_map* create_dns_map(FILE* h_domain2token, FILE* h_token2ip);

int8_t* dns_map_get_ipv4_from_token(dns_map* map, const char* token);

char* dns_map_get_token_from_domain(dns_map* map, const char* domain);

int8_t* dns_map_get_ipv4_from_domain(dns_map* map, const char* domain);

int8_t* dns_map_put_ipv4(dns_map* map, const char* token, int8_t* ip);

char* dns_map_put_token(dns_map* map, const char* domain, char* token);

void destroy_dns_map(dns_map* map);

#endif
