#include "disk/streamer.h"
#include "common.h"
#include "config.h"
#include "memory/heap/kheap.h"
#include <disk/disk.h>

struct disk_stream* diskstreamer_new(int disk_id) {
    struct disk* disk = disk_get(disk_id);
    if (!disk)
        return NULL;

    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;

    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int pos) {
    stream->pos = pos;
    return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total) {
    int sector = stream->pos / PHIX_SECTOR_SIZE;
    int offset = stream->pos % PHIX_SECTOR_SIZE;
    int total_to_read = total;
    char buf[PHIX_SECTOR_SIZE];
    int ret = 0;

    if (offset + total_to_read >= PHIX_SECTOR_SIZE)
        total_to_read -= (offset + total_to_read) - PHIX_SECTOR_SIZE;

    if ((ret = disk_read_block(stream->disk, sector, 1, buf)) < 0)
        goto out;

    for (int i = 0; i < total_to_read; i++) {
        *(char*)out++ = buf[offset + i];
    }

    /* Adjust the stream */
    stream->pos += total_to_read;

    if (total_to_read != total)
        ret = diskstreamer_read(stream, out, total - total_to_read);
out:
    return ret;
}

void diskstreamer_free(struct disk_stream* stream) { kfree(stream); }
