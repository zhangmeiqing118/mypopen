/*
 * @Filename: comm.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 05/30/2016 03:51:22 PM
 */
#include <stdio.h>
#include <pthread.h>

#include "comm.h"
#include "loadxml.h"

static int g_connect_flag = 0;
static int g_connect_sock = 0;
static int g_cli_init_flag = 0;
struct sockaddr_in g_sin_addr;
pthread_mutex_t g_pthread_mutex;

int aclk_sock_probe(void)
{
    unsigned int octeon_id;
    int len, rlen;
    aclk_dpi_cmd_t *req;
    char sdata[64], rdata[64];

    octeon_id = 2;
    memset(sdata, 0x00, 64);
    req = (aclk_dpi_cmd_t *)sdata;
    req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
    req->opcode = htonl(ACLK_MODULE_COMMAND | ACLK_COMMAND_CMD_OCTEON_PROBE);
    req->recode = 0;
    len = 0;
    req->len = htonl(len + sizeof(octeon_id));
    *((unsigned int *)(req->data + len)) = htonl(octeon_id);
    len += sizeof(octeon_id);
    rlen = 64;
    if (aclk_comm_send_and_recv(g_connect_sock, sdata, len + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
        return -1;
    }

    return 0;
}

void *aclk_sock_keep_alive(__attribute__((unused))void *argv)
{
    int flag;

    printf("start connect\n");
    flag = 0;
    g_connect_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == g_connect_sock) {
        perror("socket open failed");
        return NULL;
    }
    while (1) {
        if (g_connect_flag) {
            sleep(5);
            if (aclk_sock_probe()) {
                g_connect_flag = 0;
            }
            if(g_cli_init_flag) {
                load_rule(g_connect_sock,0);
                load_rule(g_connect_sock,1);
                g_cli_init_flag = 0;
            }
            continue;
        }
        if (flag) {
            close(g_connect_sock);
            g_connect_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (-1 == g_connect_sock) {
                ///perror("socket open failed");
                sleep(1);
                continue;
            }
        }
        if (connect(g_connect_sock, (struct sockaddr*)&g_sin_addr, sizeof(g_sin_addr)) < 0) {
            ///perror("Connect server fail!");
            sleep(2);
            continue; 
        }
        flag = 1;
        g_connect_flag = 1;
    }

    return NULL;
}

int aclk_sock_open (char *srv_ip, int port)
{
    //int sock;
    pthread_t tid;

    bzero(&g_sin_addr, sizeof(g_sin_addr));
    g_sin_addr.sin_family = AF_INET;
    g_sin_addr.sin_port = htons(port);
    g_sin_addr.sin_addr.s_addr = inet_addr(srv_ip);

    //if (connect(sock, (struct sockaddr*)&g_sin_addr, sizeof(g_sin_addr)) < 0) {
    //    perror("Connect server fail!");
    //} else {
    //    g_connect_flag = 1;
    //}
    //g_connect_sock = sock;
    g_connect_sock = 0;
    g_connect_flag = 0;
    g_cli_init_flag = 1;
    pthread_mutex_init(&g_pthread_mutex, NULL);
    if (pthread_create(&tid, NULL, aclk_sock_keep_alive, NULL)) {
        printf("create sock keep alive thread error");
        return -1;
    }
    
    return 0;
}


int aclk_comm_send_and_recv(__attribute__((unused))int sock, void *sdata, int ssize, void *rdata, int *rsize)
{
    int len;
    if (NULL == sdata || 0 == ssize) {
        perror("socket send buf null:");
        return -1;
    }
    if (!g_connect_flag) {
        printf("socket not connected!\n");
        return -1;
    }

    pthread_mutex_lock(&g_pthread_mutex);
    len = send(g_connect_sock, sdata, ssize, 0);
    if (len <= 0) {
        g_connect_flag = 0;
        perror("socket send data:");
        pthread_mutex_unlock(&g_pthread_mutex);
        return -1;
    }

    memset(rdata, 0x00, *rsize);
    len = recv(g_connect_sock, rdata, *rsize, 0);
    if (len <= 0) {
        g_connect_flag = 0;
        perror("sock recv");
        pthread_mutex_unlock(&g_pthread_mutex);
        return -1;
    }
    pthread_mutex_unlock(&g_pthread_mutex);

    return 0;
}

