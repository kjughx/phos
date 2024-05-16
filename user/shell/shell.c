#include "shell.h"
#include "phix.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

int main(void) {
    int fd = -1;
    while (1) {
        printf("# > ");
        char buf[128];
        readline(buf, sizeof(buf), true);
        char* cmd = strtok(buf, " ");
        if (strncmp(cmd, "run", 3) == 0) {
            char* filename = strtok(NULL, " ");
            printf("\nStarting %s\n", filename);
            exec(filename);
        } else if (strncmp(cmd, "open", 4) == 0) {
            char* filename = strtok(NULL, " ");
            printf("\nOpening %s\n", filename);
            fd = open(filename, "r");
            if (fd < 0) {
                printf("Failed to open %s\n", filename);
            }
            close(fd);
        } else if (strncmp(cmd, "read", 4) == 0) {
            char* filename = strtok(NULL, " ");
            int count = atoi(strtok(NULL, " "));
            printf("\nOpening %s\n", filename);
            fd = open(filename, "r");
            if (fd < 0) {
                printf("Failed to open %s\n", filename);
                continue;
            }
            int ret = read(buf, count, 1, fd);
            if (ret < 0) {
                printf("Failed to read %d bytes from %s: %d\n", filename, count, ret);
                continue;
            }
            printf("%s\n", buf);
            close(fd);
        }
        printf("\n");
    }
}
