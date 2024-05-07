#ifndef _STREAMER_H_
#define _STREAMER_H_
#include "disk.h"

/* @brief Reprsents a stream on a disk
 *
 * @member pos:   The position of the stream
 * @memeber disk: The disk from which to stream
 */
struct disk_stream {
    int pos;
    struct disk* disk;
};

/* @brief Create a new disk streamer
 *
 * @param: disk_id The disk from which to stream
 */
struct disk_stream* diskstreamer_new(int disk_id);

/* @brief Read bytes from a stream
 *
 * @param stream: The stream to read from
 * @param out:    The buffer to store the data
 * @param total:  How many bytes to read
 */
int diskstreamer_read(struct disk_stream* stream, void* out, int total);

/* @brief Seek a stream to a position
 *
 * @param stream: The stream to seek
 * @param pos:    The position to seek to
 */
int diskstreamer_seek(struct disk_stream* stream, int pos);

/* @brief Free a disk streamer
 *
 * @param stream: The stream to free
 */
void diskstreamer_free(struct disk_stream* stream);

#endif /* _STREAMER_H_ */
