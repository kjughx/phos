#include "phos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv) {
    print("Hello from C\n");
    void *p = malloc(512);

    if (p) {
        printf("Malloc'd memory: %p\n", p);
    }

    free(p);

    print(itoa(8637));
    putchar('A');
    printf("%s: %d\n", "Hello from printf", 1337);

    while (p) {
        if (getkey() != 0) {
            print("Key was pressed\n");
        }
    }
    return 0;
}
