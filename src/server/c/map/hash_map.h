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

typedef struct {
    int index;
    hash_map_entry* entry;
    hash_map* map;
} hash_map_iterator;

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
 * Returns an iterator over the entries in the 
 * given hash_map.
 * If this function returns NULL, this means we
 * have reached the end of the hash_map entries.
 */
hash_map_iterator* hash_map_get_iterator(hash_map* map, hash_map_iterator* it);

/**
 * Destroys the hash_map, by freeing allocated memory.
 * If free_keys is set to 1, all keys inside the hash_map
 * will be freed as well, otherwise these are left as the
 * user's responsibility.
 * If free_vals is set to 1, all values inside the hash_map
 * will be freed as well, otherwise these are left as the
 * user's responsibility.
 */
void destroy_hash_map(hash_map* m, int free_keys, int free_vals);

#endif
