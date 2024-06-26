#ifndef _STDLIB_H_
#define _STDLIB_H_
#include "stddef.h"

#define NULL ((void*)0)

void* malloc(size_t size);
void free(void* p);

#endif /* STDLIB_H_ */
