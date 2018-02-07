/*
 * @Filename: comm.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 03/29/2017 01:07:15 PM
 */
#ifndef __COMM_H__
#define __COMM_H__

#ifdef __cplusplus
extern "C" {
#endif
#if 0
#if defined(HAVE_IPV6) || defined(HAVE_GETADDRINFO)
int vty_serv_sock(const char *hostname, unsigned short port);
#else
int vty_serv_sock(unsigned short port);
#endif
#ifdef VTYSH
int vtysh_serv_sock(char *path);
#endif
#endif

int comm_init(void);
#ifdef __cplusplus
}
#endif
#endif
