#ifndef _OAUTH_XMALLOC_H
#define _OAUTH_XMALLOC_H 1

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/* Prototypes for functions defined in xmalloc.c  */
void* xmalloc(size_t size);
void* xcalloc(size_t nmemb, size_t size);
void* xrealloc(void* ptr, size_t size);
void xfree(void* p);
const char* xstrdup(const char* s);

#endif