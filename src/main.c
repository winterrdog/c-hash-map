#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hash_table.h"

char* rand_word(int min_len, int max_len)
{
    int i, length;
    char c;

    // seed random
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    /* using nano-seconds instead of seconds */
    srand((time_t)ts.tv_nsec);

    length = rand() % (max_len - min_len + 1) + min_len;
    char* word = xcalloc((size_t)(length + 1), sizeof(char));

    for (i = 0; i < length; i++) {
        c = 'a' + rand() % 26;
        word[i] = c;
    }
    word[i] = 0;

    return word;
}

int main()
{
    ht_hash_table* ht = ht_new();

    for (int i = 0; i != 20000; i++) {
        char* k = rand_word(8, 16);
        char* v = rand_word(5, 15);

        ht_insert(ht, k, v);
    }

    ht_del_hash_table(ht);
    return 0;
}