#include "common.h"

/*define global variables here*/

/*
 Use linear table or other data structures as you need.

 example:
 struct file_info[MAX_OPEN_FILE] fd_table;
 struct inode[MAX_INODE] inode_table;
 unsigned long long block_bit_map[];
 Your dentry index structure, and so on.


 //keep your root dentry and/or root data block
 //do path parse from your filesystem ROOT<@mount point>

 struct dentry* root;
 */

struct superblock* current_sb;
struct blockgroup* current_bg;
struct inode* current_root;
uint32_t* inode_bitmap;
uint32_t* block_bitmap;
struct file_info* file_handle;
pthread_mutex_t* inode_lock;
pthread_mutex_t inode_bitmap_lock, block_bitmap_lock;
pthread_mutex_t read_lock, write_lock;

uint32_t checknum(uint32_t* buf, uint32_t checksize) {
    checksize >>= 2;
    uint32_t i, result = 0;
    for(i=0;i<checksize;i++)
        result ^= buf[i];
    return result;
}

uint32_t timestamp() {
    return time(NULL);
}

uint8_t read_inode_bit(uint32_t inum) {
    uint32_t gnum = inum / INODE_PER_GROUP;
    inum %= INODE_PER_GROUP;
    return ((((uint8_t*)inode_bitmap)[(gnum * INODE_PER_GROUP + inum) >> 3] & (1 << (inum % 8))) != 0);
}

void write_inode_bit(uint32_t inum, uint8_t value) {
    pthread_mutex_lock(&inode_bitmap_lock);
    uint32_t gnum = inum / INODE_PER_GROUP;
    inum %= INODE_PER_GROUP;
    uint8_t old_byte, new_byte;
    old_byte = ((uint8_t*)inode_bitmap)[(gnum * INODE_PER_GROUP + inum) >> 3];
    new_byte = old_byte & ~(1 << (inum % 8)) | (value << (inum % 8));
    ((uint8_t*)inode_bitmap)[(gnum * INODE_PER_GROUP + inum) >> 3] = new_byte;
    uint8_t buf[SECTOR_SIZE];
    memcpy((void*)buf, (void*)(((uint8_t*)inode_bitmap) + (((gnum * INODE_PER_GROUP + inum) >> 3) & 0xfffff000)), SECTOR_SIZE);
    device_write_sector(buf, gnum * SECTOR_PER_GROUP + INODE_BITMAP_ENTRY_SECTOR + (inum >> 15));

    if(value) {
        current_sb->sb->s_free_inodes_count--;
        current_bg[gnum].bg->bg_free_inodes_count--;
    } else {
        current_sb->sb->s_free_inodes_count++;
        current_bg[gnum].bg->bg_free_inodes_count++;
    }
    update_superblock();
    update_blockgroup(gnum);
    device_flush();
    pthread_mutex_unlock(&inode_bitmap_lock);
    return;
}

uint8_t read_block_bit(uint32_t bnum) {
    uint32_t gnum = bnum / BLOCK_PER_GROUP;
    bnum %= BLOCK_PER_GROUP;
    return ((((uint8_t*)block_bitmap)[(gnum * BLOCK_PER_GROUP + bnum) >> 3] & (1 << (bnum % 8))) != 0);
}

void write_block_bit(uint32_t bnum, uint8_t value) {
    pthread_mutex_lock(&block_bitmap_lock);
    uint32_t gnum = bnum / BLOCK_PER_GROUP;
    bnum %= BLOCK_PER_GROUP;
    uint8_t old_byte, new_byte;
    old_byte = ((uint8_t*)block_bitmap)[(gnum * BLOCK_PER_GROUP + bnum) >> 3];
    new_byte = old_byte & ~(1 << (bnum % 8)) | (value << (bnum % 8));
    ((uint8_t*)block_bitmap)[(gnum * BLOCK_PER_GROUP + bnum) >> 3] = new_byte;
    uint8_t buf[SECTOR_SIZE];
    memcpy((void*)buf, (void*)(((uint8_t*)block_bitmap) + (((gnum * BLOCK_PER_GROUP + bnum) >> 3) & 0xfffff000)), SECTOR_SIZE);
    device_write_sector(buf, gnum * SECTOR_PER_GROUP + BLOCK_BITMAP_ENTRY_SECTOR + (bnum >> 15));

    if(value) {
        current_sb->sb->s_free_blocks_count--;
        current_bg[gnum].bg->bg_free_blocks_count--;
    } else {
        current_sb->sb->s_free_blocks_count++;
        current_bg[gnum].bg->bg_free_blocks_count++;
    }
    update_superblock();
    update_blockgroup(gnum);
    device_flush();
    pthread_mutex_unlock(&block_bitmap_lock);
    return;
}

void read_inode(uint32_t inum, struct inode_t* inode) {
    uint32_t gnum = inum / INODE_PER_GROUP;
    inum %= INODE_PER_GROUP;
    uint8_t buf[SECTOR_SIZE];
    device_read_sector(buf, SECTOR_PER_GROUP * gnum + INODE_ENTRY_SECTOR + (inum / INODE_PER_SECTOR));
    memcpy((void*)inode, (void*)(buf + (inum % INODE_PER_SECTOR) * INODE_SIZE), INODE_SIZE);
    return;
}

void write_inode(uint32_t inum, struct inode_t* inode) {
    pthread_mutex_lock(&inode_lock[inum]);
    uint32_t gnum = inum / INODE_PER_GROUP;
    inum %= INODE_PER_GROUP;
    uint8_t buf[SECTOR_SIZE];
    device_read_sector(buf, SECTOR_PER_GROUP * gnum + INODE_ENTRY_SECTOR + (inum / INODE_PER_SECTOR));
    memcpy((void*)(buf + (inum % INODE_PER_SECTOR) * INODE_SIZE), (void*)inode, INODE_SIZE);
    device_write_sector(buf, SECTOR_PER_GROUP * gnum + INODE_ENTRY_SECTOR + (inum / INODE_PER_SECTOR));
    device_flush();
    pthread_mutex_unlock(&inode_lock[inum]);
    return;
}

void read_block(uint32_t bnum, uint8_t* buf) {
    uint32_t gnum = bnum / BLOCK_PER_GROUP;
    bnum %= BLOCK_PER_GROUP;
    device_read_sector(buf, SECTOR_PER_GROUP * gnum + bnum);
    return;
}

void write_block(uint32_t bnum, uint8_t* buf) {
    uint32_t gnum = bnum / BLOCK_PER_GROUP;
    bnum %= BLOCK_PER_GROUP;
    if(bnum % BLOCK_PER_GROUP < DATA_ENTRY_SECTOR) {
        DEBUG("error occured when writing (group %u, block %u).", gnum, bnum);
        return;
    }
    device_write_sector(buf, SECTOR_PER_GROUP * gnum + bnum);
    device_flush();
    return;
}

void update_superblock() {
    pthread_mutex_lock(&current_sb->lock);
    current_sb->sb->s_checknum = checknum((uint32_t*)current_sb->sb, sizeof(struct superblock_t) - 4);
    uint8_t buf[SECTOR_SIZE];
    memset(buf, 0, sizeof(buf));
    memcpy((void*)buf, (void*)current_sb->sb, sizeof(struct superblock_t));
    device_write_sector(buf, FIRST_SUPERBLOCK_SECTOR);
    device_write_sector(buf, SECOND_SUPERBLOCK_SECTOR);
    device_flush();
    pthread_mutex_unlock(&current_sb->lock);
    return;
}

void update_blockgroup(int gnum) {
    pthread_mutex_lock(&current_bg[gnum].lock);
    current_bg[gnum].bg->bg_checknum = checknum((uint32_t*)current_bg[gnum].bg, sizeof(struct superblock_t) - 4);
    uint8_t buf[SECTOR_SIZE];
    memset(buf, 0, sizeof(buf));
    memcpy((void*)buf, (void*)current_bg[gnum].bg, sizeof(struct blockgroup_t));
    device_write_sector(buf, gnum * SECTOR_PER_GROUP + 1);
    device_flush();
    pthread_mutex_unlock(&current_bg[gnum].lock);
    return;
}

uint32_t find_free_inode() {
    pthread_mutex_lock(&inode_bitmap_lock);
    uint32_t i, j, k, len_bitmap;
    uint8_t* p;
    len_bitmap = (current_sb->sb->s_groups_count * INODE_BITMAP_SECTOR_PER_GROUP * SECTOR_SIZE) >> 2;
    for(i=0;i<len_bitmap;i++) {
        if(inode_bitmap[i] != 0xffffffff) {
            p = (uint8_t*)&(inode_bitmap[i]);
            for(j=0;j<4;j++)
                if(p[j] != 0xff)
                    for(k=0;k<8;k++)
                        if((p[j] & (1<<k)) == 0) {
                            pthread_mutex_unlock(&inode_bitmap_lock);
                            return (i*32+j*8+k);
                        }
        }
    }
    DEBUG("error occured when looking for free inode.");
    pthread_mutex_unlock(&inode_bitmap_lock);
    return -ENOSPC;
}

uint32_t find_free_block() {
    pthread_mutex_lock(&block_bitmap_lock);
    uint32_t i, j, k, len_bitmap;
    uint8_t* p;
    len_bitmap = (current_sb->sb->s_groups_count * BLOCK_SECTOR_PER_GROUP * SECTOR_SIZE) >> 2;
    for(i=0;i<len_bitmap;i++) {
        // TODO: skip control sector
        if(block_bitmap[i] != 0xffffffff) {
            p = (uint8_t*)&(block_bitmap[i]);
            for(j=0;j<4;j++)
                if(p[j] != 0xff)
                    for(k=0;k<8;k++)
                        if((p[j] & (1<<k)) == 0) {
                            pthread_mutex_unlock(&block_bitmap_lock);
                            return (i*32+j*8+k);
                        }
        }
    }
    DEBUG("error occured when looking for free block.");
    pthread_mutex_unlock(&block_bitmap_lock);
    return -ENOSPC;
}

uint32_t read_block_index(struct inode_t* ino, uint32_t idx) {
    if(idx<FIRST_POINTER) {
        return ino->i_block[idx];
    }
    uint32_t buf1[POINTER_PER_BLOCK];
    if(idx<SECOND_POINTER) {
        if(ino->i_block[12] == 0) return -1;
        read_block(ino->i_block[12], buf1);
        return buf1[idx - FIRST_POINTER];
    }
    uint32_t buf2[POINTER_PER_BLOCK];
    if(idx<THIRD_POINTER) {
        if(ino->i_block[13] == 0) return -1;
        read_block(ino->i_block[13], buf1);
        if(buf1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK] == 0) return -1;
        read_block(buf1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK], buf2);
        return buf2[(idx - SECOND_POINTER) % POINTER_PER_BLOCK];
    }
    uint32_t buf3[POINTER_PER_BLOCK];
    if(idx<MAX_INDEX) {
        if(ino->i_block[14] == 0) return -1;
        read_block(ino->i_block[14], buf1);
        if(buf1[(idx - THIRD_POINTER) / (POINTER_PER_BLOCK * POINTER_PER_BLOCK)] == 0) return -1;
        read_block(buf1[(idx - THIRD_POINTER) / (POINTER_PER_BLOCK * POINTER_PER_BLOCK)], buf2);
        if(buf2[((idx - THIRD_POINTER) % (POINTER_PER_BLOCK * POINTER_PER_BLOCK)) / POINTER_PER_BLOCK] == 0) return -1;
        read_block(buf2[((idx - THIRD_POINTER) % (POINTER_PER_BLOCK * POINTER_PER_BLOCK)) / POINTER_PER_BLOCK], buf3);
        return buf3[(idx - THIRD_POINTER) % POINTER_PER_BLOCK];
    }
    return -1;
}

void write_block_index(struct inode_t* ino, uint32_t idx, uint32_t bnum) {
    if(idx<FIRST_POINTER) {
        ino->i_block[idx] = bnum;
        write_inode(ino->i_num, ino);
        return;
    }
    uint8_t empty_buf[SECTOR_SIZE];
    memset(empty_buf, 0, sizeof(empty_buf));
    uint32_t free_bnum1, buf1[POINTER_PER_BLOCK];
    if(idx<SECOND_POINTER) {
        if(ino->i_block[12] == 0) {
            free_bnum1 = find_free_block();
            if(free_bnum1 == -ENOSPC) return;
            write_block_bit(free_bnum1, 1);
            write_block(free_bnum1, empty_buf);
            ino->i_block[12] = free_bnum1;
            write_inode(ino->i_num, ino);
        }
        read_block(ino->i_block[12], buf1);
        buf1[idx - FIRST_POINTER] = bnum;
        write_block(ino->i_block[12], buf1);
        return;
    }
    uint32_t free_bnum2, buf2[POINTER_PER_BLOCK];
    if(idx<THIRD_POINTER) {
        if(ino->i_block[13] == 0) {
            free_bnum1 = find_free_block();
            if(free_bnum1 == -ENOSPC) return;
            write_block_bit(free_bnum1, 1);
            write_block(free_bnum1, empty_buf);
            ino->i_block[13] = free_bnum1;
            write_inode(ino->i_num, ino);
        }
        read_block(ino->i_block[13], buf1);
        if(buf1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK] == 0) {
            free_bnum2 = find_free_block();
            if(free_bnum2 == -ENOSPC) return;
            write_block_bit(free_bnum2, 1);
            write_block(free_bnum2, empty_buf);
            buf1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK] = free_bnum2;
            write_block(ino->i_block[13], buf1);
        }
        read_block(buf1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK], buf2);
        buf2[(idx - SECOND_POINTER) % POINTER_PER_BLOCK] = bnum;
        write_block(buf1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK], buf2);
        return;
    }
    uint32_t free_bnum3, buf3[POINTER_PER_BLOCK];
    if(idx<MAX_INDEX) {
        if(ino->i_block[14] == 0) {
            free_bnum1 = find_free_block();
            if(free_bnum1 == -ENOSPC) return;
            write_block_bit(free_bnum1, 1);
            write_block(free_bnum1, empty_buf);
            ino->i_block[14] = free_bnum1;
            write_inode(ino->i_num, ino);
        }
        read_block(ino->i_block[14], buf1);
        if(buf1[(idx - THIRD_POINTER) / (POINTER_PER_BLOCK * POINTER_PER_BLOCK)] == 0) {
            free_bnum2 = find_free_block();
            if(free_bnum2 == -ENOSPC) return;
            write_block_bit(free_bnum2, 1);
            write_block(free_bnum2, empty_buf);
            buf1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK] = free_bnum2;
            write_block(ino->i_block[14], buf1);
        }
        read_block(buf1[(idx - THIRD_POINTER) / (POINTER_PER_BLOCK * POINTER_PER_BLOCK)], buf2);
        if(buf2[((idx - THIRD_POINTER) % (POINTER_PER_BLOCK * POINTER_PER_BLOCK)) / POINTER_PER_BLOCK] == 0) {
            free_bnum3 = find_free_block();
            if(free_bnum3 == -ENOSPC) return;
            write_block_bit(free_bnum3, 1);
            write_block(free_bnum3, empty_buf);
            buf2[((idx - THIRD_POINTER) % (POINTER_PER_BLOCK * POINTER_PER_BLOCK)) / POINTER_PER_BLOCK] = free_bnum3;
            write_block(buf1[(idx - THIRD_POINTER) / (POINTER_PER_BLOCK * POINTER_PER_BLOCK)], buf2);
        }
        read_block(buf2[((idx - THIRD_POINTER) % (POINTER_PER_BLOCK * POINTER_PER_BLOCK)) / POINTER_PER_BLOCK], buf3);
        buf3[(idx - THIRD_POINTER) % POINTER_PER_BLOCK] = bnum;
        write_block(buf2[((idx - THIRD_POINTER) % (POINTER_PER_BLOCK * POINTER_PER_BLOCK)) / POINTER_PER_BLOCK], buf3);
        return;
    }
    return;
}

uint32_t find_file(struct inode_t* ino, const char* name) {
    uint32_t i, j;
    uint8_t buf[SECTOR_SIZE];
    struct dentry* p = buf;
    for(i=0;i<MAX_INDEX;i++) {
        read_block(read_block_index(ino, i), buf);
        for(j=0;j<DENTRY_PER_BLOCK;j++) {
            if(i * DENTRY_PER_BLOCK + j >= ino->i_fnum + 2)
                return -ENOENT;
            if(strcmp(name, p[j].d_name) == 0)
                return p[j].d_inum;
        }
    }
    // TODO: add indirect block mapping
    return -ENOENT;
}

uint32_t parse_path(const char *path) {
    DEBUG("path parse: %s", path);
    char input[MAX_PATH_LENGTH];
    char* p;
    strcpy(input, path);
    p = strtok(input, "/");
    if(p==NULL) return 0; // root
    uint32_t result = find_file(current_root->ino, p); // root
    DEBUG("path parse result: (%s, %u)", p, result);
    if(result==-ENOENT) {
        DEBUG("error occured when parsing path: %s.", path);
        return -ENOENT; // no such file
    }
    struct inode_t ino;
    while((p=strtok(NULL, "/")) != NULL) {
        read_inode(result, &ino);
        result = find_file(&ino, p);
        DEBUG("path parse result: (%s, %u)", p, result);
        if(result == -ENOENT) {
            DEBUG("error occured when parsing path: %s.", path);
            return -ENOENT;
        }
    }
    return result;
}

void separate_path(const char *path, char *parent, char *name) {
    strcpy(parent, path);
    char* loc = strrchr(parent, '/');
    strcpy(name, loc + 1);
    name[MAX_NAME_LENGTH - 1] = 0;
    if(loc == parent) loc++;
    *loc = 0;
    return;
}

void read_dentry(struct inode_t* ino, uint32_t dnum, struct dentry* den) {
    if(dnum >= ino->i_fnum + 2) return -ENOENT;
    uint32_t major_loc, minor_loc;
    major_loc = dnum / DENTRY_PER_BLOCK;
    minor_loc = dnum % DENTRY_PER_BLOCK;
    uint8_t buf[SECTOR_SIZE];
    struct dentry* den_tmp = buf;
    read_block(read_block_index(ino, major_loc), buf);
    memcpy((void*)den, (void*)&(den_tmp[minor_loc]), sizeof(struct dentry));
    return;
}

void write_dentry(struct inode_t* ino, uint32_t dnum, struct dentry* den) {
    if(dnum < 2) return -EROFS; // . and ..
    if(dnum > ino->i_fnum + 2) return -ENOENT;
    uint32_t major_loc, minor_loc;
    major_loc = dnum / DENTRY_PER_BLOCK;
    minor_loc = dnum % DENTRY_PER_BLOCK;
    uint8_t buf[SECTOR_SIZE];
    memset(buf, 0, sizeof(buf));
    struct dentry* den_tmp = buf;
    if(read_block_index(ino, major_loc) == 0) {
        uint32_t free_bnum = find_free_block();
        if(free_bnum == -ENOSPC)
            return -ENOSPC;
        write_block_bit(free_bnum, 1);
        write_block_index(ino, major_loc, free_bnum);
        ino->i_size += SECTOR_SIZE;
    } else {
        read_block(read_block_index(ino, major_loc), buf);
    }
    memcpy((void*)&(den_tmp[minor_loc]), (void*)den, sizeof(struct dentry));
    write_block(read_block_index(ino, major_loc), buf);

    if(dnum == ino->i_fnum + 2) ino->i_fnum++;

    device_flush();
    return;
}

uint32_t find_dentry(struct inode_t* ino, const char* name) {
    uint32_t i, j;
    uint8_t buf[SECTOR_SIZE];
    struct dentry* p = buf;
    for(i=0;i<MAX_INDEX;i++) {
        read_block(read_block_index(ino, i), buf);
        for(j=0;j<DENTRY_PER_BLOCK;j++) {
            if(i * DENTRY_PER_BLOCK + j >= ino->i_fnum + 2)
                return -ENOENT;
            if(strcmp(name, p[j].d_name) == 0)
                return (i * DENTRY_PER_BLOCK + j);
        }
    }
    return -ENOENT;
}

void remove_dentry(struct inode_t* ino, uint32_t dnum) {
    if(dnum < 2) return -EROFS;
    if(dnum >= ino->i_fnum + 2) return -ENOENT;
    struct dentry last_dentry;
    read_dentry(ino, ino->i_fnum + 1, &last_dentry);
    write_dentry(ino, dnum, &last_dentry);
    // TODO: release block
    ino->i_fnum--;
    return;
}

void release_block(struct inode_t* ino) {
    uint32_t i, j, k;
    for(i=0;i<FIRST_POINTER;i++) {
        if(ino->i_block[i] == 0)
            return;
        write_block_bit(ino->i_block[i], 0);
    }

    if(ino->i_block[12] == 0)
        return;
    uint32_t buf1[POINTER_PER_BLOCK];
    read_block(ino->i_block[12], buf1);
    for(i=0;i<POINTER_PER_BLOCK;i++) {
        if(buf1[i] == 0) {
            write_block_bit(ino->i_block[12], 0);
            return;
        }
        write_block_bit(buf1[i], 0);
    }
    write_block_bit(ino->i_block[12], 0);

    if(ino->i_block[13] == 0)
        return;
    uint32_t buf2[POINTER_PER_BLOCK];
    read_block(ino->i_block[13], buf1);
    for(i=0;i<POINTER_PER_BLOCK;i++) {
        if(buf1[i] == 0) {
            write_block_bit(ino->i_block[13], 0);
            return;
        }
        read_block(buf1[i], buf2);
        for(j=0;j<POINTER_PER_BLOCK;j++) {
            if(buf2[j] == 0) {
                write_block_bit(buf1[i], 0);
                write_block_bit(ino->i_block[13], 0);
                return;
            }
            write_block_bit(buf2[j], 0);
        }
        write_block_bit(buf1[i], 0);
    }
    write_block_bit(ino->i_block[13], 0);

    if(ino->i_block[14] == 0)
        return;
    uint32_t buf3[POINTER_PER_BLOCK];
    read_block(ino->i_block[14], buf1);
    for(i=0;i<POINTER_PER_BLOCK;i++) {
        if(buf1[i] == 0) {
            write_block_bit(ino->i_block[14], 0);
            return;
        }
        read_block(buf1[i], buf2);
        for(j=0;j<POINTER_PER_BLOCK;j++) {
            if(buf2[j] == 0) {
                write_block_bit(buf1[i], 0);
                write_block_bit(ino->i_block[14], 0);
                return;
            }
            read_block(buf2[j], buf3);
            for(k=0;k<POINTER_PER_BLOCK;k++) {
                if(buf3[k] == 0) {
                    write_block_bit(buf2[j], 0);
                    write_block_bit(buf1[i], 0);
                    write_block_bit(ino->i_block[14], 0);
                    return 0;
                }
                write_block_bit(buf3[k], 0);
            }
            write_block_bit(buf2[j], 0);
        }
        write_block_bit(buf1[i], 0);
    }
    write_block_bit(ino->i_block[14], 0);
    return;
}

void p6fs_mkfs() {
    uint64_t size;
    size = device_size();
    uint32_t group_num, remain_size;
    group_num = size / GROUP_SIZE;
    remain_size = size % GROUP_SIZE;
    if(remain_size >= MIN_DISK_SIZE)
        group_num ++; // create last group
    uint32_t inode_num = 0, block_num = 0;
    uint8_t buf[SECTOR_SIZE];
    struct superblock_t* sb = buf;
    struct blockgroup_t* bg = buf;
    uint32_t i, j;
    for(i=0;i<group_num;i++) {
        // block group descriptor
        memset(buf, 0, sizeof(buf));
        bg->bg_total_inodes_count = INODE_PER_GROUP;
        if(i<group_num-1)
            bg->bg_total_blocks_count = BLOCK_SECTOR_PER_GROUP;
        else
            bg->bg_total_blocks_count = (remain_size / SECTOR_SIZE) - CONTROL_SECTOR_PER_GROUP;
        bg->bg_free_inodes_count = bg->bg_total_inodes_count;
        bg->bg_free_blocks_count = bg->bg_total_blocks_count;
        inode_num += bg->bg_total_inodes_count;
        block_num += bg->bg_total_blocks_count;
        bg->bg_checknum = checknum((uint32_t*)bg, sizeof(struct blockgroup_t) - 4);
        device_write_sector(buf, i * SECTOR_PER_GROUP + 1);
        // inode bitmap
        memset(buf, 0, sizeof(buf));
        for(j=INODE_BITMAP_ENTRY_SECTOR;j<BLOCK_BITMAP_ENTRY_SECTOR;j++)
            device_write_sector(buf, i * SECTOR_PER_GROUP + j);
        // block bitmap
        memset(buf, 0, sizeof(buf));
        memset(buf, 0xff, CONTROL_SECTOR_PER_GROUP >> 3);
        buf[CONTROL_SECTOR_PER_GROUP >> 3] = (uint8_t)((1 << (CONTROL_SECTOR_PER_GROUP % 8)) - 1);
        device_write_sector(buf, i * SECTOR_PER_GROUP + BLOCK_BITMAP_ENTRY_SECTOR);
        memset(buf, 0, sizeof(buf));
        for(j=BLOCK_BITMAP_ENTRY_SECTOR+1;j<INODE_ENTRY_SECTOR;j++)
            device_write_sector(buf, i * SECTOR_PER_GROUP + j);
        // unused superblock
        if(i>=2)
            device_write_sector(buf, i * SECTOR_PER_GROUP);
    }
    // used superblock
    memset(buf, 0, sizeof(buf));
    sb->s_size = size;
    sb->s_magic = MAGIC;
    sb->s_groups_count = group_num;
    sb->s_total_inodes_count = inode_num;
    sb->s_total_blocks_count = block_num;
    sb->s_free_inodes_count = sb->s_total_inodes_count;
    sb->s_free_blocks_count = sb->s_total_blocks_count;
    sb->s_checknum = checknum((uint32_t*)sb, sizeof(struct superblock_t) - 4);
    device_write_sector(buf, 0);
    if(size >= MIN_DISK_SIZE_FOR_DOUBLE_SUPERBLOCK)
        device_write_sector(buf, SECTOR_PER_GROUP);

    p6fs_mount();

    // root
    write_inode_bit(0, 1);
    // current_sb->sb->s_free_inodes_count--;
    // current_bg[0].bg->bg_free_inodes_count--;

    write_block_bit(DATA_ENTRY_SECTOR, 1);
    // current_sb->sb->s_free_blocks_count--;
    // current_bg[0].bg->bg_free_blocks_count--;

    struct inode_t root_inode;
    root_inode.i_mode = S_IFDIR | 0755;
    root_inode.i_links_count = 1;
    root_inode.i_atime = root_inode.i_ctime = root_inode.i_mtime = timestamp();
    root_inode.i_size = SECTOR_SIZE;
    root_inode.i_fnum = 0;
    memset(root_inode.i_block, 0, sizeof(root_inode.i_block));
    root_inode.i_block[0] = DATA_ENTRY_SECTOR;
    memset(root_inode.i_padding, 0, sizeof(root_inode.i_padding));
    root_inode.i_num = 0;
    write_inode(0, &root_inode);
    memcpy((void*)current_root->ino, (void*)&root_inode, INODE_SIZE);

    memset(buf, 0, sizeof(buf));
    struct dentry* den = buf;
    den[0].d_inum = 0;
    strcpy(den[0].d_name, ".");
    den[1].d_inum = 0;
    strcpy(den[1].d_name, "..");
    write_block(DATA_ENTRY_SECTOR, buf);

    // update_superblock();
    // update_blockgroup(0);

    return;
}

void p6fs_mount() {
    uint8_t buf[SECTOR_SIZE];
    // superblock
    current_sb = malloc(sizeof(struct superblock));
    struct superblock_t *sb = malloc(sizeof(struct superblock_t));
    device_read_sector(buf, FIRST_SUPERBLOCK_SECTOR);
    memcpy((void*)sb, (void*)buf, sizeof(struct superblock_t));
    current_sb->sb = sb;
    pthread_mutex_init(&current_sb->lock, NULL);
    uint32_t group_num = current_sb->sb->s_groups_count;
    // blockgroup
    current_bg = malloc(sizeof(struct blockgroup) * group_num);
    struct blockgroup_t *bg;
    uint32_t i, j;
    for(i=0;i<group_num;i++) {
        bg = malloc(sizeof(struct blockgroup_t));
        device_read_sector(buf, i * SECTOR_PER_GROUP + 1);
        memcpy((void*)bg, (void*)buf, sizeof(struct blockgroup_t));
        current_bg[i].bg = bg;
        pthread_mutex_init(&current_bg[i].lock, NULL);
    }
    // inode bitmap
    inode_bitmap = malloc(group_num * INODE_BITMAP_SECTOR_PER_GROUP * SECTOR_SIZE);
    for(i=0;i<group_num;i++) {
        for(j=0;j<INODE_BITMAP_SECTOR_PER_GROUP;j++) {
            device_read_sector(buf, i * SECTOR_PER_GROUP + INODE_BITMAP_ENTRY_SECTOR + j);
            memcpy((void*)(((uint8_t*)inode_bitmap) + (i * INODE_BITMAP_SECTOR_PER_GROUP + j) * SECTOR_SIZE), (void*)buf, SECTOR_SIZE);
        }
    }
    // block bitmap
    block_bitmap = malloc(group_num * BLOCK_BITMAP_SECTOR_PER_GROUP * SECTOR_SIZE);
    for(i=0;i<group_num;i++) {
        for(j=0;j<BLOCK_BITMAP_SECTOR_PER_GROUP;j++) {
            device_read_sector(buf, i * SECTOR_PER_GROUP + BLOCK_BITMAP_ENTRY_SECTOR + j);
            memcpy((void*)(((uint8_t*)block_bitmap) + (i * BLOCK_BITMAP_SECTOR_PER_GROUP + j) * SECTOR_SIZE), (void*)buf, SECTOR_SIZE);
        }
    }
    // root
    current_root = malloc(sizeof(struct inode));
    current_root->ino = malloc(sizeof(struct inode_t));
    read_inode(0, current_root->ino);
    // file handle
    file_handle = malloc(MAX_OPEN_FILE * sizeof(struct file_info));
    memset(file_handle, 0, MAX_OPEN_FILE * sizeof(struct file_info));
    // inode lock
    inode_lock = malloc(current_sb->sb->s_total_inodes_count * sizeof(pthread_mutex_t));
    for(i=0;i<current_sb->sb->s_total_inodes_count;i++) {
        pthread_mutex_init(&inode_lock[i], NULL);
    }
    // bitmap lock
    pthread_mutex_init(&inode_bitmap_lock, NULL);
    pthread_mutex_init(&block_bitmap_lock, NULL);
    // rw lock
    pthread_mutex_init(&read_lock, NULL);
    pthread_mutex_init(&write_lock, NULL);
    return;
}

int p6fs_mkdir(const char *path, mode_t mode)
{
    /*do path parse here
     create dentry  and update your index*/
    DEBUG("mkdir: (%s, %u)", path, mode);
    char parent[MAX_PATH_LENGTH], name[MAX_NAME_LENGTH];
    separate_path(path, parent, name);
    uint32_t parent_inum, free_inum, free_bnum;
    parent_inum = parse_path(parent);
    if(parent_inum == -ENOENT) {
        ERR("error occured when mkdir.");
        return -ENOENT;
    }
    struct inode_t parent_ino, new_ino;
    read_inode(parent_inum, &parent_ino);
    free_inum = find_free_inode();
    free_bnum = find_free_block();
    if(free_inum == -ENOSPC || free_bnum == -ENOSPC) {
        ERR("error occured when mkdir.");
        return -ENOSPC;
    }
    DEBUG("mkdir: inum=%u, bnum=%u", free_inum, free_bnum);
    write_inode_bit(free_inum, 1);
    write_block_bit(free_bnum, 1);

    // new_ino.i_mode = S_IFDIR;
    new_ino.i_mode = S_IFDIR | mode;
    new_ino.i_links_count = 1;
    new_ino.i_atime = new_ino.i_ctime = new_ino.i_mtime = timestamp();
    new_ino.i_size = SECTOR_SIZE;
    new_ino.i_fnum = 0;
    memset(new_ino.i_block, 0, sizeof(new_ino.i_block));
    new_ino.i_block[0] = free_bnum;
    memset(new_ino.i_padding, 0, sizeof(new_ino.i_padding));
    new_ino.i_num = free_inum;
    write_inode(free_inum, &new_ino);

    uint8_t buf[SECTOR_SIZE];
    memset(buf, 0, sizeof(buf));
    struct dentry* den = buf;
    den[0].d_inum = free_inum;
    strcpy(den[0].d_name, ".");
    den[1].d_inum = parent_inum;
    strcpy(den[1].d_name, "..");
    write_block(free_bnum, buf);

    struct dentry parent_den;
    parent_den.d_inum = free_inum;
    strcpy(parent_den.d_name, name);
    write_dentry(&parent_ino, parent_ino.i_fnum + 2, &parent_den);
    write_inode(parent_inum, &parent_ino);

    if(strcmp(parent, "/") == 0)
        read_inode(0, current_root->ino);

    return 0;
}

int p6fs_rmdir(const char *path)
{
    DEBUG("rmdir: %s", path);
    uint32_t parent_inum, inum;
    inum = parse_path(path);
    if(inum == -ENOENT)
        return -ENOENT;


    struct inode_t parent_ino, ino;
    read_inode(inum, &ino);
    if(ino.i_mode & S_IFDIR == 0)
        return -ENOTDIR;
    if(ino.i_fnum > 0)
        return -ENOTEMPTY;

    write_inode_bit(inum, 0);
    release_block(&ino);

    char parent[MAX_PATH_LENGTH], name[MAX_NAME_LENGTH];
    separate_path(path, parent, name);

    parent_inum = parse_path(parent);
    read_inode(parent_inum, &parent_ino);
    uint32_t dnum;
    dnum = find_dentry(&parent_ino, name);
    remove_dentry(&parent_ino, dnum);
    parent_ino.i_mtime = parent_ino.i_ctime = timestamp();
    write_inode(parent_inum, &parent_ino);

    if(strcmp(parent, "/") == 0)
        read_inode(0, current_root->ino);

    return 0;
}

int p6fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo)
{
    DEBUG("readdir: %s", path);
    uint32_t inum, i, j;
    inum = parse_path(path);
    if(inum == -ENOENT)  {
        ERR("error occured when readdir.");
        return -ENOENT;
    }
    struct inode_t ino;
    read_inode(inum, &ino);
    uint8_t buffer[SECTOR_SIZE];
    struct dentry* den = buffer;
    uint32_t count = 0;
    for(i=0;i<MAX_INDEX;i++) {
        read_block(read_block_index(&ino, i), &buffer);
        for(j=0;j<DENTRY_PER_BLOCK;j++) {
            if(count >= ino.i_fnum + 2) return 0; // . and ..
            DEBUG("readdir: (%s, %s)", path, den[j].d_name);
            if(filler(buf, den[j].d_name, NULL, 0) == 1) {
                ERR("error occured when readdir.");
                return -ENOMEM;
            }
            count++;
        }
    }
    ino.i_atime = timestamp();
    write_inode(inum, &ino);

    if(strcmp(path, "/") == 0)
        read_inode(0, current_root->ino);

    return 0;
}

//optional
//int p6fs_opendir(const char *path, struct fuse_file_info *fileInfo)
//int p6fs_releasedir(const char *path, struct fuse_file_info *fileInfo)
//int p6fs_fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo)


//file operations
int p6fs_mknod(const char *path, mode_t mode, dev_t dev)
{
 /*do path parse here
  create file*/
    DEBUG("mknod: (%s, %x, %u)", path, mode, dev);
    char parent[MAX_PATH_LENGTH], name[MAX_NAME_LENGTH];
    separate_path(path, parent, name);
    uint32_t parent_inum, free_inum;
    parent_inum = parse_path(parent);
    if(parent_inum == -ENOENT) {
        ERR("error occured when mkdir.");
        return -ENOENT;
    }
    struct inode_t parent_ino, new_ino;
    read_inode(parent_inum, &parent_ino);
    free_inum = find_free_inode();

    if(free_inum == -ENOSPC) {
        ERR("error occured when mkdir.");
        return -ENOSPC;
    }
    DEBUG("mknod: inum=%u", free_inum);
    write_inode_bit(free_inum, 1);

    new_ino.i_mode = S_IFREG | mode;
    new_ino.i_links_count = 1;
    new_ino.i_atime = new_ino.i_ctime = new_ino.i_mtime = timestamp();
    new_ino.i_size = 0;
    new_ino.i_fnum = 0;
    memset(new_ino.i_block, 0, sizeof(new_ino.i_block));
    memset(new_ino.i_padding, 0, sizeof(new_ino.i_padding));
    new_ino.i_num = free_inum;
    write_inode(free_inum, &new_ino);

    struct dentry parent_den;
    parent_den.d_inum = free_inum;
    strcpy(parent_den.d_name, name);
    write_dentry(&parent_ino, parent_ino.i_fnum + 2, &parent_den);
    parent_ino.i_mtime = parent_ino.i_ctime = timestamp();
    write_inode(parent_inum, &parent_ino);

    if(strcmp(parent, "/") == 0)
        read_inode(0, current_root->ino);

    return 0;
}

int p6fs_readlink(const char *path, char *link, size_t size) {
    DEBUG("readlink: %s", path);
    uint32_t inum;
    inum = parse_path(path);
    if(inum == -ENOENT) {
        ERR("error occured when readlink.");
        return -ENOENT;
    }

    struct inode_t ino;
    read_inode(inum, &ino);

    uint8_t buf[SECTOR_SIZE];
    read_block(ino.i_block[0], buf);
    memcpy(link, buf, (size > SECTOR_SIZE ? SECTOR_SIZE : size));

    ino.i_atime = timestamp();
    write_inode(inum, &ino);

    return 0;
}

int p6fs_symlink(const char *path, const char *link)
{
    DEBUG("symlink: (%s, %s)", path, link);
    if(strlen(link) > SECTOR_SIZE - 1) {
        ERR("error occured when symlink.");
        return -ENOBUFS;
    }
    char parent[MAX_PATH_LENGTH], name[MAX_NAME_LENGTH];
    separate_path(link, parent, name);
    uint32_t parent_inum, free_inum, free_bnum;
    parent_inum = parse_path(parent);
    if(parent_inum == -ENOENT) {
        ERR("error occured when symlink.");
        return -ENOENT;
    }
    struct inode_t parent_ino, new_ino;
    read_inode(parent_inum, &parent_ino);
    free_inum = find_free_inode();
    free_bnum = find_free_block();

    if(free_inum == -ENOSPC || free_bnum == -ENOSPC) {
        ERR("error occured when symlink.");
        return -ENOSPC;
    }
    DEBUG("symlink: inum=%u, bnum=%u", free_inum, free_bnum);
    write_inode_bit(free_inum, 1);
    write_block_bit(free_bnum, 1);

    new_ino.i_mode = S_IFLNK | 0777;
    new_ino.i_links_count = 1;
    new_ino.i_atime = new_ino.i_ctime = new_ino.i_mtime = timestamp();
    new_ino.i_size = SECTOR_SIZE;
    new_ino.i_fnum = 0;
    memset(new_ino.i_block, 0, sizeof(new_ino.i_block));
    new_ino.i_block[0] = free_bnum;
    memset(new_ino.i_padding, 0, sizeof(new_ino.i_padding));
    new_ino.i_num = free_inum;
    write_inode(free_inum, &new_ino);

    uint8_t buf[SECTOR_SIZE];
    memset(buf, 0, sizeof(buf));
    strcpy(buf, path);
    write_block(free_bnum, &buf);

    struct dentry parent_den;
    parent_den.d_inum = free_inum;
    strcpy(parent_den.d_name, name);
    write_dentry(&parent_ino, parent_ino.i_fnum + 2, &parent_den);
    parent_ino.i_mtime = parent_ino.i_ctime = timestamp();
    write_inode(parent_inum, &parent_ino);

    if(strcmp(parent, "/") == 0)
        read_inode(0, current_root->ino);

    return 0;
}

int p6fs_link(const char *path, const char *newpath)
{
    DEBUG("link: (%s, %s)", path, newpath);

    uint32_t old_inum;
    old_inum = parse_path(path);
    if(old_inum == -ENOENT) {
        ERR("error occured when link.");
        return -ENOENT;
    }
    DEBUG("link: inum=%u", old_inum);

    char parent[MAX_PATH_LENGTH], name[MAX_NAME_LENGTH];
    separate_path(newpath, parent, name);
    uint32_t parent_inum;
    parent_inum = parse_path(parent);
    if(parent_inum == -ENOENT) {
        ERR("error occured when link.");
        return -ENOENT;
    }
    struct inode_t parent_ino, old_ino;
    read_inode(parent_inum, &parent_ino);
    read_inode(old_inum, &old_ino);

    old_ino.i_links_count++;
    old_ino.i_ctime = timestamp();
    write_inode(old_inum, &old_ino);

    struct dentry parent_den;
    parent_den.d_inum = old_inum;
    strcpy(parent_den.d_name, name);
    write_dentry(&parent_ino, parent_ino.i_fnum + 2, &parent_den);
    parent_ino.i_mtime = parent_ino.i_ctime = timestamp();
    write_inode(parent_inum, &parent_ino);

    if(strcmp(parent, "/") == 0)
        read_inode(0, current_root->ino);

    return 0;
}

int p6fs_unlink(const char *path)
{
    DEBUG("unlink: %s", path);

    uint32_t inum;
    inum = parse_path(path);
    if(inum == -ENOENT) {
        ERR("error occured when unlink.");
        return -ENOENT;
    }
    DEBUG("unlink: inum=%u", inum);

    char parent[MAX_PATH_LENGTH], name[MAX_NAME_LENGTH];
    separate_path(path, parent, name);
    uint32_t parent_inum;
    parent_inum = parse_path(parent);

    struct inode_t parent_ino, ino;
    read_inode(parent_inum, &parent_ino);
    read_inode(inum, &ino);

    ino.i_links_count--;
    ino.i_ctime = timestamp();
    if(ino.i_links_count == 0) {
        write_inode_bit(inum, 0);
        release_block(&ino);
    } else {
        write_inode(inum, &ino);
    }

    uint32_t dnum;
    dnum = find_dentry(&parent_ino, name);
    remove_dentry(&parent_ino, dnum);
    parent_ino.i_mtime = parent_ino.i_ctime = timestamp();
    write_inode(parent_inum, &parent_ino);

    if(strcmp(parent, "/") == 0)
        read_inode(0, current_root->ino);

    return 0;
}

int p6fs_open(const char *path, struct fuse_file_info *fileInfo)
{
 /*
  Implemention Example:
  S1: look up and get dentry of the path
  S2: create file handle! Do NOT lookup in read() or write() later
  */
    DEBUG("open: %s", path);
    uint32_t inum, err;
    inum = parse_path(path);
    if(inum == -ENOENT) {
        if(fileInfo->flags & O_CREAT) {
            err = p6fs_mknod(path, 0755, 0);
            if(err != 0) {
                ERR("error occured when open.");
                return err;
            }
        } else {
            ERR("error occured when open.");
            return -ENOENT;
        }
    } else if(fileInfo->flags & O_EXCL) {
        ERR("error occured when open.")
        return -EEXIST;
    } else if(fileInfo->flags & O_TRUNC) {
        err = p6fs_truncate(path, 0);
        if(err != 0) {
            ERR("error occured when open.");
            return err;
        }
    }

    //assign and init your file handle
    struct file_info *fi = NULL;
    uint32_t i;
    for(i=0;i<MAX_OPEN_FILE;i++) {
        if(file_handle[i].fi_allocated == 0) {
            fi = &(file_handle[i]);
            break;
        }
    }
    if(fi == NULL) {
        ERR("error occured when open.");
        return -EMFILE;
    }

    struct inode_t ino;
    read_inode(inum, &ino);

    // DEBUG("%u", ino.i_mode & S_IFLNK);
    /* if(ino.i_mode & 0xf000 == S_IFLNK) {
        uint8_t buf[SECTOR_SIZE];
        read_block(ino.i_block[0], buf);
        if(strlen(buf) == 0)
            return -ENOENT;
        if(buf[0] != '/') {
            char parent[MAX_PATH_LENGTH], name[MAX_NAME_LENGTH];
            separate_path(path, parent, name);
            strcat(parent, buf);
            strcpy(buf, parent);
        }
        DEBUG("open symlink: %s", buf);
        return p6fs_open(buf, fileInfo);
    } */

    fi->fi_allocated = 1;
    memcpy((void*)&(fi->fi_ino), (void*)&ino, INODE_SIZE);
    // fi->fi_allocated_blk = ino.i_size > 0 ? (ino.i_size - 1) / SECTOR_SIZE + 1 : 0;
    /* if(fileInfo->flags & O_APPEND) {
        fi->fi_file_ptr = ino.i_size;
    } else {
        fi->fi_file_ptr = 0;
    } */
    fi->fi_mode = fileInfo->flags & (O_RDONLY | O_WRONLY | O_RDWR);

    //check open flags, such as O_RDONLY
    //O_CREATE is tansformed to mknod() + open() by fuse ,so no need to create file here
    /*
     if(fileInfo->flags & O_RDONLY){
     fi->xxxx = xxxx;
     }
     */

    fileInfo->fh = (uint64_t)fi;
    DEBUG("file handle: %llu", fileInfo->fh);
    DEBUG("mode: %x, size: %llu", (fi->fi_ino).i_mode, (fi->fi_ino).i_size);

    ino.i_atime = ino.i_mtime = ino.i_ctime = timestamp();
    write_inode(inum, &ino);

    return 0;

}

int p6fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo)
{
    /* get inode from file handle and do operation*/
    DEBUG("read: (%s, %llu, %u)", path, offset, size);
    struct file_info *fi;
    fi = (struct file_info*)fileInfo->fh;
    DEBUG("file handle: %llu", fileInfo->fh);
    if(fi->fi_mode & O_WRONLY) return 0;

    pthread_mutex_lock(&read_lock);
    uint32_t i, begin_blk, end_blk, allocated_blk, off, idx;
    begin_blk = offset / SECTOR_SIZE;
    end_blk = (offset + size - 1) / SECTOR_SIZE;
    allocated_blk = (fi->fi_ino).i_size > 0 ? ((fi->fi_ino).i_size - 1) / SECTOR_SIZE + 1 : 0;
    off = offset % SECTOR_SIZE;
    if(end_blk > allocated_blk - 1) end_blk = allocated_blk - 1;

    uint8_t buffer[SECTOR_SIZE];
    idx = read_block_index(&(fi->fi_ino), begin_blk);
    read_block(idx, buffer);
    memcpy((void*)buf, (void*)(buffer + off), size < SECTOR_SIZE - off ? size : SECTOR_SIZE - off);
    DEBUG("(%u, %u)", begin_blk, idx);
    for(i=begin_blk+1;i<=end_blk;i++) {
        idx = read_block_index(&(fi->fi_ino), i);
        read_block(idx, buffer);
        memcpy((void*)(buf - off + (i - begin_blk) * SECTOR_SIZE), (void*)buffer,
            (i < end_blk) || ((size + off) % SECTOR_SIZE == 0) ? SECTOR_SIZE : (size + off) % SECTOR_SIZE);
        DEBUG("(%u, %u)", i, idx);
    }
    pthread_mutex_unlock(&read_lock);
    return size;
}

int p6fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo)
{
    /* get inode from file handle and do operation*/
    DEBUG("write: (%s, %llu, %u)", path, offset, size);
    struct file_info *fi;
    fi = (struct file_info*)fileInfo->fh;
    DEBUG("file handle: %llu", fileInfo->fh);
    if(fi->fi_mode & O_RDONLY) return 0;
    DEBUG("mode: %x, oldsize: %llu, newsize: %llu", (fi->fi_ino).i_mode, (fi->fi_ino).i_size, offset + size);
    if(offset + size > (fi->fi_ino).i_size) {
        if(p6fs_truncate(path, offset + size) == -ENOSPC) {
            ERR("error occured when write.");
            return -ENOSPC;
        }
        read_inode((fi->fi_ino).i_num, &(fi->fi_ino));
    }

    pthread_mutex_lock(&write_lock);
    uint32_t i, begin_blk, end_blk, allocated_blk, off, idx;
    begin_blk = offset / SECTOR_SIZE;
    end_blk = (offset + size - 1) / SECTOR_SIZE;
    allocated_blk = (fi->fi_ino).i_size > 0 ? ((fi->fi_ino).i_size - 1) / SECTOR_SIZE + 1 : 0;
    off = offset % SECTOR_SIZE;
    DEBUG("(%u, %u, %u, %u)", begin_blk, end_blk, allocated_blk, off);
    if(end_blk > allocated_blk - 1) end_blk = allocated_blk - 1;

    uint8_t buffer[SECTOR_SIZE];
    idx = read_block_index(&(fi->fi_ino), begin_blk);
    read_block(idx, buffer);
    memcpy((void*)(buffer + off), (void*)buf, size < SECTOR_SIZE - off ? size : SECTOR_SIZE - off);
    write_block(idx, buffer);
    DEBUG("(%u, %u)", begin_blk, idx);

    for(i=begin_blk+1;i<=end_blk;i++) {
        idx = read_block_index(&(fi->fi_ino), i);
        if(i==end_blk) {
            read_block(idx, buffer);
        }
        memcpy((void*)buffer, (void*)(buf - off + (i - begin_blk) * SECTOR_SIZE),
            (i < end_blk) || ((size + off) % SECTOR_SIZE == 0) ? SECTOR_SIZE : (size + off) % SECTOR_SIZE);
        write_block(idx, buffer);
        DEBUG("(%u, %u)", i, idx);
    }
    pthread_mutex_unlock(&write_lock);
    return size;
}

int p6fs_truncate(const char *path, off_t newSize)
{
    DEBUG("truncate: (%s, %llu)", path, newSize);

    uint32_t inum;
    inum = parse_path(path);
    if(inum == -ENOENT) {
        ERR("error occured when unlink.");
        return -ENOENT;
    }

    struct inode_t ino;
    read_inode(inum, &ino);

    uint32_t old_blk, new_blk, i, free_bnum;
    old_blk = ino.i_size > 0 ? (ino.i_size - 1) / SECTOR_SIZE + 1 : 0;
    new_blk = newSize > 0 ? (newSize - 1) / SECTOR_SIZE + 1 : 0;
    uint8_t buf[SECTOR_SIZE];
    memset(buf, 0, sizeof(buf));
    if(old_blk < new_blk) {
        for(i=old_blk;i<new_blk;i++) { // TODO: deal with indirect block mapping
            free_bnum = find_free_block();
            if(free_bnum == -ENOSPC) {
                for(i=i-1;i>=old_blk && i!=-1;i--) {
                    write_block_bit(read_block_index(&ino, i), 0);
                    write_block_index(&ino, i, 0);
                }
                ERR("error occured when truncate.");
                return -ENOSPC;
            }
            write_block(free_bnum, buf);
            write_block_bit(free_bnum, 1);
            write_block_index(&ino, i, free_bnum);
        }
    } else if(old_blk > new_blk) {
        for(i=old_blk-1;i>=new_blk && i!=-1;i--) {
            write_block_bit(read_block_index(&ino, i), 0);
            write_block_index(&ino, i, 0);
        }
    }

    ino.i_size = newSize;
    ino.i_mtime = ino.i_ctime = timestamp();
    write_inode(inum, &ino);

    return 0;
}

//optional
//p6fs_flush(const char *path, struct fuse_file_info *fileInfo)
//int p6fs_fsync(const char *path, int datasync, struct fuse_file_info *fi)
int p6fs_release(const char *path, struct fuse_file_info *fileInfo)
{
    /*release fd*/
    DEBUG("release");
    struct file_info *fi;
    fi = (struct file_info*)fileInfo->fh;
    fi->fi_allocated = 0;
    return 0;
}

int p6fs_getattr(const char *path, struct stat *statbuf)
{
    /*stat() file or directory */
    DEBUG("getattr: %s", path);
    uint32_t inum;
    inum = parse_path(path);
    if(inum == -ENOENT) return -ENOENT;
    struct inode_t ino;
    read_inode(inum, &ino);
    statbuf->st_ino = inum;
    statbuf->st_size = ino.i_size;
    statbuf->st_dev = 0;
    statbuf->st_rdev = 0;
    statbuf->st_uid = 0;
    statbuf->st_gid = 0;
    statbuf->st_mtime = ino.i_mtime;
    statbuf->st_atime = ino.i_atime;
    statbuf->st_ctime = ino.i_ctime;
    statbuf->st_mode = ino.i_mode;
    statbuf->st_nlink = ino.i_links_count;
    // statbuf->st_blocksize = SECTOR_SIZE;
    // statbuf->st_nblocks = ino.i_size / SECTOR_SIZE;
    statbuf->st_blksize = SECTOR_SIZE;
    statbuf->st_blocks = 0; // TODO: add block number
    return 0;
}

int p6fs_utime(const char *path, struct utimbuf *ubuf) {
    DEBUG("utime: %s", path);
    uint32_t inum;
    inum = parse_path(path);
    if(inum == -ENOENT) return -ENOENT;
    struct inode_t ino;
    read_inode(inum, &ino);
    ino.i_atime = ubuf->actime;
    ino.i_mtime = ubuf->modtime;
    ino.i_ctime = timestamp();
    write_inode(inum, &ino);
    return 0;
}

int p6fs_chmod(const char *path, mode_t mode) {
    DEBUG("chmod: (%s, %u)", path, mode);
    uint32_t inum;
    inum = parse_path(path);
    if(inum == -ENOENT) return -ENOENT;
    struct inode_t ino;
    read_inode(inum, &ino);
    ino.i_mode = (ino.i_mode & 0177000) | (mode & 0177000);
    ino.i_ctime = timestamp();
    write_inode(inum, &ino);
    return 0;
}

/*
int p6fs_chown(const char *path, uid_t uid, gid_t gid);//optional
*/

int p6fs_rename(const char *path, const char *newpath)
{
    DEBUG("rename: (%s, %s)", path, newpath);
    if(p6fs_link(path, newpath) == -ENOENT) {
        ERR("error occured when rename.");
        return -ENOENT;
    }

    return p6fs_unlink(path);
}

int p6fs_statfs(const char *path, struct statvfs *statInfo)
{
    /*print fs status and statistics */
    DEBUG("statfs");
    statInfo->f_bsize = (unsigned long) current_sb->sb->s_size;
    statInfo->f_frsize = SECTOR_SIZE;
    statInfo->f_blocks = current_sb->sb->s_total_blocks_count;
    statInfo->f_bfree = current_sb->sb->s_free_blocks_count;
    statInfo->f_bavail = current_sb->sb->s_free_blocks_count;
    statInfo->f_files = current_sb->sb->s_total_inodes_count;
    statInfo->f_ffree = current_sb->sb->s_free_inodes_count;
    statInfo->f_favail = current_sb->sb->s_free_inodes_count;
    statInfo->f_fsid = MAGIC;
    statInfo->f_flag = 0;
    statInfo->f_namemax = MAX_NAME_LENGTH;
    return 0;
}

void* p6fs_init(struct fuse_conn_info *conn)
{
    /*init fs
     think what mkfs() and mount() should do.
     create or rebuild memory structures.

     e.g
     S1 Read the magic number from disk
     S2 Compare with YOUR Magic
     S3 if(exist)
        then
            mount();
        else
            mkfs();
     */
    DEBUG("init");
    uint64_t size;
    size = device_size();
    if(size < MIN_DISK_SIZE) {
        DEBUG("error occurred when reading disk size.");
        return NULL;
    }

    uint8_t double_superblock = (size >= MIN_DISK_SIZE_FOR_DOUBLE_SUPERBLOCK);

    uint8_t buf[SECTOR_SIZE];

    device_read_sector(buf, FIRST_SUPERBLOCK_SECTOR);

    struct superblock_t* sb = buf;
    if(sb->s_magic != MAGIC || sb->s_checknum != checknum((uint32_t*)sb, sizeof(struct superblock_t) - 4)) {
        if(double_superblock) {
            device_read_sector(buf, SECOND_SUPERBLOCK_SECTOR);
            if(sb->s_magic == MAGIC && sb->s_checknum == checknum((uint32_t*)sb, sizeof(struct superblock_t) - 4)) {
                INFO("it seems that filesystem is broken. restoring...");
                device_write_sector(buf, FIRST_SUPERBLOCK_SECTOR);
                device_flush();
                p6fs_mount();
            } else {
                INFO("no valid filesystem. creating...");
                p6fs_mkfs();
            }
        } else {
            INFO("no valid filesystem. creating...");
            p6fs_mkfs();
        }
    } else {
        p6fs_mount();
    }

    /*HOWTO use @return
     struct fuse_context *fuse_con = fuse_get_context();
     fuse_con->private_data = (void *)xxx;
     return fuse_con->private_data;

     the fuse_context is a global variable, you can use it in
     all file operation, and you could also get uid,gid and pid
     from it.

     */
    return NULL;
}
void p6fs_destroy(void* private_data)
{
    /*
     flush data to disk
     free memory
     */
    DEBUG("destroy");
    if(inode_lock) free(inode_lock);
    if(file_handle) free(file_handle);
    uint32_t i;
    if(current_root && current_root->ino) free(current_root->ino);
    if(current_root) free(current_root);
    if(current_sb && current_sb->sb && current_bg) {
        for(i=0;i<current_sb->sb->s_groups_count;i++) {
            if(current_bg[i].bg) free(current_bg[i].bg);
        }
    }
    if(current_bg) free(current_bg);
    if(current_sb && current_sb->sb) free(current_sb->sb);
    if(current_sb) free(current_sb);
    if(inode_bitmap) free(inode_bitmap);
    if(block_bitmap) free(block_bitmap);
    device_close();
    logging_close();
}
