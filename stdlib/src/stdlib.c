#include "stdlib.h"
#include "phix.h"

void* malloc(size_t size) { return phix_malloc(size); }

void free(void* p) { phix_free(p); }
