CFLAGS := -std=gnu99 -Wall -Wextra -Werror -Wno-cpp -Wno-unused-parameter -Wno-unused-function -O0 \
	-I../../stdlib/include/ -ffreestanding \
	-falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce  -fomit-frame-pointer -finline-functions \
	-fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -g

LDFLAGS := -g -relocatable
CC := i686-elf-gcc
LD := i686-elf-ld

C_SRCS := $(shell find  -name "*.c")

OBJS += $(patsubst %.c, %.o, $(C_SRCS))

all: $(OBJS)
	i686-elf-gcc -g -T linker.ld -o shell.elf -ffreestanding -O0 -nostdlib -fpic -g $< ../../stdlib/stdlib.elf

%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf *.o shell.elf
