SRC := src
BIN := bin
OBJ := build

CFLAGS := -std=gnu99 -Wall -Wextra -Werror -Wno-cpp -Wno-unused-parameter -Wno-unused-function -O0 -Iinclude/ -ffreestanding \
	-falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce  -fomit-frame-pointer -finline-functions \
	-fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -g
LDFLAGS := -g -relocatable
CC := i686-elf-gcc
LD := i686-elf-ld

BINS = $(BIN)/boot.bin $(BIN)/kernel.bin
# kernel.asm.o has to be first to be linked
OBJS := $(OBJ)/kernel.asm.o

C_SRCS := $(shell find src -name "*.c")
ASM_SRCS := $(filter-out src/boot/boot.asm, $(shell find src -name "*.asm"))

OBJS += $(patsubst src/%.c, $(OBJ)/%.o, $(C_SRCS))
OBJS += $(filter-out build/kernel.asm.o, $(patsubst src/%.asm, $(OBJ)/%.asm.o, $(ASM_SRCS)))

all: $(BINS) stdlib_all user_all
	@rm -f $(BIN)/os.bin
	dd if=$(BIN)/boot.bin >> $(BIN)/os.bin
	dd if=$(BIN)/kernel.bin >> $(BIN)/os.bin
	dd if=/dev/zero bs=1024 count=1024 >> $(BIN)/os.bin

	# Copy over an executable
	sudo mount -t vfat $(BIN)/os.bin /mnt/d
	sudo cp ./user/blank/blank.elf /mnt/d
	sudo cp ./user/shell/shell.elf /mnt/d
	sudo umount /mnt/d

$(BIN)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(OBJ)/kernelfull.o
	$(CC) $(CFLAGS) -T linker.ld $(OBJ)/kernelfull.o -o $@

$(BIN)/boot.bin: $(SRC)/boot/boot.asm
	@mkdir -p $(dir $@)
	nasm -f bin $< -o $@

$(OBJ)/%.asm.o: $(SRC)/%.asm
	@mkdir -p $(dir $@)
	nasm -f elf $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

stdlib_all:
	@make -C stdlib all

stdlib_clean:
	@make -C stdlib clean

user_all:
	@make -C user
user_clean:
	@make -C user clean

gdb: all
	gdb --command=debug.gdb

qemu: all
	qemu-system-i386 -hda bin/os.bin

clean: stdlib_clean user_clean
	@rm -rf $(OBJ) $(BIN)

cc:
	@make clean
	@bear -- make all

.PHONY: all _all gdb qemu clean cc stdlib_all stdlib_clean user_all user_clean
