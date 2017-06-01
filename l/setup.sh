i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
cp myos.bin isodir/boot/myos.bin
grub-mkrescue -o myos.iso isodir
qemu-system-i386 -cdrom myos.iso
