#include "allocator_internal.h"

char *heap_start = NULL;

size_t align_size(size_t size)
{
    if (size > SIZE_MAX - (ALIGNMENT - 1)) {
        return 0;
    }
    return ALIGN_UP(size);
}

my_stats *get_malloc_header(void)
{
    assert(heap_start != NULL);
    my_stats *header = (my_stats *)heap_start;
    assert(header->magical_bytes == MAGICAL_BYTES);
    return header;
}

area *find_first_block(void)
{
    my_stats *header = get_malloc_header();
    return (area *)((char *)header + STATS_SIZE);
}

area *find_last_block(void)
{
    area *block = find_first_block();
    while (block->next != NULL) {
        block = block->next;
    }
    return block;
}

size_t heap_usable_size(void)
{
    return PAGE_SIZE -
            STATS_SIZE -
           AREA_SIZE;
}

bool extend_heap(size_t bytes)
{
    if (bytes == 0) {
        return true;
    }

    size_t pages =
        (bytes + PAGE_SIZE - 1) / PAGE_SIZE;

    if (pages > SIZE_MAX / PAGE_SIZE) {
        return false;
    }

    size_t total_bytes =
        pages * PAGE_SIZE;

    if (total_bytes > (size_t)INTPTR_MAX) {
        return false;
    }

    if (sbrk((intptr_t)total_bytes) ==
        (void *)-1) {
        return false;
    }

    my_stats *header =
        get_malloc_header();

    header->amount_of_pages += pages;

    return true;
}

void shrink_heap_if_possible(void)
{
    my_stats *header =
        get_malloc_header();

    area *last_block =
        find_last_block();

    area *last_used_block = NULL;

    if (last_block->in_use) {
        last_used_block = last_block;
    } else {
        last_used_block =
            find_previous_used_block(last_block);
    }

    if (last_used_block == NULL) {

        area *first_block =
            find_first_block();

        first_block->marker =
            BLOCK_MARKER;

        first_block->prev = NULL;
        first_block->next = NULL;
        first_block->in_use = false;

        first_block->length =
            heap_usable_size();

        header->amount_of_blocks = 1;

        char *minimum_end =
            heap_start + PAGE_SIZE;

        char *heap_end =
            (char *)sbrk(0);

        while (heap_end > minimum_end) {

            if (sbrk(-PAGE_SIZE) ==
                (void *)-1) {
                break;
            }

            heap_end =
                (char *)sbrk(0);

            if (header->amount_of_pages > 1) {
                header->amount_of_pages--;
            }
        }

        return;
    }

    char *new_end =
        (char *)last_used_block +
        AREA_SIZE +
        last_used_block->length;

    char *heap_end =
        (char *)sbrk(0);

    while (heap_end - new_end >
           PAGE_SIZE) {

        if (sbrk(-PAGE_SIZE) ==
            (void *)-1) {
            break;
        }

        heap_end =
            (char *)sbrk(0);

        if (header->amount_of_pages > 1) {
            header->amount_of_pages--;
        }
    }

    heap_end =
        (char *)sbrk(0);

    if (heap_end - new_end >=
        (ptrdiff_t)(AREA_SIZE + ALIGNMENT)) {

        area *new_free =
            (area *)new_end;

        new_free->marker =
            BLOCK_MARKER;

        new_free->prev =
            last_used_block;

        new_free->next = NULL;
        new_free->in_use = false;

        new_free->length =
            (size_t)(heap_end -
                     new_end -
                     AREA_SIZE);

        last_used_block->next =
            new_free;

        header->amount_of_blocks++;

    } else {
        last_used_block->next = NULL;
    }
}
