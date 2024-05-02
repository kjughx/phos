#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "common.h"

void* memset(void* p, int c, size_t size);
int memcmp(void* p1, void* p2, size_t count);
void* memcpy(void* dst, void* src, size_t size);

#endif /* _MEMORY_H_ */
