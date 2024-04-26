#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hash_table.h"
#include "xmalloc.h"

#ifdef TEST

#define NUM_OF_ITEMS 50000 // You can change this to what you wish
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
#endif

int main()
{
    ht_hash_table* ht = ht_new();

#ifdef TEST
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
#else
    ht_insert(ht, "js", "1995");
    ht_insert(ht, "py", "1991");
    ht_insert(ht, "C++", "1980");

    printf("js value: %s\n", ht_search(ht, "js"));
    printf("C++ value: %s\n", ht_search(ht, "C++"));
    printf("py value: %s\n", ht_search(ht, "py"));
    printf("number of elements in hash table: %ld", ht_size(ht));
#endif

    ht_del_hash_table(ht);
    return 0;
}