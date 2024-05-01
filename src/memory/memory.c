#include "memory.h"
#include "common.h"

void* memset(void* p, int c, size_t size) {
    char* cp = (char*)p;
    for (int i = 0; i < size; i++) {
        cp[i] = c;
    }

    return p;
}

int memcmp(void* p1, void* p2, size_t count) {
    char* c1 = (char*)p1;
    char* c2 = (char*)p2;
    while (count-- > 0) {
        if (*c1++ != *c2++)
            return c1[-1] < c2[-1] ? -1 : 1;
    }

    return 0;
}
