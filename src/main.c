#include "hash_table.h"

int main(int argc, char const* argv[])
{
    ht_hash_table* map = ht_create_hash_table();
    if (!map) {
        return 1;
    }

    ht_delete_hash_table(map);
    return 0;
}
