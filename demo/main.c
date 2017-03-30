/**
 * @Filename: main.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 10/11/2016 08:55:21 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

#include "event.h"
#include "comm.h"

#define DEFAULT_ZLOG_CONF "/etc/zlog.conf"
char *default_motd = 
"\r\n\
    *******************************************************************\r\n\
    *   ##     ####    ####   ######  #          #    #    #  #    #  *\r\n\
    *  #  #   #    #  #    #  #       #          #    ##   #  #   #   *\r\n\
    * #    #  #       #       #####   #          #    # #  #  ####    *\r\n\
    * ######  #       #       #       #          #    #  # #  #  #    *\r\n\
    * #    #  #    #  #    #  #       #          #    #   ##  #   #   *\r\n\
    * #    #   ####    ####   ######  ######     #    #    #  #    #  *\r\n\
    *******************************************************************\r\n\
\r\n";


struct event_base *g_master_base_ev = NULL;
extern void vty_serv_sock_family(unsigned short port, int family);

void sig_handler(int sig)
{
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    if (zlog_init(DEFAULT_ZLOG_CONF)) {
        printf("zlog init failed\n");
        return -1;
    }

    g_master_base_ev = event_base_new();
    if (NULL == g_master_base_ev) {
        printf("master event vase init failed\n");
        return -1;
    }

    vty_serv_sock(2601);

    event_base_dispatch(g_master_base_ev);

    return 0;
}
