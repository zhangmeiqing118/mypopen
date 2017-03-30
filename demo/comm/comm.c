/**
 * @Filename: comm.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 03/29/2017 09:21:43 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#if defined(HAVE_IPV6) || defined(HAVE_GETADDRINFO)
#include <netdb.h>
#endif

#include "event.h"
#include "zlog.h"
#include "vty.h"

#define BACKLOG     5
#define COMM_ZLOG_PROMPT "comm"

////global variable
extern struct event_base *g_master_base_ev;
static zlog_category_t *g_zlog_comm = NULL;

static void set_sockopt_reuseaddr(int sock)
{
    int recode;
    int on = 1;

    recode = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));
    if (recode < 0) {
        zlog_warn(g_zlog_comm, "set sockopt REUSEADDR failed(%s)", strerror(errno));
        return; 
    }

    return;
}

#ifdef SO_REUSEPORT
static void set_sockopt_reuseport(int sock)
{
    int recode;
    int on = 1;

    recode = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void *)&on, sizeof(on));
    if (recode < 0) {
        zlog_warn(g_zlog_comm, "can't set sockopt SO_REUSEPORT to socket %d", sock);
        return; 
    }

    return;
}
#endif

void set_sock_keepalive(int fd)
{
    int opt;

    opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&opt, sizeof(opt))<0) {
        zlog_error(g_zlog_comm, ":set sock keepalive failed(%s)", strerror(errno));
        return;
    }       

    return;
}

static void sock_read(evutil_socket_t sock, short events, void *user_data)
{
    struct vty *vty;

    if (NULL == user_data) {
        return;
    }
    vty = (struct vty*)user_data;
    printf("read event %d, sock:%d, events:%d\n", vty->fd, sock, events);
    vty_read(vty);
    ///printf("len:%d, buff:%s\n", len, buf);

    return;
}

#if 0
static void sock_write(evutil_socket_t sock, short events, void *user_data)
{
    struct vty *vty;

    if (NULL == user_data) {
        return;
    }
    vty = (struct vty*)user_data;
    printf("write:%d, sock:%d, events:%d\n", vty->fd, sock, events);

    return;
}
#endif

static void sock_accept_v4(evutil_socket_t sock, short events, __attribute__((unused))void *user_data)
{
    int recode, accept_fd, on;
    struct sockaddr_in sin;
    socklen_t sin_size;
    struct event *read_ev;//, *write_ev;
    struct vty *vty;

    if (events) {
        zlog_error(g_zlog_comm, "socket accept fd:%d, events:%d", sock, events);
        printf("socket accept fd:%d, events:%d", sock, events);
    }

    sin_size = sizeof(struct sockaddr_in);
    memset(&sin, 0x00, sizeof(struct sockaddr_in));
    accept_fd = accept(sock, (struct sockaddr*)&sin, &sin_size);
    if (accept_fd < 0) { 
        zlog_error(g_zlog_comm, "socket accept failed(%s)", strerror(errno));
        return;
    }    

    on = 1;
    recode = setsockopt(accept_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));
    if (recode < 0) {
        zlog_error(g_zlog_comm, "set sock nodelay failed(%s)", strerror(errno));
        close(accept_fd);
        return;
    }

    vty = vty_create(accept_fd, &sin, AF_INET);
    if (NULL == vty) {
        zlog_error(g_zlog_comm, "vty create failed(%s)", strerror(errno));
        close(accept_fd);
        return;
    }

    read_ev = event_new(g_master_base_ev, accept_fd, EV_READ|EV_PERSIST, sock_read, vty);
    if (NULL == read_ev) {
        zlog_error(g_zlog_comm, "event new failed");
        close(sock);
        return;
    }
    recode = event_add(read_ev, NULL);
    if (recode) {
        zlog_error(g_zlog_comm, "event add failed(recode:%d)", recode);
        if (read_ev) {
            event_free(read_ev);
        }
        close(sock);
        return;
    }
#if 0
    write_ev = event_new(g_master_base_ev, accept_fd, EV_WRITE|EV_PERSIST, sock_write, vty);
    if (NULL == read_ev) {
        zlog_error(g_zlog_comm, "event new failed");
        if (read_ev) {
            event_free(read_ev);
        }
        close(sock);
        return;
    }
    recode = event_add(write_ev, NULL);
    if (recode) {
        zlog_error(g_zlog_comm, "event add failed(recode:%d)", recode);
        if (read_ev) {
            event_free(read_ev);
        }
        if (write_ev) {
            event_free(read_ev);
        }
        close(sock);
        return;
    }
#endif

    return;
}

static void sock_accept_v6(evutil_socket_t sock, short events, void *user_data)
{
    int recode, accept_fd, on;
    struct sockaddr_in6 sin;
    socklen_t sin_size;

    if (events) {
        zlog_info(g_zlog_comm, "socket accept fd:%d, events:%d\n", sock, events);
    }
    if (user_data) {
        ;
    }

    sin_size = sizeof(struct sockaddr_in6);
    memset(&sin, 0x00, sizeof(struct sockaddr_in6));
    accept_fd = accept(sock, (struct sockaddr*)&sin, &sin_size);
    if (accept_fd < 0) { 
        zlog_error(g_zlog_comm, "socket accept failed(%s)", strerror(errno));
        return;
    }    

    on = 1;
    recode = setsockopt(accept_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));
    if (recode < 0) {
        zlog_error(g_zlog_comm, "set sock nodelay failed(%s)", strerror(errno));
        close(accept_fd);
        return;
    }

#if 0
    recode = vty_create(accept_fd, &sin);
    if (recode) {
        zlog_error(g_zlog_comm, "vty create failed(%s)", strerror(errno));
        close(accept_fd);
        return;
    }
#endif

    return;
}

#if defined(HAVE_IPV6) || defined(HAVE_GETADDRINFO)
static int vty_serv_sock_addrinfo(const char *hostname, unsigned short port)
{
    int recode, sock; 
    struct addrinfo req, *ainfo, *save;
    char port_str[BUFSIZ];
    struct event *listen_ev;

    sprintf(port_str, "%d", port);
    port_str[sizeof(port_str) - 1] = '\0';
    memset(&req, 0, sizeof(struct addrinfo));
    req.ai_flags = AI_PASSIVE;
    req.ai_family = AF_UNSPEC;
    req.ai_socktype = SOCK_STREAM;
    recode = getaddrinfo(hostname, port_str, &req, &ainfo);
    if (recode) { 
        zlog_error(g_zlog_comm, "getaddrinfo failed(%s)", gai_strerror(recode));
        return -1;
    }    

    ainfo_save = ainfo;
    do {
        if (ainfo->ai_family != AF_INET) {
            continue;
        }

        sock = socket(ainfo->ai_family, ainfo->ai_socktype, ainfo->ai_protocol);
        if (sock < 0) {
            continue;
        }

        set_sockopt_reuseaddr(sock);
#ifdef SO_REUSEPORT
        set_sockopt_reuseport(sock);
#endif
        recode = bind(sock, ainfo->ai_addr, ainfo->ai_addrlen);
        if (recode < 0) {
            close(sock);
            continue;
        }

        recode = listen(sock, BACKLOG);
        if (recode < 0) {
            close(sock);
            continue;
        }
        
        if (g_master_base_ev) {
            listen_ev = event_new(g_master_base_ev, sock, EV_READ|EV_PERSIST, sock_accept_v4, NULL);
            if (NULL == listen_ev) {
                close(sock);
                continue
            }
            recode = event_add(listen_ev, NULL);
            if (recode) {
                if (listen_ev) {
                    event_free(listen_ev);
                }
                close(sock);
            }
        }
    } while ((ainfo = ainfo->ai_next) != NULL);
    
    if (ainfo_save) {
        freeaddrinfo(ainfo_save);
    }
    
    return;
}

int vty_serv_sock(const char *hostname, unsigned short port)
{
    int recode;

    g_zlog_comm = zlog_get_category(COMM_ZLOG_PROMPT);
    if (!g_zlog_comm) {
        printf("comm actegroy init error\n");
        return -1;
    }

    if (!port) {
        zlog_error(g_zlog_comm, "Invalid port number!");
        return -1;
    }

    recode = vty_serv_sock_addrinfo(hostname, port);
    if (recode) {
        zlog_error(g_zlog_comm, "serv sock init error(recode:%d)", recode);
        return recode;
    }

    return -1;
}
#else
static int vty_serv_sock_family_v4(unsigned short port)
{
    int recode, sock; 
    struct sockaddr_in sin;
    struct event *listen_ev;

    memset(&sin, 0x00, sizeof(struct sockaddr_in));
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { 
        zlog_error(g_zlog_comm, "socket open failed(%s)", strerror(errno));
        return -1;
    }

    set_sockopt_reuseaddr(sock);
#ifdef SO_REUSEPORT
    set_sockopt_reuseport(sock);
#endif
    set_sock_keepalive(sock);

    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    recode = bind(sock, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
    if (recode < 0) { 
        zlog_error(g_zlog_comm, "socket bind failed(recode:%d)", recode);
        close(sock);
        return -1;
    }    

    printf("start listen\n");
    recode = listen(sock, BACKLOG);
    if (recode < 0) { 
        zlog_error(g_zlog_comm, "socket listen failed(recode:%d)", recode);
        close(sock);
        return -1;
    }    

    printf("create accept event\n");
    if (!g_master_base_ev) {
        return -1;
    }
    listen_ev = event_new(g_master_base_ev, sock, EV_READ|EV_PERSIST, sock_accept_v4, NULL);
    if (NULL == listen_ev) {
        zlog_error(g_zlog_comm, "event new failed");
        close(sock);
        return -1;
    }
    recode = event_add(listen_ev, NULL);
    if (recode) {
        zlog_error(g_zlog_comm, "event add failed(recode:%d)", recode);
        if (listen_ev) {
            event_free(listen_ev);
        }
        close(sock);
        return -1;
    }

    return 0;
}

static int vty_serv_sock_family_v6(unsigned short port)
{
    int recode, sock; 
    struct sockaddr_in6 sin6;
    struct event *listen_ev;

    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock < 0) { 
        zlog_error(g_zlog_comm, "socket open failed(%s)", strerror(errno));
        return -1;
    }

    set_sockopt_reuseaddr(sock);
#ifdef SO_REUSEPORT
    set_sockopt_reuseport(sock);
#endif
   
    memset(&sin6, 0x00, sizeof(struct sockaddr_in6));
    sin6.sin6_family = AF_INET6;
    sin6.sin6_port = htons(port);
    sin6.sin6_addr = in6addr_any;
    recode = bind(sock, (struct sockaddr *)&sin6, sizeof(struct sockaddr_in6));
    if (recode < 0) { 
        zlog_error(g_zlog_comm, "socket bind failed(recode:%d)", recode);
        close(sock);
        return -1;
    }    

    recode = listen(sock, BACKLOG);
    if (recode < 0) { 
        zlog_error(g_zlog_comm, "socket listen failed(recode:%d)", recode);
        close(sock);
        return -1;
    }    

    if (!g_master_base_ev) {
        return -1;
    }
    listen_ev = event_new(g_master_base_ev, sock, EV_READ|EV_PERSIST, sock_accept_v6, NULL);
    if (NULL == listen_ev) {
        zlog_error(g_zlog_comm, "event new failed");
        close(sock);
        return -1;
    }
    recode = event_add(listen_ev, NULL);
    if (recode) {
        zlog_error(g_zlog_comm, "event add failed(recode:%d)", recode);
        if (listen_ev) {
            event_free(listen_ev);
        }
        close(sock);
        return -1;
    }

    return 0;
}

static int vty_serv_sock_family(unsigned short port, int family)
{
    int recode; 

    recode = 0;
    if (AF_INET6 == family) {
        recode = vty_serv_sock_family_v6(port);
    } else {
        recode = vty_serv_sock_family_v4(port);
    }

    return recode;
}

int vty_serv_sock(unsigned short port)
{
    int recode;

    g_zlog_comm = zlog_get_category(COMM_ZLOG_PROMPT);
    if (!g_zlog_comm) {
        printf("comm actegroy init error\n");
        return -1;
    }

    if (!port) {
        zlog_error(g_zlog_comm, "Invalid port number!");
        return -1;
    }

    recode = vty_serv_sock_family(port, AF_INET);
    if (recode) {
        zlog_error(g_zlog_comm, "serv sock init error(recode:%d)", recode);
        return recode;
    }

    return 0;
}
#endif

#ifdef VTYSH
static int vty_serv_path(char *path)
{
    int recode, sock, size;
    mode_t old_mask;
    struct sockaddr_un serv;
    struct event *listen_ev;

    unlink(path);
    old_mask = umask(0077);
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        zlog_error(g_zlog_comm, "socket open failed(recode:%s)", strerror(errno));
        return -1;
    }

    memset(&serv, 0, sizeof(struct sockaddr_un));
    serv.sun_family = AF_UNIX;
    strncpy(serv.sun_path, path, strlen(path));
    size = sizeof(serv.sun_family) + strlen(serv.sun_path);
    recode = = bind(sock, (struct sockaddr *)&serv, size);
    if (recode < 0) {
        zlog_error(g_zlog_comm, "socket bind failed(recode:%d)", recode);
        close(sock);
        return -1;
    }
    
    recode = listen(sock, BACKLOG);
    if (recode < 0) { 
        zlog_error(g_zlog_comm, "socket listen failed(recode:%d)", recode);
        close(sock);
        return -1;
    }    

    umask(old_mask);

    if (!g_master_base_ev) {
        return -1;
    }
    listen_ev = event_new(g_master_base_ev, sock, EV_READ|EV_PERSIST, sock_accept_v4, NULL);
    if (NULL == listen_ev) {
        zlog_error(g_zlog_comm, "event new failed");
        close(sock);
        return -1;
    }
    recode = event_add(listen_ev, NULL);
    if (recode) {
        zlog_error(g_zlog_comm, "event add failed(recode:%d)", recode);
        if (listen_ev) {
            event_free(listen_ev);
        }
        close(sock);
        return -1;
    }
    
    return 0;
}

int vtysh_serv_sock(char *path)
{
    int recode;

    g_zlog_comm = zlog_get_category(COMM_ZLOG_PROMPT);
    if (!g_zlog_comm) {
        printf("comm actegroy init error\n");
        return -1;
    }

    if (!path) {
        zlog_error(g_zlog_comm, "Invalid path!");
        return -1;
    }

    recode = vty_serv_path(path)
    if (recode) {
        zlog_error(g_zlog_comm, "serv un init error(recode:%d)",recode);
        return recode;
    }
}
#endif
