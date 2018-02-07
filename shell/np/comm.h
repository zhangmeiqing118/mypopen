#include <arpa/inet.h>/*
 * @Filename: comm.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 05/30/2016 03:51:22 PM
 */
#ifndef __COMM_H__
#define __COMM_H__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#ifdef  __cpulsplus
extern "C" {
#endif

#define ACLK_CMD_MAGIC_NUM  0xdeadbeef
#define ACLK_MODULE_COMMAND             0x400
#define ACLK_COMMAND_CMD_OCTEON_PROBE   0x02
#define ACLK_COMMAND_CMD_SEND_STRING    0x03

typedef struct aclk_dpi_cmd {
    uint32_t magic_num;
    uint32_t opcode;
    int32_t recode;
    uint32_t len;
    uint8_t  data[0];
} aclk_dpi_cmd_t;

int aclk_sock_open (char *srv_ip, int port);
int aclk_comm_send_and_recv(int sock, void *sdata, int ssize, void *rdata, int *rsize);

#ifdef  __cpulsplus
}
#endif

#endif
