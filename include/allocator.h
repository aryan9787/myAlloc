#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

void *an_malloc(ssize_t requested_size);
bool an_free(void *ptr);

#endif