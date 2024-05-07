#ifndef _FILE_H_
#define _FILE_H_

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

/* @brief Description of a file
 *
 * @member index:   The file descriptor index
 * @member fs:      The filesystem which the file belongs to
 * @member private: Private data for file system descriptor
 * @member disk:    The disk that the file descriptor should be used on
 */
struct file_descriptor {
    int index;
    struct filesystem* fs;
    void* private;
    struct disk* disk;
};

/* @brief Status of a file
 *
 * @member flags:    Flags of the file
 * @member filesize: The size of the file
 */
struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

struct disk;
struct path_part;
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

/* @brief Initialize the filesystems */
void fs_init();

/* @brief Resolve the filesystem for @disk */
struct filesystem* fs_resolve(struct disk* disk);

/* @brief Open the file @filename in @mode */
int fopen(const char* filename, const char* mode);

/* @brief Return the file status */
int fstat(int fd, struct file_stat* stat);

/* @brief Seek the file with file descriptor @fd to @offset */
int fseek(int fd, int offset, FILE_SEEK_MODE whence);

/* @brief Read @n blocks of @size from @fd into @p */
int fread(void* p, uint32_t size, uint32_t n, int fd);
/* @brief Close the file with file descriptor @fd */
int fclose(int fd);

/* @brief Insert the filesystem @fs */
void fs_insert_fs(struct filesystem* fs);

#endif /* _FILE_H_ */
