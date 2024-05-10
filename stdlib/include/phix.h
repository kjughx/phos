#ifndef _phix_H_
#define _phix_H_
#include "common.h"

void phix_print(const char* message);
int phix_getkey();
void* phix_malloc(size_t size);
void phix_free(void* p);
void phix_putchar(char c);
void phix_exec(const char* filename);

#endif /* _phix_H_ */
