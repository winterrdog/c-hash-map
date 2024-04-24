# ABOUT REPO

This is a simple hash table implementation in C with functions to _insert/update, search, create, get size,_ and _delete_ the hash table. It only operates on string keys and string values ONLY. It handles collisions using _open addressing with double hashing_ which is the most used technique for most production-level hash tables. The hash table is implemented using a _dynamic array_ so it can grow and shrink as needed based on the current load factor.

## Requirements

-   `gcc`
-   Compatible with any operating system that supports `gcc`

## Usage

-   Run the following command to compile the program:

```bash
./compile.sh
```

-   Run the following command to execute the program:

```bash
./hash_table
```

## TODO

-   Add support for multiple data types
