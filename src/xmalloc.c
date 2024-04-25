/* xmalloc.c -- memory allocation including 'out of memory' checks
 *
 * Copyright 2010 Robin Gareus <robin@gareus.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * winterrdog: i readapted xstrdup() to fit my needs
 */

#include "xmalloc.h"

static void* xmalloc_fatal(size_t size)
{
    if (size == 0)
        return NULL;
    fprintf(stderr, "Out of memory. exiting..!\n");
    exit(1);
}

void* xmalloc(size_t size)
{
    void* ptr = malloc(size);
    if (ptr)
        return ptr;
    return xmalloc_fatal(size);
}

void* xcalloc(size_t nmemb, size_t size)
{
    void* ptr = calloc(nmemb, size);
    if (ptr)
        return ptr;
    return xmalloc_fatal(nmemb * size);
}

void* xrealloc(void* ptr, size_t size)
{
    void* p = realloc(ptr, size);
    if (p)
        return p;
    return xmalloc_fatal(size);
}

char* xstrdup(const char* s)
{
    size_t len = strlen(s) + 1;
    void* ptr = xmalloc(len);
    strncpy(ptr, s, len);

    // make sure the null is last
    ((char*)ptr)[len - 1] = '\0';

    return (char*)ptr;
}

void xfree(void* p)
{
    if (p) {
        free(p), p = NULL;
    }
}