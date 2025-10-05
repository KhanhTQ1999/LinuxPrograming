#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include "../CmnBase/log.h"

// struct stat {
//     dev_t     st_dev;     /* ID of device containing file */
//     ino_t     st_ino;     /* inode number */
//     mode_t    st_mode;    /* protection */
//     nlink_t   st_nlink;   /* number of hard links */
//     uid_t     st_uid;     /* user ID of owner */
//     gid_t     st_gid;     /* group ID of owner */
//     dev_t     st_rdev;    /* device ID (if special file) */
//     off_t     st_size;    /* total size, in bytes */
//     blksize_t st_blksize; /* blocksize for file system I/O */
//     blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
//     time_t    st_atime;   /* time of last access */
//     time_t    st_mtime;   /* time of last modification */
//     time_t    st_ctime;   /* time of last status change */
// };

const char* get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return "Regular File";
    if (S_ISDIR(mode)) return "Directory";
    if (S_ISCHR(mode)) return "Character Device";
    if (S_ISBLK(mode)) return "Block Device";
    if (S_ISFIFO(mode)) return "FIFO";
    if (S_ISLNK(mode)) return "Symbolic Link";
    if (S_ISSOCK(mode)) return "Socket";
    return "Unknown";
}

int main(int argc, char* argv[]){
    if(argc < 2){
        log_info("Usage: %s <file_path>\n", argv[0]);
        return -1;
    }

    struct stat fs;
    if(lstat(argv[1], &fs) == -1){
        log_error("Failed to retrieve file info for: %s\n", argv[1]);
        return -1;
    }

    log_info("\n"
        "File:               %s\n"
        "File Type:          %s\n"
        "Size:               %lld Byte\n"
        "Last Modified:      %s",
        argv[1],
        get_file_type(fs.st_mode),
        (long long) fs.st_size,
        ctime(&fs.st_mtime));

    return 0;
}
