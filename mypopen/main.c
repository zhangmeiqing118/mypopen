/**
 * @Filename: main.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 10/11/2016 08:55:21 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>  

#define __POPEN_WRITE
#define __POPEN_READ

#define SHELL       "/bin/sh"  
#define OPEN_MAX    1024

FILE *g_popen_fp[2];
static char *prompt="em>>";
static pid_t *childpid = NULL;  
static int run = 1;  

struct option g_long_opts[] = {
    { "exec",       1, NULL, 'e'},
    { "version",     0, NULL, 'v'},
    { "help",        0, NULL, 'h'},
    { NULL, 0, NULL, 0 } 
};

void usage(char *progname)
{
    printf("Usage : %s [OPTION...]\n\n\
            -e, --exec         exec shell command\n\
            -v, --version      Print program version and exit\n\
            -h, --help         Display this help and exit\n\
            \n\
            Report bugs to meiqing.zhang@accelink.com\n", progname);

    exit(0);
}

void version(char *progname)
{
    printf("%s version 1.0\n", progname);

    exit(0);
}

void print_help(void)
{
    printf("command help:\n");

    return;
}

void sig_handler(int sig)
{
    printf("sig:%d\n", sig);
    run = 0;

    return;
}

int set_sock_nonblocking(int fd)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) { 
        printf("%s[%d]:get sock fl:%s", __func__, __LINE__, strerror(errno));
        return -1;
    }    
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        printf("%s[%d]:set sock nonblocking:%s", __func__, __LINE__, strerror(errno));
        return -1;
    }

    return 0;
}

int
mypopen(const char *cmd, FILE *fps[2])  
{  
    int     i;
#ifdef __POPEN_WRITE
    int wfd[2];
#endif
#ifdef __POPEN_READ
    int rfd[2];  
#endif
    pid_t   pid;  
#ifdef __POPEN_WRITE
    FILE *wfp;
#endif
#ifdef __POPEN_READ
    FILE *rfp;  
#endif
 
    if (NULL == childpid) {
        childpid = calloc(OPEN_MAX, sizeof(pid_t));
        if (NULL == childpid) {
            printf("calloc for childpid error\n");
            return -1;
        }
    }  
  
#ifdef __POPEN_WRITE
    if (pipe(wfd) < 0) { 
        printf("pipe for write error\n");
        return -1;
    }
#endif

#ifdef __POPEN_READ
    if (pipe(rfd) < 0) {
        printf("pipe for read error\n");
        return -1;
    }
#endif

#ifdef __POPEN_WRITE
    printf("wfd[0]:%d, wfd[1]:%d\n", wfd[0], wfd[1]);
#endif
#ifdef __POPEN_READ
    printf("rfd[0]:%d, rfd[1]:%d\n", rfd[0], rfd[1]);
#endif

    if ((pid = fork()) < 0) {
        printf("fork new process error\n");
        return -1;
    } else if (pid == 0) {
        /* child */  
#ifdef __POPEN_READ
            close(rfd[0]);  
            if (rfd[1] != STDOUT_FILENO) {  
                dup2(rfd[1], STDOUT_FILENO);  
                close(rfd[1]);  
            }  
#endif
#ifdef __POPEN_WRITE
            close(wfd[1]);  
            if (wfd[0] != STDIN_FILENO) {  
                dup2(wfd[0], STDIN_FILENO);  
                close(wfd[0]);  
            }  
#endif
        for (i = 0; i < OPEN_MAX; i++) { 
            if (childpid[i] > 0) {
                close(i);
            }
        }
  
        execl(SHELL, "sh", "-c", cmd, (char *) 0);  
        _exit(127);
    }  
    
    ///parent process  
#ifdef __POPEN_READ
    close(rfd[1]);
    rfp = fdopen(rfd[0], "r");
    if (NULL == rfp) {
        return -1;
    }
#endif

#ifdef __POPEN_WRITE
    close(wfd[0]);  
    wfp = fdopen(wfd[1], "w");
    if (NULL == wfp) {
        printf("fdopen wfd error\n");
        return -1;
    }
#endif

#ifdef __POPEN_WRITE
    childpid[fileno(wfp)] = pid; /* remember child pid for this fd */  
    fps[1] = wfp;
#endif    
#ifdef __POPEN_READ
    childpid[fileno(rfp)] = pid; /* remember child pid for this fd */  
    fps[0] = rfp;
#endif

    return 0;
}  


int exec_shell(char *cmd)
{
    memset(g_popen_fp, 0x00, sizeof(g_popen_fp));
    if (mypopen(cmd, g_popen_fp)) {
        printf("mypopen %s error\n", cmd);
        return -1;
    }
    printf("popen rds[0]:%d, wds[1]:%d\n", fileno(g_popen_fp[0]), fileno(g_popen_fp[1]));
    ///set_sock_nonblocking(fileno(g_popen_fp[0]));

    return 0;
}

int exec_shell_write(char *cmd)
{
    int recode;

    ///recode = fwrite(cmd, strlen(cmd), 1, g_popen_fp[1]);
    ///recode = fwrite("\n", 1, 1, g_popen_fp[1]);
    ///fflush(g_popen_fp[1]);
    recode = write(fileno(g_popen_fp[1]), cmd, strlen(cmd));
    write(fileno(g_popen_fp[1]), "\n", 1);


    return 0;
}

int exec_shell_read(void *data)
{
    int recode;
    
    ///recode = fread(data, 1, BUFSIZ, g_popen_fp[0]);
    recode = read(fileno(g_popen_fp[0]), data, BUFSIZ);
    printf("read , recode:%d\ndata:\n%s\n", recode, (char *)data);

    return recode;
}

int main(int argc, char *argv[])
{
    int opt, index, ch, recode;
    char *progname, *cmd;
    char data[256], buf[BUFSIZ];
    ///struct termios initial_settings, new_settings;  
    
    progname = strrchr(argv[0], '/');
    if (progname) {
        ++progname;
    } else {
        progname = argv[0];
    }

    cmd = NULL;
    while (1) {
        opt = getopt_long(argc, argv, "e:vh", g_long_opts, 0);
        if (EOF == opt) {
            break;
        }
        switch (opt) {
            case 'e':
                cmd = optarg;
                break;
            case 'v':
                version(progname);
                break;
            case 'h':
                usage(progname);
                break;
            default:
                break;
        }
    }
    if (NULL == cmd) {
        usage(progname);
        return 0;
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    //tcgetattr(STDIN_FILENO,&initial_settings);  
    //new_settings = initial_settings;  
    //new_settings.c_lflag &= ~ICANON;  
    //new_settings.c_lflag &= ~ECHO;  
    //new_settings.c_cc[VMIN] = 1;  
    //new_settings.c_cc[VTIME] = 0;  
    //new_settings.c_lflag &= ~ISIG;  
    //if(tcsetattr(STDIN_FILENO, TCSANOW, &new_settings) != 0) {  
    //    fprintf(stderr,"could not set attributes\n");  
    //} 

    exec_shell(cmd);
    recode = exec_shell_read(buf);
    while (run) {
        printf("%s", prompt);
        index = 0;
        memset(data, 0x00, 256);
        while (index < 256) {
            ch = getchar();
            printf("%d\n", ch);
            if (ch == '\n') {
                data[index] = ch;
                printf("\n");
                break;
            }
            if ((index == 0) && (ch == '?')) {
                print_help();
                break;
            }
            ///if (isalpha(ch) || isspace(ch)) {
            data[index] = ch;
            index++;
            //}
        }
        if (index) {
            if (data[0] == 'q') {
                exit(0);
            }
            printf("exec command %s\n", data);
            exec_shell_write(data);
            printf("after shell write\n");
            memset(buf, 0x00, BUFSIZ);
            recode = exec_shell_read(buf);
            printf("after shell read\n");
            printf("recode:%d\n", recode);
            if (recode && recode != -1) {
                printf("data:\n%s\n", buf);
            }
        }
    }

    return 0;
}
