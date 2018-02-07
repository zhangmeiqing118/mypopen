/**
 * @Filename: srv.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 08/10/2017 09:45:46 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <net/if.h>// struct ifreq  
#include <sys/ioctl.h> // ioctl、SIOCGIFADDR  
#include <sys/socket.h> // socket  
#include <netinet/ether.h> // ETH_P_ALL  
#include <netpacket/packet.h> // struct sockaddr_ll  

#include <netinet/in.h>  


#define LISTEN_PORT 80
int main(int argc, char *argv[])
{
    int len, recode;
    char data[2048];
    int listenfd, connfd, chilen;
    struct sockaddr_in chiaddr,servaddr;

    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd==-1) {
        printf("socket established error: %s\n",(char*)strerror(errno));
        return -1;
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(LISTEN_PORT);

    int opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int))) {
        perror("setsockopt");
        return -1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int))) {
        perror("setsockopt");
        return -1;
    }
    recode =bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(recode < 0) {
        printf("bind error: %s\n",strerror(errno));
        return -1;
    }

    printf("socket listen\n");
    listen(listenfd,5);
    printf("socket accept\n");
    connfd=accept(listenfd,(struct sockaddr*)&chiaddr,&chilen);
    if(connfd==-1) {
        printf("accept client error: %s\n",strerror(errno));
        return -1;
    }
    printf("client connected\n");

    do {
        len = recv(connfd, data, 2048, 0);
        if (len <= 0) {
            printf("sock recv error\n");
            break;
        }

        printf("tcp server recv data:%s\n", data);
        ///hex_dump(data, len);
    } while (1);
    close(connfd);
    close(listenfd);

    return 0;
}

