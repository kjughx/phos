#include "shell.h"
#include "phos.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"

int main(void) {
    printf("PHOS v1.0.0\n");
    while (1) {
        printf("# > ");
        char buf[1024];
        readline(buf, sizeof(buf), true);
        if (strncmp(buf, "RUN ", 4) == 0) {
            exec(buf + 4);
        }
        printf("\n");
    }
}
