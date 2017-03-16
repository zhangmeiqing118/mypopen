/**
 * @Filename: linux_if.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 03/15/2017 02:29:14 PM
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include "type.h"
#define MAXINTERFACES 16
#define SIN_ADDR(x) (((struct sockaddr_in *) (&(x)))->sin_addr.s_addr)
#define SET_SA_FAMILY(addr, family) \
    memset((char*)&(addr), '\0',   sizeof(addr)); \
    addr.sa_family = (family)
char device_name[20];
ET_ENET_CONFIG eth;
ET_ROUTE_CONFIG route;
int get_ip(char* ipaddr, char *netmask, const char *ethname )
{
    int ret = -1;
    if ( ipaddr && netmask && ethname )
    {
        int fd, intrface;
        struct ifreq buf[16];
        struct ifconf ifc;

        if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
            return -1;
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;

        if (ioctl (fd, SIOCGIFCONF, (char *) &ifc) < 0)
            goto _error_;

        intrface = ifc.ifc_len / sizeof(struct ifreq);

        while(intrface-- > 0)
        {
            if (strstr(buf[intrface].ifr_name, ethname))
            {
                if ((ioctl (fd, SIOCGIFADDR, (char*)&buf[intrface])) < 0)
                    goto _error_;
                sprintf(ipaddr, "%s",
                        inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));

                if ((ioctl (fd, SIOCGIFNETMASK , (char*)&buf[intrface])) < 0)
                    goto _error_;

                sprintf(netmask, "%s",
                        inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_netmask))->sin_addr));
                ret = 0;
            }
        }

    _error_:
        close(fd);
    }

    return ret;
}
int get_mac(char *mac)
{
    int fd, ret = -1;
    if ( mac )
    {
        struct ifreq ifr;

        if( (fd = socket(AF_INET, SOCK_DGRAM, 0) ) < 0)
        {
            perror("socket");
            return -1;
        }

        ifr.ifr_addr.sa_family = AF_INET;
        strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

        if(ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
        {
            sprintf(mac, "x-x-x-x-x-x",
                    (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

            strupr(mac);

            ret = 0;
        }

        close(fd);
    }

    return ret;
}


static int set_ipaddr(char *ipaddr)
{
    int s;
    sprintf(ipaddr, "%d.%d.%d.%d", (ip & 0xff000000) >> 24, (ip & 0xff0000) >> 16, (ip & 0xff00) >> 8, (ip & 0xff));
    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return FAIL;
    }

    struct ifreq ifr;
    bzero((char *)&ifr, sizeof(ifr));
    strcpy(ifr.ifr_name, "eth0");

    struct sockaddr_in addr;
    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    inet_aton(ipaddr, &addr.sin_addr); //将输入字符转成网络地址

    memcpy((char*)&ifr.ifr_ifru.ifru_addr, (char*)&addr, sizeof(struct sockaddr_in));
    if(ioctl(s, SIOCSIFADDR, &ifr) < 0)
    {
        close(s);
        return FAIL;
    }
    close(s);
    return OK;
}

static int set_netmask(char *netmask)
{
    int s;
    sprintf(netmask, "%d.%d.%d.%d", (mask & 0xff000000) >> 24, (mask & 0xff0000) >> 16, (mask & 0xff00) >> 8,   (mask & 0xff));
    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        KK_ERROR("[set_netmask]create socket failed:line:%d\n", __LINE__);
        return FAIL;
    }

    struct ifreq ifr;
    bzero((char *)&ifr, sizeof(ifr));
    strcpy(ifr.ifr_name, "eth0");

    struct sockaddr_in netmask_addr;
    bzero(&netmask_addr, sizeof(struct sockaddr_in));
    netmask_addr.sin_family = AF_INET;
    inet_aton(netmask, &netmask_addr.sin_addr);

    memcpy((char*)&ifr.ifr_ifru.ifru_netmask, (char*)&netmask_addr, sizeof(struct sockaddr_in));
    if(ioctl(s, SIOCSIFNETMASK, &ifr) < 0)
    {
        close(s);
        KK_ERROR("[set_netmask]ioctl failed:line:%d\n", __LINE__);
        return FAIL;
    }
    close(s);
    return OK;
}

static int set_gateway(char *gateway)
{
    static int sock_fd = -1;
    struct rtentry rt;
    U32 dstaddr, gwaddr;
    sprintf(gateway, "%d.%d.%d.%d", (gw & 0xff000000) >> 24, (gw & 0xff0000) >> 16, (gw & 0xff00) >> 8,   (gw & 0xff));
    dstaddr = inet_addr("0.0.0.0");
    gwaddr = inet_addr(gateway);

   
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&rt, 0, sizeof(rt));

   
    SET_SA_FAMILY (rt.rt_dst, AF_INET);
    SIN_ADDR(rt.rt_dst) = dstaddr;

   
    SET_SA_FAMILY (rt.rt_gateway, AF_INET);
    SIN_ADDR(rt.rt_gateway) = gwaddr;

   
    SET_SA_FAMILY (rt.rt_genmask, AF_INET);
    SIN_ADDR(rt.rt_genmask) = 0L;
    rt.rt_dev = "eth0";
    rt.rt_flags = RTF_GATEWAY;
    if(ioctl(sock_fd, SIOCADDRT, &rt) < 0)
    {
        close(sock_fd);
        KK_ERROR("[set_gateway]ioctl failed:line:%d\n", __LINE__);
        return FAIL;
    }
    return OK;
}
