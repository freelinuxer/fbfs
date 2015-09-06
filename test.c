#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#define FN "/tmp/mytest"
#define DOTS "......................................\n"
int main()
{
    int fd = -1;
    int i = 0;
    char *c = NULL;
    int flag = 0;
    int len = 4096;
    char *addr = NULL;
    off_t off = 0;
    unlink(FN);
    fd = open(FN, O_CREAT| O_RDWR| O_EXCL, S_IRUSR | S_IWUSR);

    if (fd < 0) {
        fprintf(stdout, "Failed to open file %s", FN);
        exit(1);
    }
    if (ftruncate(fd, len/4) == -1) {
        printf("error in truncate");
       exit(0);
    }
    c = malloc(len);

    for (i = 0; i < len; i++) {
        if (i % 3) {
            c[i] = 'a';
        } else {
            c[i] = 'b';
        }
    }
    addr = mmap(NULL, len/4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, off);
    if (addr == NULL) {
        printf("Error in mmap");
        exit(0);
    }
    for (i = 0; i < len/4; i++) {
        addr[i] = '1';
    }

    for (i= 0; i < 10000; i++) {
        fprintf(stdout, "Writing ....");
        #if 0
            write(fd, c, 1024);
        #else
            memcpy(addr, c, len);
        #endif
        fprintf(stdout, "Sleeping ....");
        sleep(10);
    }
} 
