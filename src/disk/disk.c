#include "disk.h"
#include "config.h"
#include "io/io.h"
#include "memory/memory.h"
#include "status.h"
#include <fs/file.h>

struct disk disk;

static int disk_read_sector(int lba, int total, void* buf) {
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    unsigned short* p = (unsigned short*)buf;
    for (int b = 0; b < total; b++) {
        /* Wait for the buffer to be ready */
        while (!(insb(0x1F7) & 0x08))
            ;

        /* Copy from hard disk to memory */
        for (int i = 0; i < 256; i++) {
            /* Read two bytes into p */
            *p = insw(0x1F0);
            p++;
        }
    }

    return 0;
}

void disk_search_and_init() {
    memset(&disk, 0, sizeof(struct disk));
    disk.type = disk_type_real;
    disk.sector_size = PHOS_SECTOR_SIZE;
    disk.id = 0; /* TODO: Make this dynamic */
    disk.filesystem = fs_resolve(&disk);
}

struct disk* disk_get(int index) {
    if (index != 0)
        return NULL;

    return &disk;
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf) {
    if (idisk != &disk)
        return -EIO;

    return disk_read_sector(lba, total, buf);
}
