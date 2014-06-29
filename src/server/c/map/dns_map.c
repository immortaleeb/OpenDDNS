#include "dns_map.h"
#include <string.h>
#include <stdlib.h>

int str_cmp(void* p1, void* p2) {
    return strcmp((char*) p1, (char*) p2);
}

dns_map* create_dns_map(FILE* h_domain2token, FILE* h_token2ip) {
    dns_map* map = (dns_map*) malloc(sizeof(dns_map));

    map->domain_to_token = create_hash_map(str_cmp);
    map->token_to_ip = create_hash_map(str_cmp);

    // TODO: read contents from file

    return map;
}

int8_t* dns_map_get_ipv4_from_token(dns_map* map, const char* token) {
    return (int8_t*) hash_map_get(map->token_to_ip, (void*) token, strlen(token));
}

char* dns_map_get_token_from_domain(dns_map* map, const char* domain) {
    return (char*) hash_map_get(map->domain_to_token, (void*) domain, strlen(domain));
}

int8_t* dns_map_get_ipv4_from_domain(dns_map* map, const char* domain) {
    char* token = dns_map_get_token_from_domain(map, domain);
    if (token == NULL)
        return NULL;

    return dns_map_get_ipv4_from_token(map, token);
}

int8_t* dns_map_put_ipv4(dns_map* map, const char* token, int8_t* ip) {
    return hash_map_put(map->token_to_ip, token, strlen(token), ip);
}

char* dns_map_put_token(dns_map* map, const char* domain, char* token) {
    return hash_map_put(map->domain_to_token, domain, strlen(domain), token);
}

void destroy_dns_map(dns_map* map) {
    destroy_hash_map(map->domain_to_token);
    destroy_hash_map(map->token_to_ip);
}
