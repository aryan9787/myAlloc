#ifndef ALLOCATOR_INTERNAL_H
#define ALLOCATOR_INTERNAL_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * sbrk() is a POSIX/Linux function.
 * With -std=c11, some system headers may not expose its declaration,
 * so declare it explicitly for this allocator.
 */
extern void *sbrk(intptr_t increment);

#define MAGICAL_BYTES 0x55
#define BLOCK_MARKER  0xDD
#define PAGE_SIZE     4096
#define ALIGNMENT     _Alignof(max_align_t)

struct free_area {
    uint8_t marker;
    struct free_area *prev;
    bool in_use;
    size_t length;
    struct free_area *next;
};

struct stats {
    int magical_bytes;
    bool lock;
    size_t amount_of_blocks;
    size_t amount_of_pages;
};

typedef struct stats my_stats;
typedef struct free_area area;


#define ALIGN_UP(x) \
(((x) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))

#define STATS_SIZE ALIGN_UP(sizeof(my_stats))
#define AREA_SIZE ALIGN_UP(sizeof(area))

// Global heap state
extern char *heap_start;

// Alignment
size_t align_size(size_t size);

// Heap information
my_stats *get_malloc_header(void);
area *find_first_block(void);
area *find_last_block(void);
size_t heap_usable_size(void);

// Heap management
bool extend_heap(size_t bytes);
void shrink_heap_if_possible(void);

// Block management
area *find_block_from_payload(void *ptr);
area *find_previous_used_block(area *ptr);
void merge_with_next(area *block, my_stats *header);
void *add_used_block(size_t size);

// Synchronization
void lock_heap(my_stats *header);
void unlock_heap(my_stats *header);

// Debugging
void debug_log(const char *msg);

#endif