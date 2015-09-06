#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include "fbfs.h"

int main (int argc, char *argv[])
{

    FS_META *fm;
    FS_META *fm_read;
    int fd = -1;
    int len = 4096;
    char *addr = NULL;
    off_t off = 0;

    char *fs_fn = NULL;
    int opt = 0;

    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch(opt) {
        case 'f':
            fs_fn = optarg;
            break;
        default:
	    printf("\tUsage: cmd -f <file name for filesystem> \n");
            printf("\tInvalid options/args...\n");
  	    break;
 	}
    }

    if(!fs_fn) {
        fprintf(stderr, "\tNeed to specify -f <filename for filesystem> ...\n");
        exit(1);
    }

    if(access(fs_fn, F_OK) != -1) {
        // file exists
        fd = open(fs_fn, O_RDWR| O_EXCL, S_IRUSR | S_IWUSR);
    } else {
        fd = open(fs_fn, O_CREAT| O_RDWR| O_EXCL, S_IRUSR | S_IWUSR);
       // file doesn't exist
    }

    if (fd < 0) {
        fprintf(stdout, "Failed to open file %s",fs_fn);
        exit(1);
    }

    if (ftruncate(fd, len/4) == -1) {
        printf("error in truncate");
       exit(0);
    }

    fm = (FS_META *)malloc(sizeof(FS_META));

    strcpy(fm->name, "foo_fs");
    fm->meta_type= 0; /* meta data type: 0 - root dir */
    strcpy(fm->rootdir, "/");
    fm->fs_size = 512 * 1024;
    fm->block_size = 512;
    
    addr = mmap(NULL, len/4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, off);
    if (addr == NULL) {
        printf("Error in mmap");
        exit(0);
    }

    fprintf(stdout, "Writing to %s....\n", fs_fn);
    memcpy(addr, fm, sizeof(FS_META));

    close(fd);


    fprintf(stdout, "Opening %s to read data ....\n", fs_fn);
    fd = open(fs_fn, O_RDWR| O_EXCL, S_IRUSR | S_IWUSR);

    if (fd < 0) {
        fprintf(stdout, "Failed to open file %s", fs_fn);
        exit(1);
    }

    addr = mmap(NULL, len/4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, off);
    if (addr == NULL) {
        printf("Error in mmap");
        exit(0);
    }

    fm_read = (FS_META *)malloc(sizeof(FS_META));
    memcpy(fm_read, addr, sizeof(FS_META));

    fprintf(stdout, "name: %s\n", fm_read->name);
    fprintf(stdout, "meta_type: %d\n", fm_read->meta_type);
    fprintf(stdout, "rootdir: %s\n", fm_read->rootdir);
    fprintf(stdout, "fs_size: %d\n", fm_read->fs_size);
    fprintf(stdout, "block_size: %d\n", fm_read->block_size);

    return 0;
} 
