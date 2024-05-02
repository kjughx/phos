#ifndef _FILE_H_
#define _FILE_H_

#include "fs/pparser.h"
#include <common.h>

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

typedef unsigned int FILE_STAT_FLAGS;
enum {
    FILE_STAT_READ_ONLY = 0b00000001,
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

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

struct disk;
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk);
typedef void* (*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk* disk, void* private, uint32_t size, uint32_t nmemb,
                                char* out);
typedef int (*FS_SEEK_FUNCTION)(void* private, uint32_t offset, FILE_SEEK_MODE seek_mode);
typedef int (*FS_STAT_FUNCTION)(struct disk* disk, void* private, struct file_stat* stat);
typedef int (*FS_CLOSE_FUNCTION)(void* private);

struct filesystem {
    /* Filesystem should return zero from resolve if the provided disk is using its filesystem */
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    FS_READ_FUNCTION read;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat;
    FS_CLOSE_FUNCTION close;

    char name[20];
};

void fs_init();
int fopen(const char* filename, const char* mode);
int fstat(int fd, struct file_stat* stat);
int fseek(int fd, int offset, FILE_SEEK_MODE whence);
int fread(void* p, uint32_t size, uint32_t n, int fd);
int fclose(int fd);
void fs_insert_fs(struct filesystem* fs);
struct filesystem* fs_resolve(struct disk* disk);

#endif /* _FILE_H_ */