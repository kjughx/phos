#include "shell.h"
#include "phix.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "stdlib.h"

int main(void) {
    while (1) {
        printf("# > ");
        char buf[1024];
        readline(buf, sizeof(buf), true);
        char* cmd = strtok(buf, " ");
        if (strncmp(cmd, "RUN", 3) == 0) {
            char* filename = strtok(NULL, " ");
            printf("\nStarting %s\n", filename);
            exec(filename);
        }
        printf("\n");
    }
}
