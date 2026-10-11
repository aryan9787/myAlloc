# myAlloc

A small memory allocator written in C as a learning project. It implements `an_malloc()` and `an_free()` using a process heap, with block metadata to track allocations and free space.

The project focuses on understanding how allocators manage memory: finding a suitable free block, splitting and merging blocks, extending the heap when needed, and returning memory from the end of the heap when possible.

## Features

- Custom `an_malloc()` and `an_free()` functions
- Alignment of requested allocation sizes
- Best-fit search for a free block
- Splitting larger free blocks to reduce wasted space
- Coalescing adjacent free blocks
- Heap growth in page-sized increments using `sbrk()`
- Attempts to shrink the heap when trailing space can be released
- Basic allocator metadata and block-state checks
- Tests for allocation, freeing, best-fit selection, invalid requests, larger allocations, and alignment

## Project structure

```text
myAlloc/
├── include/
│   ├── allocator.h
│   └── allocator_internal.h
├── src/
│   ├── allocator.c
│   ├── block.c
│   ├── heap.c
│   └── sync.c
├── tests/
│   ├── test_allocator.c
│   └── test_runner.c
├── Makefile
└── README.md
```

- `allocator.c` — public allocation and deallocation entry points.
- `block.c` — free-block search, block splitting, merging, and allocation bookkeeping.
- `heap.c` — heap initialization helpers, alignment, heap growth, and heap shrinking.
- `sync.c` — allocator locking helpers.
- `allocator.h` — public API.
- `allocator_internal.h` — internal structures, constants, and function declarations.
- `tests/` — allocator tests and the test runner.

## Build and run

A C compiler and `make` are required. The current implementation uses `sbrk()`, so it is intended for Unix-like environments such as Linux or WSL rather than native Windows.

```bash
make
make run
```

To remove the generated object files and test executable:

```bash
make clean
```

## API

```c
void *an_malloc(ssize_t requested_size);
bool an_free(void *ptr);
```

- `an_malloc(size)` returns a pointer to the allocated payload, or `NULL` for invalid requests or when allocation fails.
- `an_free(ptr)` releases a pointer returned by this allocator. It returns `true` when the pointer is accepted and freed, and `false` for `NULL`, unknown pointers, or blocks that are not currently allocated.

## How it works

1. On the first allocation, the allocator initializes its metadata and obtains an initial page of heap memory.
2. Allocation sizes are rounded up for alignment.
3. The allocator searches free blocks and chooses the smallest block that can satisfy the request.
4. If the chosen block has enough spare space, it is split into an allocated block and a free block.
5. If no suitable block exists, the heap is extended and a new block is added.
6. On `an_free()`, the block is marked free, its payload is cleared, and adjacent free blocks are merged.
7. If free space at the end of the heap can be returned, the allocator attempts to shrink the heap.

## Tests

The test suite covers alignment, basic allocation, allocations larger than the initial available space, freeing and coalescing, best-fit selection, invalid requests, and sequences of mixed allocations and frees.

Run the tests with:

```bash
make run
```

## Limitations

- This is an educational allocator, not a drop-in replacement for the standard C allocator.
- It relies on `sbrk()`, which is not portable and may conflict with other code that manages the process break.
- It does not currently provide the full behavior, guarantees, or portability of the system `malloc()`.
- Review the test output before treating the implementation as complete; in particular, alignment should pass on the target platform.
