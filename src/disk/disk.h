#ifndef _DISK_H_
#define _DISK_H_

#include "fs/file.h"

typedef enum disk_type {
    disk_type_real = 0,
} disk_type_e;

struct disk {
    disk_type_e type;
    int sector_size;

    struct filesystem* filesystem;
};

void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif /* _DISK_H_ */
