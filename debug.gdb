target remote | qemu-system-i386 -S -gdb stdio -hda kernel/bin/os.bin
add-symbol-file kernel/build/kernelfull.o 0x100000
