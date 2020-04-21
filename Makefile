PROJECT = pong

SOURCES += src/main.c \
		lib/src/usart.c

LIBPATH = lib
include $(LIBPATH)/lib.mk

CC = gcc
AS = nasm
LD = ld
CP = cp
DEL = rm
QEMU = qemu-system-i386

CFLAGS  += -nostdinc -nostdlib -nodefaultlibs -nostartfiles -static \
	   -ffreestanding -fno-builtin -fno-exceptions -fno-stack-protector \
	   -m32 -std=gnu99 -O0 -Wall -Wextra -g
ASFLAGS = -f elf32 -g -F dwarf -O0
LDFLAGS = -m elf_i386 -nostdlib

OBJECTS = $(SOURCES:.c=.o)

all: $(PROJECT).elf

$(PROJECT).iso: $(PROJECT).elf
	$(CP) $< iso/boot/kernel.elf
	grub-mkrescue -o $@ iso

$(PROJECT).elf: boot/loader.o boot/isr.o $(OBJECTS)
	$(LD) $(LDFLAGS) -T linker.ld -o $@ $^

%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $(CCFLAGS) $<

clean:
	$(DEL) -f **/*.elf *.o **/*.o **/**/*.o iso/boot/*.elf *.img *.iso *.elf

run: $(PROJECT).iso
	$(QEMU) -soundhw pcspk -serial mon:stdio -hda $<

run-multi-1: $(PROJECT).iso
	$(QEMU) -soundhw pcspk -serial telnet:localhost:1111,nowait -drive format=raw,file=$<

run-multi-2: $(PROJECT).iso
	cp $< $<-2
	$(QEMU) -soundhw pcspk -serial telnet:localhost:2222,nowait -drive format=raw,file=$<-2
	
debug: kernel.elf
	$(QEMU) -soundhw pcspk -serial mon:stdio -s -S -kernel $< &
	gdb -iex "set auto-load safe-path .gdbinit" $<
