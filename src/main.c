#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hash_table.h"
#include "xmalloc.h"

#define NUM_OF_ITEMS 300 // You can change this to what you wish

char* rand_word(int min_len, int max_len)
{
    int i, length;
    char c, *word;

    // seed random
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    /* using nano-seconds instead of seconds */
    srand((time_t)ts.tv_nsec);

    length = rand() % (max_len - min_len + 1) + min_len;
    word = xcalloc((size_t)(length + 1), sizeof(char));

    for (i = 0; i != length; i++) {
        c = 'a' + rand() % 26;
        word[i] = c;
    }

    // add NULL terminator
    word[i] = 0;

    return word;
}

int main()
{
    ht_hash_table* ht = ht_new();
    char **k_arr, **v_arr, *k, *v;

    k_arr = xcalloc(NUM_OF_ITEMS, sizeof(char*));
    v_arr = xcalloc(NUM_OF_ITEMS, sizeof(char*));

    for (int i = 0; i != NUM_OF_ITEMS; ++i) {
        k_arr[i] = rand_word(12, 45);
        v_arr[i] = rand_word(10, 100);
    }

    for (int i = 0; i != NUM_OF_ITEMS; ++i) {
        k = k_arr[i], v = v_arr[i];
        ht_insert(ht, k, v);
    }

    for (int i = 0; i != NUM_OF_ITEMS; ++i)
        xfree(k_arr[i]), xfree(v_arr[i]);

    xfree(k_arr), xfree(v_arr);

    ht_del_hash_table(ht);
    return 0;
}