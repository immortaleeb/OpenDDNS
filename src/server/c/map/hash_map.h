#ifndef HASH_MAP_H_
#define HASH_MAP_H_

typedef struct hash_map_entry {
    void* key;
    void* value;
    int len;        // length of the key (used for hashing)
    struct hash_map_entry* next; 
} hash_map_entry;

typedef struct {
    hash_map_entry** entries;
    unsigned int size;
    unsigned int nentries;
    int (*cmp)(const void*, const void*);   // function to compare keys
} hash_map;

/**
 * Creates a new hash_map
 */
hash_map* create_hash_map();

/**
 * Insert a new entry into the hashmap.
 * Note: the len value indicates length of the key array
 * and is necessary for hash value calculations
 */
void* hash_map_put(hash_map* m, const void* key, int len, void* value);

/**
 * Returns the value for a given key stored in the hashmap.
 * Note: the len value indicates the length of the key
 * array and is necessary for hash value calculations
 */
void* hash_map_get(hash_map* m, const void* key, int len);

/**
 * Destroys the hash_map, by freeing allocated memory.
 * Note: this will not free the memory of any
 * allocated pointers saved in the keys or values,
 * this is the user's responsibility!
 */
void destroy_hash_map(hash_map* m);

#endif
