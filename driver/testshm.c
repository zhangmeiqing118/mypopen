/**
 * @Filename: testshm.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 08/16/2017 12:22:38 PM
 */
#include <stdio.h>

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/mman.h>
    #include <errno.h>

int main(int argc, char* argv[])
{
    char *map_addr;
    int fd;

    if( (fd = open("/proc/shmfile", O_RDWR)) < 0){
        printf("open /proc/shmfile error!\n");
        return -1;
    }

    map_addr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0); // ignored, because we don't use "vm->pgoff" in driver.
    if( MAP_FAILED == map_addr){
        printf("mmap() error:[%d]\n", errno);
        return -1;
    }
    ///strcpy(map_addr, "hello world\n");
    printf("shm:%s\n", map_addr);
    munmap(map_addr, 4096);
    close(fd);

    return 0;
}
