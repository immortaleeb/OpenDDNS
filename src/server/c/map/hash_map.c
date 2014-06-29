#include "hash_map.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define INITIAL_SIZE 10
#define MAP_SEED 59

/**** Helper function declarations ****/
void init_hash_map(hash_map* m, unsigned int size);
hash_map_entry* create_entry(const void* key, const void* value, int len);

void* hash_map_put_entry(hash_map* m, hash_map_entry* entry);
void hash_map_rehash(hash_map* m, unsigned int new_size);

int calculate_hash(hash_map* m, const void* key, int len);

/****  Actual Implementation ****/
hash_map* create_hash_map(int (*cmp)(const void*, const void*)) {
    hash_map* m = (hash_map*) malloc(sizeof(hash_map));

    init_hash_map(m, INITIAL_SIZE);
    m->cmp = cmp;

    return m;
}

void* hash_map_put(hash_map* m, const void* key, int len, void* value) {
    hash_map_entry* entry = create_entry(key, value, len);

    return hash_map_put_entry(m, entry);
}

void* hash_map_put_entry(hash_map* m, hash_map_entry* entry) {
    int index;

    if (m->nentries >= m->size / 2) {
        hash_map_rehash(m, m->size * 2);
    }

    index = calculate_hash(m, entry->key, entry->len);

    if (m->entries[index] == NULL) {
        m->entries[index] = entry;
        m->nentries++;

        return NULL;
    } else {
        hash_map_entry* current;
        hash_map_entry* previous;
        int res;

        previous = m->entries[index];
        current = previous;

        // Special case in which we need to replace the first element
        res = m->cmp(current->key, entry->key);
        if (res > 0) {
            m->entries[index] = entry;
            entry->next = current;
            m->nentries++;

            return NULL;
        } else if (res == 0) {
            void* oldval = current->value;
            current->value = entry->value;
            free(entry);

            return oldval;
        }

        /**
         * Keep looping through the linked list until we find
         * the correct spot to put the element in
         */
        current = current->next;
        while (current != NULL) {
            res = m->cmp(current->key, entry->key);
            if (res > 0) {
                previous->next = entry;
                entry->next = current;
                m->nentries++;

                return NULL;
            } else if (res == 0) {
                void* oldval = current->value;
                current->value = entry->value;
                free(entry);

                return oldval;
            }

            previous = current;
            current = current->next;
        }

        // At the end of the linked list
        previous->next = entry;
        m->nentries++;

        return NULL;
    }
}

void* hash_map_get(hash_map* m, const void* key, int len) {
    int index = calculate_hash(m, key, len);

    if (m->entries[index] != NULL) {
        hash_map_entry* current = m->entries[index];

        while (current != NULL) {
            if (m->cmp(current->key, key) == 0)
                return current->value;

            current = current->next;
        }
    }

    return NULL;
}

void destroy_hash_map(hash_map* m) {
    int i;

    for (i = 0; i < m->size; i++) {
        hash_map_entry* entry = m->entries[i];

        while (entry != NULL) {
            hash_map_entry* temp = entry->next;
            /* to free or not to free?
            free(entry->key);
            free(entry->value);
            */
            free(entry);
            entry = temp;
        }
    }

    free(m->entries);
    free(m);
}

/**** Helper function implementations ****/
void init_hash_map(hash_map* m, unsigned int size) {
    m->nentries = 0;
    m->size = size;
    m->entries = (hash_map_entry**) malloc(sizeof(hash_map_entry*) * size);

    // Initialize everything to 0
    memset(m->entries, 0, sizeof(hash_map_entry*) * m->size);
}

hash_map_entry* create_entry(const void* key, const void* value, int len) {
    hash_map_entry* entry = (hash_map_entry*) malloc(sizeof(hash_map_entry));

    entry->key = (void*) key;
    entry->value = (void*) value;
    entry->len = len;
    entry->next = NULL;

    return entry;
}

void hash_map_rehash(hash_map* m, unsigned int size) {
    hash_map_entry** old_entries = m->entries;
    unsigned int old_size = m->size;
    int i;

    // Create new hash_map entries
    init_hash_map(m, size);

    // Insert all old elements in the new hash_map
    i = 0;
    for (i = 0; i < old_size; i++) {
        hash_map_entry* entry = old_entries[i];

        while (entry != NULL) {
            hash_map_entry* temp = entry->next;
            
            entry->next = NULL;
            hash_map_put_entry(m, entry);

            entry = temp;
        }
    }

    free(old_entries);
}

/**** Hash function implementation ****/

/**
 * Murmur3 hash function, based on code
 * written by Austin Appleby
 * https://github.com/PeterScott/murmur3/blob/master/murmur3.c
 */

#ifdef __GNUC__
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE
#endif

#define getblock(p, i) (p[i])
#define ROTL32(x,y) rotl32(x,y)

static inline FORCE_INLINE uint32_t rotl32 ( uint32_t x, int8_t r )
{
      return (x << r) | (x >> (32 - r));
}

static inline FORCE_INLINE uint32_t fmix32 ( uint32_t h )
{
      h ^= h >> 16;
      h *= 0x85ebca6b;
      h ^= h >> 13;
      h *= 0xc2b2ae35;
      h ^= h >> 16;

      return h;
}

int calculate_hash(hash_map* m, const void* key, int len) {
    uint32_t seed = MAP_SEED;
    const uint8_t* data = (const uint8_t*) key;
    const int nblocks = len / 4;
    int i;

    uint32_t h1 = seed;
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;

    // Body
    {
        const uint32_t* blocks = (const uint32_t *)(data + nblocks*4);

        for (i = -nblocks; i; i++) {
            uint32_t k1 = getblock(blocks, i);

            k1 *= c1;
            k1 = ROTL32(k1, 15);
            k1 *= c2;

            h1 ^= k1;
            h1 = ROTL32(h1, 13);
            h1 = h1*5+0xe6546b64;
        }
    }

    // Tail
    {
        const uint8_t* tail = (const uint8_t*)(data + nblocks*4);
        uint32_t k1 = 0;

        switch(len & 3) {
            case 3: k1 ^= tail[2] << 16;
            case 2: k1 ^= tail[1] << 8;
            case 1: k1 ^= tail[0];
                    k1 *= c1; k1 = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;
        }
    }

    // Finalization
    {
        h1 ^= len;
        h1 = fmix32(h1);

        return h1 % m->size;
    }
}
