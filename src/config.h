#ifndef _CONFIG_H_
#define _CONFIG_H_

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define PHOS_TOTAL_INTERRUPTS 512

/* 100MB heap size */
#define PHOS_HEAP_SIZE_BYTES (100 * 1024 * 1024)
#define PHOS_HEAP_BLOCK_SIZE 4096
#define PHOS_HEAP_SADDRESS 0x01000000
#define PHOS_HEAP_TABLE_ADDRESS 0x00007E00

#define PHOS_SECTOR_SIZE 512

#define PHOS_MAX_PATH 108

#endif /* _CONFIG_H */
