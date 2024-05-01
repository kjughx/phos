#ifndef _DISK_H_
#define _DISK_H_

int disk_read_sector(int lba, int total, void* buf);

#endif /* _DISK_H_ */
