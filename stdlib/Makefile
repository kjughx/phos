SRC=src
OBJ=build

CFLAGS := -std=gnu99 -Wall -Wextra -Werror -Wno-cpp -Wno-unused-parameter -Wno-unused-function -O0 -Iinclude/ \
	-falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce  -fomit-frame-pointer -finline-functions \
	-fno-builtin -nostartfiles -nodefaultlibs -nostdlib -g

CC := i686-elf-gcc

C_SRCS := $(shell find  src/ -name "*.c")
ASM_SRCS := $(shell find src/ -name "*.asm")

OBJS += $(patsubst %.asm, $(OBJ)/%.asm.o, $(notdir $(ASM_SRCS)))
OBJS += $(patsubst %.c, $(OBJ)/%.o, $(notdir $(C_SRCS)))

all: $(OBJS)
	i686-elf-ld -m elf_i386 -relocatable $(OBJS) -o stdlib.elf

$(OBJ)/%.asm.o: $(SRC)/%.asm
	@mkdir -p $(dir $@)
	nasm -f elf $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf build/* stdlib.elf

cc:
		@make clean
		@bear -- make all
