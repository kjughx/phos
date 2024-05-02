#ifndef _FILE_H_
#define _FILE_H_

#include "fs/pparser.h"

typedef unsigned int FILE_SEEK_MODE;

enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};

typedef unsigned int FILE_MODE;
enum {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID,
};

struct disk;
typedef void* (*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk);

struct filesystem {
    /* Filesystem should return zero from resolve if the provided disk is using its filesystem */
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;

    char name[20];
};

struct file_descriptor {
    /* The descriptor index */
    int index;
    struct filesystem* fs;

    /* Private data for internal file descriptor*/
    void* private;

    /* Disk that the file descriptor should be used on */
    struct disk* disk;
};

void fs_init();
int fopen(const char* filename, const char* mode);
void fs_insert_fs(struct filesystem* fs);
struct filesystem* fs_resolve(struct disk* disk);

#endif /* _FILE_H_ */