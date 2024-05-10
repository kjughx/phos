#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "common.h"

/* @brief Initialize a region of memory
 *
 * @param p:    The start of the memory region
 * @param c:    The value to initialize the memory to
 * @param size: The size of the memory region
 */
void* memset(void* p, int c, size_t size);

/* @brief Compare two regions of memory
 *
 * @param p1:    Start of the first memory region
 * @param p2:    Start of the second memory region
 * @param count: How many bytes to compare
 *
 * @returns: 0 if they're equal
 */
int memcmp(void* p1, void* p2, size_t count);

/* @brief Copy the contents of a memory region to another
 *
 * @param dst:  Start of the destination memory region
 * @param src:  Start of the source memory region
 * @param size: How many bytes to copy
 *
 */
void* memcpy(void* dst, void* src, size_t size);

#endif /* _MEMORY_H_ */
