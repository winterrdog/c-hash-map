#ifndef HASH_TABLE_H
#define HASH_TABLE_H 1

#include "prime.h"
#include "xmalloc.h"
#include <math.h>
#include <stddef.h>
#include <string.h>

#define STRINGS_EQUAL(a, b, len_b) strncmp(a, b, len_b) == 0

typedef const char* cstr;
typedef struct
{
    cstr key;
    cstr value;
} ht_item;
typedef struct
{
    size_t size;
    size_t base_size;
    size_t count;
    const ht_item** items;
} ht_hash_table;

// prototypes
ht_hash_table* ht_new();
void ht_del_hash_table(ht_hash_table* t);
void ht_insert(ht_hash_table* table, cstr k, cstr v);
void ht_delete(ht_hash_table* table, cstr k);
cstr ht_search(ht_hash_table* table, cstr k);

#endif
