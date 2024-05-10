#include "unistd.h"
#include "phos.h"

int exec(const char* filename) {
    phos_exec(filename);

    return 0;
}
