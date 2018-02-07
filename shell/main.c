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

#include <sys/types.h>
#include <sys/stat.h>

#include "zebra.h"

#include "npcmd.h"

/*  Master of threads. */
struct thread_master *g_thread_master;
struct option g_long_opts[] = {
    { "daemon",      0, NULL, 'd'},
    { "log_mode",    0, NULL, 'l'},
    { "config_file", 1, NULL, 'c'},
    { "vty_addr",    1, NULL, 'A'},
    { "vty_port",    1, NULL, 'P'},
    { "version",     0, NULL, 'v'},
    { "help",        0, NULL, 'h'},
    { NULL, 0, NULL, 0 } 
};

void usage(char *progname, int status)
{
    if (status != 0) {
        fprintf (stderr, "Try `%s --help' for more information.\n", progname);
    } else {    
        printf("Usage : %s [OPTION...]\n\n\
                -d, --daemon       Runs in daemon mode\n\
                -l, --log_mode     Set verbose log mode flag\n\
                -c, --config_file  Set configuration file name\n\
                -A, --vty_addr     Set vty's bind address\n\
                -P, --vty_port     Set vty's port number\n\
                -v, --version      Print program version\n\
                -h, --help         Display this help and exit\n\
                \n\
                Report bugs to bugs@accelink.com\n", progname);
    }

    exit(status);
}

void sighup(int sig)
{
    zlog_info("SIGHUP received");
}

void sigint(int sig)
{
    zlog_info ("Terminating on signal");
    exit(0);
}

void sigusr1(int sig)
{
    zlog_rotate(NULL);
}

void *signal_set(int signo, void (*func)(int))
{
    struct sigaction sig;
    struct sigaction osig;

    sig.sa_handler = func;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;

    if (-1 == sigaction (signo, &sig, &osig)) {
        return SIG_ERR;
    } else {
        return (osig.sa_handler);
    }
}

void signal_init(void)
{
    signal_set (SIGHUP, sighup);
    signal_set (SIGINT, sigint);
    signal_set (SIGTERM, sigint);
    signal_set (SIGPIPE, SIG_IGN);
    signal_set (SIGUSR1, sigusr1);

    return;
}

int exec_cmd(char *cmd, int fd)
{
    int i;
    FILE *fp;
    char data[1024], *ptr;
    int len, tlen;

    if ((memcmp(cmd, "quit", 4) == 0) || (memcmp(cmd, "exit", 4) == 0)) {
        return 1;
    }

    //printf("cmd:=%s=\n", cmd);
    fp = popen(cmd, "r");
    memset(data, 0x00, 1024);
    len = fread(data, 1, 1024, fp);
    ptr = data;
    //printf("data len:%d, data:\n%s\n", len, data);
    for (i = 0; i < len; i++) {
        if (data[i] == '\n') {
            data[i] = '\0';
            tlen = &(data[i]) - ptr;
            write(fd, ptr, tlen);
            write(fd, "\r\n", 2);
            ptr = &(data[i + 1]);
        }
    }
    pclose(fp);

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
            if ((idx == 0) && (buf[idx] == 127)) {
                continue;
            }
            write(vty->fd, buf + idx, recode);

            idx += recode;
            if ((idx == 2) && (buf[idx - 2] == '\r') && (buf[idx - 1] == '\n')) {
                idx = 0;
                continue;
            }
            if ((buf[idx - 1] == 127) && (idx > 2)) {
                idx -= 2;
            }
        } while (((idx > 0) & (buf[idx - 1] != '\n')));

        if (idx == 0) {
            continue;
        }

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

int main(int argc, char *argv[])
{
    int vty_port, opt;
    int log_mode, daemon_mode;
    char *vty_addr, *progname;
    char *config_file;
    struct thread thread;

    /**Set umask before anything for security */
    umask(0027);
    progname = strrchr(argv[0], '/');
    if (progname) {
        ++progname;
    } else {
        progname = argv[0];
    }

    ///open log file
    zlog_default = openzlog(progname, ZLOG_STDOUT, ZLOG_ZEBRA, LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON);
   
    vty_port = 0;
    daemon_mode = 0;
    log_mode = 0;
    config_file = NULL;
    vty_addr = NULL;
    while (1) {
        opt = getopt_long(argc, argv, "dlc:A:P:vh", g_long_opts, 0);
        if (EOF == opt) {
            break;
        }
        switch (opt) {
            case 'd':
                daemon_mode = 1;
                break;
            case 'l':
                log_mode = 1;
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'A':
                vty_addr = optarg;
                break;
            case 'P':
                vty_port = atoi(optarg);
                break;
            case 'v':
                version(progname);
                exit(0);
                break;
            case 'h':
                usage(progname, 0);
                break;
            default:
                break;
        }
    }

    /*  Daemonize. */
    if (daemon_mode) {
        daemon(0, 1);
    }

    /* Make master thread emulator.*/
    g_thread_master = thread_master_create();

    ///init signal process
    signal_init();

    ///
    cmd_init(1);
    vty_init();
    memory_init();
    install_element(VIEW_NODE, &shell_cmd);

    ///np cmd
    np_init();

    ///sort vty command
    sort_node();

    /*  Configuration file read*/
    vty_read_config(config_file, DEFAULT_CONFIG_FILE, SYSTEM_CONFIG_FILE);

    /** Make vty server socket. */
    vty_serv_sock(vty_addr, vty_port ? vty_port : DEFAULT_VTY_PORT, DEFAULT_VTYSH_PATH);
    while (thread_fetch(g_thread_master, &thread)) {
        thread_call(&thread);
    }

    return 0;
}
