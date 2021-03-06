#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include "fbfs.h"


int
add_block (int fd, void *start, size_t *len, off_t at, void *block, size_t size)
{
    if (at + size > *len)
    {
        /* Resize the file and remap.  */
        size_t ps = sysconf (_SC_PAGESIZE);
        size_t ns = (at + size + ps - 1) & ~(ps - 1);
        void *np;
        if (ftruncate (fd, ns) < 0)
            return -1;
        np = mmap (NULL, ns, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if (np == MAP_FAILED)
            return -1;
        start = np;
        *len = ns;
    }
    memcpy ((char *) start + at, block, size);
    return 0;
}


int 
main (int argc, char *argv[])
{

    FS_META *fm;
    FS_META *fm_read;
    int fd = -1;
    size_t mmap_len = sysconf(_SC_PAGE_SIZE);
    char *addr = NULL;
    off_t off = 0;
    int block_size = 512; //this would be set in stanza file. 
    int fs_size = 1024 * 1024;

    char *fs_fn = NULL;
    int opt = 0;

    fprintf(stdout, "\nsystem page size: %ld \n\n", sysconf(_SC_PAGE_SIZE));
    while ((opt = getopt(argc, argv, "f:b:")) != -1) {
        switch(opt) {
        case 'f':
            fs_fn = optarg;
            break;
        case 'b':
            block_size = atoi(optarg);
            break;
        default:
	    printf("\tUsage: cmd -f <file name for filesystem> [-b <block_size>]\n");
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
        fprintf(stdout, "Opening existing file...\n");
        fd = open(fs_fn, O_RDWR| O_EXCL, S_IRUSR | S_IWUSR);
    } else {
        fprintf(stdout, "Opening new file...\n");
        fd = open(fs_fn, O_CREAT| O_RDWR| O_EXCL, S_IRUSR | S_IWUSR);
       // file doesn't exist
    }

    if (fd < 0) {
        fprintf(stdout, "Failed to open file %s",fs_fn);
        exit(1);
    }

    if (ftruncate(fd, mmap_len) == -1) {
        printf("error in truncate");
        exit(0);
    }

    /* 1st write */
    fm = (FS_META *)malloc(sizeof(FS_META));

    strcpy(fm->name, "foo_fs");
    fm->meta_type= 0; /* meta data type: 0 - root dir */
    strcpy(fm->rootdir, "/");
    fm->fs_size = fs_size;
    fm->block_size = block_size;
    
    addr = mmap(NULL, mmap_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, off);
    if (addr == NULL) {
        printf("Error in mmap");
        exit(0);
    }

    fprintf(stdout, "Writing to %s for %s metadata....\n\n", fs_fn, fm->name);
    //memcpy(addr, fm, sizeof(FS_META));
    add_block (fd, addr,  &mmap_len, 0, fm, sizeof(FS_META));
    /* end of 1st write */


    /* 2nd write assuming directory */
    strcpy(fm->name, "boo");
    fm->meta_type= 1; /* meta data type: 0 - root dir */
    strcpy(fm->rootdir, "/boo");
    fm->fs_size = fs_size;
    fm->block_size = block_size;
    
    // no mmap for 2nd write. just jump 512 bytes from addr 
    fprintf(stdout, "Writing to %s for %s metadata....\n\n", fs_fn, fm->name);
    add_block (fd, addr,  &mmap_len, block_size, fm, sizeof(FS_META));
    //memcpy(addr+block_size, fm, sizeof(FS_META));
    /* end of 2nd write */

    munmap(addr, mmap_len);
    close(fd);


    /* open file and mmap to read */
    fprintf(stdout, "Opening %s to read data ....\n", fs_fn);
    fd = open(fs_fn, O_RDWR| O_EXCL, S_IRUSR | S_IWUSR);

    if (fd < 0) {
        fprintf(stdout, "Failed to open file %s", fs_fn);
        exit(1);
    }

    addr = mmap(NULL, mmap_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, off);
    if (addr == NULL) {
        printf("Error in mmap");
        exit(0);
    }

    /* 1st read */
    fm_read = (FS_META *)malloc(sizeof(FS_META));
    memcpy(fm_read, addr, sizeof(FS_META));

    fprintf(stdout, "name: %s\n", fm_read->name);
    fprintf(stdout, "meta_type: %d\n", fm_read->meta_type);
    fprintf(stdout, "rootdir: %s\n", fm_read->rootdir);
    fprintf(stdout, "fs_size: %d\n", fm_read->fs_size);
    fprintf(stdout, "block_size: %d\n\n", fm_read->block_size);
     
    /* 2nd read */
    memset(fm_read, 0, sizeof(FS_META));
    memcpy(fm_read, addr+block_size, sizeof(FS_META));

    fprintf(stdout, "name: %s\n", fm_read->name);
    fprintf(stdout, "meta_type: %d\n", fm_read->meta_type);
    fprintf(stdout, "rootdir: %s\n", fm_read->rootdir);
    fprintf(stdout, "fs_size: %d\n", fm_read->fs_size);
    fprintf(stdout, "block_size: %d\n\n", fm_read->block_size);

    munmap(addr, mmap_len);
    close(fd);

    return 0;
} 

