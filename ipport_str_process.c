/**
 * @Filename: test.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 09/06/2016 08:53:17 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

char *ipstr="1.2.3.4, 192.168.6.1-192.168.6.128, 10.3.44.56";
char *portstr="1234, 80-128, 44, 56";

int ipstr_process(char *ptr, uint32_t *ip, uint32_t *ip_num)
{
    char *tmp;
    char startip[20], endip[20];
    uint32_t i, start, end, max_num;
    struct in_addr addr;

    if (*ptr == '"') {
        printf("ptr[0] == \"\n");
        ++ptr;
    }

    max_num = *ip_num;
    *ip_num = 0;
    while (*ptr) {
        memset(startip, 0x00, 20);
        memset(endip, 0x00, 20);
        
        if (isspace(*ptr)) {
            ++ptr;
            continue;
        }

        tmp = ptr;
        while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\"')) {
            tmp++;
        }
        memcpy(startip, ptr, tmp - ptr);
        printf("startip:%s\n", startip);
        if (0 == inet_aton(startip, &addr)) {
            printf("Invalid ip str\n");
            return -1;
        }
        start = inet_network(startip);

        if (*ip_num < max_num) {
            ip[*ip_num] = start;
            (*ip_num)++;
        } else {
            printf("ip num is too many\n");
            return -1;
        }
        if (*tmp == '\"') {
            break;
        }
        ptr = tmp;
        if (*ptr == ',') {
            ptr++;
            continue;
        }

        printf("*ptr:%c\n", *ptr);
        if (*ptr == '-') {
            ptr++;
            tmp = ptr;
            while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\"')) {
                tmp++;
            }
            if (*tmp == '-') {
                printf("Invalid ip str\n");
                return -1;
            }
            memcpy(endip, ptr, tmp - ptr);
            printf("endip:%s\n", endip);
            if (0 == inet_aton(endip, &addr)) {
                printf("Invalid ip str\n");
                return -1;
            }
            end = inet_network(endip);
            printf("startip:%x, endip:%x\n", start, end);
            for (i = start + 1; i <= end; i++) { 
                if (*ip_num < max_num) {
                    ip[*ip_num] = i;
                    (*ip_num)++;
                } else {
                    printf("ip num is too many\n");
                    return -1;
                }
            }
            if (*tmp == '\"') {
                break;
            }
            ptr = tmp;
            ptr++;
        }
    }

    return 0;
}

int portstr_process(char *ptr, uint16_t *port, uint16_t *port_num)
{
    char *tmp;
    char startport[20], endport[20];
    uint16_t i, start, end, max_num;
    struct in_addr addr;

    if (*ptr == '"') {
        printf("ptr[0] == \"\n");
        ++ptr;
    }

    max_num = *port_num;
    *port_num = 0;
    while (*ptr) {
        memset(startport, 0x00, 20);
        memset(endport, 0x00, 20);

        if (isspace(*ptr)) {
            ++ptr;
            continue;
        }

        tmp = ptr;
        while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\"')) {
            tmp++;
        }
        memcpy(startport, ptr, tmp - ptr);
        printf("startport:%s\n", startport);
        start = atoi(startport);

        if (*port_num < max_num) {
            port[*port_num] = start;
            (*port_num)++;
        } else {
            printf("port num is too many\n");
            return -1;
        }
        if (*tmp == '\"') {
            break;
        }
        ptr = tmp;
        if (*ptr == ',') {
            ptr++;
            continue;
        }

        ///printf("*ptr:%c\n", *ptr);
        if (*ptr == '-') {
            ptr++;
            tmp = ptr;
            while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\"')) {
                tmp++;
            }
            if (*tmp == '-') {
                printf("Invalid port str\n");
                return -1;
            }
            memcpy(endport, ptr, tmp - ptr);
            printf("endport:%s\n", endport);
            end = atoi(endport);
            printf("startport:%x, endport:%x\n", start, end);
            for (i = start + 1; i <= end; i++) { 
                if (*port_num < max_num) {
                    port[*port_num] = i;
                    (*port_num)++;
                } else {
                    printf("port num is too many\n");
                    return -1;
                }
            }
            if (*tmp == '\"') {
                break;
            }
            ptr = tmp;
            ptr++;
        }
    }

    return 0;
}
int main(int argc, char *argv[])
{
    unsigned int i;
    unsigned int ip[256], ip_num;
    unsigned short port[256], port_num;
    struct in_addr addr;

    ip_num = 256;
    if (ipstr_process(ipstr, ip, &ip_num)) {
        printf("Invalid ip str\n");
        return -1;
    }

    printf("ip num:%d\n", ip_num);
    for (i = 0; i < ip_num; i++) {
        addr.s_addr = htonl(ip[i]);
        printf("%s\n", inet_ntoa(addr));
    }
   
    port_num = 256;
    if (portstr_process(portstr, port, &port_num)) {
        printf("Invalid ip str\n");
        return -1;
    }

    printf("port num:%d\n", port_num);
    for (i = 0; i < port_num; i++) {
        printf("%d\n", port[i]);
    }

    return 0;
}
