#include "file.h"
#include "common.h"
#include "config.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "status.h"

struct filesystem* filesystems[PHOS_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[PHOS_MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_filesystem() {
    for (int i = 0; i < PHOS_MAX_FILESYSTEMS; i++) {
        if (!filesystems[i])
            return &filesystems[i];
    }

    return NULL;
}

void fs_insert_fs(struct filesystem* filesystem) {
    struct filesystem** fs;
    if (!(fs = fs_get_free_filesystem()))
        panic("Could not insert filesystem");

    *fs = filesystem;
}

static void fs_static_load() { /* fs_insert_fs(fat16_init()); */ }

void fs_load() {
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init() {
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_descriptor_new(struct file_descriptor** desc) {
    for (int i = 0; i < PHOS_MAX_FILE_DESCRIPTORS; i++) {
        if (file_descriptors[i] == 0) {
            *desc = kzalloc(sizeof(struct file_descriptor));
            (*desc)->index = i + 1; /* Descriptors start at 1 */
            file_descriptors[i] = *desc;
            return 0;
        }
    }

    return -ENOMEM;
}

static struct file_descriptor* file_descriptor_get(int fd) {
    if (fd <= 0 || fd >= PHOS_MAX_FILE_DESCRIPTORS)
        return NULL;

    return file_descriptors[fd - 1];
}

struct filesystem* fs_resolve(struct disk* disk) {
    for (int i = 0; i < PHOS_MAX_FILESYSTEMS; i++) {
        if (filesystems[i] && filesystems[i]->resolve(disk) == 0) {
            return filesystems[i];
        }
    }

    return NULL;
}

int fopen(const char* filename, const char* mode) { return -EIO; }
