#include "dns_map.h"
#include <string.h>
#include <stdlib.h>

int str_cmp(void* p1, void* p2) {
    return strcmp((char*) p1, (char*) p2);
}

int read_string(FILE* handle, char** str) {
    int32_t str_len;

    // Read the length of the string
    if (fread(&str_len, sizeof(int32_t), 1, handle) != 1) return 0;

    // Read and allocate the actual string
    *str = (char*) malloc(sizeof(char) * (str_len + 1));
    if (fread(*str, sizeof(char), str_len, handle) != str_len) return 0;

    // Make sure the string is \0-terminated
    (*str)[str_len] = '\0';

    return 1;
}

int write_string(FILE* handle, const char* str) {
    int32_t str_len = (int32_t) strlen(str);

    // Write the length of the string
    if (fwrite(&str_len, sizeof(int32_t), 1, handle) != 1) return 0;

    // Write the actual string
    if (fwrite(str, sizeof(char), str_len, handle) != str_len) return 0;

    return 1;
}

int read_ipv4(FILE* handle, int8_t** ip) {

    *ip = (int8_t*) malloc(sizeof(int8_t) * 4);
    if (fread(*ip, sizeof(int8_t), 4, handle) != 4) return 0;

    return 1;
}

int write_ipv4(FILE* handle, int8_t* ip) {

    if (fwrite(ip, sizeof(int8_t), 4, handle) != 4) return 0;

    return 1;
}

int read_domain_token_entry(dns_map* map, FILE* h_domain2token) {
    char* domain;
    char* token;

    // Read domain
    if (!read_string(h_domain2token, &domain))
        return 0;

    // Read token
    if (!read_string(h_domain2token, &token))
        return 0;

    // Insert data into the map
    dns_map_put_token(map, domain, token, 0);

    return 1;
}

int write_domain_token_entry(FILE* h_domain2token, const char* domain, const char* token) {

    if (!write_string(h_domain2token, domain))
        return 0;

    if (!write_string(h_domain2token, token))
        return 0;

    return 1;
}

int read_token_ip_entry(dns_map* map, FILE* h_token2ip) {
    char* token;
    int8_t* ip;

    // Read token
    if (!read_string(h_token2ip, &token))
        return 0;

    // Read ip
    if (!read_ipv4(h_token2ip, &ip))
        return 0;

    // Insert data into the map
    dns_map_put_ipv4(map, token, ip, 0);

    return 1;
}

int write_token_ip_entry(FILE* h_token2ip, const char* token, int8_t* ip) {

    if (!write_string(h_token2ip, token))
        return 0;

    if (!write_ipv4(h_token2ip, ip))
        return 0;

    return 1;
}

void read_files(dns_map* map, FILE* h_domain2token, FILE* h_token2ip) {
    long file_size = 0;

    fseek(h_domain2token, 0L, SEEK_END);
    file_size = ftell(h_domain2token);
    fseek(h_domain2token, 0L, SEEK_SET);
    if (file_size > 0L) {
        while (read_domain_token_entry(map, h_domain2token));
    }

    fseek(h_token2ip, 0L, SEEK_END);
    file_size = ftell(h_token2ip);
    fseek(h_token2ip, 0L, SEEK_SET);
    if (file_size > 0L) {
        while (read_token_ip_entry(map, h_token2ip));
    }
}

dns_map* create_dns_map(FILE* h_domain2token, FILE* h_token2ip) {
    dns_map* map = (dns_map*) malloc(sizeof(dns_map));

    map->domain_to_token = create_hash_map(str_cmp);
    map->token_to_ip = create_hash_map(str_cmp);

    map->h_domain2token = h_domain2token;
    map->h_token2ip = h_token2ip;

    // Read data from files
    read_files(map, h_domain2token, h_token2ip);

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

int8_t* dns_map_put_ipv4(dns_map* map, const char* token, int8_t* ip, int flag_update) {
    int8_t* old_ip = hash_map_put(map->token_to_ip, token, strlen(token), ip);

    if (flag_update)
        write_token_ip_entry(map->h_token2ip, token, ip);

    return old_ip;
}

char* dns_map_put_token(dns_map* map, const char* domain, char* token, int flag_update) {
    char* old_token = hash_map_put(map->domain_to_token, domain, strlen(domain), token);

    if (flag_update)
        write_domain_token_entry(map->h_domain2token, domain, token);

    return old_token;
}

void destroy_dns_map(dns_map* map) {
    destroy_hash_map(map->domain_to_token);
    destroy_hash_map(map->token_to_ip);
}
