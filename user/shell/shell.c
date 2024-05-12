#include "shell.h"
#include "phix.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

int main(void) {
    while (1) {
        printf("# > ");
        char buf[1024];
        readline(buf, sizeof(buf), true);
        char* cmd = strtok(buf, " ");
        if (strncmp(cmd, "run", 3) == 0) {
            char* filename = strtok(NULL, " ");
            printf("\nStarting %s\n", filename);
            exec(filename);
        }
        printf("\n");
    }
}
