#include "streamer.h"
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
    int sector = stream->pos / PHOS_SECTOR_SIZE;
    int offset = stream->pos % PHOS_SECTOR_SIZE;
    char buf[PHOS_SECTOR_SIZE];
    int res = 0;

    while (total > 0) {
        if ((res = disk_read_block(stream->disk, sector, 1, buf)) < 0)
            goto out;

        int total_to_read = MIN(PHOS_SECTOR_SIZE, total);
        for (int i = 0; i < total_to_read; i++) {
            *(char*)out++ = buf[offset + i];
        }

        /* Adjust the stream */
        stream->pos += total_to_read;
        total -= total_to_read;
    }

out:
    return res;
}

void diskstreamer_close(struct disk_stream* stream) { kfree(stream); }
