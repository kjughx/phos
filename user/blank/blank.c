#include "phos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv) {
    char buf[128];
    printf("Hello from C\n");
    void *p = malloc(512);

    if (p) {
        printf("Malloc'd memory: %p\n", p);
    }

    free(p);

    readline(buf, sizeof(buf) , true);
    printf("%s\n", buf);
    while (1) {
    }
    return 0;
}
