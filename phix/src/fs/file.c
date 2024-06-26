#include "fs/file.h"
#include "common.h"
#include "config.h"
#include "fs/fat/fat16.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "status.h"
#include <disk/disk.h>
#include <fs/pparser.h>
#include <string/string.h>

struct filesystem* filesystems[PHIX_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[PHIX_MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_filesystem() {
    for (int i = 0; i < PHIX_MAX_FILESYSTEMS; i++) {
        if (!filesystems[i])
            return &filesystems[i];
    }

    return NULL;
}

static void fs_static_load() { fs_insert_fs(fat16_init()); }

static int file_descriptor_new(struct file_descriptor** desc) {
    for (int i = 0; i < PHIX_MAX_FILE_DESCRIPTORS; i++) {
        if (file_descriptors[i] == 0) {
            *desc = kzalloc(sizeof(struct file_descriptor));
            (*desc)->index = i + 1; /* Descriptors start at 1 */
            file_descriptors[i] = *desc;
            return 0;
        }
    }

    return -ENOMEM;
}

static void file_descriptor_free(struct file_descriptor* desc) {
    file_descriptors[desc->index - 1] = 0x00;
    kfree(desc);
}

static struct file_descriptor* file_descriptor_get(int fd) {
    if (fd <= 0 || fd >= PHIX_MAX_FILE_DESCRIPTORS)
        return NULL;

    return file_descriptors[fd - 1];
}

void fs_insert_fs(struct filesystem* filesystem) {
    struct filesystem** fs;
    if (!(fs = fs_get_free_filesystem()))
        panic("Could not insert filesystem");

    *fs = filesystem;
}

struct filesystem* fs_resolve(struct disk* disk) {
    for (int i = 0; i < PHIX_MAX_FILESYSTEMS; i++) {
        if (filesystems[i] && filesystems[i]->resolve(disk) == 0) {
            return filesystems[i];
        }
    }

    return NULL;
}

void fs_load() {
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init() {
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

FILE_MODE file_get_mode_by_string(const char* m) {
    FILE_MODE mode = FILE_MODE_INVALID;

    if (strncmp(m, "r", 1) == 0)
        mode = FILE_MODE_READ;
    else if (strncmp(m, "w", 1) == 0)
        mode = FILE_MODE_WRITE;
    else if (strncmp(m, "a", 1) == 0)
        mode = FILE_MODE_APPEND;

    return mode;
}

int fopen(const char* filename, const char* mode_str) {
    int ret = 0;
    struct disk* disk = NULL;
    struct path_root* root = NULL;
    struct file_descriptor* desc = NULL;
    FILE_MODE mode = FILE_MODE_INVALID;
    void* private = NULL;

    if (!(root = pparser_parse(filename, NULL)))
        return -EINVAL;

    /* Can't open root path: 0:/ */
    if (!root->first)
        return -EINVAL;

    if (!(disk = disk_get(root->drive_no)))
        return -EIO;

    if (!disk->filesystem)
        return -EIO;

    mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID)
        return -EINVAL;

    private = disk->filesystem->open(disk, root->first, mode);
    if (ISERR(private))
        return PTR_ERR(private);

    if ((ret = file_descriptor_new(&desc)) < 0)
        return ret;

    desc->fs = disk->filesystem;
    desc->private = private;
    desc->disk = disk;

    return desc->index;
}

int fstat(int fd, struct file_stat* stat) {
    struct file_descriptor* desc;

    if (!(desc = file_descriptor_get(fd)))
        return -EINVAL;

    return desc->fs->stat(desc->disk, desc->private, stat);
}

int fseek(int fd, int offset, FILE_SEEK_MODE whence) {
    struct file_descriptor* desc;

    if ((!(desc = file_descriptor_get(fd))))
        return -EINVAL;

    return desc->fs->seek(desc->private, offset, whence);
}

int fread(void* p, uint32_t size, uint32_t n, int fd) {
    struct file_descriptor* desc;

    if (size * n == 0 || fd < 1)
        return -EINVAL;

    if (!(desc = file_descriptor_get(fd)))
        return -EINVAL;

    return desc->fs->read(desc->disk, desc->private, size, n, (char*)p);
}

int fclose(int fd) {
    int ret;
    struct file_descriptor* desc;
    if (!(desc = file_descriptor_get(fd)))
        return -EINVAL;

    if ((ret = desc->fs->close(desc->private)) == 0)
        file_descriptor_free(desc);

    return ret;
}
