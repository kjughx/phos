#ifndef _K_HEAP_H
#define _K_HEAP_H

#include "common.h"

void kheap_init();
void* kmalloc(size_t size);
void* kzalloc(size_t size);
void kfree(void* p);

#endif /* _K_HEAP_H */
