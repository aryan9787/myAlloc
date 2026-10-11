#include <string.h>
#include "allocator.h"
#include "allocator_internal.h"

void *an_malloc(ssize_t requested_size)
{
    if (requested_size <= 0) {
        return NULL;
    }

    size_t size = align_size((size_t)requested_size);

    if (size == 0) {
        return NULL;
    }

    if (heap_start == NULL) {

        heap_start = (char *)sbrk(0);

        if (sbrk(PAGE_SIZE) == (void *)-1) {
            heap_start = NULL;
            return NULL;
        }

        my_stats *header = (my_stats *)heap_start;

        memset(header, 0, STATS_SIZE);

        header->magical_bytes = MAGICAL_BYTES;

        header->lock = false;

        header->amount_of_blocks = 1;
        header->amount_of_pages = 1;

        area *first_block = find_first_block();

        first_block->marker = BLOCK_MARKER;

        first_block->prev = NULL;
        first_block->next = NULL;
        first_block->in_use = false;

        first_block->length = heap_usable_size();
    }
    return add_used_block(size);
}

bool an_free(void *ptr)
{
    if (ptr == NULL ||
        heap_start == NULL) {
        return false;
    }

    my_stats *header =
        get_malloc_header();

    lock_heap(header);

    area *block =
        find_block_from_payload(ptr);

    if (block == NULL ||
        block->marker != BLOCK_MARKER ||
        !block->in_use) {

        unlock_heap(header);
        return false;
    }

    block->in_use = false;

    memset((char *)block +
               AREA_SIZE,
           0,
           block->length);

    merge_with_next(block, header);

    if (block->prev != NULL &&
        !block->prev->in_use) {

        area *previous =
            block->prev;

        previous->length +=
            AREA_SIZE +
            block->length;

        previous->next =
            block->next;

        if (previous->next != NULL) {
            previous->next->prev =
                previous;
        }

        memset(block, 0,
               AREA_SIZE);

        if (header->amount_of_blocks > 1) {
            header->amount_of_blocks--;
        }

        block = previous;
    }

    shrink_heap_if_possible();

    unlock_heap(header);

    return true;
}