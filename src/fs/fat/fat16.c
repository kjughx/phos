#include "fat16.h"
#include "../file.h"
#include "common.h"
#include "status.h"
#include "string/string.h"
#include "disk/disk.h"
#include "disk/streamer.h"

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
    uint16_t  bytes_per_sector;
    uint8_t sectors_per_sector;
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

struct fat_item_descripor {
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

struct filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,
};

struct filesystem* fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

int fat16_resolve(struct disk* disk) { return -EIO; }

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode) { return NULL; }