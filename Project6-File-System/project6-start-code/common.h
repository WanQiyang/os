#ifndef P6_COMMON
#define P6_COMMON

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Maybe you need the pthread locks or speedup by multi-threads or background GC in task2
// check if your $(CC) add -lfuse -pthread to $(CFLAGS) automatically, if not add them manually.
#include <pthread.h>

#include "disk.h"
#include "logging.h"

#define MAGIC 0x2e575159 // .WQY
#define GROUP_SIZE 0x40000000 // 1GB
#define SECTOR_PER_GROUP (GROUP_SIZE / SECTOR_SIZE) // 262144

#define INODE_SIZE 0x80 // sizeof(inode_t)
#define INODE_PER_SECTOR (SECTOR_SIZE / INODE_SIZE)

#define INODE_BITMAP_SECTOR_PER_GROUP 2
#define BLOCK_BITMAP_SECTOR_PER_GROUP 8

#define INODE_SECTOR_PER_GROUP (INODE_SIZE * INODE_PER_GROUP / SECTOR_SIZE)
#define CONTROL_SECTOR_PER_GROUP (2 + INODE_BITMAP_SECTOR_PER_GROUP + BLOCK_BITMAP_SECTOR_PER_GROUP + INODE_SECTOR_PER_GROUP)
#define BLOCK_SECTOR_PER_GROUP (SECTOR_PER_GROUP - CONTROL_SECTOR_PER_GROUP)

#define INODE_PER_GROUP 0x10000 // 65536
#define BLOCK_PER_GROUP SECTOR_PER_GROUP // 262144

#define MIN_DISK_SIZE (CONTROL_SECTOR_PER_GROUP * SECTOR_SIZE)
#define MAX_DISK_SIZE

#define MIN_DISK_SIZE_FOR_DOUBLE_SUPERBLOCK (GROUP_SIZE + MIN_DISK_SIZE)
#define FIRST_SUPERBLOCK_SECTOR 0
#define SECOND_SUPERBLOCK_SECTOR SECTOR_PER_GROUP

#define INODE_BITMAP_ENTRY_SECTOR 2
#define BLOCK_BITMAP_ENTRY_SECTOR (INODE_BITMAP_ENTRY_SECTOR + INODE_BITMAP_SECTOR_PER_GROUP)
#define INODE_ENTRY_SECTOR (BLOCK_BITMAP_ENTRY_SECTOR + BLOCK_BITMAP_SECTOR_PER_GROUP)
#define DATA_ENTRY_SECTOR (INODE_ENTRY_SECTOR + INODE_SECTOR_PER_GROUP)

/*   i_mode   */
/* wrong
#define S_IXOTH 0x1
#define S_IWOTH 0x2
#define S_IROTH 0x4
#define S_IXGRP 0x8
#define S_IWGRP 0x10
#define S_IRGRP 0x20
#define S_IXUSR 0x40
#define S_IWUSR 0x80
#define S_IRUSR 0x100
#define S_ISVTX 0x200
#define S_ISGID 0x400
#define S_ISUID 0x800

#define S_IFIFO 0x1000
#define S_IFCHR 0x2000
#define S_IFDIR 0x4000
#define S_IFBLK 0x6000
#define S_IFREG 0x8000
#define S_IFLNK 0xA000
#define S_IFSOCK 0xC000
*/
#define DENTRY_SIZE 0x100
#define DENTRY_PER_BLOCK (SECTOR_SIZE / DENTRY_SIZE)
#define MAX_NAME_LENGTH 252
#define MAX_PATH_LENGTH 1024

#define MAX_OPEN_FILE 1024

#define POINTER_PER_BLOCK (SECTOR_SIZE / 4)
#define FIRST_POINTER 12
#define SECOND_POINTER (FIRST_POINTER + POINTER_PER_BLOCK)
#define THIRD_POINTER (SECOND_POINTER + POINTER_PER_BLOCK * POINTER_PER_BLOCK)
#define MAX_INDEX (THIRD_POINTER + POINTER_PER_BLOCK * POINTER_PER_BLOCK * POINTER_PER_BLOCK)

/*   on-disk  data structure   */
struct superblock_t{
    // complete it
    uint64_t s_size;
    uint32_t s_magic;
    uint32_t s_groups_count;
    uint32_t s_total_inodes_count;
    uint32_t s_total_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_free_blocks_count;
    uint32_t s_checknum;
};

struct blockgroup_t {
    uint32_t bg_total_inodes_count;
    uint32_t bg_total_blocks_count;
    uint32_t bg_free_inodes_count;
    uint32_t bg_free_blocks_count;
    uint32_t bg_checknum;
};

struct inode_t{
    // complete it
    uint16_t i_mode;
    uint16_t i_links_count;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint64_t i_size;
    uint32_t i_fnum;
    uint32_t i_block[15];
    uint32_t i_num;
    uint8_t  i_padding[40];
};

struct dentry{
    // complete it
    uint32_t d_inum;
    char     d_name[MAX_NAME_LENGTH];
};

/*  in-memory data structure   */

struct superblock{
    struct superblock_t *sb;
    pthread_mutex_t lock;
    // Add what you need, Like locks
};

struct blockgroup {
    struct blockgroup_t *bg;
    pthread_mutex_t lock;
};

struct inode{
    struct inode_t *ino;
    // Add what you need, Like locks
};

/*Your file handle structure, should be kept in <fuse_file_info>->fh
 (uint64_t see fuse_common.h), and <fuse_file_info> used in all file operations  */
struct file_info{
    // complete it
    uint8_t fi_allocated;
    struct inode_t fi_ino;
    // uint32_t fi_allocated_blk;
    uint32_t fi_mode;
};


//Interf.  See "fuse.h" <struct fuse_operations>
//You need to implement all the interfaces except optional ones

//dir operations
int p6fs_mkdir(const char *path, mode_t mode);
int p6fs_rmdir(const char *path);
int p6fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
int p6fs_opendir(const char *path, struct fuse_file_info *fileInfo);//optional
int p6fs_releasedir(const char *path, struct fuse_file_info *fileInfo);//optional
int p6fs_fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo);//optional


//file operations
int p6fs_mknod(const char *path, mode_t mode, dev_t dev);
int p6fs_symlink(const char *path, const char *link);
int p6fs_link(const char *path, const char *newpath);
int p6fs_unlink(const char *path);
int p6fs_readlink(const char *path, char *link, size_t size);//optional

int p6fs_open(const char *path, struct fuse_file_info *fileInfo);
int p6fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
int p6fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
int p6fs_truncate(const char *path, off_t newSize);
int p6fs_flush(const char *path, struct fuse_file_info *fileInfo);//optional
int p6fs_fsync(const char *path, int datasync, struct fuse_file_info *fi);//optional
int p6fs_release(const char *path, struct fuse_file_info *fileInfo);


int p6fs_getattr(const char *path, struct stat *statbuf);
int p6fs_utime(const char *path, struct utimbuf *ubuf);//optional
int p6fs_chmod(const char *path, mode_t mode); //optional
int p6fs_chown(const char *path, uid_t uid, gid_t gid);//optional

int p6fs_rename(const char *path, const char *newpath);
int p6fs_statfs(const char *path, struct statvfs *statInfo);
void* p6fs_init(struct fuse_conn_info *conn);
void p6fs_destroy(void* private_data);//optional

#endif
