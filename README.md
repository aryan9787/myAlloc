# myAlloc

A custom memory allocator written in C that implements the core ideas behind dynamic memory management. This project explores how standard allocation functions such as `malloc`, `free`, `calloc`, and `realloc` can be implemented at a low level while managing heap memory manually.

The primary goal of this project was to gain a deeper understanding of memory allocation strategies, heap organization, block metadata, fragmentation, and allocator design by building a working allocator from scratch.

---

## Features

- Custom implementation of dynamic memory allocation
- Heap management using custom metadata structures
- Block splitting for efficient memory utilization
- Memory deallocation and free-list management
- Reallocation support
- Memory initialization for allocated blocks
- Debugging utilities for inspecting allocator behavior

---

## Motivation

Memory allocators are one of the most fundamental components of an operating system and runtime library. While applications typically rely on the standard C library's allocator, implementing one from scratch provides valuable insight into:

- Heap organization
- Memory block metadata
- Allocation and deallocation algorithms
- Internal and external fragmentation
- Performance trade-offs in allocator design

This project was built primarily as a learning exercise to better understand these concepts through practical implementation.

---

## Build

Compile using GCC:

```bash
gcc myAlloc.c -o myAlloc
```

Depending on your testing setup, additional compiler flags or source files may be required.

---

## Project Structure

```text
.
├── myAlloc.c
└── README.md
```

---

## Disclaimer

This project is an educational implementation of a memory allocator and is **not intended to be a drop-in replacement for production allocators** such as those provided by modern C standard libraries.

It prioritizes understanding allocator internals over production-level performance, security, portability, and thread safety.

---

## References

Developing a memory allocator requires understanding many existing techniques and implementation strategies. During the development of this project, I referred to:

- The original GNU C Library (`glibc`) allocator implementation and related source code.
- Publicly available articles, documentation, and educational resources on memory allocator design.
- Open-source implementations and discussions available on the internet for learning purposes.

These references were used to better understand allocator concepts and implementation techniques. The final code represents my own implementation and learning process.

---

## License

This project is provided for educational purposes. Feel free to explore, learn from, and build upon it while providing appropriate attribution where applicable.
