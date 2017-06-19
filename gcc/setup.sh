i686-elf-as boot.s -o boot.o
if [ $? -eq 1 ]; then
	echo "Assembly failed"
	exit 1
else
	echo "Assembly sucess"
fi
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
if [ $? -eq 1 ]; then
	echo "compilation failed"
	exit 1
else
	echo "Compilation sucess"
fi
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
if [ $? -eq 1 ]; then
	echo "Binary failed"
	exit 1
else
	echo "Binary sucess"
fi
qemu-system-i386 --kernel myos.bin
pause