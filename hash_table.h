#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* key;
    char* value;
} ht_item;
typedef struct {
    size_t size;
    size_t count;
    ht_item** items;
} ht_hash_table;

// function prototypes
ht_hash_table* ht_create_hash_table();
void ht_delete_hash_table(ht_hash_table* tab);
void free_space(void* ptr);
