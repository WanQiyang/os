# C compiler and linker
CROSS_COMPILE = mipsel-linux-
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
SIZE		= $(CROSS_COMPILE)size

# Addresses of the kernel, process 1, and process 2
KERNEL_ADDR = 0xffffffffa0800200
PROC1_ADDR = 0xffffffffa0810000
PROC2_ADDR = 0xffffffffa0820000
PROC3_ADDR = 0xffffffffa0830000

INCLUDE = /home/stu/Workstation/loongson1-pmon/sys/
## C compiler flags
# No external environment
# Turn on all warnings
# Turn on optimization
# Allow use of GNU's version of C99
# -fstack-protector is not compatible with -ffreestanding
# -funit-at-time reorders functions---we need the entry point to be first in the object file
# Specify the memory layout with #defines
CFLAGS += -DKERNEL_ADDR=$(KERNEL_ADDR)
CFLAGS += -DPROC1_ADDR=$(PROC1_ADDR)
CFLAGS += -DPROC2_ADDR=$(PROC2_ADDR)
CFLAGS += -DPROC3_ADDR=$(PROC3_ADDR)
# Add debug flag
CFLAGS += -g

## Linker flags
# Don't use the standard startup files or libraries
LDFLAGS += -nostdlib
#
# Specify a location on a per-target basis
LDFLAGS += -Ttext

## First Makefile target
# Create a 1.44M floppy image for bochs etc.
floppy.img : image
	dd if=/dev/zero of=floppy.img bs=512 count=2880
	dd if=image of=floppy.img conv=notrunc

all : floppy.img

# Create a generic image to put on a boot medium
image : createimage bootblock kernel process1 process2 process3
	./createimage --extended $(wordlist 2,$(words $^),$^)   #all parameters

createimage : createimage.c
	gcc -Wall $^ -o $@

# Put the boot block at the specified address
bootblock : bootblock.o
	${CC} -G 0 -O1  -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800000 -N -o bootblock bootblock.s -nostdlib -e main -Wl,-m -Wl,elf32ltsmip -T ld.script

kernel : kernel.o 
	${CC} -G 0 -O1 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800200 -N -o kernel kernel.c entry_mips.S lock.c scheduler.c th1.c th2.c th3.c th4.c util.c queue.c -nostdlib -e _stat -Wl,-m -Wl,elf32ltsmip -T ld.script

process1 : process1.o syslib.o util.o
	${CC} -G 0 -O1 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=${PROC1_ADDR} -N -o process1 process1.c syslib.S util.c -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script

process2 : process2.o syslib.o util.o
	${CC} -G 0 -O1 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=${PROC2_ADDR} -N -o process2 process2.c syslib.S util.c -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script

process3 : process3.o syslib.o util.o
	${CC} -G 0 -O1 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=${PROC3_ADDR} -N -o process3 process3.c syslib.S util.c -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script

# Override the default implicit rule to call $(LD) directly
% : %.o
	$(LD) $(LDFLAGS) $^ -g -o $@


hello : hello1.c hello2.S
	$(CC) -mips3 $^ -o $@

# Clean up!
clean :
	rm -f floppy.img image createimage bootblock kernel process1 process2 process3 hello *.o .depend 
.PHONY : clean distclean

# Dependency management
depend : .depend
.depend :
	$(CC) -MM -g *.c *.S > $@
.PHONY : depend


# Inform make of the dependencies found by gcc
# The dash causes make to make .depend if it doesn't exist
-include .depend
