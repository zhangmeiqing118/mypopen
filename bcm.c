/**
 * @Filename: bcm.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 03/16/2017 10:12:42 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

int telnet_bcm_process(int rfd, int wfd)
{
    int sock, maxfd;
    int nbytes;
    struct sockaddr_in srv_addr;
    char buffer[8192];
    struct timeval tv;
    fd_set  readFdSet;

    memset ((char *)&srv_addr,0,sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port   = htons(8000);
    srv_addr.sin_addr.s_addr = htonl(0x7f000001);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(sock, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in)) < 0) {
        printf("connect bcm shell fail\n");
        return -1;
    }

    while (1) {
        maxfd = sock > rfd ? sock : rfd;
        FD_ZERO(&readFdSet);
        FD_SET(rfd, &readFdSet);
        FD_SET(sock, &readFdSet);
        tv.tv_sec   = 1;
        tv.tv_usec  = 0;

        if(select(maxfd+1, &readFdSet, NULL, NULL, &tv) > 0) {
            if(FD_ISSET(sock, &readFdSet)) {
                nbytes = read (sock, buffer, 8192);
                write (wfd, buffer, nbytes);
            }
            if(FD_ISSET(rfd, &readFdSet))  {
                nbytes = read (rfd, buffer, 8192);
                write (sock, buffer, nbytes);
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    telnet_bcm_process(fileno(stdin), fileno(stdout));


    return 0;
}
