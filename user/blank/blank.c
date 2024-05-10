#include "phos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv) {
    print("Hello from C\n");
    void *p = malloc(512);

    if (p) {
        print("Malloc'd memory\n");
    }

    free(p);

    print(itoa(8637));
    putchar('A');

    while (p) {
        if (getkey() != 0) {
            print("Key was pressed\n");
        }
    }
    return 0;
}
