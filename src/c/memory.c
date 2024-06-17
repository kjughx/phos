#include "memory.h"
#include "common.h"

void* _memset(void* p, int c, size_t size) {
    char* cp = (char*)p;
    for (size_t i = 0; i < size; i++) {
        cp[i] = c;
    }

    return p;
}
