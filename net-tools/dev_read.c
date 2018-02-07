/**
 * @Filename: dev_read.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 06/27/2017 08:23:53 AM
 */
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>

int main(int argc, char *argv[])
{
    int recode;
    int fd;
    struct ifreq ifrflags;

    if (argc < 2) {
        printf("%s dev name\n", argv[0]);
        return -1;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket open\n");
        return -1;
    }
    strncpy(ifrflags.ifr_name, argv[1], sizeof(ifrflags.ifr_name));
    recode = ioctl(fd, SIOCGIFFLAGS, (char *)&ifrflags);
    if (recode < 0) {
        perror("ioctl siocgifflags");
        return -1;
    }
    printf("get siocgifflags ok\n");
    printf("set promisc\n");
    ifr.ifr_flags |= IFF_PROMISC;
    recode = ioctl(fd, SIOCSIFFLAGS, &ifrflags);
    if (recode < 0) {
        perror("ioctl siocsifflags");
        return -1;
    }
    printf("set promisc ok\n");
    printf("set %s up\n", argv[1]);
    ifr.ifr_flags |= IFF_UP|IFF_RUNNING;
    recode = ioctl(fd, SIOCSIFFLAGS, &ifrflags);
    if (recode < 0) {
        perror("ioctl siocsifflags IFF_UP|IFF_RUNNING");
        return -1;
    }
    printf("set %s up\n", argv[1]);

    return 0;
}
