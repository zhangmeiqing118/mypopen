/**
 * @Filename: mac.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 09/13/2017 02:36:23 PM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <net/if_arp.h>

#define IFNAME "eth0"
typedef unsigned char UINT8;

int macAddrSet(UINT8* mac)  
{  
    struct ifreq temp;  
    struct sockaddr* addr;  
  
    int fd = 0;  
    int ret = -1;  
      
    if((0 != getuid()) && (0 != geteuid()))  
        return -1;  
  
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  
    {  
        return -1;  
    }  
  
    strcpy(temp.ifr_name, IFNAME);  
    addr = (struct sockaddr*)&temp.ifr_hwaddr;  
      
    addr->sa_family = ARPHRD_ETHER;  
    memcpy(addr->sa_data, mac, 6);  
      
    ret = ioctl(fd, SIOCSIFHWADDR, &temp);  
      
    close(fd);  
    return ret;  
}  
  
int macAddrGet(UINT8* mac)  
{  
    struct ifreq temp;  
    struct sockaddr* addr;  
  
    int fd = 0;  
    int ret = -1;  
      
    if((0 != getuid()) && (0 != geteuid()))  
        return -1;  
  
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  
    {  
        return -1;  
    }  
  
    strcpy(temp.ifr_name, IFNAME);  
    addr = (struct sockaddr*)&temp.ifr_hwaddr;  
      
    addr->sa_family = ARPHRD_ETHER;  
      
    ret = ioctl(fd, SIOCGIFHWADDR, &temp);  
    close(fd);  
  
    if(ret < 0)  
        return -1;  
  
    memcpy(mac, addr->sa_data, 6);  
  
    return ret;  
}

int main(int argc, char *argv[])
{
    int opt, flag, recode;
    UINT8 mac[6], *ptr;

    flag = 0;
    while ((opt = getopt(argc, argv, "rw:")) != -1) {
        switch (opt) {
        case 'r':
            flag = 1;
            break;
        case 'w':
            flag = 2;
            ptr = optarg;
            break;
        }
    }
    if (flag == 1) {
        memset(mac, 0x00, 6);
        recode = macAddrGet(mac);
        if (recode) {
            printf("mac get error\n");
            return -1;
        }
        printf("mac %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    if (flag == 2) {
        int i;
        printf("optind:%d, optarg:%s\n", optind, ptr);
        if (strlen(ptr) < 17) {
             printf("Invalid mac address len!!!\n");
            return -1;
        }
        memset(mac, 0x00, 6);
        for (i = 0; i < 6; i++) {
            if ((ptr[i * 3] >= 'A') && (ptr[i * 3] <= 'F')) {
                mac[i] = ptr[0] - 'A' + 10;
            } else if ((ptr[i * 3] >= 'a') && (ptr[i * 3] <= 'f')) {
                mac[i] = ptr[0] - 'a' + 10;
            } else if ((ptr[i * 3] >= '0') && (ptr[i * 3] <= '9')) {
                mac[i] = ptr[i * 3] - '0';
            } else {
                printf("Invalid mac address!!!\n");
                return -1;
            }
            mac[i] = mac[i] << 4;
            if ((ptr[i * 3 + 1] >= 'A') && (ptr[i * 3 + 1] <= 'F')) {
                mac[i] += (ptr[0] - 'A' + 10);
            } else if ((ptr[i * 3 + 1] >= 'a') && (ptr[i * 3 + 1] <= 'f')) {
                mac[i] += (ptr[0] - 'a' + 10);
            } else if ((ptr[i * 3 + 1] >= '0') && (ptr[i * 3 + 1] <= '9')) {
                mac[i] += (ptr[i * 3 + 1] - '0');
            } else {
                printf("Invalid mac address!!!\n");
                return -1;
            }
        }
        printf("write mac %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        recode = macAddrSet(mac);
        if (recode) {
            printf("mac set error\n");
            return -1;
        }
    }

    return 0;
}
