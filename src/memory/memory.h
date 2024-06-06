#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "../common.h"

/* @brief Initialize a region of memory
 *
 * @param p:    The start of the memory region
 * @param c:    The value to initialize the memory to
 * @param size: The size of the memory region
 */
void* memset(void* p, int c, size_t size);

#endif

