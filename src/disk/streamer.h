#ifndef _STREAMER_H_
#define _STREAMER_H_
#include "disk.h"

struct disk_stream {
    int pos;
    struct disk* disk;
};

struct disk_stream* diskstreamer_new(int disk_id);
int diskstreamer_read(struct disk_stream* stream, void* out, int total);
int diskstreamer_seek(struct disk_stream* stream, int pos);
void diskstreamer_close(struct disk_stream* stream);

#endif /* _STREAMER_H_ */
