all:
	nasm -f elf32 boot.asm -o boot.o
	gcc -m32 -c kernel.c -o kernel.o -ffreestanding
	ld -m elf_i386 -T linker.ld -o kernel.bin boot.o kernel.o

	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My OS" { multiboot /boot/kernel.bin }' >> iso/boot/grub/grub.cfg

	grub2-mkrescue -o myos.iso iso

run:
	qemu-system-x86_64 -cdrom myos.iso
clean:
	rm -f boot.o kernel.o kernel.bin myos.iso
	rm -rf iso