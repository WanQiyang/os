#include <execinfo.h>
#include <stddef.h>

#include "common.h"



static struct fuse_operations p6fs_ops = {
    /*  uncomment and fillup the interfaces you implemented in "common.h" */

     //task1 interfaces
     .mkdir = p6fs_mkdir,
     .rmdir = p6fs_rmdir,
     .readdir = p6fs_readdir,

     .mknod = p6fs_mknod,
     .symlink = p6fs_symlink,
     .link = p6fs_link,
     .unlink = p6fs_unlink,
     .open = p6fs_open,
     .release = p6fs_release,
     .read = p6fs_read,
     .write = p6fs_write,
     .truncate = p6fs_truncate,
     .getattr = p6fs_getattr,

     .rename = p6fs_rename,
     .statfs = p6fs_statfs,
     .init = p6fs_init,

     //task2 bonus interfaces
     .utime = p6fs_utime,
     .readlink = p6fs_readlink,
     .chmod = p6fs_chmod,
     .destroy = p6fs_destroy,

     /*//optional interfaces
     .opendir = ,
     .releasedir = ,
     .fsyncdir = ,
     .flush = ,
     .fsync = ,
     .chown = ,
     */
};

static struct p6fs_device {
    char *disk;
    char *logfile;
} p6fs_device;

static struct fuse_opt p6fs_opts[] = {
    { "logfile=%s", offsetof(struct p6fs_device, logfile), 0 },
    FUSE_OPT_END
};

static int p6fs_opt_proc(void *data, const char *arg, int key,
                        struct fuse_args *outargs)
{
    (void) data;
    (void) outargs;

    switch (key) {
    case FUSE_OPT_KEY_OPT:
        return 1;
    case FUSE_OPT_KEY_NONOPT:
        if (!p6fs_device.disk) {
            p6fs_device.disk = strdup(arg);
            return 0;
        }
        return 1;
    default:
        fprintf(stderr, "internal error\n");
        abort();
    }
}

int main(int argc, char *argv[])
{
    /*fixed parameter order p6fs <disk_path> <mount_point>
     optional: -o logfile=<> */
    int res;
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // Default options
    p6fs_device.disk = NULL;
    p6fs_device.logfile = "./p6fs.log";

    if (fuse_opt_parse(&args, &p6fs_device, p6fs_opts, p6fs_opt_proc) == -1) {
        return EXIT_FAILURE;
    }

    if (!p6fs_device.disk) {
        fprintf(stderr, "Usage: %s <disk> <mountpoint>\n", argv[0]);
        exit(1);
    }

    if (logging_open(p6fs_device.logfile) < 0) {
        fprintf(stderr, "Failed to initialize logging\n");
        fprintf(stderr, "overwrite the DEFAULT_LOG_FILE or add -o logfile=YOUR_LOG_FILE in cmd line\n");
        return EXIT_FAILURE;
    }

    if (device_open(p6fs_device.disk) < 0) {
        fprintf(stderr, "disk_open: %s: %s\n", p6fs_device.disk,
                strerror(errno));
        return EXIT_FAILURE;
    }

    /*
     You should do mount operation in p6fs_ops->init()
     */

    res = fuse_main(args.argc, args.argv, &p6fs_ops, NULL);

    fuse_opt_free_args(&args);
    free(p6fs_device.disk);

    return res;
}
