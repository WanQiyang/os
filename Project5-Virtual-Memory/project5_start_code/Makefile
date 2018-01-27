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
KERNEL_ADDR = 0xa0800200
# PROC1_ADDR = 0xa0807400
# PROC1_ADDR = 0xa0907400
PROC1_ADDR = 0x100000
# PROC2_ADDR = 0xa0809200
# PROC2_ADDR = 0xa0909200
PROC2_ADDR = 0x200000
PROC3_ADDR = 0x100000

# PROC1_LOC = 0xa0900000
PROC1_LOC = 0xa0810000
PROC1_SIZE = 0x10000
# PROC2_LOC = 0xa0a00000
PROC2_LOC = 0xa0820000
PROC2_SIZE = 0x10000

PROC3_LOC = 0xa0830000
PROC3_SIZE = 0x10000

CFLAGS += -DKERNEL_ADDR=$(KERNEL_ADDR)

CFLAGS += -DPROC1_ADDR=$(PROC1_ADDR)
CFLAGS += -DPROC1_LOC=$(PROC1_LOC)
CFLAGS += -DPROC1_SIZE=$(PROC1_SIZE)

CFLAGS += -DPROC2_ADDR=$(PROC2_ADDR)
CFLAGS += -DPROC2_LOC=$(PROC2_LOC)
CFLAGS += -DPROC2_SIZE=$(PROC2_SIZE)

CFLAGS += -DPROC3_ADDR=$(PROC3_ADDR)
CFLAGS += -DPROC3_LOC=$(PROC3_LOC)
CFLAGS += -DPROC3_SIZE=$(PROC3_SIZE)

# Add debug flag
CFLAGS += -g -I./freestanding-inc -Wl,-static,-lgcc
# CFLAGS += -g -I./freestanding-inc -Wl,-lgcc

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
bootblock : bootblock.S
	${CC} -G 0 -O2  -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800000 -N -o bootblock bootblock.S -nostdlib -e main -Wl,-m -Wl,elf32ltsmip -T ld.script

kernel : kernel.c
	${CC} ${CFLAGS} -G 0 -O2  -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=${KERNEL_ADDR} -N -o kernel kernel.c memory.c entry.S printf.c printk.c print.c  scheduler.c sync.c th1.c th2.c util.c queue.c philosophers.c barrier_test.c interrupt.c syscall.S -nostdlib -e _start -Wl,-m -Wl,elf32ltsmip -T ld.script

process1 : process1.o syslib.o util.o
	${CC} ${CFLAGS} -G 0 -O2  -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=${PROC1_ADDR} -N -o process1 process1.c syslib.c syscall.S util.c printf.c -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script
	# ${CC} ${CFLAGS} -G 0 -O2  -fPIC -mno-abicalls -fno-builtin -nostdinc -mips3 -N -o process1 process1.c syslib.c syscall.S util.c printf.c -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script

process2 : process2.o syslib.o util.o
	${CC} ${CFLAGS} -G 0 -O2  -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=${PROC2_ADDR} -N -o process2 process2.c syslib.c syscall.S util.c printf.c -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script
	# ${CC} ${CFLAGS} -G 0 -O2  -fPIC -mno-abicalls -fno-builtin -nostdinc -mips3 -N -o process2 process2.c syslib.c syscall.S util.c printf.c -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script

process3 : process3.o syslib.o util.o
	${CC} ${CFLAGS} -G 0 -O2  -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=${PROC3_ADDR} -N -o process3 process3.S -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script

# Override the default implicit rule to call $(LD) directly
% : %.o
	$(LD) $(LDFLAGS) $^ -g -o $@


hello : hello1.c hello2.S
	$(CC) -mips3 $^ -o $@

# Clean up!
clean :
	rm -f floppy.img image createimage bootblock kernel process1 process2 hello *.o .depend
.PHONY : clean distclean

# Dependency management
depend : .depend
.depend :
	$(CC) -MM -g *.c *.S > $@
.PHONY : depend


# Inform make of the dependencies found by gcc
# The dash causes make to make .depend if it doesn't exist
-include .depend
