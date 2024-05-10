#ifndef _PHOS_H_
#define _PHOS_H_
#include "common.h"

void print(const char* message);
int getkey();
void* phos_malloc(size_t size);
void phos_free(void* p);

#endif /* _PHOS_H_ */
