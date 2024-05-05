#ifndef _DISK_H_
#define _DISK_H_

#include "fs/file.h"

typedef enum disk_type {
    disk_type_real = 0,
} disk_type_e;

/* @brief Represents a hard drive
 *
 * @member type:        What kind of disk
 * @member sector_size: How big the sectors are
 * @member id:          The Id of the disk
 * @member filesystem:  The filesystem which is bound to this disk
 * @member fs_private:  Private data of the filesystem
*/
struct disk {
    disk_type_e type;
    int sector_size;
    int id;

    struct filesystem* filesystem;
    void* fs_private;
};

/* @brief: Find all available disks and initialize them */
void disk_search_and_init();

/* @brief: Get a specific disk
 *
 * @param index: The index of the disk
 */
struct disk* disk_get(int index);

/* @brief: Read from block on disk
 *
 * @param disk:  The disk to read from
 * @param lba:   The Logical Block Addressing block to read
 * @param total: How many bytes to read
 * @param buf    The buffer to store the data
 */
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif /* _DISK_H_ */
