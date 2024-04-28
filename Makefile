SRC=src
BIN=bin
OBJ=build

TARGET=i686-elf
CFLAGS=-std=gnu99 -Wall -Werror -Wno-cpp -O0 -Isrc/ -Iinc/ -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin -nostartfiles -nodefaultlibs -nostdlib
LDFLAGS=-g -relocatable
CC=gcc

BOOT_SRCS = boot/boot.asm
BINS=$(BIN)/boot.bin $(BIN)/kernel.bin
OBJS=$(OBJ)/kernel.asm.o $(OBJ)/kernel.o

KERNEL_SRCS = $(SRC)/kernel.c

all: $(BINS)
	rm -f $(BIN)/os.bin
	dd if=$(BIN)/boot.bin >> $(BIN)/os.bin
	dd if=$(BIN)/kernel.bin >> $(BIN)/os.bin
	dd if=/dev/zero bs=512 count=100 >> $(BIN)/os.bin

$(BIN)/kernel.bin: $(OBJS)
	$(TARGET)-ld $(LDFLAGS) $(OBJS) -o $(OBJ)/kernelfull.o
	$(TARGET)-$(CC) $(CFLAGS) -T linker.ld -o $@ $(OBJ)/kernelfull.o

$(BIN)/boot.bin: $(SRC)/boot/boot.asm
	nasm -f bin $< -o $@

$(OBJ)/%.asm.o: $(SRC)/%.asm
	nasm -f elf $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(TARGET)-$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* $(BIN)/*
