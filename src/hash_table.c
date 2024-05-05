#include "hash_table.h"

static const int HT_PRIME_1 = 151;
static const int HT_PRIME_2 = 163;

static const int RESIZE_UP_LOAD_LEVEL = 70;
static const int RESIZE_DOWN_LOAD_LEVEL = 20;

// sentinel value to mean item was deleted
static ht_item HT_DELETED_ITEM = {
    .key = NULL,
    .value = NULL
};

static const size_t HT_INITIAL_BASE_SIZE = 50;

static inline int is_item_deleted(const ht_item* i)
{
    return i == &HT_DELETED_ITEM;
}

static inline int is_item_present(const ht_item* i)
{
    return i && !is_item_deleted(i);
}

static ht_item* ht_new_item(cstr key, cstr value)
{
    ht_item* item = xmalloc(sizeof(ht_item));
    item->key = xstrdup(key), item->value = xstrdup(value);
    return item;
}

static void ht_del_item(const ht_item* item)
{
    xfree((void*)item->value), xfree((void*)item->key);
    xfree((void*)item);
}

static int ht_hash(cstr key, const int prime_number, const int buckets)
{
    // get length of string
    cstr c = key;
    size_t len = 0;
    while (*c++) {
        len++;
    }

    // calculate hash
    long hash = 0;
    int i = 0;
    for (c = key; *c != 0; ++c, ++i) {
        hash += ((long)pow(prime_number, len - (i + 1)) * (*c));
        hash = hash % buckets;
    }

    return (int)hash;
}

static int ht_get_hash(cstr key, const int buckets, const int attempts)
{
    // use double hashing for getting hash
    const int hash_a = ht_hash(key, HT_PRIME_1, buckets);
    int hash_b = ht_hash(key, HT_PRIME_2, buckets);
    if (!hash_b || hash_b % buckets == 0) { // handle cases hash_b == 0 || hash_b == buckets
        hash_b = 1;
    }
    const int hash = (hash_a + (attempts * hash_b));

    return hash % buckets;
}

static ht_hash_table* ht_new_sized(const size_t base_size)
{
    ht_hash_table* table = xmalloc(sizeof(ht_hash_table));

    table->base_size = base_size, table->count = 0, table->size = next_prime(base_size);
    table->items = xcalloc(table->size, sizeof(ht_item*));

    return table;
}

static void ht_resize(ht_hash_table* old_table, const size_t base_size)
{
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }

    // move items from old table to new table
    const ht_item* item;
    ht_hash_table* new_table = ht_new_sized(base_size);
    for (size_t i = 0; i != old_table->size; ++i) {
        item = old_table->items[i];
        if (is_item_present(item)) {
            ht_insert(new_table, item->key, item->value);
        }
    }

    // transfer new table's data to old table
    old_table->base_size = new_table->base_size, old_table->count = new_table->count;

    // swap info between tables
    old_table->size ^= new_table->size;
    new_table->size ^= old_table->size;
    old_table->size ^= new_table->size;

    ht_item** tmp_items = old_table->items;
    old_table->items = new_table->items;
    new_table->items = tmp_items;

    ht_del_hash_table(new_table);
}

static void ht_resize_up(ht_hash_table* t)
{
    const size_t new_size = 2 * t->base_size;
    ht_resize(t, new_size);
}

static void ht_resize_down(ht_hash_table* t)
{
    const size_t new_size = (size_t)(t->base_size / 2);
    ht_resize(t, new_size);
}

static int ht_calc_table_load(ht_hash_table* t)
{
    return (int)((t->count * 100) / t->size);
}

ht_hash_table* ht_new()
{
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

void ht_del_hash_table(ht_hash_table* table)
{
    const ht_item* item = NULL;
    for (size_t i = 0; i != table->size; ++i) {
        item = table->items[i];
        if (item) {
            ht_del_item(item);
        }
    }

    xfree(table->items), xfree(table);
}

void ht_insert(ht_hash_table* table, cstr k, cstr v)
{
    // create more room if above ceiling
    if (ht_calc_table_load(table) >= RESIZE_UP_LOAD_LEVEL) {
        ht_resize_up(table);
    }

    ht_item* item = ht_new_item(k, v);
    int index = ht_get_hash(k, table->size, 0);

    const ht_item* curr_item = table->items[index];
    if (!is_item_present(curr_item)) {
        goto INSERT_INTO_TABLE;
    }

    // look for an empty or deleted spot to place the new item
    int attempts = 1;
    size_t len_k = strlen(k);
    while (curr_item) {
        // update key if it exists
        if (is_item_present(curr_item) && STRINGS_EQUAL(curr_item->key, k, len_k)) {
            ht_del_item(curr_item);
            table->items[index] = item;
            return;
        }

        index = ht_get_hash(k, table->size, attempts);
        curr_item = table->items[index];
        if (!is_item_present(curr_item)) {
            break;
        }

        ++attempts;
    }

INSERT_INTO_TABLE:
    table->items[index] = item, table->count++;
}

cstr ht_search(ht_hash_table* table, cstr k)
{
    int index = ht_get_hash(k, table->size, 0);
    const ht_item* curr_item = table->items[index];
    if (!is_item_present(curr_item)) {
        return NULL;
    }

    // search for key's index in table
    int attempts = 1;
    size_t len_k = strlen(k);
    while (curr_item) {
        if (!is_item_deleted(curr_item) && STRINGS_EQUAL(curr_item->key, k, len_k)) {
            return curr_item->value;
        }

        index = ht_get_hash(k, table->size, attempts);
        curr_item = table->items[index];
        if (!is_item_present(curr_item)) {
            break;
        }

        ++attempts;
    }

    return NULL;
}

void ht_delete(ht_hash_table* table, cstr k)
{
    if (ht_calc_table_load(table) < RESIZE_DOWN_LOAD_LEVEL) {
        ht_resize_down(table);
    }

    // search for item
    cstr found_value = ht_search(table, k);
    if (!found_value) {
        return;
    }

    int index = ht_get_hash(k, table->size, 0);

    // delete item
    ht_del_item(table->items[index]);
    table->items[index] = &HT_DELETED_ITEM;

    table->count--;
}

inline size_t ht_size(ht_hash_table* table)
{
    return table->count;
}