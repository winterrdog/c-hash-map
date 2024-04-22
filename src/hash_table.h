#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HT_PRIME_1 151
#define HT_PRIME_2 163

// type aliases
typedef struct {
    char* key;
    char* value;
} ht_item;
typedef struct {
    size_t size;
    size_t count;
    ht_item** items;
} ht_hash_table;
typedef const char* str;

// function prototypes
ht_hash_table* ht_create_hash_table(void);
void ht_delete_hash_table(ht_hash_table* tab);
void ht_insert(ht_hash_table* table, str key, str value);
char* ht_search(ht_hash_table* table, str key);
void ht_delete(ht_hash_table* table, str key);
void free_space(void* ptr);
