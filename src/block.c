#include <string.h>
#include "allocator_internal.h"

area *find_block_from_payload(void *ptr)
{
    if (ptr == NULL || heap_start == NULL) {
        return NULL;
    }

    area *block = find_first_block();

    while (block != NULL) {
        void *payload =
            (char *)block + AREA_SIZE;

        if (payload == ptr) {
            return block;
        }

        block = block->next;
    }

    return NULL;
}

area *find_previous_used_block(area *ptr)
{
    if (ptr == NULL) {
        return NULL;
    }

    area *block = ptr->prev;

    while (block != NULL) {
        if (block->in_use) {
            return block;
        }

        block = block->prev;
    }

    return NULL;
}

void merge_with_next(area *block, my_stats *header)
{
    area *next = block->next;

    if (next == NULL || next->in_use) {
        return;
    }

    block->length += AREA_SIZE + next->length;

    block->next = next->next;

    if (block->next != NULL) {
        block->next->prev = block;
    }

    memset(next, 0, AREA_SIZE);

    if (header->amount_of_blocks > 1) {
        header->amount_of_blocks--;
    }
}

void *add_used_block(size_t size)
{
    my_stats *header = get_malloc_header();

    lock_heap(header);

    area *block = find_first_block();

    area *smallest_block = NULL;

    while (block != NULL) {
        assert(block->marker == BLOCK_MARKER);

        if (!block->in_use && block->length >= size) {

            if (smallest_block == NULL ||
                block->length <
                smallest_block->length) {

                smallest_block = block;
            }
        }

        block = block->next;
    }

    if (smallest_block == NULL) {
        area *last =
            find_last_block();

        if (!last->in_use) {
            size_t missing = 0;

            if (size > last->length) {
                missing =
                    size - last->length;
            }

            if (missing > 0) {
                size_t old_length =
                    last->length;

                if (!extend_heap(missing)) {
                    unlock_heap(header);
                    return NULL;
                }

                size_t pages =
                    (missing + PAGE_SIZE - 1) /
                    PAGE_SIZE;

                last->length =
                    old_length +
                    pages * PAGE_SIZE;
            }

            smallest_block = last;
        } else {
            size_t required =
                AREA_SIZE + size;

            if (required >
                SIZE_MAX - (PAGE_SIZE - 1)) {
                unlock_heap(header);
                return NULL;
            }

            size_t pages =
                (required + PAGE_SIZE - 1) /
                PAGE_SIZE;

            if (pages >
                SIZE_MAX / PAGE_SIZE) {
                unlock_heap(header);
                return NULL;
            }

            size_t total =
                pages * PAGE_SIZE;

            if (total >
                (size_t)INTPTR_MAX) {
                unlock_heap(header);
                return NULL;
            }

            char *old_end =
                (char *)sbrk(0);

            if (sbrk((intptr_t)total) ==
                (void *)-1) {

                unlock_heap(header);
                return NULL;
            }

            header->amount_of_pages +=
                pages;

            area *new_block =
                (area *)old_end;

            new_block->marker =
                BLOCK_MARKER;

            new_block->prev = last;
            new_block->next = NULL;
            new_block->in_use = false;

            new_block->length =
                total - AREA_SIZE;

            last->next =
                new_block;

            header->amount_of_blocks++;

            smallest_block =
                new_block;
        }
    }

    size_t remaining =
        smallest_block->length - size;

    if (remaining >=
        AREA_SIZE + ALIGNMENT) {

        area *new_block =
            (area *)((char *)smallest_block +
                     AREA_SIZE +
                     size);

        new_block->marker =
            BLOCK_MARKER;

        new_block->prev =
            smallest_block;

        new_block->next =
            smallest_block->next;

        new_block->in_use = false;

        new_block->length =
            remaining - AREA_SIZE;

        if (new_block->next != NULL) {
            new_block->next->prev =
                new_block;
        }

        smallest_block->next =
            new_block;

        smallest_block->length =
            size;

        header->amount_of_blocks++;
    } else {
        smallest_block->length =
            size;
    }

    smallest_block->in_use = true;

    unlock_heap(header);

    return (char *)smallest_block +
           AREA_SIZE;
}