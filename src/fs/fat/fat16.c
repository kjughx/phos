#include "fat16.h"
#include "../file.h"
#include "common.h"
#include "disk/disk.h"
#include "disk/streamer.h"
#include "kernel.h"
#include "status.h"
#include "string/string.h"
#include <config.h>
#include <fs/pparser.h>
#include <kernel.h>
#include <memory/heap/kheap.h>
#include <memory/memory.h>

#define PHOS_FAT16_SIGNATURE 0x29
#define PHOS_FAT16_ENTRY_SIZE 0x02
#define PHOS_FAT16_BAD_SECTOR 0xFF7
#define PHOS_FAT16_UNUSED 0x00

typedef unsigned int FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1

/* Fat directory entry attributes bitmask */
// clang-format off
#define FAT_FILE_READ_ONLY    (1 << 0)
#define FAT_FILE_HIDDEN       (1 << 1)
#define FAT_FILE_SYSTEM       (1 << 2)
#define FAT_FILE_VOLUME_LABEL (1 << 3)
#define FAT_FILE_SUBDIRECTORY (1 << 4)
#define FAT_FILE_ARCHIVED     (1 << 5)
#define FAT_FILE_DEVICE       (1 << 6)
#define FAT_FILE_RESERVERED   (1 << 7)
// clang-format on

/* Extended header stored on disk according to specification */
struct fat_header_extended {
    uint8_t drive_no;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

/* Header stored on disk according to specification */
struct fat_header {
    uint8_t short_jmp_ins[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t sectors_big;
} __attribute__((packed));

struct fat_h {
    struct fat_header primary_header;
    union fat_h_e {
        struct fat_header_extended extended_header;
    } shared;
};

/* Directory item according to specification */
struct fat_directory_item {
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attributes;
    uint8_t resevered;
    uint8_t creation_time_ds;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_data;
    uint16_t low_16_bits_first_cluster;
    uint32_t filesize;
} __attribute__((packed));

/* Internal representation of directory */
struct fat_directory {
    struct fat_directory_item* item;
    uint32_t total;
    uint32_t sector_pos;
    uint32_t ending_sector_pos;
};

/* Internal representation filesystem item */
struct fat_item {
    union {
        struct fat_directory_item* item;
        struct fat_directory* directory;
    };

    FAT_ITEM_TYPE type;
};

struct fat_file_descriptor {
    struct fat_item* item;
    uint32_t pos;
};

struct fat_private {
    struct fat_h header;
    struct fat_directory root_directory;

    /* Used to stream data clusters */
    struct disk_stream* cluster_read_stream;
    /* Used to stream file allocation table */
    struct disk_stream* fat_read_stream;

    /* Used to stream the directory */
    struct disk_stream* directory_stream;
};

int fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);
int fat16_read(struct disk* disk, void* desc, uint32_t size, uint32_t n, char* out);

struct filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,
    .read = fat16_read,
};

struct filesystem* fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

static void fat16_init_private(struct disk* disk, struct fat_private* private) {
    memset(private, 0, sizeof(struct fat_private));

    if (!(private->cluster_read_stream = diskstreamer_new(disk->id))) {
        panic("Failed to create new streamer for cluster_read");
    }

    if (!(private->fat_read_stream = diskstreamer_new(disk->id))) {
        panic("Failed to create new streamer for fat_read");
    }

    if (!(private->directory_stream = diskstreamer_new(disk->id))) {
        panic("Failed to create new streamer for directory");
    }
}

int fat_sector_to_absolute(struct disk* disk, int sector) { return sector * disk->sector_size; }

int fat16_get_total_items_for_directory(struct disk* disk, struct fat_private* private,
                                        uint32_t start_pos) {
    struct fat_directory_item item;
    struct fat_directory_item empty;
    memset(&empty, 0, sizeof(struct fat_directory_item));

    int ret = 0;
    int directory_start_pos = start_pos * disk->sector_size;
    struct disk_stream* stream = private->directory_stream;
    if (diskstreamer_seek(stream, directory_start_pos) < 0) {
        ret = -EIO;
        goto out;
    }

    for (;;) {
        if (diskstreamer_read(stream, &item, sizeof(item)) < 0) {
            ret = -EIO;
            goto out;
        }
        if (!item.filename[0]) /* Done reading */
            break;

        if (item.filename[0] == 0xE5) /* Unused item */
            continue;

        ret++;
    }

out:
    return ret;
}

int fat16_get_root_directory(struct disk* disk, struct fat_private* private,
                             struct fat_directory* directory) {
    int ret = 0;
    struct fat_header* primary_header = &private->header.primary_header;
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) +
                              primary_header->reserved_sectors;
    int root_dir_entries = private->header.primary_header.root_dir_entries;
    int root_dir_size = root_dir_entries * sizeof(struct fat_directory_item);
    int total_sectors = root_dir_size / disk->sector_size;

    if (root_dir_size % disk->sector_size)
        total_sectors += 1;

    int total_items = fat16_get_total_items_for_directory(disk, private, root_dir_sector_pos);

    struct fat_directory_item* dir = kzalloc(sizeof(struct fat_directory_item));
    if (!dir) {
        ret = -ENOMEM;
        goto out;
    }

    struct disk_stream* stream = private->directory_stream;
    if (diskstreamer_seek(stream, fat_sector_to_absolute(disk, root_dir_sector_pos)) < 0) {
        ret = -EIO;
        goto out;
    }

    if (diskstreamer_read(stream, dir, root_dir_size) < 0) {
        ret = -EIO;
        goto out;
    }

    directory->item = dir;
    directory->total = total_items;
    directory->sector_pos = root_dir_sector_pos;
    directory->ending_sector_pos = root_dir_sector_pos + total_sectors;

out:
    return ret;
}

void fat16_get_full_relative_filename(struct fat_directory_item* item, char* out, int max_len) {
    memset(out, 0, max_len);
    char* tmp = out;
    strcpy_strip(tmp, (const char*)item->filename);

    /* If the item has an extension */
    if (item->ext[0] && item->ext[0] != 0x20) {
        tmp += strlen(tmp); /* Advance to \0 */
        *tmp++ = '.';
        strcpy_strip(tmp, (const char*)item->ext);
    }
}

struct fat_directory_item* fat16_clone_directory_item(struct fat_directory_item* item,
                                                      size_t size) {
    struct fat_directory_item* cpy = NULL;
    if (size < sizeof(struct fat_directory_item))
        return NULL;

    if (!(cpy = kzalloc(size)))
        return NULL;

    memcpy(cpy, item, size);

    return cpy;
}

static uint32_t fat16_get_first_cluser(struct fat_directory_item* item) {
    return (item->high_16_bits_first_cluster | item->low_16_bits_first_cluster);
}

static int fat16_cluster_to_sector(struct fat_private* private, int cluster) {
    return private->root_directory.ending_sector_pos +
           ((cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private* private) {
    return private->header.primary_header.reserved_sectors;
}

static uint16_t fat16_get_fat_entry(struct disk* disk, int cluster) {
    int ret = -1;
    struct fat_private* private = disk->fs_private;
    struct disk_stream* stream = private->fat_read_stream;

    uint32_t fat_table_position = fat16_get_first_fat_sector(private) * disk->sector_size;
    if ((ret =
             diskstreamer_seek(stream, fat_table_position * (cluster * PHOS_FAT16_ENTRY_SIZE)) < 0))
        return ret;

    uint16_t result = 0;
    if ((ret = diskstreamer_read(stream, &result, sizeof(result))))
        return ret;

    return result;
}

static int fat16_get_cluster_for_offset(struct disk* disk, int cluster, int offset) {
    struct fat_private* private = disk->fs_private;
    int size_of_cluster_bytes =
        private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = cluster;
    int clusters_ahead = offset / size_of_cluster_bytes;
    for (int i = 0; i < clusters_ahead; i++) {
        int entry = fat16_get_fat_entry(disk, cluster_to_use);
        if (entry == 0xFF8 || entry == 0xFFF) /* Last entry of file */
            return -EIO;

        if (entry == PHOS_FAT16_BAD_SECTOR) /* Bad sector */
            return -EIO;

        if (entry == 0xFF0 || entry == 0xFF6) /* Reserved sector */
            return -EIO;

        if (entry == 0x00) /* No cluster */
            return -EIO;

        cluster_to_use = entry;
        break;
    }

    return cluster_to_use;
}

static int fat16_read_internal_from_stream(struct disk_stream* stream, struct disk* disk,
                                           int cluster, int offset, int total, void* out) {
    int ret = 0;
    struct fat_private* private = disk->fs_private;
    int size_of_cluster_bytes =
        private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);

    if (cluster_to_use < 0)
        return cluster_to_use;

    int offset_from_cluster = offset % size_of_cluster_bytes;
    int start_sector = fat16_cluster_to_sector(private, cluster_to_use);
    int start_pos = (start_sector * disk->sector_size) + offset_from_cluster;
    int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
    if ((ret = diskstreamer_seek(stream, start_pos)) < 0)
        return ret;

    if ((ret = diskstreamer_read(stream, out, total_to_read)) < 0)
        return ret;

    total -= total_to_read;

    if (total > 0) /* TODO: Don't use recursion */
        ret = fat16_read_internal_from_stream(stream, disk, cluster, offset + total_to_read, total,
                                              out + total_to_read);

    return ret;
}

static int fat16_read_internal(struct disk* disk, int start_cluster, int offset, int total,
                               void* out) {
    struct fat_private* private = disk->fs_private;
    struct disk_stream* stream = private->cluster_read_stream;
    return fat16_read_internal_from_stream(stream, disk, start_cluster, offset, total, out);
}

void fat16_free_directory(struct fat_directory* directory) {
    if (!directory)
        return;

    if (directory->item)
        kfree(directory->item);
    kfree(directory);
}

void fat16_free_item(struct fat_item* item) {
    if (item->type == FAT_ITEM_TYPE_DIRECTORY)
        fat16_free_directory(item->directory);
    else if (item->type == FAT_ITEM_TYPE_FILE)
        kfree(item->item);
    else
        panic("Invalid fat16 item");
}

struct fat_directory* fat16_load_fat_directory(struct disk* disk, struct fat_directory_item* item) {
    int ret = 0;
    struct fat_directory* directory = NULL;
    struct fat_private* private = disk->fs_private;
    if (!(item->attributes & FAT_FILE_SUBDIRECTORY))
        return ERROR(-EINVAL);

    if (!(directory = kzalloc(sizeof(struct fat_directory)))) {
        ret = -ENOMEM;
        goto out;
    }

    int cluster = fat16_get_first_cluser(item);
    int cluser_sector = fat16_cluster_to_sector(private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, private, cluser_sector);
    directory->total = total_items;
    int directory_size = directory->total * sizeof(struct fat_directory_item);
    if (!(directory->item = kzalloc(directory_size))) {
        ret = -ENOMEM;
        goto out;
    }

    ret = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);
    if (ret < 0)
        goto out;

out:
    if (ret < 0) {
        fat16_free_directory(directory);
        return ERROR(-ret);
    }

    return directory;
}

struct fat_item* fat16_new_fat_item_for_directory_item(struct disk* disk,
                                                       struct fat_directory_item* item) {
    struct fat_item* f_item = kzalloc(sizeof(struct fat_item));
    if (!f_item)
        return NULL;

    if (item->attributes & FAT_FILE_SUBDIRECTORY) {
        f_item->directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;
    } else {
        f_item->type = FAT_ITEM_TYPE_FILE;
        f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item));
    }

    return f_item;
}

struct fat_item* fat16_find_item_in_directory(struct disk* disk, struct fat_directory* directory,
                                              const char* name) {
    char tmp_filename[PHOS_MAX_PATH];
    for (uint32_t i = 0; i < directory->total; i++) {
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
        if (istrncmp(tmp_filename, name, sizeof(tmp_filename)) == 0) {
            return fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
        }
    }

    return NULL;
}

struct fat_item* fat16_get_directory_entry(struct disk* disk, struct path_part* path) {
    struct fat_private* private = disk->fs_private;
    struct fat_item* current_item = NULL;
    struct fat_item* root_item =
        fat16_find_item_in_directory(disk, &private->root_directory, path->part);

    if (!root_item) {
        goto out;
    }

    struct path_part* next_part = path->next;
    current_item = root_item;
    while (next_part) {
        if (current_item != FAT_ITEM_TYPE_DIRECTORY) {
            current_item = 0;
            break;
        }
        struct fat_item* tmp =
            fat16_find_item_in_directory(disk, current_item->directory, next_part->part);
        fat16_free_item(current_item);
        current_item = tmp;
        next_part = next_part->next;
    }

out:
    return current_item;
}

int fat16_resolve(struct disk* disk) {
    int ret = 0;
    struct fat_private* private = kzalloc(sizeof(struct fat_private));
    fat16_init_private(disk, private);

    struct disk_stream* stream = diskstreamer_new(disk->id);
    if (!stream)
        return -ENOMEM;

    if (diskstreamer_read(stream, &private->header, sizeof(private->header)) < 0) {
        ret = -EIO;
        goto out;
    }

    if (private->header.shared.extended_header.signature != PHOS_FAT16_SIGNATURE) {
        ret = -EFSNOTUS;
        goto out;
    }

    if (fat16_get_root_directory(disk, private, &private->root_directory) < 0) {
        ret = -EIO;
        goto out;
    }

    disk->fs_private = private;
    disk->filesystem = &fat16_fs;

out:
    diskstreamer_close(stream);
    if (ret < 0) {
        kfree(private);
        disk->fs_private = NULL;
    }

    return ret;
}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode) {
    struct fat_file_descriptor* desc = NULL;

    if (mode != FILE_MODE_READ)
        return ERROR(-ERDONLY);

    if (!(desc = kzalloc(sizeof(struct fat_file_descriptor))))
        return ERROR(-ENOMEM);

    if (!(desc->item = fat16_get_directory_entry(disk, path))) {
        kfree(desc);
        return ERROR(-EIO);
    }

    desc->pos = 0;

    return desc;
}

int fat16_read(struct disk* disk, void* desc, uint32_t size, uint32_t n, char* out) {
    int ret = 0;
    struct fat_file_descriptor* fat_desc = desc;
    struct fat_directory_item* item = fat_desc->item->item;
    int offset = fat_desc->pos;
    for (uint32_t i = 0; i < n; i++) {
        ret  = fat16_read_internal(disk, fat16_get_first_cluser(item), offset, size, out);
        if (ISERR(ret))
            return ret;

        out += size;
        offset += size;

    }

    return n;
}
