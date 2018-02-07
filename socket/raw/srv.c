/**
 * @Filename: srv.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 08/10/2017 08:29:51 AM
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

struct sockaddr_ll g_sock_addr_sll;     //原始套接字地址结构

int raw_sock_init(char *ethname)
{
    struct sockaddr_ll sll;                 //原始套接字地址结构  
    struct ifreq req;                   //网络接口地址  
    int sockfd;
     
    sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));  
    if (sockfd < 0) {
        perror("socket");
    }
    memset(&req, 0x00, sizeof(req));
    strncpy(req.ifr_name, ethname, IFNAMSIZ);   //指定网卡名称  
    if(-1 == ioctl(sockfd, SIOCGIFINDEX, &req)) {//获取网络接口
        perror("ioctl get ifindex");  
        close(sockfd);  
        exit(-1);  
    }  
      
    /*将网络接口赋值给原始套接字地址结构*/  
    bzero(&g_sock_addr_sll, sizeof(g_sock_addr_sll));  
    g_sock_addr_sll.sll_ifindex = req.ifr_ifindex;  
    
    if(-1 == ioctl(sockfd, SIOCGIFFLAGS, &req)) {   //获取网络接口  
        perror("ioctl get pflags");  
        close(sockfd);  
        exit(-1);  
    }  
    req.ifr_flags |= IFF_PROMISC;  
    if(-1 == ioctl(sockfd, SIOCSIFFLAGS, &req)) {    //网卡设置混杂模式  
        perror("ioctl set pflags");  
        close(sockfd);  
        exit(-1);  
    }

    return sockfd;
}

int raw_sock_send(int sockfd, char *data, int size)
{
    // 发送数据  
    // send_msg, msg_len 这里还没有定义，模拟一下  
    int len;
    
    len = sendto(sockfd, data, size, 0 , (struct sockaddr *)&g_sock_addr_sll, sizeof(g_sock_addr_sll));  
    if(len <= 0) {
        perror("sendto");  
    } 

    return len;
}

int raw_sock_recv(int sockfd, char *data, int size)
{
    int len;

    len = recvfrom(sockfd, data, size, 0, NULL, NULL);  
    if(len <= 0) {
        perror("sendto");  
    } 

    return len;
}

void hex_dump(unsigned char *data, unsigned int len)
{
    int i;

    printf("data len:%d,data:\n", len);
    for (i = 0; i < len; i++) {
        printf("%02x ", data[i]);
        if (i % 8 ==  7) {
            printf("\n");
        }
    }
    printf("\n");

    return;
}

#define LISTEN_PORT 8888
void *tcp_server(void *args)
{
    int len, recode;
    char data[2048];
    int listenfd, connfd, chilen;
    struct sockaddr_in chiaddr,servaddr;

    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd==-1) {
        printf("socket established error: %s\n",(char*)strerror(errno));
        return NULL;
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(LISTEN_PORT);

    recode =bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(recode < 0) {
        printf("bind error: %s\n",strerror(errno));
        return NULL;
    }

    listen(listenfd,5);
    connfd=accept(listenfd,(struct sockaddr*)&chiaddr,&chilen);
    if(connfd==-1) {
        printf("accept client error: %s\n",strerror(errno));
        return NULL;
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

    return NULL;
}

int main(int argc,char *argv[])  
{  
    int sockfd, len;
    char data[2048];
    pthread_t tid;

    if (argc < 2) {
        printf("Usage: %s eth0\n", argv[0]);
        return -1;
    }

    sockfd = raw_sock_init(argv[1]);
    if (sockfd < 0) {
        return -1;
    }

    pthread_create(&tid, NULL, tcp_server, NULL);
    do {
        memset(data, 0x00, 2048);
        len = raw_sock_recv(sockfd, data, 2048);
        if (len < 0) {
            return -1;
        }
        if (*((uint16_t *)(data + 36)) == htons(8888)) {
            hex_dump(data, len);
            printf("raw recv data:%s\n", data + 54);
        }
    } while (1);

    return 0;  
}  
