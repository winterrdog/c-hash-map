#include "prime.h"

// adopted from: williamfiset/Algorithms
// time complexity: O(sqrt(n))
int is_prime(const int n)
{
    if (n < 2)
        return 0;
    if (n == 2 || n == 3)
        return 1;
    if (n % 2 == 0 || n % 3 == 0)
        return 0;

    long limit = (long)floor(sqrt((double)n));
    for (long i = 5; i <= limit; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }

    return 1;
}

int next_prime(int x)
{
    while (1) {
        if (is_prime(x))
            return x;
        x++;
    }
}

// inspired by: audacity/audacity/libnyquist
static int is_prime_simple(const int n)
{
    if (n < 2)
        return 0;
    if (n == 2)
        return 1;

    // if odd
    if (n & 1) {
        long limit = (long)floor(sqrt((double)n));
        for (long i = 3; i <= limit; i += 2) {
            if (n % i == 0)
                return 0;
        }

        return 1;
    }

    return 0;
}
