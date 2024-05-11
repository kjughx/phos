#include "phix.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char** argv) {
    printf("hello from %s\n", __FILE__);

    char* p = (char*)0x00;
    p[0] = 'A';

    while (1) {
    }
    return 0;
}
