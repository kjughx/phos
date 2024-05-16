TARGETS = kernel stdlib_all user_all

all: $(TARGETS)
	sudo mount -t vfat phix/bin/os.bin /mnt/d
	sudo cp user/bin/* /mnt/d
	echo "Hello from file" | sudo tee /mnt/d/file.txt
	sudo umount /mnt/d

kernel:
	make -C phix

stdlib_all:
	make -C stdlib

user_all:
	make -C user

gdb: all
	gdb --command=debug.gdb

qemu: all
	qemu-system-i386 -hda phix/bin/os.bin

clean:
	@make -C phix clean
	@make -C stdlib clean
	@make -C user clean
