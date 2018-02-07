/**
 * @Filename: shell.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 08/18/2017 03:06:17 PM
 */
#include <stdio.h>


int exec_cmd(char *cmd, int fd)
{
    int i;
    FILE *fp;
    char data[1024], *ptr;
    int len, tlen;

    if ((memcmp(cmd, "quit", 4) == 0) || (memcmp(cmd, "exit", 4) == 0)) {
        return 1;
    }

    printf("cmd:=%s=\n", cmd);
    fp = popen(cmd, "r");
    memset(data, 0x00, 1024);
    len = fread(data, 1, 1024, fp);
    ptr = data;
    printf("data len:%d, data:\n%s\n", len, data);
    for (i = 0; i < len; i++) {
        if (data[i] == '\n') {
            data[i] = '\0';
            tlen = &(data[i]) - ptr;
            write(fd, ptr, tlen);
            write(fd, "\r\n", 2);
            ptr = &(data[i + 1]);
        }
    }

    return 0;
}

DEFUN(shell, shell_cmd, "shell", "go to shell mode\n")
{
    int recode;
    int idx;
    char buf[2048];
    char *sh;
                      
    sh = getenv("SHELL");
    if (NULL == sh) {
        sh = "/bin/sh";
    }

    idx = 0;
    memset(buf, 0x00, 2048);
    while (1) {
        write(vty->fd, "\r\nbash# ", 7);
        do {
            recode = read(vty->fd, buf + idx, 2048 - idx);
            if (recode <= 0) {
                break;
            }
            write(vty->fd, buf +idx, recode);

            idx += recode;
        } while ((idx > 0) & (buf[idx - 1] != '\n'));

        if ((idx > 2) && (buf[idx - 2] = '\r')) {
            buf[idx - 2] = '\0';
        }
        if ((idx > 1) && (buf[idx - 1] = '\r')) {
            buf[idx - 1] = '\0';
        }
        if (exec_cmd(buf, vty->fd)) {
            break;
        }
        idx = 0;
        memset(buf, 0x00, 2048);
    }

    return CMD_SUCCESS;
}
