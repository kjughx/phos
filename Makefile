SRC := src
BIN := bin
OBJ := build

CFLAGS := -std=gnu99 -Wall -Werror -Wno-cpp -O0 -Isrc/ -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -isystem src/ -nostdinc
LDFLAGS := -g -relocatable
CC := i686-elf-gcc
LD := i686-elf-ld

BINS = $(BIN)/boot.bin $(BIN)/kernel.bin
# kernel.asm.o has to be first to be linked
OBJS := $(OBJ)/src/kernel.asm.o

INCS := $(shell find src -name "*.h")
C_SRCS := $(shell find src -name "*.c")
ASM_SRCS := $(filter-out src/boot/boot.asm, $(shell find src -name "*.asm"))

OBJS += $(patsubst %.c, $(OBJ)/%.o, $(C_SRCS))
OBJS += $(filter-out build/src/kernel.asm.o, $(patsubst %.asm, $(OBJ)/%.asm.o, $(ASM_SRCS)))

all:
	@bear -- make _all

_all: $(BINS)
	rm -f $(BIN)/os.bin
	dd if=$(BIN)/boot.bin >> $(BIN)/os.bin
	dd if=$(BIN)/kernel.bin >> $(BIN)/os.bin
	dd if=/dev/zero bs=512 count=100 >> $(BIN)/os.bin
#
$(BIN)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $(patsubst %, $(OBJ)/%, $(notdir $(OBJS))) -o $(OBJ)/kernelfull.o
	$(CC) $(CFLAGS) -T linker.ld $(OBJ)/kernelfull.o -o $@

$(BIN)/boot.bin: $(SRC)/boot/boot.asm
	nasm -f bin $< -o $@

$(OBJ)/%.asm.o: %.asm
	nasm -f elf $< -o $(patsubst %, $(OBJ)/%, $(notdir $@))

$(OBJ)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(patsubst %, $(OBJ)/%, $(notdir $@))

gdb: all
	gdb --command=debug.gdb

qemu: all
	qemu-system-i386 -hda bin/os.bin

clean:
	@rm -rf $(OBJ)/* $(BIN)/*

.PHONY: all _all gdb qemu clean
