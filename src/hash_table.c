#include "hash_table.h"

static ht_item* ht_create_item(const char* k, const char* v)
{
    ht_item* ret = malloc(sizeof(ht_item));
    if (!ret) {
        return NULL;
    }

    ret->key = strndup(k, strlen(k));
    ret->value = strndup(v, strlen(v));

    return ret;
}

static void ht_delete_item(ht_item* item)
{
    free_space(item->value);
    free_space(item->key);
    free_space(item);

    return;
}

ht_hash_table* ht_create_hash_table()
{
    ht_hash_table* ret = malloc(sizeof(ht_hash_table));
    if (!ret) {
        return NULL;
    }

    ret->count = 0;
    ret->size = 53;
    ret->items = calloc(ret->size, sizeof(ht_item*));

    return ret;
}

void ht_delete_hash_table(ht_hash_table* tab)
{
    for (int i = 0; i != tab->size; ++i) {
        ht_item* item = tab->items[i];
        if (item)
            ht_delete_item(item);
    }

    free_space(tab->items);
    free_space(tab);

    return;
}

static ht_item* ht_find_entry(ht_hash_table* table, int index)
{
    ht_item* item = table->items[index];
    return item ? item : NULL;
}

void ht_insert(ht_hash_table* table, str key, str value)
{
    ht_item* item = ht_create_item(key, value);
    if (!item) {
        fprintf(stderr, "failed to insert item due to low memory\n");
        return;
    }

    int index = ht_get_hash(key, table->size, 0);
    ht_item* curr_item = ht_find_entry(table, index);
    // check if index entry is empty
    if (!curr_item)
        goto INSERT_INTO_TABLE;

    // search for an empty entry
    int i = 1;
    while (curr_item) {
        index = ht_get_hash(curr_item->key, table->size, i);
        curr_item = ht_find_entry(table, index);
        ++i;
    }

INSERT_INTO_TABLE:
    table->items[index] = item;
    table->count++;
}

void free_space(void* ptr)
{
    if (!ptr) {
        return;
    }

    free(ptr);
    ptr = NULL;

    return;
}
