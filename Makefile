SRC=src
BIN=bin
OBJ=build

TARGET=i686-elf
CFLAGS=-std=gnu99 -Wall -Werror -Wno-cpp -O0 -Isrc/ -Iinc/ -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin -nostartfiles -nodefaultlibs -nostdlib
LDFLAGS=-g -relocatable
CC=gcc

BOOT_SRCS = boot/boot.asm
BINS=$(BIN)/boot.bin $(BIN)/kernel.bin
ROBJS=$(OBJ)/compiler_builtins.o $(OBJ)/core.o $(OBJ)/rustc_std_workspace_core.o
OBJS=$(OBJ)/kernel.asm.o $(OBJ)/ruix

KERNEL_SRCS = $(SRC)/main.rs

all: $(BINS)
	rm -f $(BIN)/os.bin
	dd if=$(BIN)/boot.bin >> $(BIN)/os.bin
	dd if=$(BIN)/kernel.bin >> $(BIN)/os.bin
	dd if=/dev/zero bs=512 count=100 >> $(BIN)/os.bin

# $(BIN)/kernel.bin: $(OBJS)
# 	$(TARGET)-$(CC) $(CFLAGS) -T linker.ld -o $@ $(OBJS)

$(BIN)/boot.bin: $(SRC)/boot/boot.asm
	nasm -f bin $< -o $@

$(OBJ)/%.asm.o: $(SRC)/%.asm
	nasm -f elf $< -o $@

$(BIN)/kernel.bin: src/main.rs
	cargo build
	cp target/i686-unknown-none/debug/ruix build/kernelfull.o
	$(TARGET)-objcopy -O binary build/kernelfull.o $(BIN)/kernel.bin

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* $(BIN)/*

.PHONY: gdb
gdb: all
	gdb --command=debug.gdb
