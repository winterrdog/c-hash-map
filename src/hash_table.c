#include "hash_table.h"

static const int HT_PRIME_1 = 151;
static const int HT_PRIME_2 = 163;

// sentinel value to mean item was deleted
static ht_item HT_DELETED_ITEM = {
    .key = NULL,
    .value = NULL
};

static const size_t HT_INITIAL_BASE_SIZE = 50;

static ht_item* ht_new_item(cstr key, cstr value)
{
    ht_item* item = xmalloc(sizeof(ht_item));
    item->key = xstrdup(key);
    item->value = xstrdup(value);
    return item;
}

static inline int is_item_deleted(const ht_item* i)
{
    return i == &HT_DELETED_ITEM;
}

static int validate_index(ht_hash_table* table, int index)
{
    return index >= 0 && index < table->size;
}

static inline int is_item_present(const ht_item* i)
{
    return i && !is_item_deleted(i);
}

static void ht_del_item(const ht_item* item)
{
    xfree((void*)item->value);
    xfree((void*)item->key);
    xfree((void*)item);
}

static int ht_hash(cstr key, const int prime_number, const int buckets)
{
    long hash = 0;
    size_t len = strlen(key);
    for (size_t i = 0; i != len; ++i) {
        hash += ((long)pow(prime_number, len - (i + 1)) * key[i]);
        hash = hash % buckets;
    }

    return (int)hash;
}

static int ht_get_hash(cstr key, const int buckets, const int attempts)
{
    const int hash_a = ht_hash(key, HT_PRIME_1, buckets);
    const int hash_b = ht_hash(key, HT_PRIME_2, buckets);

    return (hash_a + (attempts * (hash_b + 1))) % buckets;
}

static ht_hash_table* ht_new_sized(const size_t base_size)
{
    ht_hash_table* table = xmalloc(sizeof(ht_hash_table));

    table->base_size = base_size, table->count = 0, table->size = next_prime(base_size);
    table->items = xcalloc(table->size, sizeof(ht_item*));
    return table;
}

static void ht_resize(ht_hash_table* curr_table, const size_t base_size)
{
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }

    const ht_item* item;
    ht_hash_table* new_table = ht_new_sized(base_size);
    for (size_t i = 0; i != curr_table->size; ++i) {
        item = curr_table->items[i];
        if (is_item_present(item)) {
            ht_insert(new_table, item->key, item->value);
        }
    }

    curr_table->base_size = new_table->base_size, curr_table->count = new_table->count;

    // swap info between tables
    size_t tmp_size = curr_table->size;
    curr_table->size = new_table->size;
    new_table->size = tmp_size;

    const ht_item** tmp_items = curr_table->items;
    curr_table->items = new_table->items;
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
    if (ht_calc_table_load(table) >= 60) {
        ht_resize_up(table);
    }

    ht_item* item = ht_new_item(k, v);
    int index = ht_get_hash(k, table->size, 0);
    if (!validate_index(table, index)) {
        return;
    }
    const ht_item* curr_item = table->items[index];
    if (!is_item_present(curr_item)) {
        goto INSERT_INTO_TABLE;
    }

    // update key if it exists
    cstr found_value = ht_search(table, k);
    if (found_value) {
        ht_del_item(curr_item);
        table->items[index] = item;
        return;
    }

    int attempts = 1;
    size_t len_k = strlen(k);
    while (curr_item) {
        index = ht_get_hash(k, table->size, attempts);
        if (!validate_index(table, index)) {
            return;
        }

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
    const ht_item* item = table->items[index];
    if (!is_item_present(item) || !validate_index(table, index)) {
        return NULL;
    }

    int attempts = 1;
    size_t len_k = strlen(k);
    while (item) {
        if (!is_item_deleted(item) && STRINGS_EQUAL(item->key, k, len_k)) {
            return item->value;
        }
        index = ht_get_hash(k, table->size, attempts);
        if (!validate_index(table, index)) {
            break;
        }
        item = table->items[index];
        if (!is_item_present(item)) {
            break;
        }
        ++attempts;
    }

    return NULL;
}

void ht_delete(ht_hash_table* table, cstr k)
{
    if (ht_calc_table_load(table) < 20) {
        ht_resize_down(table);
    }

    // search for item
    cstr found_value = ht_search(table, k);
    if (!found_value) {
        return;
    }

    int index = ht_get_hash(k, table->size, 0);
    if (!validate_index(table, index)) {
        return;
    }
    ht_del_item(table->items[index]);
    table->items[index] = &HT_DELETED_ITEM;

    table->count--;
}