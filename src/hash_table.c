#include "hash_table.h"

// sentinel value to indicate that the item was deleted
static ht_item HT_DELETED_ITEM = {
    .key = NULL,
    .value = NULL,
};

static ht_item* ht_create_item(str k, str v)
{
    ht_item* ret = malloc(sizeof(ht_item));
    if (!ret)
        return NULL;

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

static ht_hash_table* ht_new_sized(const int base_size)
{
    ht_hash_table* table = malloc(sizeof(ht_hash_table));
    if (!table)
        return NULL;

    table->base_size = base_size,
    table->size = next_prime(base_size),
    table->count = 0,
    table->items = calloc(table->size, sizeof(ht_item*));
    if (!table->items) {
        fprintf(stderr, "failed to make room for the hash table's items due to low memory\n");
        return NULL;
    }

    return table;
}

ht_hash_table* ht_create_hash_table()
{
    ht_hash_table* table = ht_new_sized(HT_INITIAL_BASE_SIZE);
    if (!table) {
        fprintf(stderr, "failed to make a new hash table due to low memory\n");
        return NULL;
    }

    return table;
}

void ht_delete_hash_table(ht_hash_table* tab)
{
    ht_item* item = NULL;
    for (int i = 0; i != tab->size; ++i) {
        item = tab->items[i];
        if (item)
            ht_delete_item(item);
    }

    free_space(tab->items);
    free_space(tab);

    return;
}

static int ht_hash(str key, const int prime_number, const int bucket_len)
{
    const int str_len = strlen(key);
    long hash = 0;

    for (int i = 0; i < str_len; ++i) {
        // for every character, get it's 'even' weight and value to convert it into large integer
        hash += ((long)pow(prime_number, str_len - (i + 1))) * key[i];

        // keep in range
        hash = hash % bucket_len;
    }

    return (int)hash;
}

// this function handles collisions via double hashing
static int ht_get_hash(str key, const int buckets, const int attempts)
{
    int hash = 0;

    const int hash_a = ht_hash(key, HT_PRIME_1, buckets);
    const int hash_b = ht_hash(key, HT_PRIME_2, buckets);
    hash = (hash_a + (attempts * (hash_b + 1))) % buckets;

    return hash;
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
    if (!curr_item || curr_item == &HT_DELETED_ITEM)
        goto INSERT_INTO_TABLE;

    // search for an empty entry
    int i = 1;
    size_t key_len = strlen(key);
    while (curr_item) {
        index = ht_get_hash(curr_item->key, table->size, i);
        curr_item = ht_find_entry(table, index);
        if (curr_item == &HT_DELETED_ITEM)
            goto INSERT_INTO_TABLE;

        // if it's an update
        if (strncmp(curr_item->key, key, key_len) == 0) {
            ht_delete_item(curr_item);
            table->items[index] = item;
            return;
        }

        ++i;
    }

INSERT_INTO_TABLE:
    table->items[index] = item;
    table->count++;
}

char* ht_search(ht_hash_table* table, str key)
{
    int index = ht_get_hash(key, table->size, 0);
    ht_item* item = ht_find_entry(table, index);
    if (!item || item == &HT_DELETED_ITEM)
        return NULL;

    int i = 1;
    size_t key_len = strlen(key);
    while (item) {
        if (strncmp(item->key, key, key_len) == 0)
            return item->value;

        index = ht_get_hash(key, table->size, i);
        item = ht_find_entry(table, index);
        if (!item || item == &HT_DELETED_ITEM)
            return NULL;

        item = table->items[index];
        ++i;
    }
}

void ht_delete(ht_hash_table* table, str key)
{
    int index = ht_get_hash(key, table->size, 0);
    ht_item* item = ht_find_entry(table, index);
    if (!item) {
        fprintf(stderr, "item with key, %s, was not found!\n", key);
        return;
    }

    int i = 1;
    size_t key_len = strlen(key);
    while (item) {
        if (item != &HT_DELETED_ITEM) {
            if (strncmp(item->key, key, key_len) == 0) {
                ht_delete_item(item);
                table->items[index] = &HT_DELETED_ITEM;
            }
        }

        index = ht_get_hash(key, table->size, i);
        item = ht_find_entry(table, index);
        if (!item) {
            fprintf(stderr, "item with key, %s, was not found!\n", key);
            return;
        }

        ++i;
    }

    table->count--;
}

void free_space(void* ptr)
{
    if (!ptr)
        return;

    free(ptr), ptr = NULL;
}
