#include "allocator.h"
#include "allocator_internal.h"

#include <assert.h>
#include <stdint.h>

void test_alignment(void)
{
    void *p1 = an_malloc(1);
    void *p2 = an_malloc(3);
    void *p3 = an_malloc(7);
    void *p4 = an_malloc(13);
    void *p5 = an_malloc(31);

    assert(p1 != NULL);
    assert(p2 != NULL);
    assert(p3 != NULL);
    assert(p4 != NULL);
    assert(p5 != NULL);

    assert((uintptr_t)p1 %
           ALIGNMENT == 0);

    assert((uintptr_t)p2 %
           ALIGNMENT == 0);

    assert((uintptr_t)p3 %
           ALIGNMENT == 0);

    assert((uintptr_t)p4 %
           ALIGNMENT == 0);

    assert((uintptr_t)p5 %
           ALIGNMENT == 0);

    assert(an_free(p1));
    assert(an_free(p2));
    assert(an_free(p3));
    assert(an_free(p4));
    assert(an_free(p5));
}

void test_basic_malloc(void)
{
    char *ptr =
        (char *)an_malloc(1);

    assert(ptr != NULL);

    area *first_block =
        (area *)((char *)ptr -
                 AREA_SIZE);

    assert(first_block->marker ==
           BLOCK_MARKER);

    *ptr = 'C';

    assert(*ptr == 'C');

    assert(an_free(ptr));
}

void test_bigger_than_available_malloc(void)
{
    uint16_t *ptr =
        (uint16_t *)an_malloc(5000);

    assert(ptr != NULL);

    area *first_block =
        (area *)((char *)ptr -
                 AREA_SIZE);

    for (uint16_t i = 0;
         i < 2500;
         ++i) {
        ptr[i] = i;
    }

    assert(first_block->marker ==
           BLOCK_MARKER);

    assert(ptr[0] == 0);
    assert(ptr[2] == 2);
    assert(ptr[2499] == 2499);

    assert(*((uint8_t *)ptr + 4999) ==
           (uint8_t)(2499 >> 8));

    assert(*((uint8_t *)ptr + 4998) ==
           (uint8_t)(2499 & 0xFF));

    assert(an_free(ptr));
}

void test_free(void)
{
    uint8_t *first =
        (uint8_t *)an_malloc(2048);

    assert(first != NULL);

    area *first_block =
        (area *)((char *)first -
                 AREA_SIZE);

    assert(first_block->next != NULL);
    assert(first_block->length ==
           align_size(2048));

    area *second_block =
        first_block->next;

    assert(second_block->marker ==
           BLOCK_MARKER);

    assert(second_block->in_use ==
           false);

    assert(second_block->next ==
           NULL);

    assert(an_free(first));

    assert(first_block->marker ==
           BLOCK_MARKER);

    assert(first_block->next ==
           NULL);

    assert(first_block->length ==
           heap_usable_size());
}

void test_best_fit(void)
{
    void *a = an_malloc(128);
    void *b = an_malloc(256);
    void *c = an_malloc(512);

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);

    assert(an_free(a));
    assert(an_free(c));

    void *d = an_malloc(100);

    assert(d == a);

    assert(an_free(b));
    assert(an_free(d));
}

void test_zero_and_invalid_free(void)
{
    assert(an_malloc(0) == NULL);
    assert(an_malloc(-1) == NULL);

    int stack_value = 42;

    assert(an_free(&stack_value) ==
           false);
}

void complex_set_of_malloc_and_free_calls(void)
{
    uint8_t *first =
        (uint8_t *)an_malloc(2048);

    assert(first != NULL);

    area *first_block =
        find_first_block();

    assert(first_block->length ==
           align_size(2048));

    area *second_block =
        first_block->next;

    assert(second_block != NULL);
    assert(second_block->next == NULL);
    assert(second_block->prev ==
           first_block);

    uint8_t *second =
        (uint8_t *)an_malloc(10000);

    assert(second != NULL);

    my_stats *malloc_header =
        get_malloc_header();

    assert(malloc_header->
           amount_of_blocks >= 2);

    assert(an_free(second));

    uint8_t *third =
        (uint8_t *)an_malloc(1000);

    assert(third != NULL);

    uint8_t *fourth =
        (uint8_t *)an_malloc(5000);

    assert(fourth != NULL);

    uint8_t *fifth =
        (uint8_t *)an_malloc(1000);

    assert(fifth != NULL);

    uint8_t *sixth =
        (uint8_t *)an_malloc(1000);

    assert(sixth != NULL);

    assert(an_free(third));
    assert(an_free(fifth));
    assert(an_free(fourth));
    assert(an_free(sixth));
    assert(an_free(first));
}