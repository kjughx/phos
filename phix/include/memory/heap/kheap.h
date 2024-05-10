#ifndef _K_HEAP_H
#define _K_HEAP_H

#include "common.h"

/* @brief Initialize the kernel heap */
void kheap_init();

/* @brief Allocate memory on the kernel heap */
void* kmalloc(size_t size);

/* @brief Allocate and zero-initialize memory on the kernel heap */
void* kzalloc(size_t size);

/* @brief Free memory on the kernel heap */
void kfree(void* p);

#endif /* _K_HEAP_H */
