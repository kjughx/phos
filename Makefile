SRC=src
BIN=bin
OBJ=build

all:
	nasm -f bin $(SRC)/boot/boot.asm -o $(BIN)/boot.bin

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* $(BIN)/*
