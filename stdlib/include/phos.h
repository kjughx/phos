#ifndef _PHOS_H_
#define _PHOS_H_
#include "common.h"

void phos_print(const char* message);
int phos_getkey();
void* phos_malloc(size_t size);
void phos_free(void* p);
void phos_putchar(char c);

void readline(char* buf, int max, bool output_while_typing);

#endif /* _PHOS_H_ */
