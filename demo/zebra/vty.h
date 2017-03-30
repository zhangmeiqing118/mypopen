/*
 * @Filename: vty.h
 * @Brief   :
 * @Author  : 
 * @Version : 1.0.0
 * @Date    : 03/29/2017 01:07:15 PM
 */
#ifndef __VTY_H__
#define __VTY_H__

#ifdef __cplusplus
extern "C" {
#endif

#define VTY_MAXHIST 20
#define VTY_NEWLINE "\r\n"

#define CONTROL(X)  ((X) - '@')
#define VTY_PRE_ESCAPE      1
#define VTY_ESCAPE          2

typedef enum {
    VTY_TERM,
    VTY_FILE,
    VTY_SHELL,
    VTY_SHELL_SERV
} vty_type_t;

typedef enum {
    VTY_NORMAL,
    VTY_CLOSE,
    VTY_MORE,
    VTY_MORELINE,
    VTY_START,
    VTY_CONTINUE
} vty_status_t;

struct vty {
    int fd; ///file descripter of the vty
    int node;
    int port;///connect port
    int privilege;   ///Privilege level of the vty

    char *address; /// connect ip
    struct buffer *obuf;
    char *buf;
    char *user_name;
    vty_status_t status;    ///current vty status

    int cp;
    int length;
    int max;
    int hp; ///History lookup current point
    int hindex; ///History insert end point
    char *hist[VTY_MAXHIST];    ///Histry of command

    int used;
    unsigned char escape;

    unsigned char iac;      /// IAC handling
    unsigned char iac_sb_in_progress;   ///IAC SB handling
    struct buffer *sb_buffer;

    int width;
    int height;
    int scroll_one;
    int lines;
    ///int (*func) (struct vty *, void *arg);
    int (*output_func) (struct vty *, int);

    int last_time;///vty
    int v_timeout;///vty timeout:second
};

struct vty *vty_new(void);
struct vty *vty_create(int vty_sock, void *addr, int family);
void vty_destroy(struct vty *vty);
int vty_read(struct vty *vty);
int vty_flush(struct vty *vty);

#ifdef __cplusplus
}
#endif
#endif
