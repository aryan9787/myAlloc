#include <unistd.h>
#include "allocator_internal.h"

void lock_heap(my_stats *header)
{
    while (__atomic_test_and_set(
        &header->lock,__ATOMIC_ACQUIRE)) {
        usleep(1000);
    }
}

void unlock_heap(my_stats *header)
{
    __atomic_clear(&header->lock , __ATOMIC_RELEASE);
}