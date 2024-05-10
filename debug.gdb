target remote | qemu-system-i386 -S -gdb stdio -hda phix/bin/os.bin
add-symbol-file phix/build/kernelfull.o 0x100000
add-symbol-file user/bin/shell 0x400000
