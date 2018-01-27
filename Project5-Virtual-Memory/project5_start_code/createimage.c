#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t extended_flag = 0;
uint32_t base_addr;
uint32_t num_sector = 0;

void write_bootblock(FILE **p_image_file, FILE **p_boot_file) {
    FILE *image_file = *p_image_file;
    FILE *boot_file = *p_boot_file;

    Elf32_Ehdr *elf = NULL;
    Elf32_Phdr *ph = NULL;
    int i;
    uint8_t buf[4096];
    fread(buf, 1, 4096, boot_file);
    elf = (void*)buf;
    const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
    assert(*p_magic == elf_magic);

    for(i = 0, ph = (void *)buf + elf->e_phoff; i < elf->e_phnum; i ++) {
		if (ph[i].p_type == PT_LOAD) {
            if (ph[i].p_memsz > 512) {
                printf("ERROR: Boot block is out of memory!\n");
                exit(1);
            }
			base_addr = ph[i].p_vaddr;
            num_sector = 1;
			fseek(boot_file, ph[i].p_offset, SEEK_SET);
            uint8_t write_buf[512];
            memset(write_buf, 0, sizeof(write_buf));
			fread(write_buf, 1, ph[i].p_filesz, boot_file);
            write_buf[510] = 0x55, write_buf[511] = 0xaa;
			fwrite(write_buf, 1, 512, image_file);
            fflush(image_file);

            if (extended_flag) {
                printf("bootblock image info\n");
                printf("sectors: %u\n", num_sector);
                printf("offset of segment in the file: 0x%x\n", ph[i].p_offset);
                printf("the image's virtural address of segment in memory: 0x%x\n", ph[i].p_vaddr);
                printf("the file image size of segment: 0x%x\n", ph[i].p_filesz);
                printf("the memory image size of segment: 0x%x\n", ph[i].p_memsz);
                printf("the size of write to the OS image: 0x%x\n", ph[i].p_memsz);
                printf("padding up to 0x%x\n", num_sector * 512);
            }

            break;
		}
	}
}

void write_kernel(FILE **p_image_file, FILE **p_kernel_file, int LOC) {
    FILE *image_file = *p_image_file;
    FILE *kernel_file = *p_kernel_file;

    Elf32_Ehdr *elf = NULL;
    Elf32_Phdr *ph = NULL;
    int i, j;
    uint8_t buf[4096];
    fread(buf, 1, 4096, kernel_file);
    elf = (void*)buf;
    const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
    assert(*p_magic == elf_magic);

    for(i = 0, ph = (void *)buf + elf->e_phoff; i < elf->e_phnum; i ++) {
		if (ph[i].p_type == PT_LOAD) {
            uint32_t offset_addr = LOC - base_addr;
            uint8_t fill_buf[512];
            memset(fill_buf, 0, sizeof(fill_buf));
            if (offset_addr + ph[i].p_memsz > num_sector * 512) {
                for (j = num_sector; j * 512 < offset_addr + ph[i].p_memsz; j++) {
                    fseek(image_file, 0, SEEK_END);
                    fwrite(fill_buf, 1, 512, image_file);
                    fflush(image_file);
                }
            }

            uint8_t *write_buf = malloc(ph[i].p_filesz);
			fseek(kernel_file, ph[i].p_offset, SEEK_SET);
			fread(write_buf, 1, ph[i].p_filesz, kernel_file);
            fseek(image_file, offset_addr, SEEK_SET);
			fwrite(write_buf, 1, ph[i].p_filesz, image_file);
            fflush(image_file);
            free(write_buf);

            if (extended_flag) {
                printf("kernel image info\n");
                printf("sectors: %u\n", j - num_sector);
                printf("offset of segment in the file: 0x%x\n", ph[i].p_offset);
                printf("the image's virtural address of segment in memory: 0x%x\n", ph[i].p_vaddr);
                printf("the file image size of segment: 0x%x\n", ph[i].p_filesz);
                printf("the memory image size of segment: 0x%x\n", ph[i].p_memsz);
                printf("the size of write to the OS image: 0x%x\n", ph[i].p_memsz);
                printf("padding up to 0x%x\n", (j - num_sector) * 512);
            }

            num_sector = j;
            break;
		}
	}
}

int main(int argc, char *argv[]) {

    if (argc > 1 && strcmp(argv[1], "--extended") == 0)
        extended_flag = 1;

    if (argc <= extended_flag + 1) {
        printf("ERROR: A boot file is required!\n");
        exit(1);
    }

    FILE *image_file = fopen("./image", "wb");
    if (image_file == NULL) {
        printf("ERROR: Cannot create image file!\n");
        exit(1);
    }

    /*
    FILE *boot_file = fopen(argv[extended_flag + 1], "rb");
    if (boot_file == NULL) {
        printf("ERROR: Cannot open boot file: %s!\n", argv[extended_flag + 1]);
        exit(1);
    }
    write_bootblock(&image_file, &boot_file);
    fclose(boot_file);

    int i;
    for (i = extended_flag + 2; i < argc; i++) {
        FILE *kernel_file = fopen(argv[i], "rb");
        if (kernel_file == NULL) {
            printf("ERROR: Cannot open kernel file: %s!\n", argv[i]);
            exit(1);
        }
        write_kernel(&image_file, &kernel_file);
        fclose(kernel_file);
    }
    */

    #define KERNEL_ADDR 0xa0800200
    #define PROC1_LOC 0xa0810000
    #define PROC2_LOC 0xa0820000
    #define PROC3_LOC 0xa0830000

    FILE *boot_file, *kernel_file;

    boot_file = fopen("bootblock", "rb");
    if (boot_file == NULL) {
        printf("ERROR: Cannot open boot file: bootblock!\n");
        exit(1);
    }
    write_bootblock(&image_file, &boot_file);
    fclose(boot_file);

    kernel_file = fopen("kernel", "rb");
    if (kernel_file == NULL) {
        printf("ERROR: Cannot open kernel file: kernel!\n");
        exit(1);
    }
    write_kernel(&image_file, &kernel_file, KERNEL_ADDR);
    fclose(kernel_file);

    kernel_file = fopen("process1", "rb");
    if (kernel_file == NULL) {
        printf("ERROR: Cannot open kernel file: process1!\n");
        exit(1);
    }
    write_kernel(&image_file, &kernel_file, PROC1_LOC);
    fclose(kernel_file);

    kernel_file = fopen("process2", "rb");
    if (kernel_file == NULL) {
        printf("ERROR: Cannot open kernel file: process2!\n");
        exit(1);
    }
    write_kernel(&image_file, &kernel_file, PROC2_LOC);
    fclose(kernel_file);

    kernel_file = fopen("process3", "rb");
    if (kernel_file == NULL) {
        printf("ERROR: Cannot open kernel file: process3!\n");
        exit(1);
    }
    write_kernel(&image_file, &kernel_file, PROC3_LOC);
    fclose(kernel_file);

    num_sector--;
    fseek(image_file, 0, SEEK_SET);
    fwrite(&num_sector, sizeof(num_sector), 1, image_file);
    fflush(image_file);
    fclose(image_file);
    return 0;
}
