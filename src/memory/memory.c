#include "memory.h"

void* memset(void* p, int c, size_t size) {
    char* cp = (char*)p;
    for (int i = 0; i < size; i++) {
        cp[i] = c;
    }

    return p;
}
