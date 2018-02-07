/**
 * @Filename: test.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/25/17 20:33:47
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <time.h>

#define NTP_PORT    123
#define JAN_1970    0x83aa7e80
#define NTPFRAC(x)  (4294 * (x) + ((1981 * (x)) >> 11))

static int g_ntp_server = 0;

static void ntpc_localtime(uint32_t *time_coarse, uint32_t *time_fine)
{
#if 1
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);
    *time_coarse = now.tv_sec + JAN_1970;
    *time_fine   = NTPFRAC(now.tv_nsec/1000);
#else
    struct timeval now;

    gettimeofday(&now, NULL);
    *time_coarse = now.tv_sec + JAN_1970;
    *time_fine   = NTPFRAC(now.tv_usec);
#endif

    return;
}

static int send_packet(int sockfd, uint32_t *time_sent)
{
    int recode;
    uint32_t data[12];
    struct sockaddr_in server;

#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4
#define PREC -6

    printf("Sending ...\n");
    memset(data, 0, sizeof(data));
    data[0] = htonl (
            ( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |
            ( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );
    data[1] = htonl(1 << 16);  /*  Root Delay (seconds) */
    data[2] = htonl(1 << 16);  /*  Root Dispersion (seconds) */
    ntpc_localtime(&(time_sent[0]), &(time_sent[1]));
    data[10] = htonl(time_sent[0]); /*  Transmit Timestamp coarse */
    data[11] = htonl(time_sent[1]); /*  Transmit Timestamp fine   */

    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(NTP_PORT);
    server.sin_addr.s_addr = g_ntp_server;
    recode = sendto(sockfd, data, 48, 0,(struct sockaddr *)&server,sizeof(server));
    if (recode != 48) {
        printf("send ntp packet error\n");
        return -1;
    }

    return 0;
}

static int recv_packet(int sockfd)
{
    int recode;
    char buf[BUFSIZ];
    fd_set fds;
    struct timeval tm_val;
    socklen_t addrlen;
    struct sockaddr_in server;

    tm_val.tv_sec = 2;
    tm_val.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);
    recode = select(sockfd + 1, &fds, NULL, NULL, &tm_val);
    if (recode > 0) {
        bzero(&server,sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(NTP_PORT);
        server.sin_addr.s_addr = g_ntp_server;
        recode = recvfrom(sockfd, buf, BUFSIZ, 0, (struct sockaddr *)&server, &addrlen);
        if (recode > 0) {
            printf("socket recv len:%d\n", recode);
        } else {
            printf("socket recv error\n");
        }
    } else {
        printf("select error\n");
        return -1;
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    int sockfd;
    uint32_t time_sent[2];

    g_ntp_server = inet_addr("10.3.56.97");
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        printf("socket open error\n");
        return -1;
    }

    printf("send ntp packet\n");
    send_packet(sockfd, time_sent);
    printf("recv ntp packet\n");
    recv_packet(sockfd);
    printf("recv ntp packet over\n");

    return 0;
}
