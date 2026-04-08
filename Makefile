CC = gcc
AS = as
NASM = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -v -Wpedantic -Wshadow -Wconversion
ASFLAGS = --32 -v
NASMFLAGS = -f elf32 -Wall -Werror
LDFLAGS = -m elf_i386 -T Operating_Systems/RazzleNormal/linker.ld -Map=kernel.map --verbose

OBJ = Boot/boot.o Operating_Systems/RazzleNormal/kernel/kernel.o Operating_Systems/RazzleNormal/kernel/idt.o Operating_Systems/RazzleNormal/drivers/VGAf/vgaf.o Operating_Systems/RazzleNormal/drivers/KEYdriver/key.o Operating_Systems/RazzleNormal/Shell/shell.o

all: kernel.bin

kernel.bin: $(OBJ) Operating_Systems/RazzleNormal/linker.ld
	$(LD) $(LDFLAGS) -o $@ Boot/boot.o Operating_Systems/RazzleNormal/kernel/kernel.o Operating_Systems/RazzleNormal/kernel/idt.o Operating_Systems/RazzleNormal/drivers/VGAf/vgaf.o Operating_Systems/RazzleNormal/drivers/KEYdriver/key.o Operating_Systems/RazzleNormal/Shell/shell.o

Operating_Systems/RazzleNormal/kernel/kernel.o: Operating_Systems/RazzleNormal/kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

Boot/boot.o: Boot/boot.s
	$(AS) $(ASFLAGS) $< -o $@

Operating_Systems/RazzleNormal/kernel/idt.o: Operating_Systems/RazzleNormal/kernel/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

Operating_Systems/RazzleNormal/drivers/VGAf/vgaf.o: Operating_Systems/RazzleNormal/drivers/VGAf/vgaf.c
	$(CC) $(CFLAGS) -c $< -o $@

Operating_Systems/RazzleNormal/drivers/KEYdriver/key.o: Operating_Systems/RazzleNormal/drivers/KEYdriver/key.c
	$(CC) $(CFLAGS) -c $< -o $@
	sleep 1
Operating_Systems/RazzleNormal/drivers/KEYdriver/keyboard.o: Operating_Systems/RazzleNormal/drivers/KEYdriver/keyboard.s
	$(AS) $(ASFLAGS) $< -o $@

Operating_Systems/RazzleNormal/Shell/shell.o: Operating_Systems/RazzleNormal/Shell/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

iso: kernel.bin
	cp kernel.bin iso/boot/
	cp -r Boot/grub iso/boot/
	grub-mkrescue -o razzle.iso iso/

clean:
	rm -f $(OBJ) kernel.bin razzle.iso