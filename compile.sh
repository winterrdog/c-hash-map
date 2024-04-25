#!/usr/bin/env bash

cd src
gcc -Ofast -o ../hash_table main.c prime.c xmalloc.c hash_table.c -lm
cd -
