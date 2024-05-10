#include "stdlib.h"
#include "phos.h"

void* malloc(size_t size) { return phos_malloc(size); }

void free(void* p) { phos_free(p); }
