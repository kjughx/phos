OBJ=build

ASM_SRCS := $(shell find  -name "*.asm")

OBJS += $(patsubst %.asm, $(OBJ)/%.asm.o, $(notdir $(ASM_SRCS)))

all: $(OBJS)
	i686-elf-ld -g -relocatable $(OBJS) -o $(OBJ)/user.o
	i686-elf-gcc -g -T linker.ld -o ./blank.elf -ffreestanding -O0 -nostdlib -fpic -g $(OBJS)

$(OBJ)/%.asm.o: blank/%.asm
	@mkdir -p $(dir $@)
	nasm -f elf $< -o $@

clean:
	@rm -rf build/* blank.elf
